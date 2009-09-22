/*
 * RaceScene.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include "RaceScene.h"

RaceScene::RaceScene(Car *p_localCar, Level *p_level) :
	m_localCar(p_localCar),
	m_level(p_level)
{
}

RaceScene::~RaceScene() {
}

void RaceScene::draw(CL_GraphicContext &p_gc) {
	m_viewport.prepareGC(p_gc);

	m_level->draw(p_gc);

	m_viewport.finalizeGC(p_gc);

	m_lapDisplayFont.draw_text(p_gc, Stage::getWidth() - 130, 40, CL_String8("Lap ") + CL_StringHelp::int_to_local8(m_localCar->getLap() + 1));

}

void RaceScene::load(CL_GraphicContext &p_gc) {
	if (m_lapDisplayFont.is_null()) {
		CL_FontDescription fontDesc;

		fontDesc.set_typeface_name("resources/DejaVuSansCondensed-BoldOblique.ttf");
		fontDesc.set_height(28);

		m_lapDisplayFont = CL_Font_Freetype(p_gc, fontDesc);
	}

	m_level->load(p_gc);
}
