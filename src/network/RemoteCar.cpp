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

const int PASS_TIME = 250;

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

		Race::Car m_oldCar;

		Race::Car m_newCar;

		// Smooth pass float
		// 0.0 is old car, 1.0 is new car
		Math::Float m_passFloat;
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
	m_impl->m_passFloat.update(p_elapsedMS);

	const float newCarRatio = m_impl->m_passFloat.get();

	Race::Car &oldCar = m_impl->m_oldCar;
	Race::Car &newCar = m_impl->m_newCar;

	if (newCarRatio != 1.0f) {

		{
			// position
			const CL_Pointf &oldPos = oldCar.getPosition();
			const CL_Pointf &newPos = newCar.getPosition();
			const CL_Vec2f oldToNew = (newPos - oldPos) * newCarRatio;

			setPosition(oldPos + oldToNew);
		}

		{
			// rotation
			const CL_Angle &oldRot = oldCar.getCorpseAngle();
			const CL_Angle &newRot = newCar.getCorpseAngle();
			const CL_Angle oldToNew = (newRot - oldRot) * newCarRatio;

			setAngle(oldRot + oldToNew);
		}
	} else {
		setPosition(newCar.getPosition());
		setAngle(newCar.getCorpseAngle());
	}
}

void RemoteCar::deserialize(const CL_NetGameEvent &p_data)
{
	Race::Car &o = m_impl->m_oldCar;
	Race::Car &n = m_impl->m_newCar;

	// deserialize to new car
	n.deserialize(p_data);

	// set non sensitive data to old one
	o.setAcceleration(n.isAcceleration());
	o.setBrake(n.isBrake());
	o.setTurn(n.getTurn());

	// start passing
	m_impl->m_passFloat.animate(0.0f, 1.0f, PASS_TIME);
}

}
