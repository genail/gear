/*
 * Checkpoint.h
 *
 *  Created on: 2009-09-19
 *      Author: chudy
 */

#ifndef CHECKPOINT_H_
#define CHECKPOINT_H_

#include <ClanLib/core.h>

class Checkpoint {
	public:
		Checkpoint(const CL_Rectf &p_rect);
		virtual ~Checkpoint();

		const CL_Rectf &getRect() { return m_rect; }

		bool isPassed() const { return m_passed; }

		void setPassed(bool p_passed) { m_passed = p_passed; }

	private:
		CL_Rectf m_rect;
		bool m_passed;
};

#endif /* CHECKPOINT_H_ */
