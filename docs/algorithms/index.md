# Localization and Beamforming algorithms

## Localization

The localization algorithms are in the /dsp subfolder. We made lightweight
localization algorithms in C. The low-level implementation allows the algorithms
to be ported to different embedded platforms. Our localization algorithm is
based on generalized cross-correlation with phase transform (GCC-PHAT) [1] and
statistical estimation of source location.

We used a slightly modified GCC-PHAT approach to calculate the cross correlation
between microphone pairs. In our case, we used normalization to the power of
-0.3. Also, while most localizers use SPR (Steered Power Response), we used an
ad-hoc lightweight statistical estimation based on Kernel Density Estimation.

## Beamforming

We have two classical beamformer implementations: Delay-and-Sum (DAS) and
Filter-and-Sum (FAS) located in the /beam subfolder. The beamformer takes four
channels and outputs one beamformer channel. We ended up focusing on the
localization in the SpeechCompass paper, so the firmware doesn't run
beamforming.

### References

[1] Knapp, C. H. and G.C. Carter, “The Generalized Correlation Method for
Estimation of Time Delay.” IEEE Transactions on Acoustics, Speech and Signal
Processing. Vol. ASSP-24, No. 4, Aug 1976.
