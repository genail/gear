/*
 * Copyright (c) 2009, Piotr Korzuszek, Paweł Rybarczyk
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

#include "common.h"
#include "common/Game.h"
#include "common/Player.h"
#include "common/Properties.h"
#include "gfx/Stage.h"
#include "gfx/DebugLayer.h"
#include "logic/race/Car.h"
#include "logic/race/level/Level.h"
#include "logic/race/level/Checkpoint.h"
#include "logic/race/level/Bound.h"
#include "network/packets/CarState.h"

namespace Race {

/* Car width in pixels */
const int CAR_WIDTH = 18;

/* Car height in pixels */
const int CAR_HEIGHT = 24;

Car::Car(const Player *p_owner) :
	m_owner(p_owner),
	m_level(NULL),
	m_lap(0),
	m_timeFromLastUpdate(0),
	m_position(300.0f, 300.0f),
	m_rotation(0, cl_degrees),
	m_speed(0.0f),
	m_inputAccel(false),
	m_inputBrake(false),
	m_inputHandbrake(false),
	m_inputTurn(0.0f),
	m_inputLocked(false),
	m_inputChanged(false),
	m_phySpeedDelta(0.0f),
	m_phyWheelsTurn(0.0f),
	m_greatestCheckpointId(0),
	m_currentCheckpoint(NULL)
{
#ifndef SERVER
	// build car contour for collision check
	CL_Contour contour;

	const int halfWidth = CAR_WIDTH / 2;
	const int halfHeight = CAR_HEIGHT / 2;
	contour.get_points().push_back(CL_Pointf(-halfWidth, halfHeight));
	contour.get_points().push_back(CL_Pointf(halfWidth, halfHeight));
	contour.get_points().push_back(CL_Pointf(halfWidth, -halfHeight));
	contour.get_points().push_back(CL_Pointf(-halfWidth, -halfHeight));

	m_phyCollisionOutline.get_contours().push_back(contour);

	m_phyCollisionOutline.set_inside_test(true);

	m_phyCollisionOutline.calculate_radius();
	m_phyCollisionOutline.calculate_smallest_enclosing_discs();
#endif // !SERVER
}

Car::~Car() {
}

void Car::update(unsigned p_timeElapsed)
{
	m_timeFromLastUpdate += p_timeElapsed;

	static const unsigned breakPoint = 1000 / 60;

	while (m_timeFromLastUpdate >= breakPoint) {
		update1_60();
		m_timeFromLastUpdate -= breakPoint;
	}
}

void Car::alignRotation(CL_Angle &p_what, const CL_Angle &p_to, float p_stepRad)
{
	// works only on normalized values
	CL_Angle normWhat(p_what);
	CL_Angle normTo(p_to);

	normalizeAngle(normWhat);
	normalizeAngle(normTo);

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

void Car::update1_60() {
	
	static const float BRAKE_POWER = 0.1f;
	static const float ACCEL_POWER = 0.14f;
	static const float WHEEL_TURN_SPEED = 1.0f / 10.0f;
	static const float TURN_POWER  = (2 * CL_PI / 360.0f) * 2.5f;
	static const float MOV_ALIGN_POWER = TURN_POWER / 2.0f;
	static const float ROT_ALIGN_POWER = TURN_POWER * 0.7f;
	static const float AIR_RESITANCE = 0.01f; // per one speed unit
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
		m_speed += ACCEL_POWER;
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

		// make backwards turning reverse
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
			normalizeAngle(m_rotation);
			normalizeAngle(m_phyMoveRot);
		}

	}

	normalizeAngle(m_phyMoveRot);
	normalizeAngle(m_rotation);


	// reduce speed
	const CL_Angle diffAngle = m_rotation - m_phyMoveRot;
	float diffDegAbs = fabs(diffAngle.to_degrees());

	if (diffDegAbs > 0.1f) {

		CL_Angle diffAngleNorm = diffAngle;
		normalizeAngle180(diffAngleNorm);

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
	

	// act to input changes
	if (m_inputChanged) {
		INVOKE_1(inputChanged, *this);
		m_inputChanged = false;
	}

#if defined(CLIENT)
#if !defined(NDEBUG)
	// print debug information
	DebugLayer *dbgl = Gfx::Stage::getDebugLayer();

	dbgl->putMessage("speed", cl_format("%1", m_speed));
	if (!m_level) {
		const float resistance = m_level->getResistance(m_position.x, m_position.y);
		dbgl->putMessage("resist", cl_format("%1", resistance));
	}
#endif // NDEBUG
#endif // CLIENT
}

#if defined(CLIENT)
CL_CollisionOutline Car::calculateCurrentCollisionOutline() const
{
	CL_CollisionOutline outline(m_phyCollisionOutline);

	// transform the outline
	CL_Angle angle(90, cl_degrees);
	angle += m_rotation;

	outline.set_angle(angle);
	outline.set_translation(m_position.x, m_position.y);

	return outline;
}

void Car::performBoundCollision(const Bound &p_bound)
{
	const CL_LineSegment2f &seg = p_bound.getSegment();
	const float side = -seg.point_right_of_line(m_position);

	const CL_Vec2f segVec = seg.q - seg.p;

	// need front normal (crash side)
	CL_Vec2f fnormal(segVec.y, -segVec.x); // right side normal
	fnormal.normalize();

	if (side < 0) {
		fnormal *= -1;
	}

	// move away
	m_position += (fnormal * fabs(m_speed));

	// calculate collision angle to estaminate speed reduction
	CL_Angle angleDiff(m_phyMoveRot - vecToAngle(fnormal));
	normalizeAngle180(angleDiff);

	const float colAngleDeg = fabs(angleDiff.to_degrees()) - 90.0f;
	const float reduction = fabs(1.0f - fabs(colAngleDeg - 90.0f) / 90.0f);

	m_speed -= m_speed * reduction;

	// bounce movement vector and angle away

	// get mirror point
	if (m_phyMoveVec.length() > 0.01f) {
		m_phyMoveVec.normalize();

		const float lengthProj = m_phyMoveVec.length() * cos(segVec.angle(m_phyMoveVec).to_radians());
		const CL_Vec2f mirrorPoint(segVec * (lengthProj / segVec.length()));

		// invert move vector by mirror point
		const CL_Vec2f mirrorVec = (m_phyMoveVec - mirrorPoint) * -1;
		m_phyMoveVec = mirrorPoint + mirrorVec;

		// update physics angle
		m_phyMoveRot = vecToAngle(m_phyMoveVec);

	}

}
#endif // CLIENT

Net::CarState Car::prepareCarState() const
{
	Net::CarState state;

	state.setPosition(m_position);
	state.setRotation(m_rotation);
	state.setMovement(m_phyMoveVec);
	state.setSpeed(m_speed);
	state.setTurn(m_inputTurn);

	if (m_inputAccel && !m_inputBrake) {
		state.setAcceleration(1.0f);
	} else if (!m_inputAccel && m_inputBrake) {
		state.setAcceleration(-1.0f);
	} else {
		state.setAcceleration(0.0f);
	}

	return state;
}

void Car::applyCarState(const Net::CarState &p_carState)
{
	m_position = p_carState.getPosition();
	m_rotation = p_carState.getRotation();
	m_phyMoveVec = p_carState.getMovement();
	m_speed = p_carState.getSpeed();
	m_inputTurn = p_carState.getTurn();
	m_inputAccel = p_carState.getAcceleration() > 0.0f;
	m_inputBrake = p_carState.getAcceleration() < 0.0f;
}

void Car::setStartPosition(int p_startPosition) {
	if (m_level != NULL) {
		m_position = m_level->getStartPosition(p_startPosition);
	} else {
		cl_log_event("warning", "Car not on Level.");
		m_position = CL_Pointf(300, 300);
	}

	// stop the car!
	m_rotation = CL_Angle::from_degrees(-90);
	m_inputTurn = 0;
	m_inputAccel = false;
	m_inputBrake = false;
	m_phyMoveVec = CL_Vec2f();
	m_speed = 0.0f;
	m_lap = 1;

	// send the status change to other players
	INVOKE_1(inputChanged, *this);
}

bool Car::isDrifting() const {
	static const float DRIFT_LIMIT = 6.0f;
	static const float ACCEL_LIMIT = 0.05f;

	if (fabs((m_rotation - m_phyMoveRot).to_degrees()) >= DRIFT_LIMIT) {
		return true;
	}

	if (fabs(m_phySpeedDelta) >= ACCEL_LIMIT) {
		return true;
	}

	return false;
}

void Car::updateCurrentCheckpoint(const Checkpoint *p_checkpoint)
{
	if (p_checkpoint != NULL) {
		// check if lap is reached
		if (
				p_checkpoint->getProgress() == 0.0f &&
				m_currentCheckpoint->getProgress() == 1.0f &&
				m_greatestCheckpointId == m_currentCheckpoint->getId()
		) {
			++m_lap;
			cl_log_event("race", "Going for lap %1", m_lap);
			m_greatestCheckpointId = 0;
		}

		const int id = p_checkpoint->getId();

		if (id == m_greatestCheckpointId + 1) {
			++m_greatestCheckpointId;
		}
	} else {
		m_greatestCheckpointId = 0;
	}

	m_currentCheckpoint = p_checkpoint;
}

bool Car::isLocked() const
{
	return m_inputLocked;
}

const Checkpoint *Car::getCurrentCheckpoint() const
{
	return m_currentCheckpoint;
}

int Car::getLap() const
{
	return m_lap;
}

const Player *Car::getOwner() const
{
	return m_owner;
}

const CL_Pointf& Car::getPosition() const
{
	return m_position;
}

float Car::getRotation() const
{
	return m_rotation.to_degrees();
}

float Car::getRotationRad() const
{
	return m_rotation.to_radians();
}

float Car::getSpeed() const
{
	return m_speed;
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

	const float m_f =  m_speed / 15.0; // m / frame
	const float m_s = m_f * 60.0f; // m / s
	const float m_h = m_s * 3600.0; // m / h
	return m_h / 1000.0; // km / h
}

void Car::setAcceleration(bool p_value)
{
	if (m_inputAccel != p_value) {
		m_inputChanged = true;
	}

	m_inputAccel = p_value;
}

void Car::setBrake(bool p_value)
{
	if (m_inputBrake != p_value) {
		m_inputChanged = true;
	}

	m_inputBrake = p_value;
}

void Car::setLap(int p_lap)
{
	m_lap = p_lap;
}

void Car::setTurn(float p_value)
{
	if (fabs(p_value - m_inputTurn) > 0.1f) {
		m_inputChanged = true;
	}

	m_inputTurn = limit(p_value, -1.0f, 1.0f);
}

void Car::setPosition(const CL_Pointf &p_position)
{
	m_position = p_position;
}

void Car::setRotation(float p_rotation)
{
	m_rotation.set_degrees(p_rotation);
}

void Car::setHandbrake(bool p_handbrake)
{
	if (m_inputHandbrake != p_handbrake) {
		m_inputChanged = true;
	}

	m_inputHandbrake = p_handbrake;
}

float Car::limit(float p_value, float p_from, float p_to) const
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
	m_inputLocked = p_locked;
}

CL_Angle Car::vecToAngle(const CL_Vec2f &p_vec)
{
	const static CL_Vec2f ANGLE_ZERO(1.0f, 0.0f);
	CL_Angle angle = p_vec.angle(ANGLE_ZERO);

	if (p_vec.y < 0) {
		angle.set_radians(-angle.to_radians());
	}

	return angle;

}

void Car::normalizeAngle(CL_Angle &p_angle)
{
#if CL_CURRENT_VERSION <= CL_VERSION(2,1,1)
	p_angle.normalize();
	if (p_angle.to_radians() < 0) {
		p_angle += CL_Angle(2 * CL_PI, cl_radians);
	}
#else
	p_angle.normalize();
#endif
}

void Car::normalizeAngle180(CL_Angle &p_angle)
{
	normalizeAngle(p_angle);
	p_angle.normalize_180();
}

} // namespace
