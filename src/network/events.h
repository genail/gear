/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
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

#pragma once

#include <ClanLib/core.h>
#include <ClanLib/network.h>


// connect / disconnect procedure

#define EVENT_CLIENT_INFO 	"client_info"
#define EVENT_GAME_MODE     "game_mode"
#define EVENT_GAME_STATE 	"game_state"
#define EVENT_GOODBYE		"goodbye"

// player events

#define EVENT_PLAYER_JOINED "player_joined"
#define EVENT_PLAYER_LEFT   "player_left"

// race events

#define EVENT_CAR_STATE		"car_state"
#define EVENT_RACE_START	"race_start"

// voting event

#define EVENT_VOTE_START	"vote:start"
#define EVENT_VOTE_END		"vote:end"
#define EVENT_VOTE_TICK		"vote:tick"

// ranking events

#define EVENT_RANKING_PREFIX "ranking:"

#define EVENT_RANKING_FIND "ranking:find"
#define EVENT_RANKING_ENTRIES "ranking:entries"
#define EVENT_RANKING_REQUEST "ranking:request"
#define EVENT_RANKING_ADVANCE "ranking:advance"

inline bool isRankingEvent(const CL_NetGameEvent &p_event)
{
	static const int PREFIX_LEN = strlen(EVENT_RANKING_PREFIX);
	return p_event.get_name().substr(0, PREFIX_LEN) == EVENT_RANKING_PREFIX;
}
