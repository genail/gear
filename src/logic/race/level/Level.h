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

#pragma once

#include <ClanLib/core.h>

namespace Race {

class Block;
class Bound;
class Car;
class Track;
class TrackTriangulator;

class LevelImpl;

class Level
{

	public:

		Level();

		virtual ~Level();


		// virtual methods

		virtual void initialize();

		virtual void destroy();


		// getters

		bool isLoaded() const;

		const Track &getTrack() const;

		/**
		 * @return Triangulator object. It if valid only if track was
		 * loaded from file.
		 */
		const TrackTriangulator &getTrackTriangulator() const;

		/**
		 * @return Triangulator object. It if valid only if track was
		 * loaded from file.
		 */
		TrackTriangulator &getTrackTriangulator();


		// operations

		void addCar(Car *p_car);

		Car &getCar(int p_index);

		const Car &getCar(int p_index) const;

		int getCarCount() const;

		float getResistance(float p_x, float p_y);

		/**
		 * @return A start position of <code>p_num</code>
		 */
		CL_Pointf getStartPosition(int p_num) const;

		void load(const CL_String &p_filename);

		void removeCar(Car *p_car);

		void save(const CL_String &p_filename);

		/**
		 * Sets the new track. Note that when you set a track, the level
		 * geometry will not be updated. You should use getTrackTriangulator()
		 * and rebuild modified segment or whole track.
		 *
		 * @param p_track Track to set.
		 */
		void setTrack(const Track &p_track);


	private:

		CL_SharedPtr<LevelImpl> m_impl;

};

} // namespace
