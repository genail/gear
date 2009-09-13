/*
 * Stage.h
 *
 *  Created on: 2009-09-09
 *      Author: chudy
 */

#ifndef STAGE_H_
#define STAGE_H_

#include <ClanLib/core.h>

#include "graphics/DebugLayer.h"

class Application;

class Stage {
	public:

		virtual ~Stage() {
		}

		static int getWidth() { return m_width; }
		static int getHeight() { return m_height; }

		static DebugLayer* getDebugLayer() { return m_debugLayer; }
		static CL_ResourceManager* getResourceManager() { return m_resourceManager; }


	private:

		/** Resource Manager */
		static CL_ResourceManager *m_resourceManager;

		/** Debug layer */
		static DebugLayer *m_debugLayer;

		/** Stage size */
		static int m_width, m_height;

		Stage() {}

		friend class Application;
};

#endif /* STAGE_H_ */
