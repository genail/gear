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

#ifndef RACESCENE_H_
#define RACESCENE_H_

#include <ClanLib/gui.h>

#include "race/Car.h"
#include "race/Level.h"
#include "race/RaceUI.h"
#include "race/Viewport.h"
#include "race/RacePlayer.h"
#include "graphics/Drawable.h"

#include "graphics/Scene.h"

class Race;

class RaceScene: public Scene
{
	public:
		RaceScene(Race* p_race, CL_GUIComponent *p_guiParent);
		virtual ~RaceScene();

		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);

		RaceUI& getUI() { return m_raceUI; }

		Viewport& getViewport() { return m_viewport; }

		virtual void update(unsigned p_timeElapsed);

	private:

		float oldSpeed;

		/** Last drift car position. If null, then no drift was doing last time. */
		CL_Pointf m_lastDriftPoint;

		/** The Race pointer */
		Race *m_race;

		/** How player sees the scene */
		Viewport m_viewport;

		/** Race user interface */
		RaceUI m_raceUI;


		void grabInput();

		void updateScale();


};

#endif /* RACESCENE_H_ */
