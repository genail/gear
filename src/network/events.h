/*
 * Events.h
 *
 *  Created on: 2009-09-26
 *      Author: chudy
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include <ClanLib/core.h>


//
// General events
//

#define EVENT_PREFIX_GENERAL		"general"

#define EVENT_HI					"general:hi"
#define EVENT_PLAYER_CONNECTED 		"general:player_connected"
#define EVENT_PLAYER_DISCONNECTED 	"general:player_disconnected"
#define EVENT_PLAYER_NICK_CHANGE	"general:nick_change"
#define EVENT_PLAYER_NICK_IN_USE	"general:nick_in_use"

//
// Race events
//

#define EVENT_PREFIX_RACE			"race"

#define EVENT_CAR_STATE_CHANGE		"race:car_state_change"
#define EVENT_TRIGGER_RACE_START	"race:trigger_race_start"
#define EVENT_LOCK_CAR				"race:lock_car"
#define EVENT_START_COUNTDOWN		"race:start_countdown"

#endif /* EVENTS_H_ */
