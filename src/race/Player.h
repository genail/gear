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
		Player(const CL_String8 &p_name);
		virtual ~Player();

		Car& getCar() { return m_car; }
		const CL_String8 &getName() const { return m_name; }

		void setName(const CL_String8 &p_name) { m_name = p_name; }

	private:
		/** Nickname of this player */
		CL_String8 m_name;

		/** Player's car */
		Car m_car;
};

#endif /* PLAYER_H_ */
