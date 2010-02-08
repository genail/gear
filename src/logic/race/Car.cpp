/*
 * Copyright (c) 2009-2010, Piotr Korzuszek, Paweł Rybarczyk
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

#include <ClanLib/core.h>

#include "Car.h"

#include "common.h"
#include "common/workarounds.h"
#include "gfx/Stage.h"
#include "gfx/DebugLayer.h"
#include "logic/race/level/Level.h"
#include "logic/race/level/Bound.h"
#include "math/Float.h"

namespace Race {

/* Car width in pixels */
const int CAR_WIDTH = 18;

/* Car height in pixels */
const int CAR_HEIGHT = 24;

class CarImpl
{
	IMPL_SIGNAL_1(inputChanged, const Car&)

	public:

		/** Base object */
		const Car *const m_base;

		/** This will help to keep 1/60 iteration speed */
		unsigned m_timeFromLastUpdate;

		/** Iteration counter */
		unsigned m_iterCnt;

		// current vehicle state

		/** Central position on map */
		CL_Pointf m_position;

		/** CW rotation from positive X axis */
		CL_Angle m_rotation;

		/** Current speed in map pixels per frame */
		float m_speed;

		/** Damage factor. 0.0 - 1.0 from new to damaged */
		float m_damage;

		/** If currently chocking */
		bool m_chocking;


		// input state

		/** Acceleration switch */
		bool m_inputAccel;

		/** Brake switch */
		bool m_inputBrake;

		/** Current turn. -1 is maximum left, 0 is center and 1 is maximum right */
		float m_inputTurn;

		/** Locked state. If true then car shoudn't move. */
		bool m_inputLocked;

		/** True if input changed from last time */
		bool m_inputChanged;


		// physics

		/** Car movement rotation */
		CL_Angle m_phyMoveRot;

		/** Car movement vector (created from movement rotation) */
		CL_Vec2f m_phyMoveVec;

		/** Speed delta (for isDrifting()) */
		float m_phySpeedDelta;

		/** Wheels turn. -1.0 is max left, 1.0 is max right */
		float m_phyWheelsTurn;

		/** Body outline for collision check */
		CL_CollisionOutline m_phyCollisionOutline;


		CarImpl(const Car *p_base) :
			m_base(p_base),
			m_timeFromLastUpdate(0),
			m_iterCnt(0),
			m_position(300.0f, 300.0f),
			m_rotation(0, cl_degrees),
			m_speed(0.0f),
			m_damage(0.0f),
			m_chocking(false),
			m_inputAccel(false),
			m_inputBrake(false),
			m_inputTurn(0.0f),
			m_inputLocked(false),
			m_inputChanged(false),
			m_phySpeedDelta(0.0f),
			m_phyWheelsTurn(0.0f)
		{ /* empty */ }


		void update1_60();

		// helpers

		void alignRotation(CL_Angle &p_what, const CL_Angle &p_to, float p_stepRad);

		float limit(float p_value, float p_from, float p_to) const;

		/** Checks if car should choke at the moment */
		bool isChoking();

		CL_Angle vecToAngle(const CL_Vec2f &p_vec);
};

METH_SIGNAL_1(Car, inputChanged, const Car&)

Car::Car() :
	m_impl(new CarImpl(this))
{
	// build car contour for collision check
	CL_Contour contour;

	const int halfWidth = CAR_WIDTH / 2;
	const int halfHeight = CAR_HEIGHT / 2;
	contour.get_points().push_back(CL_Pointf(-halfWidth, halfHeight));
	contour.get_points().push_back(CL_Pointf(halfWidth, halfHeight));
	contour.get_points().push_back(CL_Pointf(halfWidth, -halfHeight));
	contour.get_points().push_back(CL_Pointf(-halfWidth, -halfHeight));

	m_impl->m_phyCollisionOutline.get_contours().push_back(contour);

	m_impl->m_phyCollisionOutline.set_inside_test(true);

	m_impl->m_phyCollisionOutline.calculate_radius();
	m_impl->m_phyCollisionOutline.calculate_smallest_enclosing_discs();
}

Car::~Car()
{
	// empty
}

void Car::update(unsigned p_timeElapsed)
{
	m_impl->m_timeFromLastUpdate += p_timeElapsed;

	static const unsigned breakPoint = 1000 / 60;

	while (m_impl->m_timeFromLastUpdate >= breakPoint) {
		m_impl->update1_60();
		m_impl->m_timeFromLastUpdate -= breakPoint;
	}
}

void CarImpl::alignRotation(CL_Angle &p_what, const CL_Angle &p_to, float p_stepRad)
{
	// works only on normalized values
	CL_Angle normWhat(p_what);
	CL_Angle normTo(p_to);

	Workarounds::clAngleNormalize(&normWhat);
	Workarounds::clAngleNormalize(&normTo);

	const CL_Angle diffAngle = normWhat - normTo;

	float diffRad = diffAngle.to_radians();

	// if difference is higher than 180, then rotate in shorten way
	if (diffRad > CL_PI) {
		diffRad -= CL_PI * 2;
	} else if (diffRad < -CL_PI) {
		diffRad += CL_PI * 2;
	}

	const float diffRadAbs = fabs(diffRad);

	if (diffRadAbs > 0.01f) {
		if (diffRadAbs > p_stepRad) {

			const CL_Angle stepAngle(p_stepRad, cl_radians);

			if (diffRad > 0.0f) {
				p_what -= stepAngle;
			} else {
				p_what += stepAngle;
			}
		} else {
			p_what = p_to;
		}
	}
}

void CarImpl::update1_60() {
	
	static const float BRAKE_POWER = 0.1f;
	static const float ACCEL_POWER = 0.014f;
	static const float SPEED_LIMIT = 15.0f;
	static const float WHEEL_TURN_SPEED = 1.0f / 10.0f;
	static const float TURN_POWER  = (2 * CL_PI / 360.0f) * 2.5f;
	static const float MOV_ALIGN_POWER = TURN_POWER / 2.0f;
	static const float ROT_ALIGN_POWER = TURN_POWER * 0.7f;
	static const float AIR_RESITANCE = 0.003f; // per one speed unit
	static const float DRIFT_SPEED_REDUCTION_RATE = 0.1f;

	// speed limit under what physics angle reduction will be more aggressive
	static const float LOWER_SPEED_ANGLE_REDUCTION = 6.0f;
	// speed limit under what angle difference will be lower than normal
	static const float LOWER_SPEED_ROTATION_REDUCTION = 6.0f;
	// speed limit under what turn power will decrease
	static const float LOWER_SPEED_TURN_REDUCTION = 2.0f;

	// don't do anything if car is locked
	if (m_inputLocked) {
		return;
	}
	
	const float prevSpeed = m_speed; // for m_phySpeedDelta

	// apply inputs to speed
	if (m_inputBrake) {
		m_speed -= BRAKE_POWER;
	} else if (m_inputAccel) {
		// only if not choking
		if (!isChoking()) {
			m_chocking = false;
			m_speed += (SPEED_LIMIT - m_speed) * ACCEL_POWER;
		} else {
			m_chocking = true;
		}
	}
	
	// rotate steering wheels
	const float diff = m_inputTurn - m_phyWheelsTurn;

	if (fabs(diff) > WHEEL_TURN_SPEED) {
		m_phyWheelsTurn += diff > 0.0f ? WHEEL_TURN_SPEED : -WHEEL_TURN_SPEED;
	} else {
		m_phyWheelsTurn = m_inputTurn;
	}

	const float absSpeed = fabs(m_speed);

	// calculate rotations
	if (m_phyWheelsTurn != 0.0f) {

		// rotate corpse and later physics movement
		CL_Angle turnAngle(TURN_POWER * m_phyWheelsTurn, cl_radians);

		if (absSpeed <= LOWER_SPEED_TURN_REDUCTION) {
			// reduce turn if car speed is too low
			turnAngle.set_radians(turnAngle.to_radians() * (absSpeed / LOWER_SPEED_TURN_REDUCTION));
		}

		if (m_speed > 0.0f) {
			m_rotation += turnAngle;
		} else {
			m_rotation -= turnAngle;
		}

		// rotate corpse and physics movement
		if (absSpeed > LOWER_SPEED_ROTATION_REDUCTION) {
			alignRotation(m_phyMoveRot, m_rotation, MOV_ALIGN_POWER);
		} else {
			alignRotation(m_phyMoveRot, m_rotation, MOV_ALIGN_POWER * ((LOWER_SPEED_ROTATION_REDUCTION + 1.0f) - absSpeed));
		}

	} else {

		// align corpse back to physics movement
		alignRotation(m_rotation, m_phyMoveRot, MOV_ALIGN_POWER);

		// makes car stop rotating if speed is too low
		if (absSpeed > LOWER_SPEED_ANGLE_REDUCTION) {
			alignRotation(m_phyMoveRot, m_rotation, ROT_ALIGN_POWER);
		} else {
			alignRotation(m_phyMoveRot, m_rotation, ROT_ALIGN_POWER * ((LOWER_SPEED_ANGLE_REDUCTION + 1.0f) - absSpeed));
		}

		// normalize rotations only when equal
		if (m_rotation == m_phyMoveRot) {
			Workarounds::clAngleNormalize(&m_rotation);
			Workarounds::clAngleNormalize(&m_phyMoveRot);
		}

	}

	Workarounds::clAngleNormalize(&m_phyMoveRot);
	Workarounds::clAngleNormalize(&m_rotation);


	// reduce speed
	const CL_Angle diffAngle = m_rotation - m_phyMoveRot;
	float diffDegAbs = fabs(diffAngle.to_degrees());

	if (diffDegAbs > 0.1f) {

		CL_Angle diffAngleNorm = diffAngle;
		Workarounds::clAngleNormalize180(&diffAngleNorm);

		// 0.0 when going straight, 1.0 when 90 deg, > 1.0 when more than 90 deg
		const float angleRate = fabs(1.0f - (fabs(diffAngleNorm.to_degrees()) - 90.0f) / 90.0f);
		const float speedReduction = -DRIFT_SPEED_REDUCTION_RATE * angleRate;

		if (absSpeed > speedReduction) {
			m_speed += m_speed > 0.0f ? speedReduction : -speedReduction;
		} else {
			m_speed = 0.0f;
		}
	}

	// car cannot travel too quickly
	m_speed -= m_speed * AIR_RESITANCE;

	// calculate next move vector
	const float m_rotationRad = m_phyMoveRot.to_radians();

	m_phyMoveVec.x = cos(m_rotationRad);
	m_phyMoveVec.y = sin(m_rotationRad);

	m_phyMoveVec.normalize();
	m_phyMoveVec *= m_speed;

	// apply movement (invert y)
	m_position.x += m_phyMoveVec.x;
	m_position.y += m_phyMoveVec.y;

	// set speed delta
	m_phySpeedDelta = m_speed - prevSpeed;

	// increase the iteration counter
	m_iterCnt++;

	// act to input changes
	if (m_inputChanged) {
		INVOKE_1(inputChanged, *m_base);
		m_inputChanged = false;
	}

#if defined(CLIENT)
#if !defined(NDEBUG)
	// print debug information
	DebugLayer *dbgl = Gfx::Stage::getDebugLayer();

	dbgl->putMessage("speed", cl_format("%1", m_speed));
//	if (!m_level) {
//		const float resistance = m_level->getResistance(m_position.x, m_position.y);
//		dbgl->putMessage("resist", cl_format("%1", resistance));
//	}
#endif // NDEBUG
#endif // CLIENT
}

CL_CollisionOutline Car::getCollisionOutline() const
{
	CL_CollisionOutline outline(m_impl->m_phyCollisionOutline);

	// transform the outline
	CL_Angle angle(90, cl_degrees);
	angle += m_impl->m_rotation;

	outline.set_angle(angle);
	outline.set_translation(m_impl->m_position.x, m_impl->m_position.y);

	return outline;
}

void Car::applyCollision(const CL_LineSegment2f &p_seg)
{
	static const float DAMAGE_MULT = 0.2f;

	const float side = -p_seg.point_right_of_line(m_impl->m_position);

	const CL_Vec2f segVec = p_seg.q - p_seg.p;

	// need front normal (crash side)
	CL_Vec2f fnormal(segVec.y, -segVec.x); // right side normal
	fnormal.normalize();

	if (side < 0) {
		fnormal *= -1;
	}

	// move away
	m_impl->m_position += (fnormal * fabs(m_impl->m_speed));

	// calculate collision angle to estaminate speed reduction
	CL_Angle angleDiff(m_impl->m_phyMoveRot - m_impl->vecToAngle(fnormal));
	Workarounds::clAngleNormalize180(&angleDiff);

	const float colAngleDeg = fabs(angleDiff.to_degrees()) - 90.0f;
	const float reduction = fabs(1.0f - fabs(colAngleDeg - 90.0f) / 90.0f);

	// calculate and apply damage
	const float damage = m_impl->m_speed * reduction * DAMAGE_MULT;
	m_impl->m_damage =
			Math::Float::reduce(m_impl->m_damage + damage, 0.0f, 1.0f);

	cl_log_event(LOG_DEBUG, "damage: %1, total: %2", damage, m_impl->m_damage);

	// reduce speed
	m_impl->m_speed -= m_impl->m_speed * reduction;

	// bounce movement vector and angle away

	// get mirror point
	if (m_impl->m_phyMoveVec.length() > 0.01f) {
		m_impl->m_phyMoveVec.normalize();

		const float lengthProj = m_impl->m_phyMoveVec.length() * cos(segVec.angle(m_impl->m_phyMoveVec).to_radians());
		const CL_Vec2f mirrorPoint(segVec * (lengthProj / segVec.length()));

		// invert move vector by mirror point
		const CL_Vec2f mirrorVec = (m_impl->m_phyMoveVec - mirrorPoint) * -1;
		m_impl->m_phyMoveVec = mirrorPoint + mirrorVec;

		// update physics angle
		m_impl->m_phyMoveRot = m_impl->vecToAngle(m_impl->m_phyMoveVec);

	}

}

void Car::serialize(CL_NetGameEvent *p_event) const
{
	// save iteration counter
	p_event->add_argument(m_impl->m_iterCnt);

	// save inputs
	p_event->add_argument(CL_NetGameEventValue(m_impl->m_inputAccel));
	p_event->add_argument(CL_NetGameEventValue(m_impl->m_inputBrake));
	p_event->add_argument(m_impl->m_inputTurn);
	p_event->add_argument(CL_NetGameEventValue(m_impl->m_inputLocked));

	// corpse state
	p_event->add_argument(m_impl->m_position.x);
	p_event->add_argument(m_impl->m_position.y);
	p_event->add_argument(m_impl->m_rotation.to_radians());
	p_event->add_argument(m_impl->m_speed);

	// physics parameters
	p_event->add_argument(m_impl->m_phyMoveRot.to_radians());
	p_event->add_argument(m_impl->m_phyMoveVec.x);
	p_event->add_argument(m_impl->m_phyMoveVec.y);
	p_event->add_argument(m_impl->m_phySpeedDelta);
	p_event->add_argument(m_impl->m_phyWheelsTurn);

	p_event->add_argument(m_impl->m_damage);
}

void Car::deserialize(const CL_NetGameEvent &p_event)
{
	static const unsigned ARGUMENT_COUNT = 15;

	if (p_event.get_argument_count() != ARGUMENT_COUNT) {
		// when serialize data is invalid don't do anything
		cl_log_event(
				LOG_DEBUG,
				"invalid serialize data count: %1",
				p_event.get_argument_count()
		);

		return;
	}

	int idx = 0;

	// load iteration counter
	m_impl->m_iterCnt = p_event.get_argument(idx++);

	// saved inputs
	m_impl->m_inputAccel = p_event.get_argument(idx++);
	m_impl->m_inputBrake = p_event.get_argument(idx++);
	m_impl->m_inputTurn = p_event.get_argument(idx++);
	m_impl->m_inputLocked = p_event.get_argument(idx++);

	// corpse state
	m_impl->m_position.x = p_event.get_argument(idx++);
	m_impl->m_position.y = p_event.get_argument(idx++);
	m_impl->m_rotation.set_radians(p_event.get_argument(idx++));
	m_impl->m_speed = p_event.get_argument(idx++);

	// physics parameters
	m_impl->m_phyMoveRot.set_radians(p_event.get_argument(idx++));
	m_impl->m_phyMoveVec.x = p_event.get_argument(idx++);
	m_impl->m_phyMoveVec.y = p_event.get_argument(idx++);
	m_impl->m_phySpeedDelta = p_event.get_argument(idx++);
	m_impl->m_phyWheelsTurn = p_event.get_argument(idx++);

	m_impl->m_damage = p_event.get_argument(idx++);
}

bool CarImpl::isChoking()
{
	if (m_damage < 1.0f) {
		return false;
	}

	// c is iter count. 60 per second
	const unsigned c = m_iterCnt;

	// lets assume that 64 iterations is one second
	// so then...

	if (1 << 8 & c && 1 << 5 & c) { // 0.5s every 4s
		return true;
	}

	if (1 << 7 & c && 1 << 4 & c) { // 0.25s every 2s
		return true;
	}

	if (1 << 6 & c && 1 << 3 & c) { // 0.1s every 1s
		return true;
	}

	return false;
}

bool Car::isChoking() const
{
	return m_impl->m_chocking;
}

bool Car::isDrifting() const {
	static const float DRIFT_LIMIT = 6.0f;
	static const float ACCEL_LIMIT = 0.05f;

	if (fabs((m_impl->m_rotation - m_impl->m_phyMoveRot).to_degrees()) >= DRIFT_LIMIT) {
		return true;
	}

	if (
			(m_impl->m_inputAccel || m_impl->m_inputBrake)
			&& fabs(m_impl->m_phySpeedDelta) >= ACCEL_LIMIT)
	{
		return true;
	}

	return false;
}

bool Car::isLocked() const
{
	return m_impl->m_inputLocked;
}

const CL_Pointf& Car::getPosition() const
{
	return m_impl->m_position;
}

float Car::getSpeed() const
{
	return m_impl->m_speed;
}

float Car::getSpeedKMS() const
{
//	// m_speed - pixels per iteration
//	// 4 - length of avarage car in meters
//	// 25 - length of avarage car in pixels
//	// 60 - one second
//	// 60 * 60 - one minute
//	// 60 * 60 * 60 - one hour
//	// / 1000 - to kmh
//	return m_speed * (4 / 25.0f) * 60 * 60 * 60 / 1000;

	const float m_f =  m_impl->m_speed / 15.0; // m / frame
	const float m_s = m_f * 60.0f; // m / s
	const float m_h = m_s * 3600.0; // m / h
	return m_h / 1000.0; // km / h
}

void Car::setAcceleration(bool p_value)
{
	if (m_impl->m_inputAccel != p_value) {
		m_impl->m_inputChanged = true;
	}

	m_impl->m_inputAccel = p_value;
}

void Car::setBrake(bool p_value)
{
	if (m_impl->m_inputBrake != p_value) {
		m_impl->m_inputChanged = true;
	}

	m_impl->m_inputBrake = p_value;
}

void Car::setTurn(float p_value)
{
	if (fabs(p_value - m_impl->m_inputTurn) > 0.1f) {
		m_impl->m_inputChanged = true;
	}

	m_impl->m_inputTurn = m_impl->limit(p_value, -1.0f, 1.0f);
}

void Car::setPosition(const CL_Pointf &p_position)
{
	m_impl->m_position = p_position;
}

void Car::setAngle(const CL_Angle &p_angle)
{
	m_impl->m_rotation = p_angle;
	m_impl->m_phyMoveRot = m_impl->m_rotation;
}

float CarImpl::limit(float p_value, float p_from, float p_to) const
{
	if (p_value < p_from) {
		return p_from;
	}

	if (p_value > p_to) {
		return p_to;
	}

	return p_value;
}

void Car::setLocked(bool p_locked)
{
	m_impl->m_inputLocked = p_locked;

	// stop the car
	if (p_locked) {
		m_impl->m_phyMoveVec.x = m_impl->m_phyMoveVec.y = 0.0f;
	}
}

CL_Angle CarImpl::vecToAngle(const CL_Vec2f &p_vec)
{
	const static CL_Vec2f ANGLE_ZERO(1.0f, 0.0f);
	CL_Angle angle = p_vec.angle(ANGLE_ZERO);

	if (p_vec.y < 0) {
		angle.set_radians(-angle.to_radians());
	}

	return angle;

}

void Car::setSpeed(float p_speed)
{
	m_impl->m_speed = p_speed;
}

void Car::setMovement(const CL_Vec2f &p_movement)
{
	m_impl->m_phyMoveVec = p_movement;
}

const CL_Angle &Car::getCorpseAngle() const
{
	return m_impl->m_rotation;
}

bool Car::isAcceleration() const
{
	return m_impl->m_inputAccel;
}

bool Car::isBrake() const
{
	return m_impl->m_inputBrake;
}

float Car::getTurn() const
{
	return m_impl->m_inputTurn;
}

void Car::reset()
{
	m_impl->m_damage = 0.0f;
}

void Car::clone(const Car &p_car)
{
	m_impl->m_position = p_car.m_impl->m_position;
	m_impl->m_rotation = p_car.m_impl->m_rotation;
	m_impl->m_speed = p_car.m_impl->m_speed;
	m_impl->m_inputAccel = p_car.m_impl->m_inputAccel;
	m_impl->m_inputBrake = p_car.m_impl->m_inputBrake;
	m_impl->m_inputTurn = p_car.m_impl->m_inputTurn;
	m_impl->m_inputLocked = p_car.m_impl->m_inputLocked;
	m_impl->m_phyMoveRot = p_car.m_impl->m_phyMoveRot;
	m_impl->m_phyMoveVec = p_car.m_impl->m_phyMoveVec;
	m_impl->m_phySpeedDelta = p_car.m_impl->m_phySpeedDelta;
	m_impl->m_phyWheelsTurn = p_car.m_impl->m_phyWheelsTurn;
}

bool Car::operator==(const Car &p_other) const
{
	if (&p_other == this) {
		return true;
	}

	bool r = true;
	r &= m_impl->m_position == p_other.m_impl->m_position;
	r &= m_impl->m_rotation == p_other.m_impl->m_rotation;
	r &= m_impl->m_speed == p_other.m_impl->m_speed;
	r &= m_impl->m_inputAccel == p_other.m_impl->m_inputAccel;
	r &= m_impl->m_inputBrake == p_other.m_impl->m_inputBrake;
	r &= m_impl->m_inputTurn == p_other.m_impl->m_inputTurn;
	r &= m_impl->m_inputLocked == p_other.m_impl->m_inputLocked;
	r &= m_impl->m_phyMoveRot == p_other.m_impl->m_phyMoveRot;
	r &= m_impl->m_phyMoveVec == p_other.m_impl->m_phyMoveVec;
	r &= m_impl->m_phySpeedDelta == p_other.m_impl->m_phySpeedDelta;
	r &= m_impl->m_phyWheelsTurn == p_other.m_impl->m_phyWheelsTurn;

	return r;
}

bool Car::operator!=(const Car &p_other) const
{
	return !(*this == p_other);
}

} // namespace
