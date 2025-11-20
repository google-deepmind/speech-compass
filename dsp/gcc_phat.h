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


/* Performs the frequency-domain computation for generalized cross correlation
 * with (partial) phase normalization (GCC-PHAT) of two signals' transforms
 *
 *     output = fft_sig1 * conj(fft_sig2)
 *              * pow(|fft_sig1 * conj(fft_sig2)|^2, exponent).
 *
 * fft_sig1, fft_sig2, output should each point to an array of 2 * num_coeffs
 * floats, representing num_coeffs complex spectral coefficients.
 *
 * NOTE: This function performs only the frequency-domain calculation for
 * GCC-PHAT. For the full calculation, (1) compute FFTs of the two input
 * signals, (2) call `gcc_phat()`, then (3) compute the IFFT of the output.
 *
 * The `exponent` arg determines the normalization: exponent = -0.5 is full
 * normalization (GCC-PHAT), exponent = 0.0 is no normalization (basic cross
 * correlation). An intermediate value like -0.3 is recommended.
 *
 * Reference:
 * http://www.xavieranguera.com/phdthesis/node92.html
 */

#ifndef MULTI_MIC_AUDIO_GCC_PHAT_H_
#define MULTI_MIC_AUDIO_GCC_PHAT_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void gcc_phat(const float* fft_sig1, const float* fft_sig2, int num_coeffs,
              float exponent, float* output);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // MULTI_MIC_AUDIO_GCC_PHAT_H_
