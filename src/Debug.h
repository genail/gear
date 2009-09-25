/*
 * Debug.h
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>

class Debug {
	public:
		virtual ~Debug() {}

#ifndef NDEBUG
		static std::ostream &out;
		static std::ostream &err;
#endif // NDEBUG

	private:
		Debug();
};

#endif /* DEBUG_H_ */
