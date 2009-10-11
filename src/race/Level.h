/*
 * Level.h
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#ifndef LEVEL_H_
#define LEVEL_H_

#include <ClanLib/core.h>

#include "race/Car.h"
#include "race/Block.h"
#include "race/Bound.h"

#ifdef CLIENT // client-only code
#include "graphics/Stage.h"
#include "race/TyreStripes.h"
#include "graphics/Drawable.h"

class Level : public Drawable
{
	public:
		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);

	private:
		/** Tyre stripes */
		TyreStripes m_tyreStripes;

#else // server-only code

class Level
{

#endif // CLIENT

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

};

#endif /* LEVEL_H_ */
