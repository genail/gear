/*
 * RacePlayer.cpp
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#include "RacePlayer.h"

RacePlayer::RacePlayer(Player *p_player) :
	m_player(p_player),
	m_car(this)
{
}

RacePlayer::~RacePlayer() {
}
