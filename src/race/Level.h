/*
 * Level.h
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#ifndef LEVEL_H_
#define LEVEL_H_

#include <ClanLib/core.h>

#include "graphics/Drawable.h"

#include "race/Car.h"
#include "race/Block.h"
#include "race/Bound.h"

class Level : public Drawable {
	public:
		Level();
		virtual ~Level();

		// inherted from Drawable
		virtual void draw(CL_GraphicContext &p_gc);
		virtual void load(CL_GraphicContext &p_gc);


		void addCar(Car *p_car) { p_car->m_level = this; m_cars.push_back(p_car); }

		const Bound& getBound(int p_index) const { return m_bounds[p_index]; }

		int getBoundCount() const { return m_bounds.size(); }

		void loadFromFile(const CL_String& p_filename);

		float getResistance(float p_x, float p_y);

	private:

		/** level size */
		int m_width, m_height;

		/** level blocks */
		Block *m_blocks;

		/** All cars */
		std::vector<Car*> m_cars;

		/** Level bounds */
		std::vector<Bound> m_bounds;

		Level(const Level& p_level);

		Block::BlockType decodeBlock(const CL_String8& p_str);

		const Block& getBlock(int x, int y) const { return m_blocks[y * m_width + x]; }

		CL_String8 readLine(CL_File& p_file);

		void unload();

};

#endif /* LEVEL_H_ */
