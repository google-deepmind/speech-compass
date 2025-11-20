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


#include "das_beamformer.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static int16_t mic2_prev[kDasBeamformerStateArrNumSamples];
static int16_t mic3_prev[kDasBeamformerStateArrNumSamples];
static int16_t mic4_prev[kDasBeamformerStateArrNumSamples];
static int16_t mic2_next[kDasBeamformerStateArrNumSamples];
static int16_t mic3_next[kDasBeamformerStateArrNumSamples];
static int16_t mic4_next[kDasBeamformerStateArrNumSamples];

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void apply_mic_delay(const int16_t* mic, int16_t* delayed_mic,
                            size_t num_samples, int delay_samples,
                            const int16_t* mic_prev, const int16_t* mic_next) {
  das_beamformer_shift(mic, delayed_mic, num_samples, delay_samples);

  if (delay_samples > 0) {
    memcpy(delayed_mic,
           mic_prev + (kDasBeamformerStateArrNumSamples - delay_samples),
           sizeof(int16_t) * delay_samples);
  } else {
    memcpy(delayed_mic + (num_samples + delay_samples), mic_next,
           -delay_samples * sizeof(int16_t));
  }
}

void das_beamformer_beamform_2_mics(const int16_t* ref_mic, const int16_t* mic2,
                                    int16_t* output, size_t num_samples,
                                    int delay_samples) {
  const int num_mics = 2;
  /* Shift one microphone array */
  int16_t delayed_mic2[kDasBeamformerAudioBufferNumSamples];

  apply_mic_delay(mic2, delayed_mic2, num_samples, delay_samples, mic2_prev,
                  mic2_next);

  /* Sum two arrays and scale by number of mics. We are not doing proper float
   * rounding after scaling to save time, and the output signal is going to
   * become analog wave anyway. */
  for (size_t i = 0; i < num_samples; ++i) {
    output[i] =
        (int16_t)(((int32_t)delayed_mic2[i] + (int32_t)ref_mic[i]) / num_mics);
  }
}

void das_beamformer_beamform_4_mics(const int16_t* ref_mic, const int16_t* mic2,
                                    const int16_t* mic3, const int16_t* mic4,
                                    int16_t* output, size_t num_samples,
                                    int delay_mic2_samples,
                                    int delay_mic3_samples,
                                    int delay_mic4_samples) {
  const int num_mics = 4;
  /* Shift three microphone array */
  int16_t delayed_mic2[kDasBeamformerAudioBufferNumSamples];
  int16_t delayed_mic3[kDasBeamformerAudioBufferNumSamples];
  int16_t delayed_mic4[kDasBeamformerAudioBufferNumSamples];

  apply_mic_delay(mic2, delayed_mic2, num_samples, delay_mic2_samples,
                  mic2_prev, mic2_next);
  apply_mic_delay(mic3, delayed_mic3, num_samples, delay_mic3_samples,
                  mic3_prev, mic3_next);
  apply_mic_delay(mic4, delayed_mic4, num_samples, delay_mic4_samples,
                  mic4_prev, mic4_next);

  /* Sum the arrays and scale by number of mics */
  for (size_t i = 0; i < num_samples; ++i) {
    output[i] = (int16_t)(((int32_t)delayed_mic2[i] + (int32_t)delayed_mic3[i] +
                           (int32_t)delayed_mic4[i] + (int32_t)ref_mic[i]) /
                          num_mics);
  }
}

int das_beamformer_convert_local_degree_to_delay_samples(float mic_distance_m,
                                                         float beam_deg,
                                                         int sampling_rate_hz) {
  const float speed_of_sound_m_per_sec = 343.0f;
  const float max_delay_sec = mic_distance_m / speed_of_sound_m_per_sec;

  float delay_samples;

  float delay_sec = cosf(beam_deg * ((float)M_PI / 180.0f)) * max_delay_sec;

  /* Round to integer, since sample delay is discrete. */
  delay_samples = roundf(delay_sec * (float)sampling_rate_hz);

  return (int)delay_samples;
}

void das_beamformer_shift(const int16_t* input, int16_t* output,
                          size_t num_samples, int shift_samples) {
  /* Left Shift (Negative shift_samples) */
  if (shift_samples <= 0) {
    size_t copy_count =
        num_samples + shift_samples;  // shift is negative, so this subtracts

    // Copy data (Note: input - negative_shift becomes input + offset)
    memcpy(output, input - shift_samples, copy_count * sizeof(int16_t));

    // Zero the remaining tail
    memset(output + copy_count, 0, -shift_samples * sizeof(int16_t));
  }

  /* Right Shift (Positive shift_samples) */
  else {
    // Zero the head (padding)
    memset(output, 0, shift_samples * sizeof(int16_t));

    // Copy data
    memcpy(output + shift_samples, input,
           (num_samples - shift_samples) * sizeof(int16_t));
  }
}

void das_beamformer_store_prev_state(const int16_t* mic2, const int16_t* mic3,
                                     const int16_t* mic4, size_t num_samples) {
  memcpy(mic2_prev, mic2 + (num_samples - kDasBeamformerStateArrNumSamples),
         sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
  memcpy(mic3_prev, mic3 + (num_samples - kDasBeamformerStateArrNumSamples),
         sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
  memcpy(mic4_prev, mic4 + (num_samples - kDasBeamformerStateArrNumSamples),
         sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
}

void das_beamformer_store_next_state(const int16_t* mic2, const int16_t* mic3,
                                     const int16_t* mic4, size_t num_samples) {
  memcpy(mic2_next, mic2, sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
  memcpy(mic3_next, mic3, sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
  memcpy(mic4_next, mic4, sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
}

void das_beamformer_reset_states(void) {
  memset(mic2_prev, 0, sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
  memset(mic2_next, 0, sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
  memset(mic3_prev, 0, sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
  memset(mic3_next, 0, sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
  memset(mic4_prev, 0, sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
  memset(mic4_next, 0, sizeof(int16_t) * kDasBeamformerStateArrNumSamples);
}

float das_beamformer_calculate_rms(const int16_t* input, size_t num_samples) {
  float energy = 0.0f;
  for (size_t i = 0; i < num_samples; ++i) {
    float sample = (float)input[i];
    energy += sample * sample;
  }
  return sqrtf(energy / (float)num_samples);
}

float das_beamformer_calculate_rms_after_dc_offset(const int16_t* input,
                                                   size_t num_samples) {
  float sum = 0.0f;
  float energy = 0.0f;

  for (size_t i = 0; i < num_samples; ++i) {
    sum += (float)input[i];
  }
  float mean = sum / num_samples;

  for (size_t i = 0; i < num_samples; ++i) {
    float sample = (float)input[i] - mean;
    energy += sample * sample;
  }

  return sqrt(energy / num_samples);
}

float das_beamformer_convert_rms_to_dbfs(float rms) {
  // RMS for a sine wave with the max amplitude of 32767.
  const float max_rms_pcm = ((float)INT16_MAX) / sqrtf(2.0f);

  return 20.0f * log10f(rms / max_rms_pcm);
}
