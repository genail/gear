/*
 * Copyright (c) 2009, Piotr Korzuszek
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

#ifndef CARSTATE_H_
#define CARSTATE_H_

#include <ClanLib/core.h>

#include "network/Packet.h"

namespace Net {

class CarState {

	public:

		CarState() {}

		virtual ~CarState() {}


		virtual CL_NetGameEvent buildEvent() const = 0;

		virtual void parseEvent(const CL_NetGameEvent &p_event) = 0;


		const CL_Point2f &getPosition() const { return m_position; }

		const CL_Point2f &getRotation() const { return m_rotation; }

		const CL_Vec2f &getMovement() const { return m_movement; }

		float getAcceleration() const { return m_accel; }

		float getTurn() const { return m_turn; }


		void setPosition(const CL_Point2f &p_position) { m_position = p_position; }

		void setRotation(const CL_Angle &p_rotation) { m_rotation = p_rotation; }

		void setMovement(const CL_Vec2f &p_movement) { m_movement = p_movement; }

		void setAcceleration(float p_accel) { m_accel = p_accel; }

		void setTurn(float p_turn) { m_turn = p_turn; }

	private:

		CL_Point2f m_position;

		CL_Angle m_rotation;

		CL_Vec2f m_movement;

		float m_accel;

		float m_turn;
};

}

#endif /* CARSTATE_H_ */
