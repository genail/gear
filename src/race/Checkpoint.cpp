/*
 * Checkpoint.cpp
 *
 *  Created on: 2009-09-19
 *      Author: chudy
 */

#include "Checkpoint.h"

Checkpoint::Checkpoint(const CL_Rectf &p_rect) :
	m_rect(p_rect),
	m_passed(false)
{
}

Checkpoint::~Checkpoint() {
}
