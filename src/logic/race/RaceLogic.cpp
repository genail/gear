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

#include "RaceLogic.h"

#include "common/Game.h"
#include "common/Player.h"

namespace Race {

RaceLogic::RaceLogic()
{

}

RaceLogic::~RaceLogic()
{
}

void RaceLogic::update(unsigned p_timeElapsed)
{
	updateCarPhysics(p_timeElapsed);
	updateLevel(p_timeElapsed);
}

void RaceLogic::updateCarPhysics(unsigned p_timeElapsed)
{
	const unsigned carCount = m_level.getCarCount();
	for (unsigned i = 0; i < carCount; ++i) {
		Race::Car &car = m_level.getCar(i);
		car.update(p_timeElapsed);
	}
}

void RaceLogic::updateLevel(unsigned p_timeElapsed)
{
	m_level.update(p_timeElapsed);
}

Player *RaceLogic::findPlayer(const CL_String& p_name)
{
	foreach(Player *player, m_players) {
		if (player->getName() == p_name) {
			return player;
		}
	}

	return NULL;
}

} // namespace
