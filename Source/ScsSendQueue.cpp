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

#include "ScsInternal.h"

using namespace Scs;


bool SendQueue::Empty() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_queue.empty();
}

bool SendQueue::Send(SocketPtr socket)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	assert(!m_queue.empty());
	auto buffer = m_queue.front();
	size_t bytesSent = 0;
	if (!socket->Send(buffer->data(), buffer->size(), 0, &bytesSent))
		return false;
	m_bytesSent += bytesSent;
	if (m_bytesSent == buffer->size())
	{
		m_queue.pop_front();
		m_bytesSent = 0;
	}
	return true;
}

void SendQueue::Push(const void * data, size_t bytes)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	// Track data and bytes remaining to queue
	uint8_t * ptrData = (uint8_t *)data;
	size_t bytesRemaining = bytes + sizeof(MessageHeader);
	bool firstWrite = true;
	while (bytesRemaining)
	{
		size_t bytesToWrite = std::min(bytesRemaining, SEND_BUFFER_SIZE);
		BufferPtr buffer = CreateBuffer();
		if (firstWrite)
		{
			// Single message sizes over 4GB aren't supported, which is a
			// ridiculous size for a single TCP/IP message anyhow.
			assert(bytes < 0xFFFFFFFF);
			MessageHeader header;
			header.size = static_cast<uint32_t>(bytes);	
			buffer->insert(buffer->end(), reinterpret_cast<uint8_t *>(&header), reinterpret_cast<uint8_t *>(&header) + sizeof(MessageHeader));
			bytesRemaining -= sizeof(MessageHeader);
			bytesToWrite -= sizeof(MessageHeader);
			firstWrite = false;
		}
		buffer->insert(buffer->end(), ptrData, ptrData + bytesToWrite);
		m_queue.push_back(buffer);
		ptrData += bytesToWrite;
		bytesRemaining -= bytesToWrite;
	}
	assert(bytesRemaining == 0);
}


