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

namespace Net
{

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

}

void RemoteCar::applyCarState(const Net::CarState &p_state)
{
	Race::Car &o = m_impl->m_oldCar;
	Race::Car &n = m_impl->m_newCar;

	n.setPosition(p_state.getPosition());
	n.setRotation(p_state.getRotation());
	n.setSpeed(p_state.getSpeed());
	n.setMovement(p_state.getMovement());

	n.setAcceleration(p_state.getAcceleration() > 0.0f);
	o.setAcceleration(p_state.getAcceleration() > 0.0f);
	n.setBrake(p_state.getAcceleration() < 0.0f);
	o.setBrake(p_state.getAcceleration() < 0.0f);
	n.setTurn(p_state.getTurn());
	o.setTurn(p_state.getTurn());

}

}
