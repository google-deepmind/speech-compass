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


/* Delay and Sum (DAS) beamformer in time domain.
 * https://en.wikipedia.org/wiki/Discrete-time_beamforming
 *
 * By calling the store_prev_state() and store_next_state() this could
 * stream by shifting samples from previous and next audio buffers. Otherwise
 * zeros will be used as shifted samples. Store_previous_state() and
 * store_next_state() calls depend on the implementation, but need to be
 * synchronized with the audio buffer updates.
 *
 * Since we need values from the next buffer for shifting, this implementation
 * will be delayed by 1 audio buffer.
 */

#ifndef MULTI_MIC_AUDIO_BEAM_DAS_BEAMFORMER_H_
#define MULTI_MIC_AUDIO_BEAM_DAS_BEAMFORMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
/* Number of samples to keep in previous and next states. They should be
 * slightly larger than maximum delay. */
#define kDasBeamformerStateArrNumSamples 15

/* Size of the audio buffer. */
#define kDasBeamformerAudioBufferNumSamples 1024

/* DAS beamformer for two microphones. The mic2 signal is shifted in relation to
 * reference microphone. The signal is shifted by delay_samples. If the delay
 * samples are positive, the mic2 signal is delayed in relation to the reference
 * mic. If the samples are negative, the future samples from mic2 are taken to
 * the present. The output is scaled by the number of microphones.
 *
 * The maximum delay samples should not be larger than num_samples. Also,
 * num_samples is how many samples are processed from the mic buffer. This
 * number should be less or equal to kDasBeamformerAudioBufferNumSamples. */
void das_beamformer_beamform_2_mics(const int16_t* ref_mic, const int16_t* mic2,
                                    int16_t* output, size_t num_samples,
                                    int delay_samples);

/* DAS beamformer for four microphones. Similar to beamform_2_mics, but extended
 * to four microphones. All the delays are relative to reference mic. */
void das_beamformer_beamform_4_mics(const int16_t* ref_mic, const int16_t* mic2,
                                    const int16_t* mic3, const int16_t* mic4,
                                    int16_t* output, size_t num_samples,
                                    int delay_mic2_samples,
                                    int delay_mic3_samples,
                                    int delay_mic4_samples);

/* Determines the number of delay samples (used by beamformer) based on the
 * microphone spacing and the desired beam angle.
 *
 *       X
 *     X
 *   X  )<Angle
 *  M1-------------M2
 *
 * Where M1 and M2 are microphones 1 and 2, X's represent the desired beam
 * direction, )< is the beam angle.
 * In this configuration, if the sound comes at 0-degree angle, the maximum
 * positive delay is achieved (end-fire). Delay is zero at 90 degrees
 * (broadside). The maximum negative delay is at 180 degrees.
 * */
int das_beamformer_convert_local_degree_to_delay_samples(float mic_distance_m,
                                                         float beam_deg,
                                                         int sampling_rate_hz);

/* Shift microphone samples by shift_samples. Negative shift_samples means left
 * shift and positive is right shift. The shifted samples are replaced by stored
 * values from previous and next buffer or zeros if no values are stored.
 */
void das_beamformer_shift(const int16_t* input, int16_t* output,
                          size_t num_samples, int shift_samples);

/* Stores the last samples (n = kDasBeamformerStateArrNumSamples) from the
 * previous buffer.
 */
void das_beamformer_store_prev_state(const int16_t* mic2, const int16_t* mic3,
                                     const int16_t* mic4, size_t num_samples);

/* Stores the first samples (n = kDasBeamformerStateArrNumSamples) from the next
 * buffer. */
void das_beamformer_store_next_state(const int16_t* mic2, const int16_t* mic3,
                                     const int16_t* mic4, size_t num_samples);

/* Sets all the saved states to zero. */
void das_beamformer_reset_states(void);

/* Calculate root mean square (RMS) value of the input array. */
float das_beamformer_calculate_rms(const int16_t* input, size_t num_samples);

/* Calculate RMS after removing DC offset by subtracting the mean from each
 * input sample.
 */
float das_beamformer_calculate_rms_after_dc_offset(const int16_t* input,
                                                   size_t num_samples);

/* Convert RMS (root mean square value) to dBFS (decibels relative to full
 * scale) as:
 *
 * dBFS = 20 * log10(rms / (32767 / sqrt(2))).
 *
 * The RMS is assumed to have a nominal range of 0 to 32767 (2^15 - 1). The
 * denominator 32767 / sqrt(2) is the RMS of a sine wave with an amplitude of
 * 32767. The actual maximum value may depend on the microphone. We assume
 * audio is 16-bit PCM (pulse code modulation).
 * This operation is equivalent to standard deviation. */
float das_beamformer_convert_rms_to_dbfs(float rms);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* MULTI_MIC_AUDIO_BEAM_DAS_BEAMFORMER_H_ */
