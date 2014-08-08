/*
 * Copyright 2009, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Lotz <mmlr@mlotz.ch>
 *		François Revol <revol@free.fr>
 *		Alexander von Gluck IV <kallisti5@unixzen.com>
 */
#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H


#include <OS.h>
#include <String.h>
#include <SupportDefs.h>
#include <NetBuffer.h>


class BNetEndpoint;
class StreamingRingBuffer;
class BDataIO;
class HttpWorker;


class HttpHandler {
public:
								HttpHandler(const char* path,
									BDataIO* data=NULL);
								HttpHandler(const char* path,
									BNetBuffer* target);
		virtual					~HttpHandler();

		void					SetMultipart(bool multipart=true) {
									fMultipart = true; }
		bool					IsMultipart() { return fMultipart; }
		void					SetType(const char* type) { fType = type; }


		BString &				Name() { return fPath; }
//		BNetEndpoint *			Endpoint() { return fEndpoint; }
virtual status_t				HandleRequest(HttpWorker* worker,
									BString &path);

static	int						_CallbackCompare(const BString* key,
									const HttpHandler* info);
static	int						_CallbackCompare(const HttpHandler* a,
									const HttpHandler* b);

private:
		friend class HttpWorker;
		BString					fPath;

		bool					fMultipart;
		BString					fType;	// MIME
		BDataIO*				fData;
		BNetBuffer*				fTarget;
};

#endif // HTTP_HANDLER_H
