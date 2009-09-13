/*
 * RaceScene.h
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#ifndef RACESCENE_H_
#define RACESCENE_H_

#include "graphics/Drawable.h"
#include "Viewport.h"
#include "Level.h"

class RaceScene: public Drawable {
	public:
		RaceScene();
		virtual ~RaceScene();

		virtual void draw(CL_GraphicContext &p_gc);

		Level& getLevel() { return m_level; }
		Viewport& getViewport() { return m_viewport; }

	private:

		/** How player sees the scene */
		Viewport m_viewport;

		/** The level */
		Level m_level;

};

#endif /* RACESCENE_H_ */
