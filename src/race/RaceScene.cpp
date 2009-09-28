/*
 * RaceScene.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include "RaceScene.h"

#include "race/Race.h"

RaceScene::RaceScene(Race* p_race) :
	m_race(p_race),
	m_raceUI(p_race)
{
	m_viewport.attachTo(&(p_race->getLocalPlayer().getCar().getPosition()));
}

RaceScene::~RaceScene() {
}

void RaceScene::draw(CL_GraphicContext &p_gc) {
	m_viewport.prepareGC(p_gc);

	m_race->getLevel().draw(p_gc);

	m_viewport.finalizeGC(p_gc);

	m_raceUI.draw(p_gc);

}

void RaceScene::load(CL_GraphicContext &p_gc) {

	m_race->getLevel().load(p_gc);
	m_raceUI.load(p_gc);
}
