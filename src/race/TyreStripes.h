/*
 * TyreStripes.h
 *
 *  Created on: 2009-10-02
 *      Author: chudy
 */

#ifndef TYRESTRIPES_H_
#define TYRESTRIPES_H_

#include <list>
#include <ClanLib/core.h>

#include "graphics/Drawable.h"

class TyreStripes : public Drawable {

		struct Stripe {
				CL_Pointf m_from, m_to;

				Stripe(const CL_Pointf &p_from, const CL_Pointf &p_to) :
					m_from(p_from), m_to(p_to) {}
		};

	public:
		TyreStripes();
		virtual ~TyreStripes();

		void add(const CL_Pointf &p_from, const CL_Pointf &p_to);

		virtual void draw(CL_GraphicContext &p_gc);

	private:
		std::list<Stripe> m_stripes;
};

#endif /* TYRESTRIPES_H_ */
