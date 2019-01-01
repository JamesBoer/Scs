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

#pragma once
#ifndef SCS_SOCKET_H____
#define SCS_SOCKET_H____

namespace Scs
{
	class Socket;

	typedef std::shared_ptr<Socket> SocketPtr;

	class Socket
	{
	public:
		Socket(AddressPtr address);
		Socket(AddressPtr address, SOCKET sckt);
		~Socket();

		// Accept a socket connection from the client, creating a new socket
		SocketPtr Accept();

		// Bind a socket (server connection listener)
		bool Bind(addrinfo * addr);

		// Connect client socket to server
		bool Connect();

		// Check to see if the socket has any error conditions
		bool IsInvalid() const;

		// Is socket readable? (indicates a connection request on server listener sockets)
		bool IsReadable() const;

		// Is socket writable? (indicates a connection on client sockets)
		bool IsWritable() const;

		// Begin listening for client connections on a socket in listener mode
		bool Listen();

		// Receive data
		size_t Receive(void * data, size_t bytes, uint32_t flags);

		// Send data
		bool Send(void * data, size_t bytes, uint32_t flags, size_t * bytesSent);

		// Set non-blocking mode on or off
		void SetNonBlocking(bool nonBlocking);

		// Set the Nagle algorithm
		void SetNagle(bool nagle);

	private:
		SOCKET m_socket;
		AddressPtr m_address;
		//addrinfo * m_currAddress;
	};

	SocketPtr CreateSocket(AddressPtr address);
	SocketPtr CreateSocket(AddressPtr address, SOCKET sckt);

} // namespace Scs

#endif // SCS_SOCKET_H____
