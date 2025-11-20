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


#include "angle_estimation.h"

#include <math.h>
#include <stdio.h>

void histogram_calculate(const int* input, int* histogram, int input_size,
                         int num_bins) {
  // Compute 1 / bin width = num_bins / 360 with 8 fractional bits.
  const int scale = ((num_bins << 8) + 180) / 360;
  for (int i = 0; i < input_size; ++i) {
    if (0 <= input[i] && input[i] <= 360) {
      int bin_index = (scale * input[i]) >> 8;
      // Wrap 360 degrees around to 0.
      if (bin_index >= num_bins) {
        bin_index = 0;
      }
      ++histogram[bin_index];
    }
  }
}

int create_gaussian_kernel(float* kernel_output, int kernel_size) {
  if (kernel_size % 2 == 0) {
    fprintf(stderr, "Error: Kernel_size should be odd\n");
    return 1;
  }
  const float bandwidth = 1.0f;
  // Set the range to [-3 to 3] to cover the most of gaussian curve at 99%
  // cutoff.
  const float min_distribution = -3.0f;
  const float max_distribution = 3.0f;
  const float range = -min_distribution + max_distribution;
  const float step_size = range / (float)(kernel_size - 1);

  for (int i = 0; i < kernel_size; ++i) {
    float x_dist = (min_distribution) + (float)i * step_size;
    kernel_output[i] = expf(-0.5f * powf(x_dist / bandwidth, 2.0f));
  }

  return 0;
}

int create_pearsons_type_ii_kernel(float* kernel_output, int kernel_size) {
  if (kernel_size % 2 == 0) {
    fprintf(stderr, "Error: Kernel_size should be odd\n");
    return 1;
  }
  const float a = (kernel_size + 1) / 2;  // Kernel radius.
  const float m = 2.0f;                   // Exponent m.

  for (int i = 0; i < kernel_size; ++i) {
    float x = i + 1 - a;
    // Compute kernel = (1 - (x/a)^2)^m.
    kernel_output[i] = powf(1.0f - powf(x / a, 2.0f), m);
  }

  return 0;
}

void add_distribution(const float* kernel, float* kde, int kernel_size,
                      int angle) {
  const int kernel_center = kernel_size / 2;

  int i = 0;
  int j = angle - kernel_center;
  if (j < 0) {
    j += kKdeSize;
  }

  if (kKdeSize - j < kernel_size) {
    for (; j < kKdeSize; ++i, ++j) {
      kde[j] += kernel[i];
    }
    j = 0;  // Wrap 360 degrees around to zero.
  }

  for (; i < kernel_size; ++i, ++j) {
    kde[j] += kernel[i];
  }
}

void kernel_density_estimation_angle(const int* angle_arr, float* kde,
                                     const float* kernel, int kernel_size,
                                     int angle_arr_size) {
  for (int i = 0; i < angle_arr_size; ++i) {
    add_distribution(kernel, kde, kernel_size, angle_arr[i]);
  }
}
