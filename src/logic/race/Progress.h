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

#include "clanlib/core/system.h"

#include "common.h"

namespace Race
{

class Car;
class Checkpoint;
class Level;
class ProgressImpl;

class Progress
{
	public:

		Progress(const Level *p_level);
		virtual ~Progress();

		void initialize();
		void destroy();

		DEPRECATED(void addCar(const Car &p_car));
		DEPRECATED(void removeCar(const Car &p_car));

		void addCar(const Car *p_car);
		void removeCar(const Car *p_car);

		void reset(const Car &p_car);
		void resetAllCars();
		void resetClock();
		void update();

		int getCheckpointCount() const;
		const Checkpoint &getCheckpoint(const Car &p_car) const;
		const Checkpoint &getCheckpoint(int p_idx) const;


		int getLapNumber(const Car &p_car) const;

		/**
		 * Provides lap time in milliseconds. If lap isn't
		 * finished yet, then ongoing time is returned.
		 *
		 * @return lap time in milliseconds
		 */
		int getLapTime(const Car &p_car, int p_lap) const;



	private:

		CL_SharedPtr<ProgressImpl> m_impl;
};

}

