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

#ifndef CAR_H_
#define CAR_H_

#include "race/Checkpoint.h"

#include <ClanLib/core.h>
#include <ClanLib/network.h>

#include "race/Bound.h"

class Level;

#include "common.h"
#include "network/CarState.h"

#ifdef CLIENT // client-only code

#include "graphics/Stage.h"
#include "graphics/Drawable.h"

#define CLASS_CAR class Car : public Drawable


#else // server-only code

#define CLASS_CAR class Car

#endif // CLIENT

CLASS_CAR
{

	public:
		Car();
		virtual ~Car();

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

		void setAcceleration(bool p_value) {
			m_acceleration = p_value;
#ifndef NDEBUG
#ifdef CLIENT
			Stage::getDebugLayer()->putMessage(CL_String8("accel"),  CL_StringHelp::bool_to_local8(p_value));
#endif // CLIENT
#endif // !NDEBUG
		}

		void setBrake(bool p_value) {
			m_brake = p_value;
#ifndef NDEBUG
#ifdef CLIENT
			Stage::getDebugLayer()->putMessage(CL_String8("brake"),  CL_StringHelp::bool_to_local8(p_value));
#endif // CLIENT
#endif // !NDEBUG
		}

		void setLap(int p_lap) { m_lap = p_lap; }

		/**
		 * Sets if car movement should be locked (car won't move).
		 */
		void setLocked(bool p_locked);

		void setTurn(float p_value) {
			m_turn = normalize(p_value);
#ifndef NDEBUG
#ifdef CLIENT
			Stage::getDebugLayer()->putMessage(CL_String8("turn"),  CL_StringHelp::float_to_local8(p_value));
#endif // CLIENT
#endif // !NDEBUG
		}

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

		CL_Signal_v1<Car &> &sigStatusChange() { return m_statusChangeSignal; }

#ifndef SERVER
		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);

		/** @return Current collision outline based on car position and rotation */
		CL_CollisionOutline calculateCurrentCollisionOutline() const;

		/** Invoked when collision with bound has occurred */
		void performBoundCollision(const Bound &p_bound) {
			CL_Vec2f reactionVector;
			CL_Vec2f boundVector;
			
			boundVector.x = fabs(p_bound.getSegment().q.x - p_bound.getSegment().p.x);
			boundVector.y = fabs(p_bound.getSegment().q.y - p_bound.getSegment().p.y);
			
			reactionVector.x = -boundVector.y;
			
			if (m_position.x < fabs(p_bound.getSegment().q.x))
				reactionVector.x = -boundVector.y;
			
			else if (m_position.x >= fabs(p_bound.getSegment().q.x))
				reactionVector.x = boundVector.y;
			
			if (m_position.y < fabs(p_bound.getSegment().q.y))
				reactionVector.y = -boundVector.x;
			
			else if (m_position.y >= fabs(p_bound.getSegment().q.y))
				reactionVector.y = boundVector.x;
			
			reactionVector.normalize();
			reactionVector *= m_speed / 2;
			
			m_moveVector += reactionVector;
		} // FIXME: Ryba
		
#endif // !SERVER

	private:
		
		/** Parent level */
		Level* m_level;

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

		/** Level checkpoints and pass state. Filled in by parent Level */
		std::vector<Checkpoint> m_checkpoints;

		/** Final lap checkpoint. Filled by parent Level */
		Checkpoint m_lapCheckpoint;

		int calculateInputChecksum() const;

		float normalize(float p_value) const;

		bool areAllCheckpointsPassed() const;

		void resetCheckpoints();

		friend class Level;

#ifdef CLIENT
		/** Car sprite */
		CL_Sprite m_sprite;

		/** Nickname display font */
		CL_Font_Freetype m_nickDisplayFont;

		/** Body outline for collision check */
		CL_CollisionOutline m_collisionOutline;

#ifndef NDEBUG
		void debugDrawLine(CL_GraphicContext &p_gc, float x1, float y1, float x2, float y2, const CL_Color& p_color);
#endif // !NDEBUG
#endif // CLIENT


};

inline float Car::normalize(float p_value) const {
	if (p_value < -1.0f) {
		p_value = 1.0f;
	} else if (p_value > 1.0f) {
		p_value = 1.0f;
	}

	return p_value;
}

#endif /* CAR_H_ */
