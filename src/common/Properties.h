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

#include <map>
#include "clanlib/core/text.h"

//
// known properties files
//

// client configuration
#define CONFIG_FILE_CLIENT "client.cfg"

// server configuration
#define CONFIG_FILE_SERVER "server.cfg"


//
// Configuration values
//

// display mode
#define CG_SCREEN_WIDTH "cg_screen_width"
#define CG_SCREEN_HEIGHT "cg_screen_heght"
#define CG_FULLSCREEN "cg_fullscreen"
#define CG_OPENGL_VER "cg_opengl_ver"
#define CG_USE_SHADERS "cg_use_shaders"

// player settings
#define CG_PLAYER_NAME "cg_player_name"
#define CG_PLAYER_ID "cg_player_id"

// sound
#define CG_SOUND_VOLUME "cg_sound_volume"

// keyboard
#define CG_USE_WASD "cg_use_wasd"

// last used hostname
#define CG_LAST_HOSTNAME "cg_last_hostname"

// debug constants
#define DBG_ITER_SPEED "dbg_iter_speed"

//
// Server settings
//

#define SRV_PORT "srv_port"

#define SRV_LEVEL "srv_level"

// server game mode; avaiable options are 'arcade' and 'timetrail'
#define SRV_GAME_MODE "srv_game_mode"
#define SRV_GAME_MODE_TIMETRAIL "timetrail"
#define SRV_GAME_MODE_ARCADE "arcade"

/**
 * Runtime properties. There are several groups:
 * <ul>
 * <li>dbg_* - Debug properties. Available only in debug build</li>
 * <li>cg_* - User configuration properties.</li>
 * </ul>
 */
class Properties {

	public:

		// null objects
		static const bool NULL_BOOL;

		static const int NULL_INT;

		static const CL_String NULL_CL_STRING;


		static void load(const CL_String &p_filename);

		static void save(const CL_String &p_filename);


		static void set(
				const CL_String &p_key,
				bool p_value
		);

		static void set(
				const CL_String &p_key,
				int p_value
		);

		static void set(
				const CL_String &p_key,
				const CL_String &p_value
		);


		static bool getBool(
				const CL_String &p_key,
				const bool &p_defaultValue = NULL_BOOL
		);

		static int getInt(
				const CL_String &p_key,
				const int &p_defaultValue = NULL_INT
		);

		static CL_String getString(
				const CL_String &p_key,
				const CL_String &defaultValue = NULL_CL_STRING
		);

	private:

		static std::map<CL_String, CL_String> m_keyValueMap;


		Properties();

		virtual ~Properties();
};
