/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
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

#include <assert.h>
#include <libintl.h>
#include <boost/foreach.hpp>

// i18n
#define _(text) gettext(text)

// foreach macro
#define foreach BOOST_FOREACH

// default connect port
#define DEFAULT_PORT 2500

// levels directory
#define LEVELS_DIR "levels"

// signal and slots
#define SIGNAL_0(name) \
	public: \
		CL_Signal_v0 &sig_##name() { return m_sig_##name; } \
	private: \
		CL_Signal_v0 m_sig_##name;

#define SIGNAL_1(name, T1) \
	public: \
		CL_Signal_v1<T1> &sig_##name() { return m_sig_##name; } \
	private: \
		CL_Signal_v1<T1> m_sig_##name;

#define SIGNAL_2(name, T1, T2) \
	public: \
		CL_Signal_v2<T1, T2> &sig_##name() { return m_sig_##name; } \
	private: \
		CL_Signal_v2<T1, T2> m_sig_##name;

#define SIGNAL_3(name, T1, T2, T3) \
	public: \
		CL_Signal_v3<T1, T2, T3> &sig_##name() { return m_sig_##name; } \
	private: \
		CL_Signal_v3<T1, T2, T3> m_sig_##name;

// for interface and implementation

#define DEF_SIGNAL_0(name) \
	public: \
		CL_Signal_v0 &sig_##name();

#define IMPL_SIGNAL_0(name) \
	public: \
		CL_Signal_v0 m_sig_##name;

#define METH_SIGNAL_0(clazz, name) \
	CL_Signal_v0 &clazz::sig_##name() { \
		return m_impl->m_sig_##name; \
	}

#define DEF_SIGNAL_1(name, T1) \
	public: \
		CL_Signal_v1<T1> &sig_##name();

#define IMPL_SIGNAL_1(name, T1) \
	public: \
		CL_Signal_v1<T1> m_sig_##name;

#define METH_SIGNAL_1(clazz, name, T1) \
	CL_Signal_v1<T1> &clazz::sig_##name() { \
		return m_impl->m_sig_##name; \
	}

#define DEF_SIGNAL_2(name, T1, T2) \
	public: \
		CL_Signal_v2<T1, T2> &sig_##name();

#define IMPL_SIGNAL_2(name, T1, T2) \
	public: \
		CL_Signal_v2<T1, T2> m_sig_##name;

#define METH_SIGNAL_2(clazz, name, T1, T2) \
	CL_Signal_v2<T1, T2> &clazz::sig_##name() { \
		return m_impl->m_sig_##name; \
	}

#define INVOKE_0(name) \
		m_sig_##name.invoke()

#define INVOKE_1(name, arg1) \
		m_sig_##name.invoke(arg1)

#define INVOKE_2(name, arg1, arg2) \
		m_sig_##name.invoke(arg1, arg2)

#define INVOKE_3(name, arg1, arg2, arg3) \
		m_sig_##name.invoke(arg1, arg2, arg3)


// new signals implementation

#define SIG_CPP(clazz, name) \
		clazz::_TSig_##name &clazz::sig_##name() const { \
			return m_impl->m_sig_##name; \
		}

#define SIG_IMPL(clazz, name) \
		public: \
			mutable clazz::_TSig_##name m_sig_##name;

// 0 arg
#define SIG_H_0(name) \
	public: \
		typedef CL_Signal_v0 _TSig_##name; \
		CL_Signal_v0 &sig_##name() const;

// 1 arg
#define SIG_H_1(name, T1) \
	public: \
		typedef CL_Signal_v1<T1> _TSig_##name; \
		CL_Signal_v1<T1> &sig_##name() const;

// 2 args
#define SIG_H_2(name, T1, T2) \
	public: \
		typedef CL_Signal_v2<T1, T2> _TSig_##name; \
		CL_Signal_v2<T1, T2> &sig_##name() const;

// 3 args
#define SIG_H_3(name, T1, T2, T3) \
	public: \
		typedef CL_Signal_v3<T1, T2, T3> _TSig_##name; \
		CL_Signal_v3<T1, T2, T3> &sig_##name() const;


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


// assert macro

#if defined(G_ASSERT)
#error G_ASSERT already defined
#endif // G_ASSERT

#define G_ASSERT(expr) \
	assert(expr)

#if defined(G_ASSERT_PERROR)
#error G_ASSERT_PERROR already defined
#endif // G_ASSERT_PERROR

#define G_ASSERT_PERROR(expr) \
	assert_perror(expr)

// end of line string
#if defined(WINDOWS)
#define END_OF_LINE "\r\n"
#elif defined(UNIX)
#define END_OF_LINE "\n"
#elif defined(APPLE)
#define END_OF_LINE "\r"
#else
#error Unsupported platform
#endif

// log levels
#define LOG_DEBUG "debug"
#define LOG_EVENT "event"
#define LOG_ERROR "error"
#define LOG_INFO  "info"
#define LOG_WARN  "warning"
#define LOG_RACE  "race"
