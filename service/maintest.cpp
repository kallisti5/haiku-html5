
#include <NetEndpoint.h>

#include <stdio.h>
#include "StreamingRingBuffer.h"
#include "HttpServer.h"
#include "HttpHandler.h"


int
main()
{
	BNetEndpoint* receiveEndpoint = new(std::nothrow) BNetEndpoint();

	// TODO: Replace StreamingRingBuffer with BNetBuffer or BDynamicBuffer
	StreamingRingBuffer* sendBuffer = new(std::nothrow) StreamingRingBuffer(16 * 1024);
	BHttpServer* webServer = new(std::nothrow) BHttpServer(receiveEndpoint);
	HttpHandler* handler = new(std::nothrow) HttpHandler("output", sendBuffer);

	return 0;
}
