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

#include "mq_utils_math.h"
#include "mqFFT.h"

using namespace mq;

mqFFT::mqFFT()
{
    SetWindowSize(1024);
}

mqFFT::~mqFFT()
{
    Cleanup();
}

/*
 If sample rate, Fs = 44.1 kHz, and FFT size, N = 1024, then the resolution of your spectrum will be Fs / N = 44100 / 1024 = 43.1 Hz.
 In other words, each "bin" of your power spectrum is around 43 Hz wide.
 A 440 Hz component will therefore show up mostly in bin 10.
 If you need more resolution then you will have to increase N.
 */

void mqFFT::DoTransform(const unsigned int dataSize, const float* const data)
{
    int max = 0;
    
    // Convert input from float array to double array and apply window function
    for (unsigned int i = 0; i < _windowSize; i++)
    {
        max = min<unsigned int>(_windowSize, dataSize);
    
        if (i < dataSize)
        {
            _realDataForward[i] = (double)data[i] * HannFunction(i, max);
        }
        else
        {
            _realDataForward[i] = 0.0;
        }
    }
    
    fftw_execute(_FFTPlan);
    
    const unsigned int numberOfBins = GetBinCount();
    
    for (unsigned int i = 0; i < numberOfBins; i++)
    {
        //printf("re: %f - im: %f\n", out[i][0], out[i][1]);
        _magnitudes[i] = (float)sqrt(pow(_complexDataForward[i][0], 2) + pow(_complexDataForward[i][1], 2));
        //spectrum->at(i) = 10 * log(pow(out[i][0], 2) + pow(out[i][1], 2));
    }
}

const unsigned int mqFFT::GetWindowSize() const
{
    return _windowSize;
}

void mqFFT::SetWindowSize(const unsigned int windowSize)
{
    if (windowSize == _windowSize)
    {
        return;
    }
    
    _windowSize = windowSize;
    
    Cleanup();
    
    _realDataForward = (double*)fftw_malloc(sizeof(double) * _windowSize);
    _realDataInverse = (double*)fftw_malloc(sizeof(double) * _windowSize);
    _complexDataForward = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * _windowSize);
    _complexDataInverse = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * _windowSize);
    _FFTPlan = fftw_plan_dft_r2c_1d(_windowSize, _realDataForward, _complexDataForward, FFTW_MEASURE);
    _FFTPlanInverse = fftw_plan_dft_c2r_1d(_windowSize, _complexDataInverse, _realDataInverse, FFTW_MEASURE);
}

const unsigned int mqFFT::GetBinCount() const
{
    return (_windowSize / 2) + 1;
}

void mqFFT::Cleanup()
{
    delete _magnitudes;
    
    fftw_destroy_plan(_FFTPlan);
    fftw_destroy_plan(_FFTPlanInverse);
    fftw_free(_realDataForward);
    fftw_free(_realDataInverse);
    fftw_free(_complexDataForward);
    fftw_free(_complexDataInverse);
    // Null the pointers?
}
