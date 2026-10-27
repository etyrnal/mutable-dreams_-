# Mutable Dreams 1.2

**Six voices. Small worlds, endlessly unfolding.** An independent, open-source instrument for the M-VAVE FM-1, with a playable browser edition and live USB MIDI editor.

[Play and explore](https://mutable-dreams.sugarypixie10.chatgpt.site/) · [Download 1.2](https://github.com/kidbee85-beep/mutable-dreams/releases/tag/v1.2) · [Install or return to V15](https://mutable-dreams.sugarypixie10.chatgpt.site/Mutable-Dreams-1.2-Install.html) · [Update and recovery guide](https://mutable-dreams.sugarypixie10.chatgpt.site/Recovery-1.2.txt)

## Shaped by listening

Thank you for the music, encouragement and detailed feedback. Version 1.2 expands the palette and makes personal phrases easier to compose, while refining the sound and the living interface.

- Six freely assignable tracks; 15 engines including new Velvet Keys, Spectral Bloom and Breath Texture.
- 288 factory tones and 92 musical Biomes plus Blank: 64 additional tones and 16 additional Biomes compared with 1.1, alongside revised existing sounds.
- Warmer voice contours and more carefully balanced Biomes. A damped Rings strike reduces unintended equal-velocity accents while retaining expressive dynamics.
- Up to 32 steps per track: notes, rests, velocity, gate and chance; four phrase generators, keyboard/MIDI step entry and monophonic live overdub.
- Four optional chord journeys across selected tracks.
- One shared granular cloud with six independent track sends, smoother transitions and optional filtered noise. Shared delay and a more diffuse room.
- Parameter-responsive landscapes, clearer antialiased lettering, sequence pitch stems and velocity bars. Contextual help on held function buttons.
- 16 named User Biomes and 64 user sound slots, patch JSON import/export, full .mddb bank backup/restore and a live browser editor.

The device has a 240 × 240 display. The browser runs the same application/DSP source; its banks are separate from device banks. Selected Mutable Instruments algorithms are combined with original designs: these are not complete ports of every module.

## Update safely

Connect only the intended FM-1. On 1.1/1.2, save your work, export a full .mddb backup and separately export any unsaved live patch JSON. Disconnect the live editor before using the installer. The exact 1.2 build completed a Stock V15 roundtrip and data restore; restoration of all banks requires physical SEL confirmation. Returning to V15 erases banks and preferences.

For 1.0, use the legacy backup workflow and the previously qualified 1.0 → 1.1 route, then back up in 1.1 before installing 1.2. We do not claim a tested direct path from every third-party firmware. Keep a pre-update backup when downgrading: a 1.2 patch is newer than 1.1.

Published 1.0 and 1.1 tags and assets remain available and unchanged. The [1.1 browser](https://mutable-dreams.sugarypixie10.chatgpt.site/Mutable-Dreams-1.1.html) remains available to export older browser banks.

## Validation and limits

Release identity: `FM-1_710010200`  
Firmware file: `Mutable-Dreams-1.2.0.fwsc` (610,136 bytes)  
SHA-256: `828891e0f506d6a07ee0c6fa844e54207985c464e0f2d9cbb423e73ae522f7fb`

The exact release binary passed 29 controlled audio/device cases and 15 sequence/storage checks, a V15 roundtrip, byte-identical bank restoration, the visible browser installer and a physical cold start. Measured playback total: 731.7 seconds, with no observed xruns, overloads, nonfinite audio or dropped/capture errors in those cases. The highest observed load was 88.39%. This is scoped evidence, not a guarantee for every patch; automatic admission and FULL/ECO limits remain. USB MIDI is tested; physical TRS-MIDI cable testing remains outstanding. The owner approved the listening results and update film. See [qualification](docs/release-1.2/QUALIFICATION.txt).

## Browse, build and contribute

The complete source tree is directly browsable here in `app/`, `base/`, `web/`, `presets/`, `tools/` and `tests/`. See [BUILDING.md](BUILDING.md), [source provenance](SOURCE-PUBLICATION.md) and `SOURCE-SHA256.txt` in the release source archive. DSP/device sources match the qualified build; public packaging and documentation were updated afterwards without recompiling the device firmware.

Native checks: `python3 tools/build_local.py host`, `dsp --sanitize`, `quality --sanitize`, `evolution --sanitize`; see the build guide for toolchain-dependent targets. The second processor core remains disabled. Firmware audio uses 44.1 kHz and 256-frame callbacks; this does not establish end-to-end latency.

## With gratitude

Mutable Dreams owes its foundations to **Émilie Gillet and Mutable Instruments** and their generous open-source work. Selected Plaits, Rings and Elements algorithms retain their MIT notices. The fm1-base / felucca-bsp platform retains MPL-2.0 notices. Marbles-inspired remembered variation, Tides-inspired cyclic modulation and Warps-inspired ring blending are original implementations. This independent project is unaffiliated with and not endorsed by Mutable Instruments.

Combined distribution: **GPL-3.0-only**, with original component licences retained. Read [LICENSING-GPL.txt](LICENSING-GPL.txt), [platform credits](base/CREDITS.md) and the component notices before redistributing. No manufacturer firmware or personal User Banks are included in the source bundle.

Created by **Kid Bunka / Christoph Link**. Forks are welcome: please credit Mutable Dreams and its upstream authors, and give your project its own name and visual identity.
