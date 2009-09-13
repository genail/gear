/*
 * DebugLayer.h
 *
 *  Created on: 2009-09-11
 *      Author: chudy
 */

#ifndef DEBUGLAYER_H_
#define DEBUGLAYER_H_

#include <ClanLib/core.h>
#include "graphics/Drawable.h"

class DebugLayer : public Drawable {
	public:
		DebugLayer();
		virtual ~DebugLayer();

		virtual void draw(CL_GraphicContext &p_gc);

		void putMessage(const CL_String8 &p_key, const CL_String8 &p_value) { m_messages[p_key] = p_value; }
		void removeMessage(const CL_String8 &p_key) { m_messages.erase(m_messages.find(p_key)); }

	private:
		/** Debug messages */
		std::map<CL_String8, CL_String8> m_messages;

		/** Display font */
		CL_Font m_font;

		/** Font metrics */
		CL_FontMetrics m_fontMetrics;
};

#endif /* DEBUGLAYER_H_ */
