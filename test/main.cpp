/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#define BOOST_TEST_MODULE "C++ Unit Tests for libLiFFT"
#include <boost/test/unit_test.hpp>
#include "testUtils.hpp"

struct TestFixture
{
    TestFixture()
    {
        LiFFTTest::init();
    }
    ~TestFixture()
    {
        LiFFTTest::finalize();
    }
};

BOOST_GLOBAL_FIXTURE( TestFixture );
