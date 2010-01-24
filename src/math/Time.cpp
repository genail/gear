/*
 * Copyright (c) 2009, Piotr Korzuszek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Time.h"

namespace Math
{

class TimeImpl
{
	public:

		unsigned m_milis;

		mutable int m_centi;

		mutable int m_sec;

		mutable int m_min;

		TimeImpl(unsigned p_milis) :
			m_milis(p_milis),
			m_centi(-1),
			m_sec(-1),
			m_min(-1)
		{ /* empty */ }


		void calc() const;
};

Time::Time() :
	m_impl(new TimeImpl(0))
{
	// empty
}

Time::Time(unsigned p_milis) :
	m_impl(new TimeImpl(p_milis))
{
	// empty
}

Time::~Time()
{
	// empty
}

unsigned Time::get() const
{
	return m_impl->m_milis;
}

int Time::getCenti() const
{
	if (m_impl->m_centi == -1) {
		m_impl->calc();
	}

	return m_impl->m_centi;
}

int Time::getSeconds() const
{
	if (m_impl->m_sec == -1) {
		m_impl->calc();
	}

	return m_impl->m_sec;
}

int Time::getMinutes() const
{
	if (m_impl->m_min == -1) {
		m_impl->calc();
	}

	return m_impl->m_min;
}

void TimeImpl::calc() const
{
	static const unsigned MILLISECOND = 1;
	static const unsigned CENTISECOND = MILLISECOND * 10;
	static const unsigned SECOND = CENTISECOND * 100;
	static const unsigned MINUTE = SECOND * 60;

	unsigned m = m_milis;

	m_min = m / MINUTE;
	m -= m_min * MINUTE;

	m_sec = m / SECOND;
	m -= m_sec * SECOND;

	m_centi = m / CENTISECOND;
}

}
