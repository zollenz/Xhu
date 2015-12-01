/*
 * Copyright (C) 2015 by Martin Dejean
 *
 * This file is part of Modiqus.
 * Modiqus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Modiqus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Modiqus.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MQ_FFT_H__
#define __MQ_FFT_H__

#include "fftw3.h"

namespace mq
{
    class mqFFT
    {
    public:
        mqFFT();
        ~mqFFT();
        const unsigned int GetWindowSize() const;
        void SetWindowSize(const unsigned int size);
        const unsigned int GetBinCount() const;
        void DoTransform(const unsigned int dataSize, const float* const data);
    private:
        void Cleanup();
        unsigned int _windowSize;
        double* _realDataForward;
        double* _realDataInverse;
        double* _magnitudes;
        fftw_complex* _complexDataForward;
        fftw_complex* _complexDataInverse;
        fftw_plan _FFTPlan;
        fftw_plan _FFTPlanInverse;
    };
}
#endif //__MQ_FFT_H__
