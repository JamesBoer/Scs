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
#ifndef SCS_RECEIVE_QUEUE_H____
#define SCS_RECEIVE_QUEUE_H____

namespace Scs
{

	// Message receive queue
	class ReceiveQueue
	{
	public:
		BufferPtr Pop();
		void Push(const void * data, size_t bytes);

	private:

		std::queue<BufferPtr, std::deque<BufferPtr, Allocator<BufferPtr>>> m_queue;
		std::mutex m_mutex;
		size_t m_messageSize = 0;
		BufferPtr m_receiveBuffer;
	};

}; // namespace Scs

#endif // SCS_RECEIVE_QUEUE_H____