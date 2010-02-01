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

#include <unistd.h>
#include <boost/test/unit_test.hpp>

#include <ClanLib/core.h>

#include "logic/race/level/Object.h"
#include "common.h"

/*
 * Minimal testing facility:
 *
 * BOOST_CHECK( predicate )
 * BOOST_REQUIRE( predicate )
 * BOOST_ERROR( message )
 * BOOST_FAIL( message )
 *
 * Test tools:
 * http://www.boost.org/doc/libs/1_34_0/libs/test/doc/components/test_tools/index.html
 */

BOOST_AUTO_TEST_SUITE(ObjectTest)

BOOST_AUTO_TEST_CASE(collision1)
{
	const CL_Pointf pts1[] = {
			CL_Pointf(0.0f, 0.0f),
			CL_Pointf(2.0f, 0.0f),
			CL_Pointf(2.0f, 2.0f),
			CL_Pointf(0.0f, 2.0f)
	};

	const CL_Pointf pts2[] = {
			CL_Pointf(1.0f, 1.0f),
			CL_Pointf(3.0f, 1.0f),
			CL_Pointf(3.0f, 3.0f),
			CL_Pointf(1.0f, 3.0f)
	};

	Race::Object obj1(pts1, 4);
	Race::Object obj2(pts2, 4);

	const std::vector<CL_CollidingContours> & contours =
			obj1.collide(obj2.getCollisionOutline());

	const int count = static_cast<signed>(contours.size());
	for (int i = 0; i < count; ++i) {
		const CL_CollidingContours &cc = contours[i];

		const int ccCount = static_cast<signed>(cc.points.size());
		for (int j = 0; j < ccCount; ++j) {
			const CL_CollisionPoint &pt = cc.points[j];

			const std::vector<CL_Pointf> &c1pts = cc.contour1->get_points();
			const std::vector<CL_Pointf> &c2pts = cc.contour2->get_points();

			BOOST_CHECK(
					c1pts[pt.contour1_line_start] == CL_Pointf(2.0f, 0.0f) ||
					c1pts[pt.contour1_line_start] == CL_Pointf(2.0f, 2.0f)
			);

			BOOST_CHECK(
					c1pts[pt.contour1_line_end] == CL_Pointf(2.0f, 2.0f) ||
					c1pts[pt.contour1_line_end] == CL_Pointf(0.0f, 2.0f)
			);


			BOOST_CHECK(
					c2pts[pt.contour2_line_start] == CL_Pointf(1.0f, 1.0f) ||
					c2pts[pt.contour2_line_start] == CL_Pointf(1.0f, 3.0f)
			);

			BOOST_CHECK(
					c2pts[pt.contour2_line_end] == CL_Pointf(3.0f, 1.0f) ||
					c2pts[pt.contour2_line_end] == CL_Pointf(1.0f, 1.0f)
			);

		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
