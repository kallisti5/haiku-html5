/*
 * Copyright 2009, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Lotz <mmlr@mlotz.ch>
 *		François Revol <revol@free.fr>
 */
#ifndef WEB_SERVER_H
#define WEB_SERVER_H


//#include "HttpWorker.h"

#include <Locker.h>
#include <ObjectList.h>
#include <OS.h>
#include <SupportDefs.h>


class BNetEndpoint;
class HttpHandler;
class HttpWorker;


class BHttpServer {
public:
								BHttpServer(BNetEndpoint *listener);
								~BHttpServer();

//		BNetEndpoint *			Endpoint() { return fEndpoint; }

		void					AddHandler(HttpHandler *handler);

private:
static	int32					_NetworkReceiverEntry(void *data);
		status_t				_NetworkReceiver();

		BNetEndpoint *			fListener;

		thread_id				fReceiverThread;
		bool					fStopThread;

		BLocker					fLocker;
		BObjectList<HttpHandler>	fHandlers;
		BObjectList<HttpWorker>	fWorkers;
//		BNetEndpoint *			fEndpoint;
};

#endif // WEB_SERVER_H
