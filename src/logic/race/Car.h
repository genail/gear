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

#pragma once

#include <string.h>
#include <ClanLib/core.h>
#include <ClanLib/network.h>
#include <ClanLib/display.h>

#include "common.h"

class Player;

namespace Net {
	class CarState;
	class RemoteCar;
}

namespace Gfx {
	class RaceGraphics;
}

namespace Race {

class CarImpl;
class Bound;
class Level;

struct CarInputState
{
	bool accel;
	bool brake;
	float turn;


	CarInputState() : accel(false), brake(false), turn(0.0f) {}

	bool operator==(const CarInputState &p_other) const {
		return memcmp(this, &p_other, sizeof(CarInputState)) == 0;
	}

	bool operator!=(const CarInputState &p_other) const {
		return !(*this == p_other);
	}
};

class Car : boost::noncopyable
{

	public:
		Car(Player *p_owner);
		virtual ~Car();

		const Player &getOwnerPlayer() const;

		bool isChoking() const;
		bool isDrifting() const;
		bool isLocked() const;

		/** @return corpse angle starting from positive X axis CW */
		virtual const CL_Angle &getCorpseAngle() const;

		virtual const CL_Pointf& getPosition() const;

		float getSpeed() const;
		float getSpeedKMS() const;

		/**
		 * @return physical wheel turn. -1.0 is maximum left,
		 * 1.0 is maximum right
		 */
		float getPhyWheelTurn() const;

		int32_t getIterationId() const;

		virtual void serialize(CL_NetGameEvent *p_data) const;
		virtual void deserialize(const CL_NetGameEvent &p_data);

		void setAcceleration(bool p_value);
		void setBrake(bool p_value);
		void setLocked(bool p_locked);
		void setTurn(float p_value);

		const CarInputState &getInputState() const;


		// state setters

		/**
		 * Resets car states that changes over race
		 * like damage. It does not change position,
		 * speed or rotation.
		 */
		void reset();
		void resetIterationCounter();

		/**
		 * Sets the car angle. It should be clockwise
		 * oriented starting from positive X axis.
		 *
		 * @param p_angle Angle to set.
		 */
		void setAngle(const CL_Angle &p_angle);
		void setPosition(const CL_Pointf &p_position);


		// other operations

		/** Clones all given car attributes to this one */
		void clone(const Car &p_car);

		virtual void update(unsigned int elapsedTime);

		/** Updates the car state to reach the target iteration id */
		void updateToIteration(int32_t p_targetIterId);


		// collisions

		void applyCollision(const CL_LineSegment2f &p_seg);

		/** @return Current outline based on car position and rotation */
		CL_CollisionOutline getCollisionOutline() const;


		// operators

		bool operator==(const Car &p_other) const;
		bool operator!=(const Car &p_other) const;


	protected:

		void setMovement(const CL_Vec2f &p_movement);
		void setSpeed(float p_speed);

	private:

		CL_SharedPtr<CarImpl> m_impl;

		friend class Race::Level;

#if defined(DRAW_CAR_VECTORS) && !defined(NDEBUG)
friend class Gfx::RaceGraphics;
#endif // DRAW_CAR_VECTORS && !NDEBUG

};

} // namespace
