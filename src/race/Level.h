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

class Level : public Drawable {
	public:
		Level();
		virtual ~Level();

		void addCar(Car *p_car) { p_car->m_level = this; m_cars.push_back(p_car); }

		virtual void draw(CL_GraphicContext &p_gc);
		virtual void load(CL_GraphicContext &p_gc);

		void loadFromFile(const CL_String& p_filename);

		float getResistance(float p_x, float p_y);

	private:

		/** level size */
		int m_width, m_height;

		/** level blocks */
		Block *m_blocks;

		/** All cars */
		std::vector<Car*> m_cars;

		Level(const Level& p_level);

		Block::BlockType decodeBlock(const CL_String8& p_str);
		void unload();

};

#endif /* LEVEL_H_ */
