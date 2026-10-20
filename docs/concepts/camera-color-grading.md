# Camera color grading research

Research completed **2026-10-20**. Target: **SecretGoodStarPilot**, checkout `0f6a817264cf692786394528b92a08225c8d8ba4`. This is research and a design recommendation, not a production change or a device calibration result.

## Charter

- **Goal:** identify a practical camera treatment that makes full engagement conspicuously colorful, with deep blacks and useful detail, and makes AOL a muted version of that same treatment. Completion means a sourced method comparison, a recommendation, and explicit quality checks and unresolved limits.
- **Knowledge depth:** working knowledge of the current camera shader and previous previews; color-management and gamut-mapping choices require primary-source research.
- **Scope / Object / Subject:** display color grading / this checkout's road-camera presentation / tone curves, chroma controls, gamut fitting, transform representation, and the current input/output paths.
- **Type:** exploratory method comparison, followed by a bounded descriptive recommendation. This does not diagnose the reported physical-device effect or authorize production implementation.
- **Q1:** How can blacks deepen without unnecessarily crushing shadows or clipping highlights?
- **Q2:** Which color controls can produce a conspicuous, attractive effect, and how should AOL relate to it?
- **Q3:** How should colors near the display gamut boundary be protected, and do AgX/ACES assumptions fit?
- **Q4:** Which representation is the smallest appropriate solution: analytic shader, 1D LUT, or 3D LUT?
- **Q5:** Which current pipeline facts constrain grading, and which device properties remain unverified?
- **Reachable sources:** standards-body references, author/maintainer documentation, and local source inspection. Physical MICI measurements and on-device GPU measurements are unavailable.

The requested look is a strong engagement reveal: deep blacks, conspicuously rich foliage and reds, neutral whites, and useful shadow and highlight detail. **AOL must be a muted version of the same engaged look.** Earlier previews are design experiments; their saturation multipliers and software render checks do not establish perceptual quality on a device.

This review admits first-party technical documentation and the original color-space author's implementation. Its object is the road-camera image, before the HUD and alerts are composited. Other StarPilot branches, upstream comparisons, sensor exposure changes, and whole-screen recoloring are excluded. Scene-linear photography tools provide useful principles, but their default parameters are not automatically appropriate for camera video that has already passed through ISP color and gamma processing. Exact transfer behavior, active-device buffer interpretation, panel black level, ambient reflections, and GPU cost remain unverified.

## Canonical sources

The first eight sources are ordered by usefulness to the grading question. All external sources below were opened, not just searched. Versioned URLs are preferred; ACES pages report an update date of 2025-09-10. Standards and author/maintainer references are evidence ranks 1–2; local inspection is rank 3. Ordering is not an independent quality score.

| Rank / ID | Primary source and version | Question supported |
|---|---|---|
| 1 / S1 | [darktable 5.6: color balance RGB](https://docs.darktable.org/usermanual/5.6/en/module-reference/processing-modules/color-balance-rgb/) | Chroma, vibrance, tone masks, hue-aware protection |
| 2 / S2 | [darktable 5.6: filmic RGB](https://docs.darktable.org/usermanual/5.6/en/module-reference/processing-modules/filmic-rgb/) | Black/white endpoints, detail, clipping limits |
| 3 / S3 | [ACES 2: output gamut compression](https://docs.acescentral.com/system-components/output-transforms/technical-details/gamut-compression/) | Smoothly fitting colors into a display gamut |
| 4 / S4 | [Björn Ottosson: Oklab](https://bottosson.github.io/posts/oklab/) | Perceptual lightness, chroma, hue; matrices updated 2021-01-25 |
| 5 / S5 | [darktable 5.6: sigmoid](https://docs.darktable.org/usermanual/5.6/en/module-reference/processing-modules/sigmoid/) | Contrast curves and channel-dependent hue artifacts |
| 6 / S6 | [darktable 5.6: AgX](https://docs.darktable.org/usermanual/5.6/en/module-reference/processing-modules/agx/) | Toe/shoulder controls and graceful highlight color |
| 7 / S7 | [ACES 1.3: Reference Gamut Compression specification](https://docs.acescentral.com/rgc/specification/) | Input-domain limitations; analytic/LUT caveats |
| 8 / S8 | [OpenColorIO 2.4.2: transforms](https://opencolorio.readthedocs.io/en/v2.4.2/api/transforms.html#lut1dtransform) | 1D and 3D representation and interpolation |
| 9 / S9 | [ICC: display calibration](https://www.color.org/displaycalibration/) | Calibration, profiles, physical display and viewing conditions |
| 10 / S10 | [ICC: sRGB encoding registry](https://registry.color.org/rgb-registry/srgb) | Transfer-function definition and output-referred encoding |
| 11 / S11 | [Khronos: EGL_EXT_image_dma_buf_import, version 7](https://registry.khronos.org/EGL/extensions/EXT/EGL_EXT_image_dma_buf_import.txt) | YUV matrix/range hints and implementation fallback |
| 12 / S12 | [FFmpeg: scale filter](https://www.ffmpeg.org/ffmpeg-filters.html#scale) | Separate sample-range and matrix handling |
| 13 / S13 | Local source at the checkout above; exact file links below | Current shader, ISP configuration, EGL import, and state signals |

### Compact glossary

- **Black point:** the input level chosen to become output black. **Toe:** the curve's shadow bend. **Shoulder:** its highlight bend. Moving an endpoint is different from changing the bend (S2, S6).
- **Luminance:** a light-related channel; **lightness:** a perceptual correlate. Encoded RGB/luma values are not interchangeable with linear-light luminance (S1, S4).
- **Hue:** the color family. **Chroma:** distance from a neutral axis in the chosen model. **Saturation/vibrance:** controls whose exact mathematical meanings depend on the implementation (S1, S4).
- **Gamut:** colors a target can represent. **Compression:** a smooth remapping toward its boundary; **clamping:** truncating channel values at fixed limits (S3, S8).
- **Profile/transform:** a specification of color interpretation or conversion; a creative look is a separate choice. A LUT stores sampled transform results (S7, S8).

## Findings ledger

| Finding | Decision-relevant claim | Evidence / rank | Answers |
|---|---|---|---|
| F1 | Moving the black endpoint can discard shadow distinctions | S2 / 1–2 | Q1 |
| F2 | Chroma, saturation, and vibrance are different controls | S1 / 1–2 | Q2 |
| F3 | Oklab separates lightness, chroma, and hue, with input assumptions | S4 / 1 | Q2–Q3 |
| F4 | Smooth, hue-aware gamut compression is richer than channel clipping | S3 / 1–2 | Q3 |
| F5 | Per-channel contrast can shift hue | S5 / 1–2 | Q1–Q3 |
| F6 | AgX trades extreme highlight saturation for coherent rendering | S6 / 1–2 | Q1–Q3 |
| F7 | ACES input gamut repair is not a display gamut mapper | S7 / 1–2 | Q3–Q4 |
| F8 | LUT dimension describes what can be represented, not inherent quality | S8 / 1–2 | Q4 |
| F9 | Calibration and creative grading solve different problems | S9–S10 / 1–2 | Q1, Q5 |
| F10 | The current engaged device shader removes most color and changes gamma | S13, camera shader / 3 | Q2, Q5 |
| F11 | Camera processing and import must be accounted for before grading | S11–S13, ISP/EGL paths / 1–3 | Q3, Q5 |
| F12 | The app has actual activity signals beyond the shader's boolean | S13, runtime snapshot / 3 | Q2, Q5 |

The entries below distinguish documented facts from deductions and remaining unknowns.

**F1 — Strong black and visible shadows are competing constraints.** Filmic maps an input interval to the display interval, with an S-shaped contrast region. Everything below its selected black endpoint becomes black; everything above white becomes white. Its black picker is noise-sensitive. Lost, clipped source data cannot be recovered by later grading. Its reconstruction can soften clipping's appearance, not recreate absent texture. Therefore a later filter cannot promise both a raised black threshold and preservation of every near-black distinction. Source: [S2](https://docs.darktable.org/usermanual/5.6/en/module-reference/processing-modules/filmic-rgb/), high confidence within its documented workflow.

**F2 — Chroma increase is a better-defined target than an arbitrary saturation percentage.** Color balance RGB distinguishes hue-constant chroma scaling from perceptual saturation changes, which can alter both brightness and chroma. Its vibrance preferentially boosts low-chroma colors, while luminance masks distinguish shadows, midtones, and highlights. It also performs gamut protection. Its input expectation is scene-linear; copying its controls into nonlinear video without conversion would violate that expectation. Source: [S1](https://docs.darktable.org/usermanual/5.6/en/module-reference/processing-modules/color-balance-rgb/), high confidence; no device-specific gain follows from the manual.

**F3 — A perceptual color model separates controls more cleanly, but is not an aesthetic guarantee.** Oklab represents lightness separately from two color-opponent coordinates; polar conversion gives chroma and hue. Increasing chroma while holding lightness and hue fixed is directly expressible. Its reference conversion expects linear sRGB or appropriate XYZ, not arbitrary encoded RGB. Its author states normal well-lit viewing assumptions; the model does not account for the exact cabin viewing environment. Source: [S4](https://bottosson.github.io/posts/oklab/), high confidence about the mathematical definition, bounded confidence about appearance under all conditions.

**F4 — Smooth gamut fitting provides a useful model for strong color.** ACES 2's output compression preserves a modeled hue while changing colorfulness and, when beneficial, lightness to approach the target gamut. It leaves sufficiently interior colors unchanged and compresses progressively above a threshold. This is more nuanced than independently truncating RGB channels. The published method includes hue-dependent boundary tables; its benefit does not imply that its full machinery is required here. Source: [S3](https://docs.acescentral.com/system-components/output-transforms/technical-details/gamut-compression/), high confidence for ACES's stated target.

**F5 — Contrast can change hue even with a single curve applied equally to channels.** Sigmoid's per-channel mode affects brightness, chroma, and hue, with bright saturated colors especially prone to shifts toward secondary colors. RGB-ratio processing instead preserves spectral color, which the documentation distinguishes from the best approximation of perceptual hue. Its target black is normally left at its default; lifting it deliberately creates a faded look. Source: [S5](https://docs.darktable.org/usermanual/5.6/en/module-reference/processing-modules/sigmoid/), high confidence. A stronger per-channel S-curve is not automatically a neutral contrast improvement.

**F6 — AgX deliberately trades extreme highlight saturation for a coherent image.** This implementation takes scene-linear input and creates display-referred output. Its custom primaries guide bright colors toward white, avoiding unnaturally pure primary/secondary patches. The toe controls shadows, the shoulder highlights, and the slope contrast. This directly conflicts with interpreting the goal as maximum saturation at every brightness: an extremely bright red source cannot remain both maximally red and white-bright within the same bounded display. Source: [S6](https://docs.darktable.org/usermanual/5.6/en/module-reference/processing-modules/agx/), high confidence in the documented design, not evidence to apply an HDR view transform twice.

**F7 — ACES input gamut repair is not display gamut mapping.** The 1.3 RGC assumes unbounded, scene-linear exposure samples. Its scope explicitly excludes display gamut mapping and bounded gamut volumes. It is a technical input repair, with a protected region, not a ready-made vibrant look. The specification prefers analytic implementations in finishing systems and warns that LUT approximation needs a suitable shaper and introduces residual error. Source: [S7](https://docs.acescentral.com/rgc/specification/), high confidence. Reusing a film-production name does not establish correct input handling.

**F8 — A LUT is a representation, not inherently a better color algorithm.** OCIO exposes channel-sampled 1D tables and RGB-grid 3D tables, with interpolation and bit-depth metadata. A 3D transform can depend on all three input channels; an ordinary independent-channel 1D table cannot express arbitrary cross-channel operations. That last limitation follows from the indexed dimensions, not a quality comparison. Sampling accuracy must be checked against the intended transform. Source: [S8](https://opencolorio.readthedocs.io/en/v2.4.2/api/transforms.html#lut3dtransform), high confidence about representation, no performance result for StarPilot.

**F9 — Calibration is the foundation; a creative look is a separate operation.** ICC describes calibrating a display to chosen behavior, then profiling it for color communication. Black/white response, tone response, and viewing conditions matter; actual calibration uses measurements. Its sRGB registry specifies a piecewise transfer function, not the camera shader's `pow(1/1.28)`. Neither reference proves that MICI currently implements sRGB exactly. Inference: correct signal black can improve an image, but a shader cannot remove panel light leakage or reflected cabin light. Sources: [S9](https://www.color.org/displaycalibration/), [S10](https://registry.color.org/rgb-registry/srgb).

**F10 — Current code explicitly opposes the requested engagement reveal.** The [MICI device shader](/home/nik/Documents/GitHub/StarPilot/openpilot/selfdrive/ui/mici/onroad/cameraview.py:43) blends engaged RGB to 20% original color, applies clamped 1.2 contrast, then raises it to `1/1.28`; disengaged RGB is multiplied by 0.85. The desktop variant omits that power operation. The [large UI camera shader](/home/nik/Documents/GitHub/StarPilot/openpilot/selfdrive/ui/onroad/cameraview.py:43) has corresponding road-camera treatment. These are source facts, not a measurement of displayed saturation. A future look needs consistent device correction and an explicit creative state control, rather than treating the existing state-dependent power as a verified calibration.

**F11 — The camera is already processed, and recorded metadata is not a live-buffer guarantee.** The [BPS configuration](/home/nik/Documents/GitHub/StarPilot/openpilot/system/camerad/cameras/spectra.cc:609) programs an RGB gamma LUT; its [NV12 outputs](/home/nik/Documents/GitHub/StarPilot/openpilot/system/camerad/cameras/spectra.cc:687) declare `CAM_COLOR_SPACE_BT601_FULL`. The separate [IFE output](/home/nik/Documents/GitHub/StarPilot/openpilot/system/camerad/cameras/spectra.cc:925) sets `color_space = 0`. These facts narrow the uncertainty; they do not establish every device's final transfer response. The [EGL import](/home/nik/Documents/GitHub/StarPilot/openpilot/system/ui/lib/egl.py:138) omits matrix and sample-range hints. S11 says omitted hints permit implementation guesses/defaults: that is an unresolved interpretation boundary, not proof of a driver bug. S12 documents distinct full/limited range and matrix controls. Consequently, a recorded clip tagged limited range does not justify expanding every live buffer from limited range. Sources: S13, [S11](https://registry.khronos.org/EGL/extensions/EXT/EGL_EXT_image_dma_buf_import.txt), [S12](https://www.ffmpeg.org/ffmpeg-filters.html#scale).

**F12 — Use actual control activity to distinguish the proposed looks.** The [camera update](/home/nik/Documents/GitHub/StarPilot/openpilot/selfdrive/ui/mici/onroad/cameraview.py:334) currently reduces status to `status != DISENGAGED`. The [runtime snapshot](/home/nik/Documents/GitHub/StarPilot/openpilot/starpilot/ui/runtime_snapshot.py:632) separately observes `latActive`, `longActive`, and a longitudinal-override condition. Those are available inputs for a future state mapping; the camera boolean alone cannot represent all three desired color strengths. Exact override and stock-cruise presentation policy remains a product choice before implementation.

## Scope filter

- Retained the standards and grading principles; revised their application to respect finite display gamut, lost source detail, and viewing conditions. Maximum darkness, maximum color, and perfect retention of every distinction are competing objectives (F1, F4, F6, F9).
- Excluded claims that a numerical saturation percentage predicts a perceptual percentage. Controls and color spaces differ (F2–F3).
- Excluded universal claims that a LUT or a named cinema transform improves quality. Input domain and the actual transform determine the result (F6–F8).

## Object filter

- AgX/ACES defaults were excluded as drop-in solutions because this camera path already includes ISP gamma processing. Their source-domain assumptions must not be silently transferred to UI RGB (F6–F7, F11).
- Revised the broad claim that live encoding is wholly unknown: BPS explicitly declares BT.601 full range; IFE and final import/display behavior still require confirmation. Omitted EGL hints do not establish incorrect behavior (F11).
- Prior preview parity proves agreement between implementations, not physical calibration, aesthetic quality, or GPU cost. Recorded-file metadata and the live device path are distinct evidence objects (F9–F11).
- Other branches, upstream behavior, whole-screen effects, and model-input changes were not studied. This recommendation applies to this checkout's road-camera presentation only.

## Conclusions

### Methods compared

The following assessment is a design inference from F1–F8, not a source recommendation for StarPilot.

| Method | Benefit for this request | Cost or failure mode | Position |
|---|---|---|---|
| Global saturation plus hard contrast | Minimal arithmetic; conspicuous change | Hue drift, clipped channels, lost dark detail; no scene protection | Reject as the final look |
| Analytic luma/chroma treatment | Small, inspectable; can use the existing shader pass | RGB chroma directions only approximate perceptual hue; protection must be tested | Smallest practical candidate |
| Oklab/OKLCh grading with gamut fitting | Direct lightness/chroma/hue controls; useful reference | More math; conversion domain and gamut boundary matter | Quality reference and candidate if needed |
| Complete AgX/ACES rendering pipeline | Mature handling of wide-range input | Assumes the right scene data; can double-render already-tonemapped video and mute highlights | Borrow principles, do not drop in defaults |
| 1D LUT | Good compact representation of a tone curve | Cannot alone implement the desired hue/chroma-aware treatment | No advantage established yet |
| 3D LUT | Can bake a coherent, complex authored look | Asset generation, input/output definitions, sampling, texture path, and validation | Defer until the look requires it |
| Local contrast, sharpening, bloom | Additional impression of texture or glow | More spatial processing; halos/noise or bright fringes can dominate the result | Outside the smallest solution |

### Different looks from the same controls

These are proposed art directions, not downloaded profiles or measured presets (F1–F6).

| Look | Tone and color choices | Fit to the request |
|---|---|---|
| Natural vivid | Modest midtone contrast, moderate chroma, restrained highlight changes | Useful reference, likely less dramatic than requested |
| Dramatic vivid | Deep anchored blacks, stronger midtone contrast, substantial chroma gain with smooth boundary protection | Recommended direction; strong greens and reds while retaining their internal variation |
| Soft film | Gentler contrast and longer highlight roll-off, more restrained bright color | Useful alternative, but less suited to the requested saturated reveal |

Use the same dramatic vivid direction for AOL with reduced chroma. A generic cinematic LUT may include unwanted hue rotations or lifted blacks; a filename or preset name does not specify the desired result. Judge the transform itself.

### Recommended design direction

The proposed result is **one strong, protected look with three chroma strengths**. The intended hierarchy is black and tonal structure first, rich color second, and preservation of meaningful detail throughout. This is an engineering recommendation, not a calibrated preset. It follows from F1–F5 and F9–F12.

1. Establish the input and output interpretation before grading. Incorrect range or transfer handling can create raised blacks or crush detail before the aesthetic curve operates. Keep display correction consistent between states; do not use engagement as an exposure correction (F9–F11).
2. Set digital black deliberately, with a smooth toe retaining dark separations above black and a shoulder retaining highlight texture. Do not force an arbitrarily large slice of the histogram to zero. Judge dark pavement, shaded foliage, and night road markings independently (F1, F5–F6).
3. Increase midtone chroma strongly while keeping hue stable and neutral grays neutral. Use smooth, color-dependent protection near the target gamut. Protect already-rich lights rather than increasing every pixel equally. Suppress unnecessary shadow chroma gain so noise is not turned into colorful speckles (F2–F4).
4. Keep foliage visibly varied: several greens should remain several greens. Keep red lamp housings and halos red while permitting clipped or genuinely neutral light cores to remain white. Colorfulness cannot manufacture missing sensor information (F1, F4, F6).
5. Give AOL the same tone curve, black point, white balance, hue treatment, and protection, with lower chroma. This preserves the identity of the effect while making its control state legible. Keep HUD and alerts out of camera grading (design inference from F2–F3, F9, F12).

| State | Proposed relationship to the same look |
|---|---|
| Disengaged | Monochrome or near-monochrome camera, retaining usable tones |
| AOL | The engaged profile with reduced chroma; no separate faded black level |
| Full engagement | Full protected chroma strength and the intended rich tonal structure |

In a perceptual representation, the useful relationship is engaged `(L, C, h)`, AOL `(L, kC, h)` with `0 < k < 1`, and monochrome `(L, 0, h)`, followed by final gamut fitting. This expresses the intention; gamut fitting and perceived brightness can prevent perfectly identical final lightness at all strengths. Blend color intensity, rather than blending the graded image back into an ungraded gray image, which would also weaken the tonal treatment.

No researched source supplies a universal numerical saturation multiplier. Initial implementation should remain in the existing camera fragment pass, with no dependency, LUT framework, auto-leveling system, or separate image pass unless a measured limitation requires it. Oklab is useful as an independent comparison target; it is not mandatory runtime machinery before cost and appearance are evaluated.

## Next steps / Risks

Use matched frames through the actual decode and display path: daylight trees, shaded leaves, red tail lamps, saturated LEDs, white clouds, wet reflections, dusk, tunnels, and night. Check a neutral ramp, near-black steps, hue/chroma sweeps, and smoothly graded highlights. Inspect both still detail and motion: protected gamut limits can remain numerically bounded yet still flatten textures, and changing auto-exposure can reveal an unstable curve.

Measure newly clipped channels, near-black distinctions, neutral tint, hue drift, and gradient banding alongside subjective review. Check state transitions without changing the underlying exposure. Previous synthetic render parity establishes implementation agreement only; neither that nor zero new clipping proves that an image looks attractive. Finally compare the physical MICI/C3X displays under realistic light before claiming calibration or acceptable performance.

Keep processing in floating point through the grade and compare the final quantized output too: a mathematically smooth curve can still lose 8-bit distinctions. Check camera frame time on the target hardware, including high-percentile frame times. These are proposed checks, not measurements performed by this research.

Unresolved Q1/Q3/Q5 items are the useful shadow floor under real ambient light, exact active-device transfer/import behavior, available display gamut, and acceptable GPU cost. Close them with known signal ramps through the actual path, display measurements, representative scene review, and hardware profiling. Q2's final strength and override/stock-cruise state policy require visual tuning and explicit mapping. Q4's final analytic-versus-LUT choice should follow a measured limitation, rather than precede it.

Evidence-review result: **supported with qualifications**. Source assumptions and branch observations are distinguished; transfer to physical devices and numerical aesthetic tuning remain unverified. This was a source and consistency review within the research run, not an independent device test.

No runtime code was changed or runtime tests rerun for this document. A very black digital signal cannot by itself prove a very black physical display, and a strong grade cannot guarantee every future scene will remain attractive.
