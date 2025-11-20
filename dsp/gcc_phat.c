// Copyright 2025 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ==============================================================================


#include "gcc_phat.h"

#include <math.h>

void gcc_phat(const float* fft_sig1, const float* fft_sig2, int num_coeffs,
              float exponent, float* output) {
  float x_real;
  float x_imag;
  float y_real;
  float y_imag;
  float corr_real;
  float corr_imag;
  float gain;

  // Compute 4 outputs at a time with an unrolled loop.
  for (int count = num_coeffs / 4; count; --count) {
    x_real = fft_sig1[0];
    x_imag = fft_sig1[1];
    y_real = fft_sig2[0];
    y_imag = fft_sig2[1];
    // Compute corr = x * conj(y).
    corr_real = x_real * y_real + x_imag * y_imag;
    corr_imag = x_imag * y_real - x_real * y_imag;
    // Compute gain = FastPow(|corr|^2, exponent).
    gain = pow(corr_real * corr_real + corr_imag * corr_imag, exponent);
    // Scale by the gain and store.
    output[0] = corr_real * gain;
    output[1] = corr_imag * gain;

    x_real = fft_sig1[2];
    x_imag = fft_sig1[3];
    y_real = fft_sig2[2];
    y_imag = fft_sig2[3];
    corr_real = x_real * y_real + x_imag * y_imag;
    corr_imag = x_imag * y_real - x_real * y_imag;
    gain = pow(corr_real * corr_real + corr_imag * corr_imag, exponent);
    output[2] = corr_real * gain;
    output[3] = corr_imag * gain;

    x_real = fft_sig1[4];
    x_imag = fft_sig1[5];
    y_real = fft_sig2[4];
    y_imag = fft_sig2[5];
    corr_real = x_real * y_real + x_imag * y_imag;
    corr_imag = x_imag * y_real - x_real * y_imag;
    gain = pow(corr_real * corr_real + corr_imag * corr_imag, exponent);
    output[4] = corr_real * gain;
    output[5] = corr_imag * gain;

    x_real = fft_sig1[6];
    x_imag = fft_sig1[7];
    y_real = fft_sig2[6];
    y_imag = fft_sig2[7];
    corr_real = x_real * y_real + x_imag * y_imag;
    corr_imag = x_imag * y_real - x_real * y_imag;
    gain = pow(corr_real * corr_real + corr_imag * corr_imag, exponent);
    output[6] = corr_real * gain;
    output[7] = corr_imag * gain;

    fft_sig1 += 8;
    fft_sig2 += 8;
    output += 8;
  }

  // If num_coeffs is not a multiple of 4, compute remaining output here.
  for (int count = num_coeffs % 4; count; --count) {
    x_real = *fft_sig1++;
    x_imag = *fft_sig1++;
    y_real = *fft_sig2++;
    y_imag = *fft_sig2++;
    corr_real = x_real * y_real + x_imag * y_imag;
    corr_imag = x_imag * y_real - x_real * y_imag;
    gain = pow(corr_real * corr_real + corr_imag * corr_imag, exponent);
    *output++ = corr_real * gain;
    *output++ = corr_imag * gain;
  }
}
