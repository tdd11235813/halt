/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/*
 * Workaround NVCC pretending to be gcc but not supporting float128
 * https://github.com/FFTW/fftw3/issues/18
 */
#ifdef __CUDACC__
    #define __ICC
#endif
#include "fftw3.h"
#ifdef __CUDACC__
    #undef __ICC
#endif
