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

#include "common.h"
#include "Car.h"
#include "Block.h"
#include "Bound.h"
#include "Track.h"
#include "TyreStripes.h"
#include "resistance/ResistanceMap.h"

namespace Race {

class Level
{

	public:

		Level();

		virtual ~Level();

		void initialize(const CL_String &p_filename);

		void destroy();

		void addCar(Car *p_car);

		const Bound& getBound(int p_index) const { return *m_bounds[p_index].get(); }

		size_t getBoundCount() const { return m_bounds.size(); }

		float getResistance(float p_x, float p_y);

		/**
		 * @return A start position of <code>p_num</code>
		 */
		CL_Pointf getStartPosition(int p_num) const;


		size_t getCarCount() const { return m_cars.size(); }

		const Car &getCar(size_t p_index) const { return *m_cars[p_index]; }

		Car &getCar(size_t p_index) { return *m_cars[p_index]; }

		int getWidth() const { return m_width; }

		int getHeight() const { return m_height; }

		const Block& getBlock(int x, int y) const { return *m_blocks[y * m_width + x].get(); }

		const Track &getTrack() const { return m_track; }

		const TyreStripes &getTyreStripes() const { return m_tyreStripes; }


		void removeCar(Car *p_car);

		DEPRECATED(void update(unsigned p_timeElapsed));


	private:

		/** level blocks */
		std::vector< CL_SharedPtr<Race::Block> > m_blocks;

		/** The track (checkpoint system) */
		Track m_track;

		/** Level bounds */
		std::vector< CL_SharedPtr<Bound> > m_bounds;

		/** Resistance mapping */
		RaceResistance::ResistanceMap m_resistanceMap;

		/** All cars */
		std::vector<Car*> m_cars;

		/** Car's last drift points for all four tires: fr, rr, rl, fl */
		std::map<Car*, CL_Pointf*> m_carsDriftPoints;

		/** Loaded state */
		bool m_loaded;

		/** level size */
		int m_width, m_height;

		/** Map of start positions */
		std::map<int, CL_Pointf> m_startPositions;

		/** Tyre stripes */
		TyreStripes m_tyreStripes;



		Level(const Level& p_level);

		CL_String8 readLine(CL_File& p_file);


		/** Collision checking */
		void checkCollistions();

		void updateCheckpoints();

		// level loading

		void loadFromFile(const CL_String& p_filename);

		void loadMetaElement(const CL_DomNode &p_metaNode);

		void loadTrackElement(const CL_DomNode &p_trackNode);

		void loadBoundsElement(const CL_DomNode &p_boundsNode);

		void loadSandElement(const CL_DomNode &p_sandNode);

		CL_SharedPtr<RaceResistance::Geometry> buildResistanceGeometry(int p_x, int p_y, Common::GroundBlockType p_blockType) const;


		// helpers

		CL_Pointf real(const CL_Pointf &p_point) const;

		float real(float p_coord) const;

};

} // namespace
