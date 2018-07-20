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


BufferPtr ReceiveQueue::Pop()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_queue.empty())
		return nullptr;
	BufferPtr buffer = m_queue.front();
	m_queue.pop();
	return buffer;
}

void ReceiveQueue::Push(const void * data, size_t bytes)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	const uint8_t * dataBytes = static_cast<const uint8_t *>(data);
	while (bytes)
	{
		// Check to see if this is a new message, and if so, prepare the buffer and write out the header structure
		if (m_messageSize == 0)
		{
			assert(bytes >= sizeof(MessageHeader));
			MessageHeader header;
			memcpy(&header, data, sizeof(MessageHeader));
			if (header.magic != MAGIC_HEADER_VAL)
			{
				LogWriteLine("Transmission error.  Magic header mismatch.");
				return;
			}
			m_messageSize = header.size;
			if (!m_receiveBuffer)
				m_receiveBuffer = CreateBuffer();
			m_receiveBuffer->clear();
			bytes -= sizeof(MessageHeader);
			dataBytes += sizeof(MessageHeader);
		}

		// Don't write out more than the message size yet
		auto bytesToWrite = bytes;
		auto maxBytes = m_messageSize - m_receiveBuffer->size();
		if (bytesToWrite > maxBytes)
			bytesToWrite = maxBytes;

		// Write data into the receive buffer and update our counts
		m_receiveBuffer->insert(m_receiveBuffer->end(), dataBytes, dataBytes + bytesToWrite);
		assert(bytesToWrite <= bytes);
		bytes -= bytesToWrite;
		dataBytes += bytesToWrite;

		if (m_receiveBuffer->size() >= m_messageSize)
		{
			if (m_receiveBuffer->size() > m_messageSize)
			{
				LogWriteLine("Transmission error.  Message exceeds expected size.");
				break;
			}
			else
			{
				m_queue.push(m_receiveBuffer);
			}
			m_receiveBuffer = nullptr;
			m_messageSize = 0;
		}
	}
}


