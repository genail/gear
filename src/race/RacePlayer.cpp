/*
 * RacePlayer.cpp
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#include "RacePlayer.h"

RacePlayer::RacePlayer(Player *p_player) :
	m_finished(false),
	m_car(this),
	m_player(p_player)

{
}

RacePlayer::~RacePlayer() {
}
