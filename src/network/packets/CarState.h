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

#pragma once

#include <sys/types.h>

#include "Packet.h"

namespace Net {

class CarState : public Net::Packet {

	public:

		CarState();
		virtual ~CarState();

		virtual CL_NetGameEvent buildEvent() const;
		virtual void parseEvent(const CL_NetGameEvent &p_event);

		bool isAfterCollision() const;
		int32_t getIterationId() const;
		const CL_String &getName() const;
		CL_NetGameEvent getSerializedData() const;

		void setAfterCollision(bool p_afterCollision);
		/**
		 * Sets the physics iteration id number (beginning from 0).
		 * Each iteration has unique id greater by one than previous iteration.
		 * Exception when last id was is 2^31-1 then next one would be 0.
		 */
		void setIterationId(int32_t p_iterId);
		void setName(const CL_String &p_name);
		void setSerializedData(const CL_NetGameEvent &p_data);

	private:

		int32_t m_iterId;
		CL_String m_name;
		bool m_afterCollision;
		CL_NetGameEvent m_serialData;
};

}
