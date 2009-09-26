/*
 * Player.h
 *
 *  Created on: 2009-09-21
 *      Author: chudy
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <ClanLib/core.h>
#include "race/Car.h"

class Player {
	public:
		Player() {}
		Player(const CL_String8 &p_name);
		virtual ~Player();

		const CL_String &getName() const { return m_name; }

		void setName(const CL_String &p_name) { m_name = p_name; }

	private:
		/** Nickname of this player */
		CL_String8 m_name;

};

#endif /* PLAYER_H_ */
