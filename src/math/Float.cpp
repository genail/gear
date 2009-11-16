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

#include "Float.h"

#include <assert.h>

#include "common.h"

namespace Math {

Float::Float() :
	m_value(0.0f),
	m_timeFromStart(0),
	m_currentAnimation(NULL)
{
}

Float::~Float()
{
}

void Float::animate(float p_startValue, float p_endValue, unsigned p_duration, Easing p_easing, unsigned p_delay)
{
	const unsigned startTime = m_timeFromStart + p_delay;

	Animation *animation = new Animation();
	animation->m_from = p_startValue;
	animation->m_to = p_endValue;
	animation->m_startTime = startTime;
	animation->m_endTime = startTime + p_duration;
	animation->m_easing = p_easing;

	m_animationMap[startTime] = animation;
}

void Float::update(unsigned p_timeElapsed)
{
	m_timeFromStart += p_timeElapsed;

	// look for next animation
	for (TAnimationMap::iterator itor = m_animationMap.begin(); itor != m_animationMap.end();) {
		if (itor->first <= m_timeFromStart) {
			setCurrentAnimation(itor->second);
			m_animationMap.erase(itor);
		} else {
			++itor;
		}
	}

	if (m_currentAnimation != NULL) {

		if (m_currentAnimation->m_endTime <= m_timeFromStart) {

			// check current animation validity
			delete m_currentAnimation;
			m_currentAnimation = NULL;

		} else {

			// make progress

			const float progress =
					(m_timeFromStart - m_currentAnimation->m_startTime) /
					(float) (m_currentAnimation->m_endTime - m_currentAnimation->m_startTime);

			// this should be value between 0.0 and 1.0
			assert(progress >= 0.0f && progress <= 1.0f);

			m_value = easingCalculate(
					m_currentAnimation->m_from, m_currentAnimation->m_to,
					m_currentAnimation->m_easing,
					progress
			);
		}
	}
}

void Float::setCurrentAnimation(Animation *p_animation)
{
	if (m_currentAnimation != NULL) {
		delete m_currentAnimation;
	}

	m_currentAnimation = p_animation;
}

} // namespace
