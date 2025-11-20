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

/* Angle estimation test */
#include "third_party/deepmind/speech_compass/dsp/angle_estimation.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "third_party/deepmind/speech_compass/utility/logging.h"

static const float kTol = 1e-5f;  // Allow some error tolerance with floats.

/* Test histogram calculation. */
static void TestHistogram(void) {
  puts("TestHistogram");

  // Test without decimal bin width. For example, 36 bins provide 10 degree
  // width (360/36 = 10).
  const int angle_arr[10] = {10, 10, 10, 10, 20, 30, 40, 100, 200, 300};
  int hist[36] = {0};
  histogram_calculate(angle_arr, hist, 10, 36);
  CHECK(hist[1] == 4);
  CHECK(hist[2] == 1);

  // Test when the bin width is decimal. For example 80 bins provide 4.5 degree
  // bin width.
  const int angle_arr_frac[10] = {4, 10, 10, 11, 20, 30, 40, 100, 200, 359};
  int hist_frac[80] = {0};
  histogram_calculate(angle_arr_frac, hist_frac, 10, 80);
  CHECK(hist_frac[0] == 1);
  CHECK(hist_frac[1] == 0);
  CHECK(hist_frac[2] == 3);
  CHECK(hist_frac[79] == 1);
}

static void CheckKernel(const float* kernel, int kernel_size,
                        float expected_stddev) {
  double sum = 0.0;
  double sum_x2 = 0.0;
  for (int i = 0; i < kernel_size; ++i) {
    // Check that all samples are positive.
    CHECK(kernel[i] >= 0.0f);
    // Accumulate sum and second moment to compute standard deviation.
    sum += kernel[i];
    const double x = i - (kernel_size - 1) / 2;
    sum_x2 += (x * x) * kernel[i];
  }

  const float actual_stddev = (float)sqrt(sum_x2 / sum);
  // Check standard deviation. The expected stddev comes from a continuous
  // domain formula while the actual stddev is on the discretized kernel, so we
  // use a loose tolerance to allow for this discrepancy.
  CHECK(fabsf(actual_stddev - expected_stddev) <= 0.5f);

  for (int i = 0, j = kernel_size - 1; i < j; ++i, --j) {
    // Check symmetry.
    CHECK(fabsf(kernel[i] - kernel[j]) <= kTol);
    // Check monotonicity.
    CHECK(kernel[i] < kernel[i + 1]);
  }
}

/* Test Gaussian kernel generation. */
static void TestCreateGaussianKernel(void) {
  puts("TestCreateGaussianKernel");

  float kernel[39] = {0.0f};
  const int kernel_size = sizeof(kernel) / sizeof(float);
  const int kernel_error = create_gaussian_kernel(kernel, kernel_size);

  CHECK(kernel_error == 0);
  const float expected_stddev = kernel_size / 6.0f;
  CheckKernel(kernel, kernel_size, expected_stddev);
}

/* Test Pearson type II kernel. */
static void TestCreatePearsonsKernel(void) {
  puts("TestCreatePearsonsKernel");

  float kernel[39] = {0.0f};
  const int kernel_size = sizeof(kernel) / sizeof(float);
  const int kernel_error = create_pearsons_type_ii_kernel(kernel, kernel_size);

  CHECK(kernel_error == 0);
  const float expected_stddev = kernel_size * 0.189f;
  CheckKernel(kernel, kernel_size, expected_stddev);
}

/* Check adding new kernel to an existing KDE. */
static void TestAddDistribution(void) {
  puts("TestAddDistribution");

  float kde[kKdeSize] = {0.0f};
  float gaussian_kernel[39] = {0.0f};
  const int kernel_size = 39;
  create_gaussian_kernel(gaussian_kernel, kernel_size);

  // Check no wrap.
  add_distribution(gaussian_kernel, kde, kernel_size, 180);
  // Should have a peak of 1.0f at 180 degrees.
  CHECK(fabs(kde[180] - 1.0f) <= kTol);

  // Add another gaussian at 180, so peak should be 2.0f now.
  add_distribution(gaussian_kernel, kde, kernel_size, 180);
  CHECK(fabs(kde[180] - 2.0f) <= kTol);

  // Check below zero degrees wrap.
  add_distribution(gaussian_kernel, kde, kernel_size, 10);

  CHECK(fabs(kde[10] - 1.0f) <= kTol);
  CHECK(fabs(kde[355] - 0.060524f) <= kTol);

  // Check above 360 degrees wrap.
  add_distribution(gaussian_kernel, kde, kernel_size, 350);

  CHECK(fabs(kde[350] - 1.0f) <= kTol);
  CHECK(fabs(kde[4] - 0.725305f) <= kTol);
}

/* Checks few values KDE calculation. */
static void TestCalculateKde(void) {
  puts("TestCalculateKde");

  float kde[kKdeSize] = {0.0f};
  float gaussian_kernel[39] = {0.0f};
  const int kernel_size = 39;
  const int angle_arr_size = 10;
  const int angle_arr[10] = {10, 10, 10, 10, 60, 60, 60, 100, 200, 300};
  create_gaussian_kernel(gaussian_kernel, kernel_size);

  kernel_density_estimation_angle(angle_arr, kde, gaussian_kernel, kernel_size,
                                  angle_arr_size);

  // Check that the kernels add up. Here are four kernels at 10 degrees, will
  // have a peak of 4 at 10.
  CHECK(fabs(kde[10] - 4.0f) <= kTol);

  // Check that the kernel at 200 degrees is at zero at its end.
  CHECK(fabs(kde[221] - 0.0f) <= kTol);
  CHECK(fabs(kde[179] - 0.0f) <= kTol);

  // Check that the kernel decreases moving away from the peak.

  // Check that the kernel centered at 200 degrees is monotonic.
  for (int i = 180; i < 200; ++i) {
    CHECK(kde[i] < kde[i + 1]);
  }
  for (int i = 200; i < 220; ++i) {
    CHECK(kde[i] > kde[i + 1]);
  }
}

int main(int argc, char** argv) {
  TestHistogram();
  TestCreateGaussianKernel();
  TestCreatePearsonsKernel();
  TestAddDistribution();
  TestCalculateKde();

  puts("PASS");
  return EXIT_SUCCESS;
}
