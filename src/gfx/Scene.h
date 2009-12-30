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

#include <ClanLib/gui.h>
#include <ClanLib/display.h>

#include "common/types.h"

namespace Gfx {

class Scene {

	public:

		Scene() {}

		virtual ~Scene() {};


		virtual bool isLoaded() const = 0;

		virtual SceneType getType() const = 0;


		virtual void draw(CL_GraphicContext &p_gc) = 0;

		virtual void inputPressed(const CL_InputEvent &p_event) = 0;

		virtual void inputReleased(const CL_InputEvent &p_event) = 0;

		virtual void load(CL_GraphicContext &p_gc) = 0;

		virtual void pushed() = 0;

		virtual void poped() = 0;

		/**
		 * Called with <code>true</code> if scene goes on top
		 * of scene stack and it should be displayed.
		 * Called with <code>false</code> if scene goes under
		 * other scene or out of scene stack.
		 */
		virtual void setActive(bool p_active) = 0;

		virtual void update(unsigned p_timeElapsed) = 0;

};

} // namespace
