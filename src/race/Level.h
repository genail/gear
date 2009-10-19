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

#ifndef LEVEL_H_
#define LEVEL_H_

#include <ClanLib/core.h>

#include "race/Car.h"
#include "race/Block.h"
#include "race/Bound.h"

#ifdef CLIENT

#include "graphics/Stage.h"
#include "race/TyreStripes.h"
#include "graphics/Drawable.h"

#define CLASS_LEVEL class Level : public Drawable

#else // CLIENT

#define CLASS_LEVEL class Level

#endif // CLIENT

CLASS_LEVEL
{

	public:

		Level();

		virtual ~Level();

		void addCar(Car *p_car);

		const Bound& getBound(int p_index) const { return m_bounds[p_index]; }

		int getBoundCount() const { return m_bounds.size(); }

		float getResistance(float p_x, float p_y);

		/**
		 * @return A start position of <code>p_num</code>
		 */
		CL_Pointf getStartPosition(int p_num) const;

		void loadFromFile(const CL_String& p_filename);

		void removeCar(Car *p_car);

		void update(unsigned p_timeElapsed);

#ifdef CLIENT
		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);
#endif // CLIENT


	private:

		/** level blocks */
		Block *m_blocks;

		/** Level bounds */
		std::vector<Bound> m_bounds;

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



		Level(const Level& p_level);

		Block::BlockType decodeBlock(const CL_String8& p_str);

		const Block& getBlock(int x, int y) const { return m_blocks[y * m_width + x]; }

		CL_String8 readLine(CL_File& p_file);

		void unload();

#ifdef CLIENT
		/** Tyre stripes */
		TyreStripes m_tyreStripes;


		/** Collision checking */
		void checkCollistions();
#endif // CLIENT

};

#endif /* LEVEL_H_ */
