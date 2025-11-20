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


#include "fas_beamformer.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>

/* math.h implementations often define M_PI, M_SQRT2, etc., but the C/C++
 * standards do not require it. This header ensures M_PI is defined.
 */
/* The constant pi. */
#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

// Define complex_multiply based on whether USE_CMSIS is enabled.
// CMSIS can accelerate compute, but can be used only on ARM Cores.

#ifdef USE_CMSIS
#define complex_multiply arm_cmplx_mult_cmplx_f32
#else
#define complex_multiply fas_beamformer_complex_multiply
#endif

void fas_beamformer_apply(const float* st_vec0, const float* st_vec1,
                          const float* st_vec2, const float* st_vec3,
                          const float* rfft0, const float* rfft1,
                          const float* rfft2, const float* rfft3,
                          size_t num_complex_bins, float* output) {
  // Complex temporary buffers.
  float buffer[kFilterBufferSize * 2] = {};
  float accum[kFilterBufferSize * 2] = {0};

  complex_multiply(st_vec0, rfft0, buffer, num_complex_bins);

  for (size_t i = 2; i < num_complex_bins * 2; ++i) {  // Skip the DC bin.
    accum[i] += buffer[i];
  }

  complex_multiply(st_vec1, rfft1, buffer, num_complex_bins);
  for (size_t i = 2; i < num_complex_bins * 2; ++i) {
    accum[i] += buffer[i];
  }

  complex_multiply(st_vec2, rfft2, buffer, num_complex_bins);
  for (size_t i = 2; i < num_complex_bins * 2; ++i) {
    accum[i] += buffer[i];
  }

  complex_multiply(st_vec3, rfft3, buffer, num_complex_bins);
  for (size_t i = 2; i < num_complex_bins * 2; ++i) {
    accum[i] += buffer[i];
  }

/* Do an inverse FFT. Set the flag to 1 for ifft. */
#ifdef USE_CMSIS
  arm_rfft_fast_f32(&rfft, accum, output, 1);
#else
  fas_beamformer_irdft(accum, output, num_complex_bins * 2);
#endif
}

void fas_beamformer_four_mic_rectangular_coord_to_angles(
    const float* xy_coord_m, float* r_m, float* mic_angles_rad) {
  for (int i = 0; i < 4; ++i) {
    mic_angles_rad[i] = atan2(xy_coord_m[i * 2 + 1], xy_coord_m[i * 2]);
    if (mic_angles_rad[i] < 0) {
      mic_angles_rad[i] = 2.0f * (float)M_PI - -1.0f * mic_angles_rad[i];
    }
  }
  // Calculate the radius of the circle based on mic0 (any mic works).
  r_m[0] = sqrtf(xy_coord_m[0] * xy_coord_m[0] + xy_coord_m[1] * xy_coord_m[1]);
}

void fas_beamformer_compute_four_mic_non_uniform_circular_steering_vector(
    float azimuth_deg, float sampling_rate_hz, const float* mic_angle_rad,
    const float r_m, float* st_vec0, float* st_vec1, float* st_vec2,
    float* st_vec3, size_t num_output_bins) {
  static const float speed_of_sound_m_per_sec = 343.0f;
  const float frequency_step = (float)(sampling_rate_hz) / num_output_bins;

  const float azimuth_rad = azimuth_deg * (float)(M_PI / 180.0f);
  const float step_rad_per_m =
      2 * (float)M_PI * frequency_step / speed_of_sound_m_per_sec;

  // TODO(artemd): consider adding float* st_matrix[4] = {st_vec0,..} to reduce
  // redundant code and/or add a st_vec struct.
  // TODO(artemd): generalize for any four microphone geometries.
  for (size_t i = 0; i < num_output_bins; ++i) {
    /* Use Euler's Formula: e^(jx) = cos(x) + j*sin(x) to calculate steering
     * weight for each frequency bin and microphone. */
    // Calculate real part of st_vec0
    st_vec0[(i * 2)] =
        cosf((i * step_rad_per_m) * r_m * cosf(azimuth_rad - mic_angle_rad[0]));
    // Calculate imaginary part of st_vec0:
    st_vec0[(i * 2) + 1] =
        sinf((i * step_rad_per_m) * r_m * cosf(azimuth_rad - mic_angle_rad[0]));
    // Repeat for other steering weights.
    st_vec1[(i * 2)] =
        cosf((i * step_rad_per_m) * r_m * cosf(azimuth_rad - mic_angle_rad[1]));
    st_vec1[(i * 2) + 1] =
        sinf((i * step_rad_per_m) * r_m * cosf(azimuth_rad - mic_angle_rad[1]));
    st_vec2[(i * 2)] =
        cosf((i * step_rad_per_m) * r_m * cosf(azimuth_rad - mic_angle_rad[2]));
    st_vec2[(i * 2) + 1] =
        sinf((i * step_rad_per_m) * r_m * cosf(azimuth_rad - mic_angle_rad[2]));
    st_vec3[(i * 2)] =
        cosf((i * step_rad_per_m) * r_m * cosf(azimuth_rad - mic_angle_rad[3]));
    st_vec3[(i * 2) + 1] =
        sinf((i * step_rad_per_m) * r_m * cosf(azimuth_rad - mic_angle_rad[3]));

    /* Normalize steering weights so power is the same as original. */
    /* Normalization formula: W / sqrt(W^H * W) */
    float arr[] = {st_vec0[(i * 2)], st_vec0[(i * 2) + 1],
                   st_vec1[(i * 2)], st_vec1[(i * 2) + 1],
                   st_vec2[(i * 2)], st_vec2[(i * 2) + 1],
                   st_vec3[(i * 2)], st_vec3[(i * 2) + 1]};
    // Complex conjugate.
    float arr_conj[] = {st_vec0[(i * 2)], -st_vec0[(i * 2) + 1],
                        st_vec1[(i * 2)], -st_vec1[(i * 2) + 1],
                        st_vec2[(i * 2)], -st_vec2[(i * 2) + 1],
                        st_vec3[(i * 2)], -st_vec3[(i * 2) + 1]};
    float mult_out[8];
    fas_beamformer_complex_multiply(arr, arr_conj, mult_out, 4);
    float weight = 0.0f;
    for (int j = 0; j < 4; ++j) {
      weight += mult_out[j * 2];
    }
    weight = sqrtf(weight);
    st_vec0[(i * 2)] /= weight;
    st_vec0[(i * 2) + 1] /= weight;
    st_vec1[(i * 2)] /= weight;
    st_vec1[(i * 2) + 1] /= weight;
    st_vec2[(i * 2)] /= weight;
    st_vec2[(i * 2) + 1] /= weight;
    st_vec3[(i * 2)] /= weight;
    st_vec3[(i * 2) + 1] /= weight;

    // Complex conjugate steering vectors since beamformer uses conjugated
    // vectors.
    st_vec0[(i * 2) + 1] = -st_vec0[(i * 2) + 1];
    st_vec1[(i * 2) + 1] = -st_vec1[(i * 2) + 1];
    st_vec2[(i * 2) + 1] = -st_vec2[(i * 2) + 1];
    st_vec3[(i * 2) + 1] = -st_vec3[(i * 2) + 1];
  }
}

void fas_beamformer_compute_hamming_window(float* output, size_t size) {
  // Use same constant a0 as used in numpy.
  const float a_0 = 25.0f / 46.0f;
  for (size_t i = 0; i < size; ++i) {
    output[i] = a_0 - (1.0f - a_0) * cosf((2 * (float)M_PI * i) / size);
  }
}

void fas_beamformer_complex_multiply(const float* a, const float* b,
                                     float* output,
                                     size_t num_complex_samples) {
  for (size_t i = 0; i < num_complex_samples; ++i) {
    output[(2 * i)] = a[(2 * i)] * b[(2 * i)] - a[(2 * i) + 1] * b[(2 * i) + 1];
    output[(2 * i) + 1] =
        a[(2 * i)] * b[(2 * i) + 1] + a[(2 * i) + 1] * b[(2 * i)];
  }
}

void fas_beamformer_complex_real_multiply(const float* complex_arr,
                                          const float* real_arr, float* output,
                                          size_t num_complex_samples) {
  for (size_t n = 0; n < num_complex_samples; n++) {
    output[(2 * n) + 0] = complex_arr[(2 * n) + 0] * real_arr[n];
    output[(2 * n) + 1] = complex_arr[(2 * n) + 1] * real_arr[n];
  }
}

void fas_beamformer_irdft(const float* complex_in, float* real_out,
                          size_t transform_size) {
  // Mirror complex conjugate before doing the idft, since dft has only first
  // half.
  float complex_in_full[kFilterBufferSize * 2] = {};
  const float w = 2.0f * (float)M_PI;
  for (size_t i = 1; i < transform_size / 2; ++i) {
    complex_in_full[2 * i] = complex_in[2 * i];
    complex_in_full[2 * i + 1] = complex_in[2 * i + 1];
    complex_in_full[2 * (transform_size - i)] = complex_in[2 * i];
    complex_in_full[2 * (transform_size - i) + 1] = -complex_in[2 * i + 1];
  }

  // Unpack the DC and Nyquist coeffs.
  complex_in_full[0] = complex_in[0];
  complex_in_full[1] = 0.0f;
  complex_in_full[transform_size] = complex_in[1];
  complex_in_full[transform_size + 1] = 0.0f;

  for (size_t i = 0; i < transform_size; ++i) {
    for (size_t j = 0; j < transform_size; ++j) {
      real_out[i] +=
          (complex_in_full[(2 * j)] * cosf((w * i * j) / transform_size) -
           complex_in_full[(2 * j) + 1] * sinf((w * i * j) / transform_size));
    }
    real_out[i] /= transform_size;
  }
}

void fas_beamformer_rdft(const float* input, float* complex_out,
                         size_t transform_size) {
  const float w = 2.0f * (float)M_PI;
  for (size_t i = 1; i < transform_size / 2; ++i) {
    // Set to zero to avoid accidental accumulation.
    complex_out[2 * i] = 0.0f;
    complex_out[2 * i + 1] = 0.0f;
    for (size_t j = 0; j < transform_size; ++j) {
      complex_out[(2 * i)] += input[j] * cosf((w * i * j) / transform_size);
      complex_out[(2 * i) + 1] +=
          input[j] * -1.0f * sinf((w * i * j) / transform_size);
    }
  }

  // Compute DC and Nyquist coefficients.
  complex_out[0] = 0.0f;
  complex_out[1] = 0.0f;
  for (size_t j = 0; j < transform_size; ++j) {
    complex_out[0] += input[j];
    complex_out[1] += input[j] * (j % 2 ? -1.0f : 1.0f);
  }
}
