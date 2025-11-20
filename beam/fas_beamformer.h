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


/* Filter and sum (FAS) wideband beamformer in the frequency domain.
 * The general equation is :
 * output(t) = ISTFT{ SUM(W .* STFT{signal}) },
 * where W is the steering weights (vectors). */

#ifndef MULTI_MIC_AUDIO_BEAM_FAS_BEAMFORMER_H_
#define MULTI_MIC_AUDIO_BEAM_FAS_BEAMFORMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* Size of the buffers used in the beamformer. Should be sized according to the
 * size of the input audio. I am assuming input of 512 audio samples. */
#define kFilterBufferSize 512

/* Coordinate input (xy_coord_m) for 4 mic non uniform circular array in the
 * order: [x0,y0,x1,y1,x2,y2,x3,y3], where x and y are coordinates in meters of
 * each microphone.
 * The (0,0) coordinate is at the center of the array.
 * Microphones should be located on the same radius from the center of
 * the circular array, in the following order:
 *
 *   2:(x2,y2)-------3:(x3,y3)
 *      |  -       -  |
 *      |    (0,0)    |
 *      |  -       -  |
 *   0:(x0,y0)-------1:(x1,y1)      180 <--- x ---> 0 deg
 *
 * Output: angles = [angle0, angle1, angle2, angle3], angleN is the azimuth
 * angle of the mic N, in radians.
 * r_m is the radius of the circle in meters. */
void fas_beamformer_four_mic_rectangular_coord_to_angles(
    const float* xy_coord_m, float* r_m, float* mic_angles_rad);

/* Calculates complex steering weights for 4 mics according to radius and
 * angles of the mics. Assumes non-uniform circular geometry.
 * Details on the calculation:
 * https://arxiv.org/pdf/1703.02318
 * Chapter 3: https://link.springer.com/book/10.1007/978-3-540-78612-2

 * st_vec0 is the steering weight for mic0, st_vec1 for mic1...
 * The steering vector (st_vec) format is as follows:
 * <real f0><imag f0><real f1><imag f1> ... <real f_sampling_rate><imag
 * f_sampling_rate>.
 * There will be num_output_bins complex steering weights for each microphone.
 */
void fas_beamformer_compute_four_mic_non_uniform_circular_steering_vector(
    // (These args are purely inputs.)
    float azimuth_deg, float sampling_rate_hz, const float* mic_angle_rad,
    const float r_m,
    // (These args are about the output.)
    float* st_vec0, float* st_vec1, float* st_vec2, float* st_vec3,
    size_t num_output_bins);

/* Applying FAS beamformer to frequency domain 4-microphone data.
 *
 * Combine into one spectrum:
 * In matrix notation: Y(f) = W(f)^H * X(f)
 * where H is conjugate transpose, W is the steering weights, precomputed
 * for a specific azimuth angle. X is the FFTs of four mics.
 * One frequency bin computed: y = w1 * x1 + w2 * x2 + w3 * x3 + w4 * x4.
 * After summing frequency bins, an IFFT is performed to create a time domain
 * one channel audio (output).
 *
 * The rffts are calculated from the microphone audio. They have size of
 * fft_size/2 complex elements.
 *
 * */
void fas_beamformer_apply(const float* st_vec0, const float* st_vec1,
                          const float* st_vec2, const float* st_vec3,
                          const float* rfft0, const float* rfft1,
                          const float* rfft2, const float* rfft3,
                          size_t num_complex_bins, float* output);

/* Computes Hamming window.  */
void fas_beamformer_compute_hamming_window(float* output, size_t size);

/* Multiply two complex arrays of same size element by element:
 * output[n] = a[n] * b[n] for all n = num_complex_samples. */
void fas_beamformer_complex_multiply(const float* a, const float* b,
                                     float* output, size_t num_complex_samples);

/* Multiply real array by complex array of the same size element by element:
 * output[n] = complex_arr[n] * real_arr[n] for all n = num_complex_samples. */
void fas_beamformer_complex_real_multiply(const float* complex_arr,
                                          const float* real_arr, float* output,
                                          size_t num_complex_samples);

/* Using a brute force DFT implementation of IRFFT for testing when CMSIS RFFT
 * is not unavailable.
 * CMSIS is Common Microcontroller Software Interface Standard. Among other
 * things, it provides DSP libraries for ARM microcontroller:
 * https://arm-software.github.io/CMSIS_5/DSP/html/index.html
 *
 * This implementation follows CMSIS DSP RFFT inputs/outputs:
 * https://www.keil.com/pack/doc/CMSIS/DSP/html/group__RealFFT.html
 * This implementation is very slow so it should be used only for testing CMSIS
 * RFFT.
 * Assumes a maximum transform size of kFilterBufferSize.
 *
 * Like CMSIS's RFFT, `complex_in` represents `transform_size / 2` complex
 * values as interleaved real and imaginary components:
 *
 *   [real0, imag0, real1, imag1, real2, imag2, ...].
 *
 * Since the coefficients at DC and Nyquist are both necessarily real valued,
 * the real DC coeff is stored in real0 and the real Nyquist coeff in imag0.
 */
void fas_beamformer_irdft(const float* complex_in, float* real_out,
                          size_t transform_size);

/* Using a brute force implementation of DFT for CMSIS testing. Very slow.
 * `input` and `complex_output` are both assumed to be `transform_size` floats.
 * `complex_output` represents `transform_size / 2` complex values in the same
 * form as described above for irdft().
 */
void fas_beamformer_rdft(const float* input, float* complex_out,
                         size_t transform_size);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* MULTI_MIC_AUDIO_BEAM_FAS_BEAMFORMER_H_ */
