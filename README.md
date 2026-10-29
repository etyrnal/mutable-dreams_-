# Mutable Dreams 1.3

An independent, open-source instrument for the M-VAVE FM-1. Six freely assignable synth tracks, or three synth tracks and one Lo-Fi sample kit. [Play it in your browser](https://mutable-dreams.sugarypixie10.chatgpt.site/) or [download the firmware](https://github.com/kidbee85-beep/mutable-dreams/releases/tag/v1.3).

## New in 1.3

- Five original Lo-Fi kits with 40 drum and texture samples. Each kit has eight editable pads, four simultaneous sample voices, 32-step lanes, generation and keyboard/USB MIDI recording.
- Forty-eight additional warm tone presets and sixteen Biomes. Total: **336 tones, 108 musical Biomes plus Blank**.
- Two independent LFOs per synth track, each with four curves and seven destinations. Six-synth mode offers twelve routed LFOs; kit mode offers six. Their settings belong to complete Biomes.
- Five sequence directions, offset, gentle velocity feel, four more generators and four more chord journeys with up to sixteen bars per chord and Nearby root moves. Existing 1.2 sequencing and recording remain.
- Protection before a changed Biome can replace unsaved edits. Larger type, seven quiet page palettes, contextual headings, translucent controls and small parameter-responsive forms on the physical 240 × 240 display.
- Community-requested **button LED backlight** on GLO → LOOK / STARTUP → K4: Off, Dim or Soft. Held function buttons continue to explain the current controls.

The browser uses the same application and DSP sources. Its User Banks are separate from the FM-1's. The page can also edit a connected Mutable Dreams 1.3 device over USB MIDI and back up or restore its saved banks.

## Install and recover

Read [Recovery-1.3.txt](Recovery-1.3.txt) and the [illustrated guide](https://mutable-dreams.sugarypixie10.chatgpt.site/Mutable-Dreams-1.3-Guide.pdf). Save your Biome, export all saved banks as `.mddb`, and separately export unsaved live edits as JSON before an update. The full backup excludes granular audio, calibration and unsaved edits. A restore requires physical SEL confirmation on the FM-1.

The exact 1.3 package was installed from manufacturer **Stock V16** over the verified USB update protocol. Stock V15 was used in the roundtrip, and the 1.2 → 1.3 update was tested during development. The official **M-UPGRADE** app successfully installed manufacturer V16 from V15, but rejected the custom 1.3 file with “File format error”. Use the [release browser installer](https://mutable-dreams.sugarypixie10.chatgpt.site/Mutable-Dreams-1.3-Install.html) or the included Python installer kit for Mutable Dreams itself. Do not assume that every other custom firmware can update directly.

Firmware identity: `FM-1_710010300`  
Release package SHA-256: `d9400aa360048a8422028f1e005f6a434cd96f9c8db2f0fbef35cda39ebab1d6`

The exact build passed a Stock V16 update, byte-identical bank restoration, six earlier long-form development audio cases and a further 35-second final audio/UI stress check. The browser/editor protocol and source builds passed automated tests. Physical TRS MIDI has not yet been cable-tested. The automatic FULL/ECO guard remains active for dense patches. See [the qualification report](docs/release-1.3/DEVICE-TEST-REPORT-1.3.txt) for scope and limits.

## Source and credits

The complete source tree is browsable directly in `app/`, `base/`, `presets/`, `web/`, `tools/` and `tests/`. [BUILDING.md](BUILDING.md) explains the toolchain and checks. The versioned source ZIP contains a SHA-256 file inventory. Existing public 1.0, 1.1 and 1.2 tags and assets remain unchanged.

Mutable Dreams is grateful to **Émilie Gillet and Mutable Instruments** for their generous open-source work. Selected Plaits, Rings and Elements algorithms retain their original notices. The fm1-base platform retains its MPL-2.0 notices. Several other module-inspired features are original interpretations, not full ports. This project is independent, unaffiliated with and not endorsed by Mutable Instruments.

The combined distribution is **GPL-3.0-only**, with component notices retained. See [LICENSING-GPL.txt](LICENSING-GPL.txt), the platform credits and the third-party notices. No proprietary manufacturer firmware or personal User Banks are included. Created by **Kid Bunka / Christoph Link**. Forks are welcome; please credit this project and its upstream authors and give derivative work its own name and visual identity.
