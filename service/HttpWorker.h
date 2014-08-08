/*
 * Copyright 2009, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Lotz <mmlr@mlotz.ch>
 *		François Revol <revol@free.fr>
 */
#ifndef HTTP_WORKER_H
#define HTTP_WORKER_H


#include <OS.h>
#include <SupportDefs.h>


class BNetEndpoint;
class StreamingRingBuffer;
class HttpHandler;

class HttpWorker {
public:
								HttpWorker(BNetEndpoint *endpoint,
									HttpHandler *handler);
								~HttpWorker();

		BNetEndpoint *			Endpoint() { return fEndpoint; }

private:
static	int32					_WorkEntry(void *data);
		status_t				_Work();

		HttpHandler *			fHandler;

		thread_id				fWorkThread;
		bool					fStopThread;

		BNetEndpoint *			fEndpoint;
};

#endif // HTTP_WORKER_H
