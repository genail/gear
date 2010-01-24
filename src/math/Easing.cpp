/*
 * Copyright (c) 2009, Piotr Korzuszek, Interactive Pulp, LLC
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

#include "Easing.h"

#include <math.h>

#include <ClanLib/core.h>

namespace Math {

const Easing Easing::NONE(Easing::T_IN, Easing::F_LINEAR);

const Easing Easing::REGULAR_IN(Easing::T_IN, Easing::F_QUADRADIC);
const Easing Easing::REGULAR_OUT(Easing::T_OUT, Easing::F_QUADRADIC);
const Easing Easing::REGULAR_IN_OUT(Easing::T_IN_OUT, Easing::F_QUADRADIC);

Easing::Easing(Type p_type, Function p_function) :
	m_type(p_type),
	m_function(p_function)
{
	// empty
}

float Easing::ease(float p_from, float p_to, float p_progress) const
{
	if (p_progress <= 0.0f) {
		return p_from;
	}

	if (p_progress >= 1.0f) {
		return p_to;
	}

	float easedT;

	switch (m_type) {
		case T_IN:
			easedT = ease(p_progress);
			break;

		case T_OUT:
			easedT = 1.0f - ease(1.0f - p_progress);
			break;

		case T_IN_OUT:
			if (p_progress < 0.5f) {
				easedT = ease(2 * p_progress) / 2.0f;
			} else {
				easedT = 1.0f - ease(2.0f - 2.0f * p_progress) / 2.0f;
			}

			break;

		default:
			G_ASSERT(0 && "unknown Type");
	}

	return p_from + ((p_to - p_from) * easedT);
}

float Easing::ease(float p_t) const
{
	float t2, t3;
	float scale, wave;

	switch (m_function) {
		case F_LINEAR:
			return p_t;

		case F_QUADRADIC:
			return p_t * p_t;

		case F_CUBIC:
			return p_t * p_t * p_t;

		case F_QUARTIC:
			t2 = p_t * p_t;
			return t2 * t2;

		case F_QUINTIC:
			t2 = p_t * p_t;
			return t2 * t2 * p_t;

		case F_BACK:
			t2 = p_t * p_t;
			t3 = t2 * p_t;
			return t3 + t2 - p_t;

		case F_ELASTIC:
			t2 = p_t * p_t;
			t3 = t2 * p_t;

			scale = t2 * (2.0f * t3 + t2 - 4.0f * p_t + 2.0f);
			wave = static_cast<float>(-sin(p_t * 3.5f * CL_PI));

			return scale * wave;

		default:
			G_ASSERT(0 && "unknown Function");
	}
}

}
