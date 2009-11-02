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

#ifndef EVENTS_H_
#define EVENTS_H_

#include <ClanLib/core.h>


// connect / disconnect procedure

#define EVENT_CLIENT_INFO 	"client_info"

#define EVENT_GAME_STATE 	"game_state"

#define EVENT_GOODBYE		"goodbye"

// player events

#define EVENT_PLAYER_JOINED "player_joined"

#define EVENT_PLAYER_LEAVED "player_leaved"

#define EVENT_PLAYER_INFO	"player_info"

// race events

#define EVENT_CAR_STATE		"car_state"


//#define EVENT_PREFIX_GENERAL		"general"
//
//// sends as acceptance from server. From this moment connection is fully initialized.
//#define EVENT_WELCOME				"general:welcome"
//
//#define EVENT_GENERAL_GAMESTATE		"general:gamestate"
//
//#define EVENT_GRANT_PERMISSIONS		"general:grant_permissions"
//#define EVENT_HI					"general:hi"
//#define EVENT_PLAYER_CONNECTED 		"general:player_connected"
//#define EVENT_PLAYER_DISCONNECTED 	"general:player_disconnected"
//#define EVENT_PLAYER_NICK_CHANGE	"general:nick_change"
//#define EVENT_PLAYER_NICK_IN_USE	"general:nick_in_use"
//
//#define EVENT_INIT_RACE				"general:init_race"
//
////
//// Race events
////
//
//#define EVENT_PREFIX_RACE			"race"
//
//#define EVENT_CAR_STATE_CHANGE		"race:car_state_change"
//#define EVENT_TRIGGER_RACE_START	"race:trigger_race_start"
//#define EVENT_LOCK_CAR				"race:lock_car"
//#define EVENT_START_COUNTDOWN		"race:start_countdown"
//#define EVENT_RACE_STATE			"race:race_state"
//#define EVENT_PLAYER_FINISHED		"race:player_finished"

#endif /* EVENTS_H_ */
