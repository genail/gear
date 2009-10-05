/*
 * Viewport.h
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#include <ClanLib/display.h>

class Viewport {
	public:
		Viewport();
		virtual ~Viewport();

		void prepareGC(CL_GraphicContext &p_gc);
		void finalizeGC(CL_GraphicContext &p_gc);

		void attachTo(const CL_Pointf* p_point) { m_attachPoint = p_point; }

		void detach() { m_attachPoint = NULL; }

		float getScale() const { return m_scale; }

		void setScale(float p_scale) { m_scale = p_scale; }

		void update(unsigned int p_elapsedTime);

	private:
		/** Coordinates of view */
		float m_x, m_y, m_width, m_height;

		/** Stick point */
		const CL_Pointf *m_attachPoint;

		/** Scale (only when attached) */
		float m_scale;
};

#endif /* VIEWPORT_H_ */
