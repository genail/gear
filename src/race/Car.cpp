/*
 * Car.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include "race/Car.h"

#include <ClanLib/core.h>
#include <ClanLib/display.h>

Car::Car(float p_x, float p_y, float p_rotation) :
	m_sprite(),
	m_position(p_x, p_y),
	m_rotation(0, cl_degrees),
	m_turn(0.0f),
	m_acceleration(false),
	m_brake(false),
	m_speed(0.0f)
{

}

Car::~Car() {
}

void Car::draw(CL_GraphicContext &p_gc) {

	if (m_sprite.is_null()) {
		m_sprite = CL_Sprite(p_gc, "race/car", Stage::getResourceManager());
	}

	p_gc.push_modelview();

	p_gc.mult_translate(m_position.x, m_position.y);
	p_gc.mult_rotate(m_rotation, 0, 0, 1);

	m_sprite.draw(p_gc, 0, 0);

	p_gc.pop_modelview();
}

void Car::update(unsigned int elapsedTime) {

	static const float MAX_TURN_SPEED = 0.8f;
	static const float MIN_TURN_SPEED = 0.2f;
		
	static const float BRAKE_POWER = 500.0f;

	static const float ACCEL_SPEED = 300.0f;
	static const float MAX_SPEED = 400.0f;

	static const float AIR_RESIST = 0.2f;
	
	static const float TURN_RATIO = ( MAX_TURN_SPEED - MIN_TURN_SPEED ) / ( ( 1.0f - MAX_SPEED ) * ( 1.0f - MAX_SPEED ) );

	const float delta = elapsedTime / 1000.0f;

	// turning speed
	
	float turn_speed = TURN_RATIO * ( m_speed - MAX_SPEED ) * ( m_speed - MAX_SPEED ) + MIN_TURN_SPEED;
	
	// turning

	if (m_turn != 0.0f) {
		const CL_Angle deltaAngle(m_turn * delta * turn_speed * 360.0f, cl_degrees);
		m_rotation += deltaAngle;
	}


	// acceleration

	if (m_acceleration) {
		const float speedChange = ACCEL_SPEED * delta;

		m_speed += speedChange;

		if (m_speed > MAX_SPEED) {
			m_speed = MAX_SPEED;
		}

	}

	// brake
	if (m_brake) {
		const float speedChange = BRAKE_POWER * delta;

		m_speed -= speedChange;

		if (m_speed < -MAX_SPEED / 2) {
			m_speed = -MAX_SPEED / 2;
		}
	}

	// air resistance
	m_speed -= delta * AIR_RESIST * m_speed;
	
	// rotation

	const float rad = m_rotation.to_radians();

	m_moveVector.x = cos(rad);
	m_moveVector.y = sin(rad);

	m_moveVector.normalize();
	m_moveVector *= m_speed;

	// movement

	CL_Vec2f currentMoveVector = m_moveVector * delta;
	m_position.x += currentMoveVector.x;
	m_position.y += currentMoveVector.y;

#ifndef NDEBUG
			Stage::getDebugLayer()->putMessage(CL_String8("speed"),  CL_StringHelp::float_to_local8(m_speed));
#endif
}

int Car::prepareStatusEvent(CL_NetGameEvent &p_event) {
	CL_NetGameEventValue posX(m_position.x);
	CL_NetGameEventValue posY(m_position.y);
	CL_NetGameEventValue rotation(m_rotation.to_degrees());
	CL_NetGameEventValue turn(m_turn);
	CL_NetGameEventValue accel(m_acceleration);
	CL_NetGameEventValue brake(m_brake);
	CL_NetGameEventValue moveX(m_moveVector.x);
	CL_NetGameEventValue moveY(m_moveVector.y);
	CL_NetGameEventValue speed(m_speed);

	int c = 0;

	p_event.add_argument(posX);     ++c;
	p_event.add_argument(posY);     ++c;
	p_event.add_argument(rotation); ++c;
	p_event.add_argument(turn);     ++c;
	p_event.add_argument(accel);    ++c;
	p_event.add_argument(brake);    ++c;
	p_event.add_argument(moveX);    ++c;
	p_event.add_argument(moveY);    ++c;
	p_event.add_argument(speed);    ++c;

	return c;
}

int Car::applyStatusEvent(const CL_NetGameEvent &p_event, int p_beginIndex) {
	int i = p_beginIndex;

	m_position.x =   (float) p_event.get_argument(i++);
	m_position.y =   (float) p_event.get_argument(i++);
	m_rotation =     CL_Angle((float) p_event.get_argument(i++), cl_degrees);
	m_turn =         (float) p_event.get_argument(i++);
	m_acceleration =  (bool) p_event.get_argument(i++);
	m_brake =         (bool) p_event.get_argument(i++);
	m_moveVector.x = (float) p_event.get_argument(i++);
	m_moveVector.y = (float) p_event.get_argument(i++);
	m_speed =        (float) p_event.get_argument(i++);

	return i;
}
