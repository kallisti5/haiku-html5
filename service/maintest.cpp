
#include <NetBuffer.h>
#include <NetEndpoint.h>

#include <stdio.h>
#include "HttpServer.h"
#include "HttpHandler.h"


int
main()
{
	BNetEndpoint* receiveEndpoint = new(std::nothrow) BNetEndpoint();
	receiveEndpoint->Bind(8080);

	// TODO: Replace StreamingRingBuffer with BNetBuffer or BDynamicBuffer
	BNetBuffer sendBuffer(16 * 1024);
	//StreamingRingBuffer* sendBuffer = new(std::nothrow) StreamingRingBuffer(16 * 1024);
	BHttpServer* httpServer = new(std::nothrow) BHttpServer(receiveEndpoint);
	HttpHandler* handler = new(std::nothrow) HttpHandler("output", &sendBuffer);

	httpServer->AddHandler(handler);
	
	while(1);

	return 0;
}
