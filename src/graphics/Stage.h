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

#include <stack>
#include <ClanLib/core.h>

#include "graphics/DebugLayer.h"

class Scene;
class Application;

namespace Gfx {

class Stage {
	public:

		virtual ~Stage() {
		}

		static DebugLayer* getDebugLayer() { return m_debugLayer; }

		static int getWidth() { return m_width; }

		static int getHeight() { return m_height; }

		static void pushScene(Scene *p_scene);

		static void popScene();

		static Scene *peekScene();

		static void replaceScene(Scene *p_scene);

		static CL_ResourceManager* getResourceManager() { return m_resourceManager; }


	private:

		/** Resource Manager */
		static CL_ResourceManager *m_resourceManager;

		/** Stage size */
		static int m_width, m_height;

		/** Scene stack */
		static std::stack<Scene*> m_sceneStack;

		/** Debug layer */
		static DebugLayer *m_debugLayer;

		Stage() {}

		friend class ::Application;
};

} // namespace
