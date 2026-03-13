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


/* FAS beamformer tests. */

#include "speech_compass/beam/fas_beamformer.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "speech_compass/utility/logging.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void TestRectCoordToAngles(void) {
  puts("TestRectCoordToAngles");
  const float kTol = 1e-5f;
  const float xy_coord_m[8] = {1.0f,  1.0f,  -1.0f, 1.0f,
                               -1.0f, -1.0f, 1.0f,  -1.0f};
  float mic_angles_rad[4] = {0};
  float r_m;
  fas_beamformer_four_mic_rectangular_coord_to_angles(xy_coord_m, &r_m,
                                                      mic_angles_rad);

  const float true_angles_rad[4] = {
      (float)M_PI / 4.0f, 3.0f * (float)M_PI / 4.0f, 5.0f * (float)M_PI / 4.0f,
      7.0f * (float)M_PI / 4.0f};
  for (int i = 0; i < 4; ++i) {
    CHECK(fabs(mic_angles_rad[i] - true_angles_rad[i]) <= kTol);
  }
  CHECK(fabs(r_m - sqrt(2.0)) <= kTol);
}

static void TestComputeSteeringVector(void) {
  puts("TestComputeSteeringVector");
  const float kTol = 1e-5f;
  const int num_complex_freq_bins = 256;
  float st_vec0[num_complex_freq_bins * 2];
  float st_vec1[num_complex_freq_bins * 2];
  float st_vec2[num_complex_freq_bins * 2];
  float st_vec3[num_complex_freq_bins * 2];
  const float sampling_rate_hz = 44100.0f;
  float azimuth_steer_deg = 0.0f;
  // Assume the actual mic geometry on the phonecase.
  const float xy_coord_m[8] = {-0.04f, -0.0835f, 0.040f, -0.0835f,
                               -0.04f, -0.0835f, 0.04f,  0.0835f};
  float mic_angles_rad[4] = {0};
  float r_m;
  fas_beamformer_four_mic_rectangular_coord_to_angles(xy_coord_m, &r_m,
                                                      mic_angles_rad);

  for (int i = 0; i < 12; ++i) {
    fas_beamformer_compute_four_mic_non_uniform_circular_steering_vector(
        azimuth_steer_deg, sampling_rate_hz, mic_angles_rad, r_m, st_vec0,
        st_vec1, st_vec2, st_vec3, num_complex_freq_bins);
    azimuth_steer_deg += 30.0f;

    CHECK(st_vec0[1] == 0);

    // Take Y(f) = W(f)^H * W(f), where W is the steering vector.
    // This should output constant value accross all frequency bins.
    for (int i = 0; i < num_complex_freq_bins; ++i) {
      float arr[] = {st_vec0[(i * 2)], st_vec0[(i * 2) + 1],
                     st_vec1[(i * 2)], st_vec1[(i * 2) + 1],
                     st_vec2[(i * 2)], st_vec2[(i * 2) + 1],
                     st_vec3[(i * 2)], st_vec3[(i * 2) + 1]};
      float arr_conj[] = {st_vec0[(i * 2)], -1 * st_vec0[(i * 2) + 1],
                          st_vec1[(i * 2)], -1 * st_vec1[(i * 2) + 1],
                          st_vec2[(i * 2)], -1 * st_vec2[(i * 2) + 1],
                          st_vec3[(i * 2)], -1 * st_vec3[(i * 2) + 1]};
      float mult[8];
      fas_beamformer_complex_multiply(arr, arr_conj, mult, 4);
      float weight_real = 0.0f;
      float weight_imag = 0.0f;
      for (int j = 0; j < 4; ++j) {
        weight_real += mult[(j * 2)];
        weight_imag += mult[(j * 2) + 1];
      }
      CHECK(fabs(weight_real - 1.0f) <= kTol);
      CHECK(fabs(weight_imag - 0.0f) <= kTol);
    }
  }
}

static void TestComputeHammingWindow(void) {
  puts("TestComputeSteeringVector");
  const float kTol = 1e-5f;
  float window[512];
  fas_beamformer_compute_hamming_window(window, 512);

  // Check center.
  CHECK(fabs(window[256] - 1.0f) <= kTol);

  for (int i = 1, j = 512 - 1; i < j; ++i, --j) {
    // Check symmetry.
    CHECK(fabsf(window[i] - window[j]) <= kTol);
    // Check monotonicity.
    CHECK(window[i] < window[i + 1]);
    // Check that all samples are positive.
    CHECK(window[i] >= 0.0f);
  }
}

static void TestComplexMultiply(void) {
  puts("TestComplexMultiply");
  const float kTol = 1e-5f;
  const int num_complex_samples = 2;
  // Formula: (a + ib) (c + id) = (ac - bd) + i(ad + bc)
  const float a = 2.0f;
  const float b = 3.5f;
  const float c = 4.4f;
  const float d = -5.0f;
  const float num_real = (a * c) - (b * d);
  const float num_imag = (a * d) + (b * c);
  float arr_a[4] = {a, b, a, b};
  float arr_b[4] = {c, d, c, d};
  float arr_c[4];

  // Check with the formula.
  fas_beamformer_complex_multiply(arr_a, arr_b, arr_c, num_complex_samples);
  CHECK(fabs(arr_c[0] - num_real) <= kTol);
  CHECK(fabs(arr_c[1] - num_imag) <= kTol);
  CHECK(fabs(arr_c[2] - num_real) <= kTol);
  CHECK(fabs(arr_c[3] - num_imag) <= kTol);
}

static void TestDftSimpleValues(void) {
  puts("TestDftSimpleValues");
  const float kTol = 5e-5f;
  // Test with a simple signal.
  float simple_sig[8] = {12.0f, 1.0f, 2.0f, 3.0f, 4.0f, -5.0f, -3.0f, 5.0f};
  const float true_out[8] = {19.0f, 11.0f, 13.656854f, -7.828427f,
                             17.0f, 12.0f, 2.343146f,  2.171573f};
  float complex_out[8] = {};
  fas_beamformer_rdft(simple_sig, complex_out, 8);

  for (int i = 0; i < 8; ++i) {
    CHECK(fabs(complex_out[i] - true_out[i]) <= kTol);
  }
}

static void TestDftSinFrequency(void) {
  puts("TestDftSinFrequency");
  // Test with a more realistic sin wav signal.
  // Generate an arbitrary sin wave.
  const float amplitude = 300.0f;
  float complex_out_sin[512] = {0};
  float test_sin[512];
  for (int i = 0; i < 512; ++i) {
    test_sin[i] = amplitude * sinf(((float)M_PI / 32.0f) * (float)i);
  }

  fas_beamformer_rdft(test_sin, complex_out_sin, 512);

  const float kTol = amplitude * 1e-2f;
  const float kExpectedCoeff = -512 * amplitude / 2;

  CHECK(fabs(complex_out_sin[2 * 8]) < kTol);
  CHECK(fabs(complex_out_sin[2 * 8 + 1] - kExpectedCoeff) < kTol);
  for (int i = 0; i < 256; ++i) {
    CHECK(fabs(complex_out_sin[2 * i]) < kTol);
    if (i != 8) {
      CHECK(fabs(complex_out_sin[2 * i + 1]) < kTol);
    }
  }
}

static void TestInverseDft(void) {
  puts("TestInverseDft");
  const float kTol = 5e-5f;
  // Test with a simple signal.
  float simple_sig[8] = {12.0f, 1.0f, 2.0f, 3.0f, 4.0f, -5.0f, -3.0f, 5.0f};
  float complex_out[8] = {};
  float real_out[8] = {};
  fas_beamformer_rdft(simple_sig, complex_out, 8);
  fas_beamformer_irdft(complex_out, real_out, 8);
  for (int i = 0; i < 8; ++i) {
    CHECK(fabs(simple_sig[i] - real_out[i]) <= kTol);
  }
}

static void TestApplyFasBeamformer(void) {
  puts("TestApplyFasBeamformer");
  // Compute Steering vector.
  const int num_complex_freq_bins = 256;
  float st_vec0[num_complex_freq_bins * 2];
  float st_vec1[num_complex_freq_bins * 2];
  float st_vec2[num_complex_freq_bins * 2];
  float st_vec3[num_complex_freq_bins * 2];
  const float sampling_rate_hz = 44100.0f;
  const float azimuth_steer_deg = 0.0f;

  // Assume the actual mic geometry on the phonecase.
  const float xy_coord_m[8] = {-0.04f, -0.0835f, 0.040f, -0.0835f,
                               -0.04f, -0.0835f, 0.04f,  0.0835f};
  float mic_angles_rad[4] = {0};
  float r_m;
  fas_beamformer_four_mic_rectangular_coord_to_angles(xy_coord_m, &r_m,
                                                      mic_angles_rad);

  fas_beamformer_compute_four_mic_non_uniform_circular_steering_vector(
      azimuth_steer_deg, sampling_rate_hz, mic_angles_rad, r_m, st_vec0,
      st_vec1, st_vec2, st_vec3, num_complex_freq_bins);

  // Generate test signals. Let's test with a narrowband sinusoid.
  float amplitude = 100.0f;
  float m0[512];
  float m1[512];
  float m2[512];
  float m3[512];

  const float w = ((float)M_PI / 32.0f);
  for (int i = 0; i < 512; ++i) {
    m0[i] = amplitude * cosf(w * (float)i);
    m1[i] = amplitude * sinf(w * (float)i);
    m2[i] = amplitude * cosf(w * (float)i);
    m3[i] = amplitude * sinf(w * (float)i);
  }

  // Compute DFT of signals.
  float dft0[512] = {0};
  float dft1[512] = {0};
  float dft2[512] = {0};
  float dft3[512] = {0};
  fas_beamformer_rdft(m0, dft0, 512);
  fas_beamformer_rdft(m1, dft1, 512);
  fas_beamformer_rdft(m2, dft2, 512);
  fas_beamformer_rdft(m3, dft3, 512);

  // Apply beamformer to test signals.
  float real_out[512] = {0};
  fas_beamformer_apply(st_vec0, st_vec1, st_vec2, st_vec3, dft0, dft1, dft2,
                       dft3, num_complex_freq_bins, real_out);

  // Check that there are no consecutive zeros in the output, since the test
  // signal is sinusoid.
  for (int i = 0; i < 512 - 1; ++i) {
    CHECK(real_out[i] != 0 && real_out[i + 1] != 0);
  }

  // Test the resulting output signal.
  // FAS beamformer should conserve the original frequencies.
  fas_beamformer_rdft(real_out, dft0, 512);

  int max_index = 0;
  float max_magnitude = 0.0f;
  for (int i = 0; i < 512 / 2; ++i) {
    float magnitude = sqrtf(dft0[(2 * i)] * dft0[(2 * i)] +
                            dft0[(2 * i) + 1] * dft0[(2 * i) + 1]);
    if (magnitude > max_magnitude) {
      max_magnitude = magnitude;
      max_index = i;
    }
  }

  // Check that the frequency domain is similar to the test signal. The peak
  // should be at 8th bin.
  CHECK(max_index == 8);
}

int main(int argc, char** argv) {
  TestComplexMultiply();
  TestDftSimpleValues();
  TestDftSinFrequency();
  TestInverseDft();
  TestRectCoordToAngles();
  TestComputeSteeringVector();
  TestComputeHammingWindow();
  TestApplyFasBeamformer();

  puts("PASS");
  return EXIT_SUCCESS;
}
