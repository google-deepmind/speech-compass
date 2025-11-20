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


/* Library for angle estimation with statistical methods: histogram and kernel
 * density estimation (KDE). The estimation is specifically made for [0 359]
 * degree azimuth angles.  */

#ifndef AUDIO_DSP_ANGLE_ESTIMATION_H_
#define AUDIO_DSP_ANGLE_ESTIMATION_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/* KDE size of 360 works well, as it corresponds to 360 degrees azimuth.
 * Other sizes could be used but this will require appropriate scaling of the
 * kernel and input data to match physical angles.
 */
#define kKdeSize 360

/* Histogram is applied to an array of azimuth angles, to reduce the noise.
 * The order of the input angles doesn't matter.
 * The max value in the histogram represents the most likely azimuth angle.
 * The min and max of the histogram is set to 0 and 360 degrees, since we are
 * not expecting any angles outside of this range.
 * The individual bin size is calculated as:
 * bin_size = (max_value - min_value) / output_number_bins;
 * The bin size is an integer.
 * Histogram calculation is much faster than KDE, but KDE approximates the
 * localization statistics better.
 */
void histogram_calculate(const int* input, int* histogram, int input_size,
                         int num_bins);

/* Makes a Gaussian kernel, centered around kernel_size/2, and with 99% cutoff.
 * The kernel size corresponds to bump width in degrees, so kernel_size of 39
 * spans 0 to 39 azimuth degrees, as it is later mapped to 0 to 359 range. The
 * bandwidth is set to 1 to simplify mapping. Kernel size should be I found that
 * size 39 works well.
 * On successful kernel generation this function returns 0, and 1 on error.
 * Kernel size should be odd to provide symmetry.
 */
int create_gaussian_kernel(float* kernel_output, int kernel_size);

/* Creates Pearson's Type II kernel. Compared to the Gaussian, an advantage is
 * it goes exactly zero outside a finite interval and it is faster to compute.
 * This function works the same as create_gaussian_kernel(), but outputs a
 * slightly different kernel shape. More info on Pearson's kernels:
 * https://en.wikipedia.org/wiki/Pearson_distribution#The_Pearson_type_II_distribution
 * On successfull kernel generation this function returns 0, and 1 on error.
 * Kernel size should be odd.
 * By default exponent m is set to 2. In this case, half width at half max
 * (HWHM) = kernel_size * 0.2706. Standard deviation = kernel_size * 0.189.
 */
int create_pearsons_type_ii_kernel(float* kernel_output, int kernel_size);

/* Adds a new angle with a specific kernel to an existing KDE. By default the
 * KDE is mapped to the [0 359] range and has a size of 360, so the kernel
 * should be designed in a way to fit this range. A larger kernel size will
 * imply more spread. The angular data is circular, with 0 and 359 degrees close
 * to each other. For such a case, if part of the kernel falls under 0 degrees,
 * that part is moved to the other end of the distribution. For example, if the
 * angle is 10 degrees and kernel size is 40, the first 10 points of the kernel
 * are moved to 350 degrees. The same is applied if the kernel parts fall above
 * 360.
 */
void add_distribution(const float* kernel, float* kde, int kernel_size,
                      int angle);

/* Calculate KDE for an array of angles. This function just applies
 * add_distribution function to an array. By default kde is expected to be zero
 * filled and size of 360. */
void kernel_density_estimation_angle(const int* angle_arr, float* kde,
                                     const float* kernel, int kernel_size,
                                     int angle_arr_size);

/* Helper function to return the max of the two floats.*/
static float max(float num1, float num2) { return (num1 > num2) ? num1 : num2; }

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // AUDIO_DSP_ANGLE_ESTIMATION_H_
