/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace tiffWriter {

    /**
     * Wraps an Allocator into a std-compliant allocator
     */
    template< typename T, class T_Allocator >
    struct AllocatorWrapper
    {
        using value_type = T;
        using Allocator = T_Allocator;
        using pointer = T*;
        using const_pointer = const T*;
        typedef size_t size_type;

        Allocator m_alloc;

        AllocatorWrapper(const Allocator& alloc = Allocator()): m_alloc(alloc) {}

        pointer
        allocate(size_type n, const void* = 0)
        {
            pointer p;
            m_alloc.malloc(p, n*sizeof(T));
            return p;
        }

        void
        deallocate(pointer p, size_type)
        {
            m_alloc.free(p);
        }
    };

    template< typename T, class T_Allocator >
    AllocatorWrapper<T, T_Allocator>
    wrapAllocator(const T_Allocator& alloc)
    {
        return AllocatorWrapper<T, T_Allocator>(alloc);
    }

}  // namespace tiffWriter
