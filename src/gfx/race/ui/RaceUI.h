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

#pragma once

#include "Label.h"
#include "gfx/Drawable.h"
#include "gfx/race/ui/SpeedMeter.h"
#include "gfx/race/ui/PlayerList.h"

namespace Race {
	class RaceLogic;
}

namespace Gfx {

class Viewport;

class RaceUI: public Gfx::Drawable {

	public:

		explicit RaceUI(const Race::RaceLogic* p_logic, const Gfx::Viewport *p_viewport);

		virtual ~RaceUI();

		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);

		SpeedMeter &getSpeedMeter() { return m_speedMeter; }

	private:

		/** Speed control widget */
		SpeedMeter m_speedMeter;

		/** Player list widget */
		PlayerList m_playerList;

		/** Vote label */
		Label m_voteLabel;

		/** Message board label */
		Label m_messageBoardLabel;

		/** Lap count label */
		Label m_lapLabel;

		/** Player names under cars label */
		Label m_carLabel;

		// Race logic pointer
		const Race::RaceLogic *m_logic;

		// Viewport pointer
		const Gfx::Viewport *m_viewport;


		// draw routines

		void drawMeters(CL_GraphicContext &p_gc);

		void drawVote(CL_GraphicContext &p_gc);

		void drawMessageBoard(CL_GraphicContext &p_gc);

		void drawLapLabel(CL_GraphicContext &p_gc);

		void drawCarLabels(CL_GraphicContext &p_gc);

		void drawPlayerList(CL_GraphicContext &p_gc);
};

} // namespace

