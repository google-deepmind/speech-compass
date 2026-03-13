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

/* DAS beamformer tests */
#include "speech_compass/beam/das_beamformer.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "speech_compass/utility/logging.h"

/* Test the conversion from degrees to delay samples. */
static void TestConvertLocalDegeeToDelaySamples(void) {
  puts("TestConvertLocalDegeeToDelaySamples");
  const int kTol = 1;  // Allow a difference of at most 1 sample.
  const int sampling_rate_hz = 44100;
  const float mic_spacing_m = 0.08f;
  int delay_samples;

  delay_samples = das_beamformer_convert_local_degree_to_delay_samples(
      mic_spacing_m, 0.0f, sampling_rate_hz);
  CHECK(abs(delay_samples - 10) <= kTol);
  delay_samples = das_beamformer_convert_local_degree_to_delay_samples(
      mic_spacing_m, 90.0f, sampling_rate_hz);
  CHECK(abs(delay_samples - 0) <= kTol);
  delay_samples = das_beamformer_convert_local_degree_to_delay_samples(
      mic_spacing_m, 180.0f, sampling_rate_hz);
  CHECK(abs(delay_samples - (-10)) <= kTol);
  delay_samples = das_beamformer_convert_local_degree_to_delay_samples(
      mic_spacing_m, -90.0f, sampling_rate_hz);
  CHECK(abs(delay_samples - 0) <= kTol);
  delay_samples = das_beamformer_convert_local_degree_to_delay_samples(
      mic_spacing_m, 720.0f, sampling_rate_hz);
  CHECK(abs(delay_samples - 10) <= kTol);
}

static void TestShift(void) {
  puts("TestShift");

  const int num_samples = 19;

  /* Make an arbitrary signal */
  static const int16_t mic_sig[19] = {1,  2,  3,  4,  5,  6,  7,  8,  9, 0,
                                      -1, -2, -3, -4, -5, -6, -7, -8, -9};
  int16_t shifted_mic_sig[19] = {0};

  /* Check few values for the right shift */
  das_beamformer_shift(mic_sig, shifted_mic_sig, num_samples, 2);
  CHECK(shifted_mic_sig[0] == 0);
  CHECK(shifted_mic_sig[2] == 1);

  /* Check few values for the left shift */
  das_beamformer_shift(mic_sig, shifted_mic_sig, num_samples, -2);
  CHECK(shifted_mic_sig[18] == 0);
  CHECK(shifted_mic_sig[16] == -9);
}

/* Checks few values for two mic beamforming. We add the prev and next
 * buffers to fill in shifted values. */
static void TestBeamformTwoMics(void) {
  puts("TestBeamformTwoMics");

  static const int16_t mic_ref_sig[19] = {1,  2,  3,  4,  5,  6,  7,  8,  9, 0,
                                          -1, -2, -3, -4, -5, -6, -7, -8, -9};
  static const int16_t mic_2_sig[19] = {-9, -8, -7, -6, -5, -4, -3, -2, -1, 0,
                                        1,  2,  3,  4,  5,  6,  7,  8,  9};
  static const int16_t mic_3_sig[19] = {-5, 10, -7, -6, -5, -4, -3, -2, -1, 0,
                                        1,  2,  3,  4,  5,  6,  7,  8,  9};
  static const int16_t mic_4_sig[19] = {2, 3, -7, -6, -5, -4, -3, -2, -1, 0,
                                        1, 2, 3,  4,  5,  6,  7,  8,  9};
  int16_t beamformed_sig[19] = {0};

  const int number_of_samples = 19;

  /* Rest the saved states to zero */
  das_beamformer_reset_states();

  /* First check without storing previous and next buffer states. In such case
   * shifted samples are replaced by zeros.  */
  das_beamformer_beamform_2_mics(mic_ref_sig, mic_2_sig, beamformed_sig,
                                 number_of_samples, 2);
  CHECK(beamformed_sig[0] == 0);
  CHECK(beamformed_sig[2] == -3);

  /* Store states from previous and next buffers. */
  das_beamformer_store_prev_state(mic_2_sig, mic_3_sig, mic_4_sig,
                                  number_of_samples);
  das_beamformer_store_next_state(mic_2_sig, mic_3_sig, mic_4_sig,
                                  number_of_samples);

  das_beamformer_beamform_2_mics(mic_ref_sig, mic_2_sig, beamformed_sig,
                                 number_of_samples, 2);

  CHECK(beamformed_sig[0] == 4);
  CHECK(beamformed_sig[2] == -3);

  das_beamformer_beamform_2_mics(mic_ref_sig, mic_2_sig, beamformed_sig,
                                 number_of_samples, -2);

  CHECK(beamformed_sig[0] == -3);
  CHECK(beamformed_sig[2] == -1);
}

/* Checks few values for 4 microphone beamforming. */
static void TestBeamformFourMics(void) {
  puts("TestBeamformFourMics");

  static const int16_t mic_ref_sig[19] = {1,  2,  3,  4,  5,  6,  7,  8,  9, 0,
                                          -1, -2, -3, -4, -5, -6, -7, -8, -9};
  static const int16_t mic_2_sig[19] = {-9, -8, -7, -6, -5, -4, -3, -2, -1, 0,
                                        1,  2,  3,  4,  5,  6,  7,  8,  9};
  static const int16_t mic_3_sig[19] = {-5, 10, -7, -6, -5, -4, -3, -2, -1, 0,
                                        1,  2,  3,  4,  5,  6,  7,  8,  9};
  static const int16_t mic_4_sig[19] = {2, 3, -7, -6, -5, -4, -3, -2, -1, 0,
                                        1, 2, 3,  4,  5,  6,  7,  8,  9};
  int16_t beamformed_sig[19] = {0};

  const int number_of_samples = 19;

  /* Set the saved states to zero */
  das_beamformer_reset_states();

  /* Check without stored states, this assumes shifted samples are zeros. */
  das_beamformer_beamform_4_mics(mic_ref_sig, mic_2_sig, mic_3_sig, mic_4_sig,
                                 beamformed_sig, number_of_samples, -2, 3, 0);

  CHECK(beamformed_sig[0] == -1);
  CHECK(beamformed_sig[2] == -2);
  CHECK(beamformed_sig[18] == 1);

  /* Store states from previous and next buffers and check again */
  das_beamformer_store_prev_state(mic_2_sig, mic_3_sig, mic_4_sig,
                                  number_of_samples);
  das_beamformer_store_next_state(mic_2_sig, mic_3_sig, mic_4_sig,
                                  number_of_samples);

  das_beamformer_beamform_4_mics(mic_ref_sig, mic_2_sig, mic_3_sig, mic_4_sig,
                                 beamformed_sig, 19, -2, 3, 0);

  CHECK(beamformed_sig[0] == 0);
  CHECK(beamformed_sig[1] == 1);
  CHECK(beamformed_sig[18] == 0);
}

/* Test Rms calculation. */
static void TestRms(void) {
  puts("TestRms");

  static const float kTol = 0.5f;  // Allow some error tolerance with floats.
  const float kPi = 3.141592653589793f;

  /* Generate an arbitrary sin wave */
  const int16_t amplitude = 300;
  int16_t test_signal[512];
  for (int i = 0; i < 512; ++i) {
    test_signal[i] = amplitude * sinf((kPi / 32.0f) * (float)i);
  }

  float rms = das_beamformer_calculate_rms(test_signal, 512);

  /* Analytical RMS value is peak * 1/sqrt(2) for a sin */
  double rms_true = (double)amplitude * 1.0 / sqrt(2.0);
  CHECK(fabs(rms - (float)rms_true) <= kTol);
}

/* Test RMS to Dbfs conversion. */
static void TestDbfs(void) {
  puts("TestDbfs");

  static const float kTol = 1.0e-3f;  // Allow some error tolerance with floats.

  const float max_rms = 32767.0f / sqrtf(2.0f);

  /* Max rms should be at 0 dbfs */
  float dbfs = das_beamformer_convert_rms_to_dbfs(max_rms);
  CHECK(fabs(dbfs - 0.0f) <= kTol);

  /* 50% max rms should be at -6 dbfs. */
  dbfs = das_beamformer_convert_rms_to_dbfs(max_rms / 2.0f);
  CHECK(fabs(dbfs - -6.0210f) <= kTol);

  /* 10% max should be at -20 dbfs. */
  dbfs = das_beamformer_convert_rms_to_dbfs(max_rms / 10.0f);
  CHECK(fabs(dbfs - -20.0f) <= kTol);

  /* 1% max should be at -40 dbfs. */
  dbfs = das_beamformer_convert_rms_to_dbfs(max_rms / 100.0f);
  CHECK(fabs(dbfs - -40.0f) <= kTol);
}

int main(int argc, char** argv) {
  TestConvertLocalDegeeToDelaySamples();
  TestShift();
  TestBeamformTwoMics();
  TestBeamformFourMics();
  TestRms();
  TestDbfs();

  puts("PASS");
  return EXIT_SUCCESS;
}
