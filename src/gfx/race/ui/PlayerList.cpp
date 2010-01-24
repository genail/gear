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

#include "PlayerList.h"

#include "common/Player.h"
#include "gfx/race/ui/Label.h"
#include "logic/race/Car.h"
#include "logic/race/RaceLogic.h"


namespace Gfx {

class PlayerListImpl
{
	public:

		const Race::RaceLogic *m_logic;

		CL_Pointf m_position;

		Label m_label;

		int m_labelHeight;


		PlayerListImpl(const Race::RaceLogic *p_logic) :
			m_logic(p_logic),
			m_label(CL_Pointf(), "", Label::F_BOLD, 16)
		{ /* empty */ }

		const CL_String &ownerName(const Race::Car &p_car);
};

PlayerList::PlayerList(const Race::RaceLogic *p_logic) :
	m_impl(new PlayerListImpl(p_logic))
{
	// empty
}

PlayerList::~PlayerList()
{
	// empty
}

void PlayerList::setPosition(const CL_Pointf &p_pos)
{
	m_impl->m_position = p_pos;
}


void PlayerList::draw(CL_GraphicContext &p_gc)
{
	const Race::Level &level = m_impl->m_logic->getLevel();
	const int carCount = level.getCarCount();

	float h = 0.0f;

	p_gc.mult_translate(m_impl->m_position.x, m_impl->m_position.y);

	for (int i = 0; i < carCount; ++i) {
		const Race::Car &car = level.getCar(i);

		m_impl->m_label.setPosition(CL_Pointf(0, h));
		m_impl->m_label.setText(cl_format("%1. %2", i + 1, m_impl->m_logic->getPlayer(car).getName())); // FIXME: not optimal

		m_impl->m_label.draw(p_gc);

		h += m_impl->m_labelHeight;
	}

	p_gc.pop_modelview();
}

void PlayerList::load(CL_GraphicContext &p_gc)
{
	m_impl->m_label.load(p_gc);

	m_impl->m_label.setText("X");
	m_impl->m_labelHeight = m_impl->m_label.size(p_gc).height;

	Drawable::load(p_gc);
}

const CL_String &PlayerListImpl::ownerName(const Race::Car &p_car)
{
	const int playerCount = m_logic->getPlayerCount();

	for (int i = 0; i < playerCount; ++i) {
		const Player &player = m_logic->getPlayer(i);

		if (player.getCar() == p_car) {
			return player.getName();
		}
	}

	G_ASSERT(0 && "owner not found");
}

} // namespace
