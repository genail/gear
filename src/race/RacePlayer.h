/*
 * RacePlayer.h
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#ifndef RACEPLAYER_H_
#define RACEPLAYER_H_

#include "Player.h"
#include "race/Car.h"

class RacePlayer {
	public:
		RacePlayer(Player *p_player);
		virtual ~RacePlayer();

		Car& getCar() { return m_car; }

		Player& getPlayer() { return *m_player; }

	private:
		/** Base player */
		Player* m_player;

		/** The car */
		Car m_car;
};

#endif /* RACEPLAYER_H_ */
