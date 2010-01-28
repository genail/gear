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

#include "RemoteCar.h"

#include "math/Float.h"

namespace Net
{

const int PASS_TIME = 500;

class RemoteCarImpl
{
	public:

		// To preserve lags, there are two car instances.
		// The old one is car physics situated when car position didn't change.
		// The new one is car physics forced by remote player.
		//
		// When new packet is received only the new car is positioned at new
		// place and moving from one position to another is made step by step
		// to make things smooth.

		Race::Car m_phantomCar;

		// Smooth pass float
		// 0.0 is old car, 1.0 is new car
		Math::Float m_passFloat;

		mutable CL_Pointf m_pos;

		mutable CL_Angle m_rot;
};

RemoteCar::RemoteCar() :
	m_impl(new RemoteCarImpl())
{
	// empty
}

RemoteCar::~RemoteCar()
{
	// empty
}

void RemoteCar::update(unsigned int p_elapsedMS)
{
	Car::update(p_elapsedMS);

	m_impl->m_passFloat.update(p_elapsedMS);
	const float newCarRatio = m_impl->m_passFloat.get();

	if (fabs(newCarRatio - 1.0f) > 0.01) {
		m_impl->m_phantomCar.update(p_elapsedMS);
	}
}

void RemoteCar::deserialize(const CL_NetGameEvent &p_data)
{
	CL_NetGameEvent ev("");
	serialize(&ev);
	m_impl->m_phantomCar.deserialize(ev);

	Car::deserialize(p_data);

	// start passing
	m_impl->m_passFloat.animate(0.0f, 1.0f, PASS_TIME);
}

const CL_Pointf& RemoteCar::getPosition() const
{
	const CL_Pointf &thisPos = Car::getPosition();

	const float newCarRatio = m_impl->m_passFloat.get();

	if (fabs(newCarRatio - 1.0f) > 0.01) {
		const CL_Pointf &phanPos = m_impl->m_phantomCar.getPosition();

		const CL_Vec2f delta = (thisPos - phanPos) * newCarRatio;
		m_impl->m_pos = phanPos + delta;

		return m_impl->m_pos;
	}

	return thisPos;
}

const CL_Angle &RemoteCar::getCorpseAngle() const
{
	const CL_Angle &thisAngle = Car::getCorpseAngle();

//	const float newCarRatio = m_impl->m_passFloat.get();
//
//	if (fabs(newCarRatio - 1.0f) > 0.01) {
//		const CL_Angle &phanAngle = m_impl->m_phantomCar.getCorpseAngle();
//
//		const CL_Angle delta = (thisAngle - phanAngle) * newCarRatio;
//		m_impl->m_rot = phanAngle + delta;
//
//		return m_impl->m_rot;
//	}

	return thisAngle;
}

}
