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

#include "clanlib/core/system.h"

#include "Easing.h"

//#include "boost/utility.hpp"

namespace Math {

class Easing;

class FloatImpl;

class Float {

	public:

		static float clamp(float p_val, float p_min, float p_max);

		static bool cmp(float p_a, float p_b, float p_precision);

		static float reduce(float p_val, float p_min, float p_max);


		Float();

		Float(float p_val);

		virtual ~Float();


		float get() const;


		void animate(
				float p_startValue, float p_endValue,
				unsigned p_duration,
				const Easing &p_easing = Easing::NONE,
				unsigned p_delay = 0
		);

		void set(float p_value);

		/**
		 * Changes the object state by next <code>p_timeElapsed</code> time.
		 * <p>
		 * In other words it pushes the calculations forward by specified
		 * amount of time.
		 *
		 * @param p_timeElapsed Time in milliseconds.
		 */
		void update(unsigned p_timeElapsed);

	private:

		CL_SharedPtr<FloatImpl> m_impl;

};

} // namespace
