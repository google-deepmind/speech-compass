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


/* This is a library to estimate the time difference of arrival (TDOA) on the
 * ARM Cortex. It uses the CMSIS library to speed up the calculations of Fourier
 * transforms and complex math.
 * https://www.keil.com/pack/doc/cmsis/DSP/html/index.html
 *
 * There are also support functions to extract the delay and angle from the
 * cross-correlation.
 *
 * Here is a snippet of how to use it:
 *
 * // First filter the signals to remove DC offset.
 *
 * #define FFT_SIZE 512
 * #define HISTOGRAM_LEN 36
 * const int kAudioSamplingRate = 44100;
 * static float cross_corelation[FFT_SIZE];
 * static float cross_corelation_dft[FFT_SIZE];
 * static float fft_input0[FFT_SIZE];
 * static float fft_input1[FFT_SIZE];
 * arm_rfft_fast_instance_f32 rfft;
 *
 * arm_rfft_fast_init_f32(&rfft, FFT_SIZE)
 *
 * gcc_phat_arm_dsp(fft_input0, fft_input1, cross_corelation_dft, FFT_SIZE);
 * // Do a inverse FFT. Set the flag to 1 for ifft.
 * arm_rfft_fast_f32(&rfft, cross_corelation_dft, cross_corelation, 1);
 * float delay01 =  extract_delay_sec_gcc(cross_corelation, 80, FFT_SIZE,
 * kAudioSamplingRate);
 *
 * // Run GCC-PHAT in the opposite direction to get the negative delay.
 * gcc_phat_arm_dsp(fft_input1, fft_input0, cross_corelation, FFT_SIZE);
 * arm_rfft_fast_f32(&rfft, cross_corelation_dft, cross_corelation, 1);
 * float delay10 =
 * extract_delay_sec_gcc(cross_corelation, 80, FFT_SIZE,
 * kAudioSamplingRate);
 *
 * int deg01 = delay_to_azimuth_global_degrees(delay01, 80.0f, 270.0f);
 *
 * int deg10 = delay_to_azimuth_global_degrees(delay10, 80.0f, -270.0f);
 *
 * // Add the angles into a generic FIFO buffer.
 * append_to_angle_buffer(angle_buffer, deg01);
 * append_to_angle_buffer(angle_buffer, deg10);
 *
 * // Apply histogram to the buffer to extract the angle of arrival.
 * int histogram[HISTOGRAM_LEN] = { 0 };
 * histogram_calculate (angle_buffer, histogram, ANGLE_BUF_LEN,
 * HISTOGRAM_LEN);
 */

#ifndef MULTI_MIC_AUDIO_TDOA_H_
#define MULTI_MIC_AUDIO_TDOA_H_

#include "/dsp/CMSIS/CMSIS_5/CMSIS/Include/arm_math.h"
#include "/dsp/fast_fun.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

static const float speed_of_sound_mm_per_sec = 343.0f * 1000.0f;

arm_rfft_fast_instance_f32 rfft;

/* Performs Generalized cross-correlation (GCC-PHAT) between two signals.
 * Algorithm is described here:
 * http://www.xavieranguera.com/phdthesis/node92.html
 * GCC-PHAT only runs in one direction. To get the negative delays run in again
 * with sig1 and sig2 reversed. The input signals are microphone signals in time
 * domain. The output is cross-correlation between sig1 and sig2 in the STFT
 * domain. Run inverse fft (ifft) to get the actual cross-correlation.
 */
void gcc_phat_arm_dsp(float* sig1, float* sig2, float* output, int num_samples);

/* Same as gcc_phat_arm_dsp, but takes in complex signals in the frequency
 * domain, instead of the time domain. For >2 microphones, this can save a
 * significant amount of time by precomputing FFTs beforehand. For example, for
 * 4 mics, the number of FFTs are reduced from 24 to 4.
 * "transform_size is the FFT transform size" and that "sig1_fft and sig2_fft
 * should each point to an array of transform_size float elements, representing
 * a half spectrum of complex values in the format that arm_rfft_fast_f32()
 * produces".
 */
void gcc_phat_arm_dsp_fft_in(float* sig1_fft, float* sig2_fft, float* output,
                             int transform_size);

/* Extracts the delay in seconds from the generalized correlation.
 * Max_value can also be obtained. This corresponds to the height of the peak.
 */
float32_t extract_delay_sec_gcc(float* cross_correlation, int num_samples,
                                int sampling_rate_sec, float* max_value);

/* Converts the delay (sec) into azimuth angle.
 * The angle is converted to the global coordinate system using the
 * coordinate_transform_degrees constant. The global system follows the polar
 * coordinates (0 degrees on the positive x-axis).
 *
 * The constant coordinate_transform_degrees should be adjusted that angles are
 * in [0, 360] range.
 *
 * Note that due to front-back confusion each angle will have a second angle,
 * that is mirrored around the axis between the microphones.
 */
int delay_to_azimuth_global_degrees(float delay_sec, float distance_mics_mm,
                                    float coordinate_transform_degrees);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // MULTI_MIC_AUDIO_TDOA_H_
