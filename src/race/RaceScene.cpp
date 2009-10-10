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
	oldSpeed = 0.0f;
}

RaceScene::~RaceScene() {
}

void RaceScene::draw(CL_GraphicContext &p_gc) {
	updateScale();
	
	m_viewport.prepareGC(p_gc);

	m_race->getLevel().draw(p_gc);

	m_viewport.finalizeGC(p_gc);

	m_raceUI.draw(p_gc);

}

void RaceScene::load(CL_GraphicContext &p_gc) {

	m_race->getLevel().load(p_gc);
	m_raceUI.load(p_gc);
}

void RaceScene::updateScale() {
	static const float ZOOM_SPEED = 0.005f;
	static const float MAX_SPEED = 500.0f; // FIXME
	
	float speed = fabs( ceil(m_race->getLocalPlayer().getCar().getSpeed() * 10.0f ) / 10.0f);
	
	float properScale = -( 1.0f / MAX_SPEED ) * speed + 2.0f;
	properScale = ceil( properScale * 100.0f ) / 100.0f;
	
	float scale =  m_viewport.getScale();
	
	if( properScale > scale ) {
		m_viewport.setScale( scale + ZOOM_SPEED );
	} else if ( properScale < scale ){
		m_viewport.setScale( scale - ZOOM_SPEED );
	}
	
	oldSpeed = speed;
	
		
#ifndef NDEBUG
	Stage::getDebugLayer()->putMessage(CL_String8("scale"),  CL_StringHelp::float_to_local8(scale));
#endif
}

void RaceScene::update(unsigned p_timeElapsed) {
	// FIXME: ceplus: move updateScene() to this method

	const Car &car = m_race->getLocalPlayer().getCar();

	if (car.getLap() > m_race->getLapsNum()) {
		m_viewport.detach();
	}

	// update race UI
	m_raceUI.update(p_timeElapsed);
}
