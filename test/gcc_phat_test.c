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

/* GCC-PHAT tests. */
#include "speech_compass/dsp/gcc_phat.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "audio_to_tactile/src/dsp/complex.h"
#include "speech_compass/utility/logging.h"

// Generates a random value uniformly in [0, 1].
static float rand_unif(void) { return (float)rand() / RAND_MAX; }

// Tests `gcc_phat()` on random arrays of `num_coeffs` complex values.
static void TestGccPhat(int num_coeffs) {
  printf("TestGccPhat(%d)\n", num_coeffs);
  const float exponent = -0.3f;
  const int num_bytes = 2 * num_coeffs * sizeof(float);
  float* fft_sig1 = (float*)malloc(num_bytes);
  float* fft_sig2 = (float*)malloc(num_bytes);
  float* output = (float*)malloc(num_bytes);

  for (int i = 0; i < num_coeffs; ++i) {
    fft_sig1[2 * i] = rand_unif() - 0.5f;
    fft_sig1[2 * i + 1] = rand_unif() - 0.5f;
    fft_sig2[2 * i] = rand_unif() - 0.5f;
    fft_sig2[2 * i + 1] = rand_unif() - 0.5f;
  }

  gcc_phat(fft_sig1, fft_sig2, num_coeffs, exponent, output);

  for (int i = 0; i < num_coeffs; ++i) {
    const ComplexFloat* coeff1 = (const ComplexFloat*)&fft_sig1[2 * i];
    const ComplexFloat* coeff2 = (const ComplexFloat*)&fft_sig2[2 * i];
    ComplexFloat corr = ComplexFloatMul(*coeff1, ComplexFloatConj(*coeff2));
    float gain = powf(ComplexFloatAbs2(corr), exponent);
    ComplexFloat expected = ComplexFloatMulReal(corr, gain);

    const ComplexFloat* actual = (const ComplexFloat*)&output[2 * i];
    CHECK(fabs(actual->real - expected.real) <= 1e-5f);
    CHECK(fabs(actual->imag - expected.imag) <= 1e-5f);
  }

  free(output);
  free(fft_sig2);
  free(fft_sig1);
}

int main(int argc, char** argv) {
  srand(0);
  TestGccPhat(3);
  TestGccPhat(8);
  TestGccPhat(9);
  TestGccPhat(10);
  TestGccPhat(11);

  puts("PASS");
  return EXIT_SUCCESS;
}
