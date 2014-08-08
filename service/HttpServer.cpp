/*
 * Copyright 2009-2014, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Lotz <mmlr@mlotz.ch>
 *		François Revol <revol@free.fr>
 *		Alexander von Gluck IV <kallisti5@unixzen.com>
 */

#include "HttpServer.h"

#include <NetEndpoint.h>
#include <Autolock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HttpHandler.h"
#include "HttpWorker.h"


#define TRACE(x...)			debug_printf("BHttpServer: "x)
#define TRACE_ERROR(x...)	debug_printf("BHttpServer: "x)


BHttpServer::BHttpServer(BNetEndpoint *listener)
	:
	fListener(listener),
	fReceiverThread(-1),
	fStopThread(false),
	fLocker("BHttpServer locker")
{
	fReceiverThread = spawn_thread(_NetworkReceiverEntry, "html5 server",
		B_NORMAL_PRIORITY, this);
	resume_thread(fReceiverThread);
}


BHttpServer::~BHttpServer()
{
	fStopThread = true;

	if (fListener != NULL)
		fListener->Close();

	//int32 result;
	//wait_for_thread(fReceiverThread, &result);
		// TODO: find out why closing the endpoint doesn't notify the waiter

	kill_thread(fReceiverThread);
}


void
BHttpServer::AddHandler(HttpHandler *handler)
{
	BAutolock lock(fLocker);
	fHandlers.AddItem(handler);
}

int32
BHttpServer::_NetworkReceiverEntry(void *data)
{
	return ((BHttpServer *)data)->_NetworkReceiver();
}


status_t
BHttpServer::_NetworkReceiver()
{
	status_t result = fListener->Listen();
	if (result != B_OK) {
		TRACE_ERROR("failed to listen on port: %s\n", strerror(result));
		return result;
	}

	fHandlers.SortItems(&HttpHandler::_CallbackCompare);

	while (!fStopThread) {
		BNetEndpoint *endpoint = fListener->Accept(1000);
		if (endpoint == NULL)
			continue;

		TRACE("new endpoint connection: %p\n", endpoint);
		while (!fStopThread) {
			int32 errorCount = 0;
			uint8 buffer[4096];
			int32 readSize = endpoint->Receive(buffer, sizeof(buffer) - 1);
			if (readSize < 0) {
				TRACE_ERROR("read failed, closing connection: %s\n",
					strerror(readSize));
				delete endpoint;
				break;
			}

			if (readSize == 0) {
				TRACE("read 0 bytes, retrying\n");
				snooze(100 * 1000);
				errorCount++;
				if (errorCount == 5) {
					TRACE_ERROR("failed to read, assuming disconnect\n");
					delete endpoint;
					break;
				}
				continue;
			}
			buffer[readSize] = '\0';

			const char err404[] = "HTTP/1.1 404 Not found\r\n\r\n";

			// XXX: HACK HACK HACK
			const char *p = (const char *)buffer;
			if (strncmp(p, "GET ", 4) == 0) {
				p += 4;
				const char *s = strchr(p, ' ');
				if (s && strncmp(s, " HTTP/", 6) == 0) {
					if (*p == '/')
						p++;
					BString path(p, s - p);
					if (p == s)
						path = "desktop.html";
					TRACE("searching handler for '%s'\n", path.String());
					HttpHandler *handler = fHandlers.BinarySearchByKey(
						path, &HttpHandler::_CallbackCompare);
					if (handler) {
						TRACE("found handler '%s'\n", handler->Name().String());
						HttpWorker *worker =
							new (std::nothrow) HttpWorker(endpoint, handler);
						if (worker) {
							fWorkers.AddItem(worker);
							break;
						}
					}
				}
			}

			// some error
			endpoint->Send(err404, sizeof(err404) - 1);
			delete endpoint;
			break;

#if 0
			status_t result = fTarget->Write(buffer, readSize);
			if (result != B_OK) {
				TRACE_ERROR("writing to ring buffer failed: %s\n",
					strerror(result));
				return result;
			}
#endif
		}
	}

	return B_OK;
}
