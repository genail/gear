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

#include "Smoke.h"

#include <stdlib.h>

#include "common.h"

#include "gfx/Stage.h"
#include "math/Easing.h"

namespace Gfx {

const int SMOKE_SPRITES_COUNT = 3;

CL_Sprite Smoke::m_smokeSprites[SMOKE_SPRITES_COUNT];

Smoke::Smoke(const CL_Pointf &p_position) :
		m_position(p_position)
{
	m_spriteIdx = rand() % SMOKE_SPRITES_COUNT;
}

Smoke::~Smoke()
{
	// empty
}

void Smoke::start()
{
	Animation::start();

	m_alpha.animate(0.0f, 0.3f, 250, Math::Easing::NONE, 0);
	m_alpha.animate(0.3f, 0.0f, 5500, Math::Easing::NONE, 500);

	m_size.animate(0.1f, 1.0f, 6000);
}

void Smoke::update(unsigned p_timeElapsed)
{
	Animation::update(p_timeElapsed);

	m_alpha.update(p_timeElapsed);
	m_size.update(p_timeElapsed);
}

void Smoke::draw(CL_GraphicContext &p_gc)
{
	G_ASSERT(!m_smokeSprites[0].is_null());

	static const unsigned ANIMATION_END = 6000;

	const unsigned now = getTimeFromStart();

	if (now < ANIMATION_END) {

		m_smokeSprites[m_spriteIdx].set_alpha(m_alpha.get());
		m_smokeSprites[m_spriteIdx].set_scale(m_size.get(), m_size.get());

		m_smokeSprites[m_spriteIdx].draw(p_gc, m_position.x, m_position.y);
	} else {
		setFinished(true);
	}

}

void Smoke::load(CL_GraphicContext &p_gc)
{
	if (m_smokeSprites[0].is_null()) {
		for (int i = 0; i <= SMOKE_SPRITES_COUNT - 1; ++i) {
			m_smokeSprites[i] =
					CL_Sprite(
							p_gc,
							cl_format("race/smoke%1", i + 1),
							Stage::getResourceManager()
					);
		}
	}

}

}
