/*
 * RaceServer.h
 *
 *  Created on: 2009-09-27
 *      Author: chudy
 */

#ifndef RACESERVER_H_
#define RACESERVER_H_

#include <ClanLib/core.h>
#include "race/RacePlayer.h"

class Server;

class RaceServer {
	public:
		RaceServer(Server* p_server);
		virtual ~RaceServer();

		void destroy();

		void handleEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void initialize();

	private:
		/** Is this server initialized */
		bool m_initialized;

		/** Number of laps in this race */
		int m_lapsNum;

		/** All race players */
		std::map<CL_NetGameConnection*, RacePlayer*> m_racePlayers;

		/** Main server */
		Server *m_server;

		/** Slots container */
		CL_SlotContainer m_slots;

		/** Are the slots connected */
		bool m_slotsConnected;

		//
		// slots
		//

		void slotPlayerConnected(CL_NetGameConnection *p_connection, Player *p_player);

		void slotPlayerDisconnected(CL_NetGameConnection *p_connection, Player *p_player);

		//
		// event handlers
		//

		void handleCarStateChangeEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void handleTriggerRaceStartEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

};

#endif /* RACESERVER_H_ */
