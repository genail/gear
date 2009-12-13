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

#pragma once

#include <libintl.h>
#include <boost/foreach.hpp>

// i18n
#define _(text) gettext(text)

// foreach macro
#define foreach BOOST_FOREACH

// default connect port
#define DEFAULT_PORT 2500

// signal and slots
#define SIGNAL_0(name) \
	public: \
		CL_Signal_v0 &sig_##name() { return m_sig_##name; } \
	private: \
		CL_Signal_v0 m_sig_##name;

#define SIGNAL_1(T1, name) \
	public: \
		CL_Signal_v1<T1> &sig_##name() { return m_sig_##name; } \
	private: \
		CL_Signal_v1<T1> m_sig_##name;


#define INVOKE_0(name) \
		m_sig_##name.invoke()

#define INVOKE_1(name, arg1) \
		m_sig_##name.invoke(arg1)

// callbacks
#define CALLBACK_0(name) \
	public: \
		CL_Callback_v0 &func_##name() { return m_func_##name; } \
	private: \
		CL_Callback_v0 m_func_##name;

#define C_INVOKE_0(name) \
	if (!m_func_##name.is_null()) \
		m_func_##name.invoke()

// deprecated macro
#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

// log levels
#define LOG_DEBUG "debug"
#define LOG_ERROR "error"
#define LOG_RACE  "race"
#define LOG_EVENT "event"
