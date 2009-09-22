/*
 * Player.cpp
 *
 *  Created on: 2009-09-21
 *      Author: chudy
 */

#include "Player.h"
#include "race/Car.h"

Player::Player(const CL_String8& p_name) :
	m_name(p_name),
	m_car(this)
{

}

Player::~Player() {
}
