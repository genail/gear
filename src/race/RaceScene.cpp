/*
 * RaceScene.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include "RaceScene.h"

RaceScene::RaceScene() {
	// TODO Auto-generated constructor stub

}

RaceScene::~RaceScene() {
	// TODO Auto-generated destructor stub
}

void RaceScene::draw(CL_GraphicContext &p_gc) {
	m_viewport.prepareGC(p_gc);

	m_level.draw(p_gc);

	m_viewport.finalizeGC(p_gc);
}
