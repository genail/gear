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

		const Player& getPlayer() const { return *m_player; }

		bool isFinished() const { return m_finished; }

		void setFinished(bool p_finished) { m_finished = p_finished; }

	private:

		/** Finished state */
		bool m_finished;

		/** The car */
		Car m_car;

		/** Base player */
		Player* m_player;
};

#endif /* RACEPLAYER_H_ */
