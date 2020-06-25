/*
The MIT License (MIT)

Copyright (c) 2018 James Boer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <chrono>
#include <thread>
#include <atomic>
#include "catch.hpp"
#include "../../Source/Scs.h"

using namespace Scs;


TEST_CASE("Test Connection", "[Connection]")
{
	// Initialize client-server library
	InitParams params;
	params.logFn = [] (const char *) {};
	Initialize(params);

	SECTION("Test simple client-server connection")
	{	
		// Create a server
		ServerParams serverParams;
		serverParams.port = "5656";
		auto server = CreateServer(serverParams);

		// Handler for when server connects to client
		bool serverConnected = false;
		server->OnConnect([&serverConnected](int32_t clientId)
		{
			serverConnected = true;
		});

		// Start listening for client connections
		server->StartListening();

		// Create a client
		ClientParams clientParams;
		clientParams.address = "127.0.0.1";
		clientParams.port = "5656";
		auto client = CreateClient(clientParams);

		// Handler for when client connects
		bool clientConnected = false;
		client->OnConnect([&clientConnected]
		{
			clientConnected = true;
		});

		// Attempt to make a connection
		client->Connect();

		// Wait a short time for connection
		auto timeout = std::chrono::system_clock::now() + std::chrono::seconds(1);
		while (!clientConnected || !serverConnected)
		{
			if (std::chrono::system_clock::now() > timeout)
				break;
		}

		REQUIRE(client->IsConnected());
		REQUIRE(clientConnected);
		REQUIRE(serverConnected);
	}

	SECTION("Test max num client-server connection")
	{
		// Create a server
		ServerParams serverParams;
		serverParams.port = "5656";
		serverParams.maxConnections = 100;
		auto server = CreateServer(serverParams);

		// Handler for when server connects to client
		std::atomic<uint32_t> serverConnections = 0;
		server->OnConnect([&] (int32_t) { ++serverConnections; });

		// Start listening for client connections
		server->StartListening();

		// Start up maxConnections number of clients, and ensure they all connect to the server
		std::vector<std::thread> threads;
		std::atomic<uint32_t> clientConnections = 0;
		for (uint32_t i = 0; i < serverParams.maxConnections; ++i)
		{
			threads.emplace_back(std::thread([&] () {
				// Create a client
				ClientParams clientParams;
				clientParams.address = "127.0.0.1";
				clientParams.port = "5656";
				clientParams.timeoutSeconds = 10;
				auto client = CreateClient(clientParams);

				// Handler for when client connects
				client->OnConnect([&] { ++clientConnections; });

				// Attempt to make a connection
				client->Connect();

				// Wait until client has connected or signalled an error
				while (!client->IsConnected() && !client->HasError())
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}));
		}

		// Wait for server to register max connections
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(static_cast<long long>(serverParams.timeoutSeconds * 1000.0f));
		while (serverConnections < serverParams.maxConnections)
		{
			if (server->HasError())
				return;
			if (std::chrono::system_clock::now() > timeout)
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		// Join all threads
		for (auto & t : threads)
			t.join();

		// Both client and server connections should equal max connections
		REQUIRE(clientConnections == serverParams.maxConnections);
		REQUIRE(serverConnections == serverParams.maxConnections);
	}

	// Shut down client-server library
	ShutDown();

}