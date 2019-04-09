/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <utility>

namespace LiFFT {
namespace types {

    /**
     * Wrapper used to add dimensionality Information to a class without using traits
     *
     * \tparam T_Base Base class to wrap
     * \tparam T_numDims number of dimensions this class should have
     */
    template< class T_Base, unsigned T_numDims >
    struct AddDimsWrapper: T_Base
    {
        using Base = T_Base;
        using Base::Base;
        static constexpr unsigned numDims = T_numDims;

        explicit AddDimsWrapper(const Base& base): Base(base){}
        explicit AddDimsWrapper(Base&& base): Base(std::move(base)){}
    };

}  // namespace types
}  // namespace LiFFT
