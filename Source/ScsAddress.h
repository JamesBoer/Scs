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
#ifndef SCS_ADDRESS_H____
#define SCS_ADDRESS_H____

namespace Scs
{

	class Address
	{
	public:
		Address(const String & port, const String & address);
		Address(const String & port, bool passive);
		~Address();
        
        void Log() const;

		addrinfo * GetHead() const { return m_address; }
		addrinfo * GetCurrent() const { return m_current; }
		bool IsValid() { return m_current ? true : false; }
		bool Next();
		void Reset() { m_current = m_address; }
	private:
		addrinfo * m_address;
		addrinfo * m_current;
	};

	using AddressPtr = std::shared_ptr<Address>;

	AddressPtr CreateAddress(const String & port, const String & address);
	AddressPtr CreateAddress(const String & port, bool passive);

}; // namespace Scs

#endif // SCS_ADDRESS_H____