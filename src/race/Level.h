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

		void addCar(Car *p_car) { m_cars.push_back(p_car); }

		virtual void draw(CL_GraphicContext &p_gc);

		void loadFromFile(const CL_String& p_filename);

		float getResistance(float p_x, float p_y);

	private:

		/** level size */
		int m_width, m_height;

		/** level blocks */
		Block *m_blocks;

		/** All cars */
		std::vector<Car*> m_cars;

		/** Block sprite */
		CL_Sprite m_blockSprite;

		/** Vertical street sprite */
		CL_Sprite m_streetVert;

		/** Horizontal street sprite */
		CL_Sprite m_streetHoriz;

		/** Bottom-right turn */
		CL_Sprite m_turnBottomRight;

		/** Bottom-left turn */
		CL_Sprite m_turnBottomLeft;

		/** Top-right turn */
		CL_Sprite m_turnTopRight;

		/** Top-left turn */
		CL_Sprite m_turnTopLeft;


		Block::BlockType decodeBlock(const CL_String8& p_str);
		void unload();

};

#endif /* LEVEL_H_ */
