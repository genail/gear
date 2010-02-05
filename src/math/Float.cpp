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

#include "Float.h"

#include <map>
#include <math.h>

#include "common.h"
#include "Easing.h"

namespace Math {

bool Float::cmp(float p_a, float p_b, float p_precision)
{
	return fabs(p_a - p_b) <= p_precision;
}

float Float::reduce(float p_val, float p_min, float p_max)
{
	if (p_val > p_max) {
		return p_max;
	} else if (p_val < p_min) {
		return p_min;
	}

	return p_val;
}

class Animation {

	public:

		float m_from, m_to;

		unsigned m_stime, m_etime;

		unsigned m_dur;

		const Easing *m_easing;
};

class FloatImpl
{
	public:

		typedef std::map<size_t, Animation*> TAnimationMap;


		/** Current value container */
		float m_value;

		/** Time registered from the beginning object life */
		size_t m_timeFromStart;

		/** Animations to do in time. Key is the start time. */
		TAnimationMap m_animMap;

		/** Currently running animation */
		Animation *m_currAnim;


		FloatImpl(float p_val) :
			m_value(p_val),
			m_timeFromStart(0),
			m_currAnim(NULL)
		{ /* empty */ }


		void setCurrentAnimation(Animation *p_animation);

		void update(unsigned p_timeElapsed);
};

Float::Float() :
	m_impl(new FloatImpl(0.0f))
{
	// empty
}

Float::Float(float p_val) :
	m_impl(new FloatImpl(p_val))
{
	// empty
}

Float::~Float()
{
	// empty
}

float Float::get() const
{
	return m_impl->m_value;
}

void Float::set(float p_value)
{
	m_impl->m_value = p_value;
}

void Float::animate(
		float p_startValue, float p_endValue,
		unsigned p_duration,
		const Easing &p_easing,
		unsigned p_delay
)
{
	const unsigned startTime = m_impl->m_timeFromStart + p_delay;

	Animation *animation = new Animation();
	animation->m_from = p_startValue;
	animation->m_to = p_endValue;
	animation->m_stime = startTime;
	animation->m_etime = startTime + p_duration;
	animation->m_easing = &p_easing;

	m_impl->m_animMap[startTime] = animation;

	// set start value if animation should start now
	if (p_delay == 0) {
		m_impl->m_value = p_startValue;
	}
}

void Float::update(unsigned p_timeElapsed)
{
	m_impl->update(p_timeElapsed);
}

void FloatImpl::update(unsigned p_timeElapsed)
{
	m_timeFromStart += p_timeElapsed;

	// look for the next animation
	if (!m_animMap.empty()) {
		FloatImpl::TAnimationMap::iterator itor;
		for (itor = m_animMap.begin(); itor != m_animMap.end();) {
			if (itor->first <= m_timeFromStart) {
				setCurrentAnimation(itor->second);
				m_animMap.erase(itor++);
			} else {
				++itor;
			}
		}
	}

	if (m_currAnim != NULL) {

		if (m_currAnim->m_etime <= m_timeFromStart) {
			// animation should end now
			m_value = m_currAnim->m_to;

			delete m_currAnim;
			m_currAnim = NULL;

		} else {

			// make progress
			const float progress =
					(m_timeFromStart - m_currAnim->m_stime) /
					static_cast<float> (
							m_currAnim->m_etime - m_currAnim->m_stime
					);

			// this should be value between 0.0 and 1.0
			G_ASSERT(progress >= 0.0f && progress <= 1.0f);

			m_value = m_currAnim->m_easing->ease(
					m_currAnim->m_from, m_currAnim->m_to,
					progress
			);

		}
	}
}

void FloatImpl::setCurrentAnimation(Animation *p_animation)
{
	if (m_currAnim != NULL) {
		delete m_currAnim;
	}

	m_currAnim = p_animation;
}

} // namespace
