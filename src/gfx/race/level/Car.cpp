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

#include "Car.h"

#include "clanlib/display/2d.h"

#include <assert.h>

#include "gfx/Stage.h"
#include "logic/race/Car.h"
#include "math/Float.h"

namespace Gfx {

const int WHEEL_COUNT = 4;
const int WHEEL_BL = 0; // back left
const int WHEEL_BR = 1; // back right
const int WHEEL_FL = 2; // front left
const int WHEEL_FR = 3; // front right

class CarImpl
{
	public:

		const Race::Car *m_raceCar;

		CL_Pointf m_position;

		CL_Angle m_rotation;


		// body sprites
		// low will be drawn before the wheels
		// hight after the wheels
		CL_Sprite m_bodyLow, m_bodyHigh;

		// wheel sprites
		CL_Sprite m_wheels[WHEEL_COUNT];

		// wheel turn factor (for animation)
		float m_wheelTurn;


		CarImpl(const Race::Car *p_car) :
			m_raceCar(p_car),
			m_wheelTurn(0.0f)
		{ /* empty */ }
};

Car::Car(const Race::Car *p_car) :
	m_impl(new CarImpl(p_car))
{
	// empty
}

Car::~Car()
{
	// empty
}

void Car::load(CL_GraphicContext &p_gc)
{
	Drawable::load(p_gc);

	CL_ResourceManager *res = Stage::getResourceManager();

	m_impl->m_bodyLow = CL_Sprite(
		p_gc,
		"race/cars/buggy/body_low",
		res
	);

	m_impl->m_bodyHigh = CL_Sprite(
		p_gc,
		"race/cars/buggy/body_high",
		res
	);

	m_impl->m_wheels[WHEEL_BL] = CL_Sprite(
		p_gc,
		"race/cars/buggy/wheels/back_left",
		res
	);

	m_impl->m_wheels[WHEEL_BR] = CL_Sprite(
		p_gc,
		"race/cars/buggy/wheels/back_right",
		res
	);

	m_impl->m_wheels[WHEEL_FL] = CL_Sprite(
		p_gc,
		"race/cars/buggy/wheels/front_left",
		res
	);

	m_impl->m_wheels[WHEEL_FR] = CL_Sprite(
		p_gc,
		"race/cars/buggy/wheels/front_right",
		res
	);
}

void Car::draw(CL_GraphicContext &p_gc)
{
	// max wheel turn value in radians
	static const float WHEEL_TURN_MAX = CL_PI / 8;

	// set from wheels turn
	const CL_Angle wheelTurnAngle(
			-m_impl->m_raceCar->getPhyWheelTurn() * WHEEL_TURN_MAX,
			cl_radians
	);
	m_impl->m_wheels[WHEEL_FL].set_base_angle(wheelTurnAngle);
	m_impl->m_wheels[WHEEL_FR].set_base_angle(wheelTurnAngle);


	// start drawing
	p_gc.push_modelview();

	const CL_Pointf &pos = m_impl->m_raceCar->getPosition();
	const CL_Angle &angle = m_impl->m_raceCar->getCorpseAngle();

	// put car in right position
	p_gc.mult_translate(pos.x, pos.y);
	p_gc.mult_rotate(angle + CL_Angle(-CL_PI/2, cl_radians), 0, 0, 1);

	// low body, wheels, then high body
	m_impl->m_bodyLow.draw(p_gc, 0, 0);

	for (int i = 0; i < WHEEL_COUNT; ++i) {
		m_impl->m_wheels[i].draw(p_gc, 0, 0);
	}

//	m_impl->m_bodyHigh.draw(p_gc, 0, 0);

	p_gc.pop_modelview();
}

void Car::update(unsigned p_timeElapsed)
{
	// what distance car need to make to make one animation iteration
	static const float WHEEL_TURN_DISTANCE = 2.0f;

	static const float FRAME_0 = WHEEL_TURN_DISTANCE / 3;
	static const float FRAME_1 = WHEEL_TURN_DISTANCE / 3 * 2;

	// get the turn value
	float turn = m_impl->m_wheelTurn;

	turn += (p_timeElapsed / 1000.0f) * m_impl->m_raceCar->getSpeed();
	turn = Math::Float::clamp(turn, 0.0f, WHEEL_TURN_DISTANCE);

	// set the turn back
	m_impl->m_wheelTurn = turn;

	int frame;

	if (turn <= FRAME_0) {
		frame = 2;
	} else if (turn <= FRAME_1) {
		frame = 1;
	} else {
		frame = 0;
	}

	for (int i = 0; i < WHEEL_COUNT; ++i) {
		m_impl->m_wheels[i].set_frame(frame);
	}
}

}
