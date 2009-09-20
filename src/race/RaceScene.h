/*
 * RaceScene.h
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#ifndef RACESCENE_H_
#define RACESCENE_H_

#include "graphics/Drawable.h"
#include "race/Viewport.h"
#include "race/Level.h"
#include "race/Car.h"

class RaceScene: public Drawable {
	public:
		RaceScene(Car *p_localCar, Level *p_level);
		virtual ~RaceScene();

		virtual void draw(CL_GraphicContext &p_gc);
		virtual void load(CL_GraphicContext &p_gc);

		Viewport& getViewport() { return m_viewport; }

	private:

		/** How player sees the scene */
		Viewport m_viewport;

		/** The level */
		Level *m_level;

		/** Local player's car */
		Car* m_localCar;

		/** Lap display font */
		CL_Font_Freetype m_lapDisplayFont;

};

#endif /* RACESCENE_H_ */
