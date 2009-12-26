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
#include "logic/race/Level.h"
#include "logic/race/Checkpoint.h"
#include "logic/race/Bound.h"
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
	m_inputChecksum(0),
	m_phySpeedDelta(0.0f),
	m_phyBoundHitTest(false),
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
	const CL_Angle diffAngle = p_what - p_to;
	const float diffRad = diffAngle.to_radians();
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
	
	static const float BRAKE_POWER = 0.05f;
	static const float ACCEL_POWER = 0.07f;
	static const float TURN_POWER  = (2 * M_PI / 360.0f) * 2.5f;
	static const float MOV_ALIGN_POWER = TURN_POWER / 2.0f;
	static const float ROT_ALIGN_POWER = TURN_POWER * 0.7f;
	static const float AIR_RESITANCE = 0.005f; // per one speed unit

	// speed limit under what physics angle reduction will be more agresive
	static const float LOWER_SPEED_ANGLE_REDUCTION = 3.0f;

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
	
	// calculate rotations
	if (m_inputTurn != 0.0f) {

		// rotate corpse and later physics movement
		m_rotation += CL_Angle(TURN_POWER * -m_inputTurn, cl_radians);
		alignRotation(m_phyMoveRot, m_rotation, MOV_ALIGN_POWER);

	} else {

		// align corpse back to physics movement
		alignRotation(m_rotation, m_phyMoveRot, MOV_ALIGN_POWER);

		// makes car stop rotating if speed is too low
		if (m_speed > LOWER_SPEED_ANGLE_REDUCTION) {
			alignRotation(m_phyMoveRot, m_rotation, ROT_ALIGN_POWER);
		} else {
			alignRotation(m_phyMoveRot, m_rotation, ROT_ALIGN_POWER * ((LOWER_SPEED_ANGLE_REDUCTION + 1.0f) - m_speed));
		}

	}

	// reduce speed
	const CL_Angle diffAngle = m_rotation - m_phyMoveRot;
	float diffDegAbs = fabs(diffAngle.to_degrees());

	if (diffDegAbs > 0.1f) {

		CL_Angle diffAngleNorm = diffAngle;
		diffAngleNorm.normalize_180();

		// 0.0 when goin straight, 1.0 when 90 deg, > 1.0 when more than 90 deg
		const float angleRate = fabs(1.0f - (fabs(diffAngleNorm.to_degrees()) - 90.0f) / 90.0f);
		const float speedReduction = -0.05f * angleRate;

		if (m_speed > speedReduction) {
			m_speed += speedReduction;
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
	m_position.y -= m_phyMoveVec.y;


	// set speed delta
	m_phySpeedDelta = m_speed - prevSpeed;

//	static const float BRAKE_POWER = 100.0f;
//
//	static const float ACCEL_SPEED = 200.0f;
//	static const float MAX_SPEED = 500.0f;
//
//	static const float AIR_RESIST = 0.2f;
//	const float GROUND_RESIST = m_level->getResistance(m_position.x, m_position.y);
//
//	static const float MAX_ANGLE = 50.0f;
//
//	static const float MAX_TENACITY = 0.2f;
//	static const float MIN_TENACITY = 0.05f;
//
//
//
//	const static float delta = (1000.f / 60.0f) / 1000.0f;
//
//	// calculate input checksum and if its different than last one, then
//	// invoke the signal
//	const int inputChecksum = calculateInputChecksum();
//
//	if (inputChecksum != m_inputChecksum) {
//		INVOKE_1(inputChanged, *this);
//		m_inputChecksum = inputChecksum;
//	}
//
//	//turning
//	const float turnAngle = MAX_ANGLE * m_inputTurn;
//
//	// acceleration speed
//	if (m_inputAccel) {
//		const float speedChange = ACCEL_SPEED * delta;
//
//		m_speed += speedChange;
//
//		if (m_speed > MAX_SPEED) {
//			m_speed = MAX_SPEED;
//		}
//	}
//
//	// brake
//	if (m_inputBrake) {
//		const float speedChange = BRAKE_POWER * delta;
//
//		m_speed -= speedChange;
//
//		if (m_speed < -MAX_SPEED / 2) {
//			m_speed = -MAX_SPEED / 2;
//		}
//	}
//
//	// Resistance
//	const float finalResist = (1.0f - GROUND_RESIST) - (AIR_RESIST * (1.0f - GROUND_RESIST));
//	cl_log_event("debug", "%1", finalResist);
//
//	// rotation
//	const float rad = m_rotation.to_radians(); // kąt autka w radianach
//
//	// Bouncing from bounds
//	if (m_phyBoundHitTest) {
//
//		// przeniesienie autka tak, by już się nie stykał z bandą
//
//		m_position.x -= m_phyMoveVec.x * 2 *delta;
//		m_position.y -= m_phyMoveVec.y * 2 *delta;
//
//		float x2, y2;
//		if (m_phyCollideBound.p.x == m_phyCollideBound.q.x) {
//			x2 = -m_phyMoveVec.x;
//			y2 = m_phyMoveVec.y;
//		}
//		else {
//			float a, xp, yp, xq, yq, x, y;
//			xp = m_phyCollideBound.p.x;
//			yp = m_phyCollideBound.p.y;
//			xq = m_phyCollideBound.q.x;
//			yq = m_phyCollideBound.q.y;
//			x = m_phyMoveVec.x;
//			y = m_phyMoveVec.y;
//
//			a = (yq - yp) / (xq - xp);
//
//			x2 = (((1 - (a * a)) / (1 + (a * a))) * x) + (((2 * a) / (1 + (a * a))) * y);
//			y2 = (((2 * a) / (1 + (a * a))) * x) + ((((a * a) - 1) / ((a * a) + 1)) * y);
//		}
//		m_phyMoveVec.x = x2;
//		m_phyMoveVec.y = y2;
//		m_phyBoundHitTest = false;
//	}
//
//	// wektor zmiany kierunku
//	CL_Vec2f changeVector;
//
//	if(turnAngle < 0.0f) { // skręt w jedną stronę
//		// przygotowanie wektora skrętu (zmiany kierunku jazdy)
// 		changeVector.x = sin(rad);
//		changeVector.y = -cos(rad);
//		changeVector.normalize();
//		changeVector *= tan(-turnAngle) * fabs(m_speed) / 7.0f; // modyfikacja siły skrętu
//	}
//	else if(turnAngle > 0.0f){ // skręt w drugą stronę, wszystko analogicznie
//		changeVector.x = -sin(rad);
//		changeVector.y = cos(rad);
//		changeVector.normalize();
//		changeVector *= tan(turnAngle) * fabs(m_speed) / 7.0f;
//	}
//
//		m_phyAccelVec.x = cos(rad);
//		m_phyAccelVec.y = sin(rad);
//
//		m_phyAccelVec.normalize();
//		m_phyAccelVec *= m_speed;
//
//		if(turnAngle != 0.0f) { // zmiana wektora przyspieszenia w wypadku kiedy auto skreca
//			m_phyAccelVec = changeVector + m_phyAccelVec;
//		}
//
//	// calculates current tenacity
//	const float max_tratio = MAX_TENACITY / MIN_TENACITY;
//
//	float tratio = (max_tratio / MAX_SPEED) * fabs(m_speed);
//	if (fabs(m_speed) == 0)
//		tratio = 1;
//
//	const float tenacity = MAX_TENACITY / tratio;
//
//	// wektor o który zostanie przesunięte autko (już z poślizgiem)
//	CL_Vec2f realVector = m_phyMoveVec + ( m_phyAccelVec * tenacity );
//	realVector.normalize();
//	realVector *= finalResist * fabs(m_speed);
//	m_phyMoveVec = realVector;
//
//	// nowa metoda oblicznia moveVectora
//
//
//
//	// update position
//	m_position.x += m_phyMoveVec.x * delta;
//	m_position.y += m_phyMoveVec.y * delta;
//
//
//
//	// update rotation of car when changing direction
//	if( m_inputTurn != 0.0f ) { // wykonywany jest skręt
//
//		// tworzymy sobie osobne wektory dla rotacji
//		CL_Vec2f rotVector; // wektor rotacji
//		CL_Vec2f changeRotVector; // wektor zmiany rotacji (w danej chwili)
//
//		rotVector.x = cos(rad);
//		rotVector.y = sin(rad);
//		rotVector.normalize();
//
//		if( m_inputTurn < 0.0f ) { // skręt w jedną stronę
//			changeRotVector.x = sin(rad);
//			changeRotVector.y = -cos(rad);
//			changeRotVector.normalize();
//			changeRotVector *= 2.4f * delta;
//		}
//		else if ( m_inputTurn > 0.0f ) { // skręt w drugą stronę
//			changeRotVector.x = -sin(rad);
//			changeRotVector.y = cos(rad);
//			changeRotVector.normalize();
//			changeRotVector *= 2.4f * delta;
//		}
//
//		rotVector += changeRotVector;
//		m_rotation.set_degrees( atan2( rotVector.y, rotVector.x ) * 180.0f / 3.14f );
//	}
	
#ifdef CLIENT
#ifndef NDEBUG
			Gfx::Stage::getDebugLayer()->putMessage(CL_String8("speed"),  CL_StringHelp::float_to_local8(m_speed));
			if (m_level != NULL) {
				Gfx::Stage::getDebugLayer()->putMessage(CL_String8("resist"), CL_StringHelp::float_to_local8(m_level->getResistance(m_position.x, m_position.y)));
			}
#endif // NDEBUG
#endif // CLIENT
}

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

int Car::calculateInputChecksum() const {
	int checksum = 0;
	checksum |= (int) m_inputAccel;
	checksum |= ((int) m_inputBrake) << 1;
	checksum += m_inputTurn * 10000.0f;

	return checksum;
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
//	m_phyAccelVec = CL_Vec2f();
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

#if defined(CLIENT)
CL_CollisionOutline Car::calculateCurrentCollisionOutline() const
{
	CL_CollisionOutline outline(m_phyCollisionOutline);

//	outline.calculate_smallest_enclosing_discs();
//	outline.set_inside_test(true);


	// transform the outline
	CL_Angle angle(90, cl_degrees);
	angle += m_rotation;

	outline.set_angle(angle);
	outline.set_translation(m_position.x, m_position.y);

	return outline;
}

void Car::performBoundCollision(const Bound &p_bound)
{
//	m_phyCollideBound = p_bound.getSegment();
//	m_phyBoundHitTest = true;


}
#endif // CLIENT

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
	// m_speed - pixels per iteration
	// 4 - length of avarage car in meters
	// 50 - length of avarage car in pixels
	// 60 - one second
	// 60 * 60 - one minute
	// 60 * 60 * 60 - one hour
	// / 1000 - to kmh
	return m_speed * (4 / 50.0f) * 60 * 60 * 60 / 1000;
}

void Car::setAcceleration(bool p_value)
{
	m_inputAccel = p_value;
}

void Car::setBrake(bool p_value)
{
	m_inputBrake = p_value;
}

void Car::setLap(int p_lap)
{
	m_lap = p_lap;
}

void Car::setTurn(float p_value)
{
	m_inputTurn = normalize(p_value);
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
	m_inputHandbrake = p_handbrake;
}

float Car::normalize(float p_value) const {
	if (p_value < -1.0f) {
		p_value = 1.0f;
	} else if (p_value > 1.0f) {
		p_value = 1.0f;
	}

	return p_value;
}

void Car::setLocked(bool p_locked)
{
	m_inputLocked = p_locked;
}

} // namespace
