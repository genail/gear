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

#pragma once

#include <ClanLib/core.h>
#include <ClanLib/network.h>

#include "common.h"
#include "logic/race/Bound.h"
#include "logic/race/Checkpoint.h"
#include "network/CarState.h"

namespace Gfx {
	class RaceGraphics;
}

namespace Race {

class Level;

class Car
{

	public:
		Car();
		virtual ~Car();

		const Checkpoint *getCurrentCheckpoint() const { return m_currentCheckpoint; }

		int getLap() const { return m_lap; }

		const CL_Pointf& getPosition() const { return m_position; }

		float getRotation() const { return m_rotation.to_degrees(); }
		
		float getRotationRad() const { return m_rotation.to_radians(); }

		float getSpeed() const { return m_speed; }

		/** @return Car speed in km/s */
		float getSpeedKMS() const { return m_speed / 3.0f; }
		
		bool isDrifting() const;

		DEPRECATED(int prepareStatusEvent(CL_NetGameEvent &p_event));

		Net::CarState prepareCarState();

		DEPRECATED(int applyStatusEvent(const CL_NetGameEvent &p_event, int p_beginIndex = 0));

		void applyCarState(const Net::CarState &p_carState);

		void setAcceleration(bool p_value) { m_acceleration = p_value; }

		void setBrake(bool p_value) { m_brake = p_value; }

		void setLap(int p_lap) { m_lap = p_lap; }

		/**
		 * Sets if car movement should be locked (car won't move).
		 */
		void setLocked(bool p_locked);

		void setTurn(float p_value) { m_turn = normalize(p_value); }

		void setPosition(const CL_Pointf &p_position) { m_position = p_position; }

		void setRotation(float p_rotation) { m_rotation.set_degrees(p_rotation); }
		
		void setHandbrake(bool p_handbrake) { m_handbrake = p_handbrake; }

		/**
		 * Sets the car position at selected <code>p_startPosition</code>
		 * which is a number >= 1.
		 *
		 * @param p_startPosition Car start position.
		 */
		void setStartPosition(int p_startPosition);

		void update(unsigned int elapsedTime);

		void update1_60();

		/**
		 * Sets the new checkpoint and calculates car progress on track.
		 * If lap is reached, then lap number will increase by one.
		 */
		void updateCurrentCheckpoint(const Checkpoint *p_checkpoint);

		CL_Signal_v1<Car &> &sigStatusChange() { return m_statusChangeSignal; }

#ifndef SERVER
		/** @return Current collision outline based on car position and rotation */
		CL_CollisionOutline calculateCurrentCollisionOutline() const;

		/** Invoked when collision with bound has occurred */
		void performBoundCollision(const Bound &p_bound) {
			m_boundSegment = p_bound.getSegment();
			m_boundHitTest = true;
		}
		
#endif // !SERVER

	private:
		
		/** Parent level */
		Race::Level* m_level;

		/** Locked state. If true then car shoudn't move. */
		bool m_locked;

		/** Central position on map */
		CL_Pointf m_position;

		/** Rotation in degrees CCW from positive X axis */
		CL_Angle m_rotation;

		/** Current turn. -1 is maximum left, 0 is center and 1 is maximum right */
		float m_turn;

		/** Acceleration switch */
		bool m_acceleration;

		/** Brake switch */
		bool m_brake;
		
		/** Handbrake switch */
		bool m_handbrake;

		/** Move vector */
		CL_Vec2f m_moveVector;
		CL_Vec2f accelerationVector;

		/** Current speed */
		float m_speed;
		
		/** angle */
		float m_angle;

		/** Input state changed signal */
		CL_Signal_v1<Car &> m_statusChangeSignal;

		/** Input checksum */
		int m_inputChecksum;

		/** Lap number */
		int m_lap;

		unsigned m_timeFromLastUpdate;

		// checkpoint system

		/** The greatest checkpoint id met on this lap */
		int m_greatestCheckpointId;

		/** Current checkpoint position */
		const Checkpoint *m_currentCheckpoint;
		
		// Bound collision vars
		CL_LineSegment2f m_boundSegment;
		bool m_boundHitTest;

		int calculateInputChecksum() const;

		float normalize(float p_value) const;

		friend class Race::Level;

#ifdef CLIENT
		/** Body outline for collision check */
		CL_CollisionOutline m_collisionOutline;

#ifndef NDEBUG
		void debugDrawLine(CL_GraphicContext &p_gc, float x1, float y1, float x2, float y2, const CL_Color& p_color);
#endif // !NDEBUG
#endif // CLIENT

#if defined(DRAW_CAR_VECTORS) && !defined(NDEBUG)
friend class Gfx::RaceGraphics;
#endif // DRAW_CAR_VECTORS && !NDEBUG

};

inline float Car::normalize(float p_value) const {
	if (p_value < -1.0f) {
		p_value = 1.0f;
	} else if (p_value > 1.0f) {
		p_value = 1.0f;
	}

	return p_value;
}

} // namespace
