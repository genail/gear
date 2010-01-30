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

#include <ClanLib/core.h>
#include <ClanLib/network.h>

#if defined(CLIENT)
#include <ClanLib/display.h>
#endif // CLIENT

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

class Car : boost::noncopyable
{

	DEF_SIGNAL_1(inputChanged, const Car&);

	public:

		Car();

		virtual ~Car();


		// attributes

		bool isDrifting() const;

		bool isLocked() const;

		virtual const CL_Angle &getCorpseAngle() const;

		virtual const CL_Pointf& getPosition() const;

		float getSpeed() const;

		/** @return Car speed in km/s */
		float getSpeedKMS() const;

		
		// implementation data serialization (for network)

		virtual void deserialize(const CL_NetGameEvent &p_data);

		virtual void serialize(CL_NetGameEvent *p_data) const;


		// input setters

		void setAcceleration(bool p_value);

		void setBrake(bool p_value);

		void setLocked(bool p_locked);

		void setTurn(float p_value);


		// state setters

		/**
		 * Sets the car angle. It should be counter clockwise
		 * oriented starting from positive X axis.
		 *
		 * @param p_angle Angle to set.
		 */
		void setAngle(const CL_Angle &p_angle);

		void setPosition(const CL_Pointf &p_position);


		// other operations

		virtual void update(unsigned int elapsedTime);


		// operators

		bool operator==(const Car &p_other) const;

		bool operator!=(const Car &p_other) const;


#if defined(CLIENT)
		/** @return Current collision outline based on car position and rotation */
		CL_CollisionOutline calculateCurrentCollisionOutline() const;

		/** Invoked when collision with bound has occurred */
		void performBoundCollision(const Bound &p_bound);
#endif // CLIENT

	protected:

		bool isAcceleration() const;

		bool isBrake() const;

		float getTurn() const;

		void setMovement(const CL_Vec2f &p_movement);

		void setSpeed(float p_speed);

	private:

		CL_SharedPtr<CarImpl> m_impl;


		friend class Race::Level;
		friend class Net::RemoteCar;

#if defined(DRAW_CAR_VECTORS) && !defined(NDEBUG)
friend class Gfx::RaceGraphics;
#endif // DRAW_CAR_VECTORS && !NDEBUG

};

} // namespace
