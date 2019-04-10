/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "testUtils.hpp"
#include "libLiFFT/FFT.hpp"
#include "libLiFFT/mem/DataContainer.hpp"
#include "libLiFFT/types/View.hpp"
#include "libLiFFT/types/SliceView.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/int.hpp>

namespace LiFFTTest {

    BOOST_AUTO_TEST_SUITE(Wrappers)

    BOOST_AUTO_TEST_CASE(View)
    {
        using Extents = LiFFT::types::Vec<2>;
        const Extents size(100u, 120u);
        const Extents offset(10u, 5u);
        const Extents viewSize(40u, 65u);
        auto data = LiFFT::mem::RealContainer<2, float>(size);
        auto view = LiFFT::types::makeView(data, LiFFT::types::makeRange(offset, viewSize));
        Extents idx, viewIdx;
        for(idx[0] = offset[0], viewIdx[0] = 0; viewIdx[0] < viewSize[0]; ++idx[0], ++viewIdx[0])
            for(idx[1] = offset[1], viewIdx[1] = 0; viewIdx[1] < viewSize[1]; ++idx[1], ++viewIdx[1])
            {
                float value = rand() / RAND_MAX;
                view(viewIdx) = value;
                BOOST_REQUIRE_EQUAL(data(idx), view(viewIdx));
                BOOST_REQUIRE_EQUAL(data(idx), value);
            }
    }

    unsigned getIdx(unsigned idx, unsigned fixedDim)
    {
        return idx >= fixedDim ? idx + 1 : idx;
    }

    using boost::mpl::int_;
    using SliceViewArgs = boost::mpl::list< int_<0>, int_<1>, int_<2> >;

    BOOST_AUTO_TEST_CASE_TEMPLATE(SliceView, T_FixedDim, SliceViewArgs)
    {
        using Extents3 = LiFFT::types::Vec<3>;
        using Extents2 = LiFFT::types::Vec<2>;
        const Extents3 size(100u, 120u, 214u);
        const Extents3 offset(10u, 5u, 23u);
        const Extents2 viewSize(40u, 65u);
        auto data = LiFFT::mem::RealContainer<3, float>(size);
        auto view = LiFFT::types::makeSliceView<T_FixedDim::value>(data, LiFFT::types::makeRange(offset, viewSize));
        Extents3 idx;
        Extents2 viewIdx;
        idx[T_FixedDim::value] = offset[T_FixedDim::value];
        unsigned idxOut = getIdx(0, T_FixedDim::value);
        unsigned idxIn = getIdx(1, T_FixedDim::value);
        for(idx[idxOut] = offset[idxOut], viewIdx[0] = 0; viewIdx[0] < viewSize[0]; ++idx[idxOut], ++viewIdx[0])
            for(idx[idxIn] = offset[idxIn], viewIdx[1] = 0; viewIdx[1] < viewSize[1]; ++idx[idxIn], ++viewIdx[1])
            {
                float value = rand() / RAND_MAX;
                view(viewIdx) = value;
                BOOST_REQUIRE_EQUAL(data(idx), view(viewIdx));
                BOOST_REQUIRE_EQUAL(data(idx), value);
            }
    }

    BOOST_AUTO_TEST_CASE(DataWrappers)
    {
        const unsigned size = 100u;
        using Extents = LiFFT::types::Vec<3>;
        using FFT = LiFFT::FFT_3D_R2C_F<>;
        auto input = FFT::wrapInput(
                        LiFFT::mem::RealContainer<3, float>(
                                Extents(size, size, size)
                        )
                     );
        auto data = LiFFT::mem::RealContainer<3, float>(
                Extents(size, size, size)
        );
        auto output = FFT::wrapInput(data);

        Extents idx = Extents::all(0u);
        const float val = 1337;
        const float val2 = 1338;
        auto acc = LiFFT::traits::getIdentityAccessor(input);
        auto acc2 = LiFFT::traits::getIdentityAccessor(output);
        auto acc3 = LiFFT::traits::getIdentityAccessor(data);
        for(unsigned i=0; i<4; i++){
            input(idx) = val;
            output(idx) = val;
            BOOST_REQUIRE_EQUAL(input(idx), val);
            BOOST_REQUIRE_EQUAL(output(idx), val);
            BOOST_REQUIRE_EQUAL(acc3(idx, data), val);
            acc(idx, input) = val2;
            acc2(idx, output) = val2;
            BOOST_REQUIRE_EQUAL(acc(idx, input), val2);
            BOOST_REQUIRE_EQUAL(input(idx), val2);
            BOOST_REQUIRE_EQUAL(acc2(idx, output), val2);
            BOOST_REQUIRE_EQUAL(acc3(idx, data), val2);
            BOOST_REQUIRE_EQUAL(output(idx), val2);
            if(i<3)
                idx[i] = size/2;
        }
    }

    BOOST_AUTO_TEST_SUITE_END()

}  // namespace LiFFTTest
