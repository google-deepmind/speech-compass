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


#include "tdoa.h"

void gcc_phat_arm_dsp(float* sig1, float* sig2, float* output,
                      int num_samples) {
  float32_t buffer1[num_samples];
  float32_t buffer2[num_samples];
  float32_t fft_sig1_buffer[num_samples];
  float32_t fft_sig2_buffer[num_samples];

  /* Copy sig1 and sig2 into buffer since fft modifies the input signal. */
  arm_copy_f32((float32_t*)sig1, buffer1, num_samples);
  arm_copy_f32((float32_t*)sig2, buffer2, num_samples);

  /* Perform real fft for the two signals. */
  arm_rfft_fast_f32(&rfft, buffer1, fft_sig1_buffer, 0);
  arm_rfft_fast_f32(&rfft, buffer2, fft_sig2_buffer, 0);

  gcc_phat_arm_dsp_fft_in(fft_sig1_buffer, fft_sig2_buffer, output,
                          num_samples);
}

void gcc_phat_arm_dsp_fft_in(float* sig1_fft, float* sig2_fft, float* output,
                             int transform_size) {
  float32_t fft_complex_conj_buffer[transform_size];
  float32_t multiply_fft_sig1_by_complex_conj_sig2[transform_size];
  float32_t abs_multiply_fft_sig1_by_complex_conj_sig2[transform_size / 2];

  /* Normalization exponent. Partial normalization makes a better GCC by
   * improving the noise robustness. */
  const float norm_exponent = -0.3f;

  /* Take the complex conjugate of the second signal. Process half of the points
   * in frequency domain because of symmetry. */
  arm_cmplx_conj_f32(fft_sig2_buffer, fft_complex_conj_buffer,
                     transform_size / 2);

  /* multiply complex conjugate by the rfft of the first signal. */
  arm_cmplx_mult_cmplx_f32(fft_sig1_buffer, fft_complex_conj_buffer,
                           multiply_fft_sig1_by_complex_conj_sig2,
                           transform_size / 2);

  /* Calculate the magnitude squared (abs), which is real value. To make it
   * faster avoid taking a square root, so its not exactly abs. */
  arm_cmplx_mag_squared_f32(multiply_fft_sig1_by_complex_conj_sig2,
                            abs_multiply_fft_sig1_by_complex_conj_sig2,
                            transform_size / 2);

  /* TODO(artemd): Add cut off frequencies to restrict audio to speech range.
   * Alternatively, apply a frequency weighting function. */

  /* Use FastPow() for partial normalization, pow() is too slow and we
   * don't need all the precision. */
  for (int i = 0; i < transform_size / 2; ++i) {
    abs_multiply_fft_sig1_by_complex_conj_sig2[i] =
        FastPow(abs_multiply_fft_sig1_by_complex_conj_sig2[i], norm_exponent);
  }
  /* Do the multiplication */
  arm_cmplx_mult_real_f32(multiply_fft_sig1_by_complex_conj_sig2,
                          abs_multiply_fft_sig1_by_complex_conj_sig2, output,
                          transform_size / 2);
}

int delay_to_azimuth_global_degrees(float delay_sec, float distance_mics_mm,
                                    float coordinate_transform_degrees) {
  const float32_t max_delay_sec =
      (distance_mics_mm) / speed_of_sound_mm_per_sec;

  /* Limit the max delay, based on the distance between the microphones.
     At 60 mm spacing and 44100 Hz sampling, max delay is 7.7 samples. */
  if (delay_sec > max_delay_sec) {
    delay_sec = max_delay_sec;
  }

  float32_t angle_deg_local = atan(delay_sec / max_delay_sec);
  angle_deg_local = angle_deg_local * (180.0f / PI);

  /* Return -1 if the angles are outside of [0, 45] range.
   * Otherwise the TDOA will be saturated by such angles */
  if (angle_deg_local <= 0.0f || angle_deg_local >= 45.0f) {
    return -1;
  }

  float32_t angle_deg_global =
      fabs((2 * angle_deg_local) + coordinate_transform_degrees);

  /* Deal with discontinuity at 0 or 360 degrees.*/
  if (angle_deg_global == 360.0f) {
    angle_deg_global = 0.0f;
  }
  return (int)angle_deg_global;
}

float extract_delay_sec_gcc(float* cross_correlation, int num_samples,
                            int sampling_rate_sec, float* max) {
  uint32_t max_index;
  float32_t delay_sec;
  arm_max_f32((float32_t*)cross_correlation, num_samples, &max, &max_index);

  delay_sec = (float32_t)max_index / sampling_rate_sec;

  return (float)delay_sec;
}
