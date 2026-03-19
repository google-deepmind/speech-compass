# DSP Algorithms

Lightweight, platform-agnostic C (C11) implementations of the localization and beamforming
algorithms. Designed to run on low-power microcontrollers but fully testable on desktop
with Bazel.

## Localization (`dsp/`)

Localization is based on Generalized Cross-Correlation with Phase Transform (GCC-PHAT) [1].

- **`gcc_phat.c/.h`** — Frequency-domain cross-correlation with partial phase normalization
  (exponent −0.3). Operates on a single microphone pair.
- **`tdoa.c/.h`** — Extracts Time Difference of Arrival (TDOA) from GCC-PHAT peaks and
  converts delays to azimuth angles. Uses ARM CMSIS DSP for FFTs on embedded targets.
- **`angle_estimation.c/.h`** — Aggregates TDOA measurements from all mic pairs into a
  single azimuth estimate (0–359°) using histogram accumulation and Kernel Density
  Estimation (KDE) with Gaussian or Pearson Type II kernels.

Unlike most localizers that use Steered Power Response (SPR), we use a lightweight
statistical KDE approach that is well-suited to real-time embedded constraints.

## Beamforming (`beam/`)

Two classical beamformer implementations are included. The SpeechCompass firmware uses
localization only (not beamforming), but these are provided for completeness.

- **`beam/das_beamformer.c/.h`** — Time-domain Delay-and-Sum beamformer; supports 2- and
  4-microphone circular arrays; stateful ring buffer.
- **`beam/fas_beamformer.c/.h`** — Frequency-domain Filter-and-Sum beamformer with complex
  steering weights for circular arrays.

## Building and testing

Build rules are defined in `defs.bzl` using Bazel wrapper rules (`c_binary`, `c_library`,
`c_test`) that enforce C11 and a consistent warning set.

```bash
# Run all tests
bazel test //...

# Run a specific test
bazel test //test:angle_estimation_test
bazel test //test:gcc_phat_test
bazel test //test:das_beamformer_test
bazel test //test:fas_beamformer_test
```

Tests use the `CHECK()` assertion macro from `utility/logging.h`.

## References

[1] Knapp, C. H. and G.C. Carter, "The Generalized Correlation Method for Estimation of
Time Delay." *IEEE Transactions on Acoustics, Speech and Signal Processing*, Vol. ASSP-24,
No. 4, Aug 1976.
