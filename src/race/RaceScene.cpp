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

	m_tyreStripes.draw(p_gc);

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

void RaceScene::update(unsigned timeElapsed) {
	// FIXME: ceplus: move updateScene() to this method

	const Car &car = m_race->getLocalPlayer().getCar();
	const CL_Pointf &carPosition = car.getPosition();

	if (car.isDrifting()) {
		if (m_lastDriftPoint.x != 0 && m_lastDriftPoint.y != 0) {

			static const float tyreRadius = 10.0f;
			CL_Angle carAngle(car.getRotationRad(), cl_radians);

			CL_Vec2f v;
			float rad;

			for (int i = 0; i < 4; ++i) {
				carAngle += CL_Angle(i == 0 ? 45 : 90, cl_degrees);

				rad = carAngle.to_radians();

				v.x = cos(rad);
				v.y = sin(rad);

				v.normalize();

				v *= tyreRadius;

				CL_Pointf stripePoint1(m_lastDriftPoint), stripePoint2(carPosition);

				stripePoint1 += v;
				stripePoint2 += v;

				m_tyreStripes.add(stripePoint1, stripePoint2);
			}


		}

		m_lastDriftPoint.x = carPosition.x;
		m_lastDriftPoint.y = carPosition.y;
	} else {
		m_lastDriftPoint.x = 0;
		m_lastDriftPoint.y = 0;
	}
}
