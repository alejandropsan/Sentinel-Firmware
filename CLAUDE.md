# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Sentinel is a security-focused Flipper Zero firmware, forked from [Momentum](https://github.com/Next-Flip/Momentum-Firmware) (itself derived from the official flipperdevices firmware). Target MCU is STM32WB55 (hardware target `f7`). Remotes: `origin` = alejandropsan/Sentinel-Firmware, `upstream` = Next-Flip/Momentum-Firmware. PRs target the `dev` branch. Project docs (ReadMe, ROADMAP, diagnostics) are partly in Spanish.

Most of the codebase is inherited upstream code. Sentinel-specific work so far is branding (`FIRMWARE_ORIGIN = "Sentinel"` in `fbt_options.py`, `rebrand.sh`), the Sentinel asset pack, CI workflows, and planned security features (duress PIN, encrypted storage, audit log — see ROADMAP.md). Keep diffs against upstream small where possible so Momentum changes can still be merged.

## Build commands (fbt)

`./fbt` is the Flipper Build Tool (SCons wrapper). On first run it downloads its own toolchain into `toolchain/`; no system ARM compiler is needed. Clone with `--recursive` (many `lib/*` and `applications/external` are submodules).

- `./fbt` — default target `fw_dist`: build firmware into `dist/f7-C/`
- `./fbt updater_package` — full self-update package (firmware + radio stack + SD resources). This is what CI builds.
- `./fbt flash_usb_full` — build and install on a USB-connected Flipper
- `./fbt fap_<appid>` — build a single app as `.fap` (appid from its `application.fam`); `./fbt faps` builds all
- `./fbt launch APPSRC=applications_user/path/to/app` — build, upload and run one app on the device
- `./fbt lint` / `./fbt format` — clang-format check / apply for C (`.clang-format`: 4 spaces, 99 cols)
- `./fbt lint_py` / `format_py` (black), `lint_img` / `format_img`, `lint_all` / `format_all` (the CI lint workflow runs `lint_all`)
- `./fbt firmware_cdb` — regenerate `compile_commands.json` for clangd
- `./fbt cli` — serial CLI session to the device
- `./fbt debug` — flash over SWD and attach gdb (needs a probe)

Local overrides go in `fbt_options_local.py`, not `fbt_options.py`. Options can also be set on the command line, e.g. `COMPACT=0 DEBUG=1`.

### Tests

Tests run on the device, not the host:
1. `./fbt FIRMWARE_APP_SET=unit_tests updater_package` (or `flash_usb_full`)
2. Open the CLI (`./fbt cli`) and run `unit_tests`, or `unit_tests <test_name>` for a single suite. Test names are the app names in `applications/debug/unit_tests/application.fam`.

Each test suite is a `PLUGIN` app under `applications/debug/unit_tests/tests/<name>/`; test data lives in `applications/debug/unit_tests/resources/unit_tests/`. (CONTRIBUTING.md mentions `./fbt firmware_test`, which is not an fbt target.)

## Architecture

- **`furi/`**: core OS layer on top of FreeRTOS (threads, message queues, mutexes, records/pub-sub, `furi_check`, logging). Services are accessed through `furi_record_open(RECORD_*)`.
- **`targets/f7/`**: HAL (`furi_hal_*`) for the real hardware. `targets/f7/api_symbols.csv` is the exported API table for `.fap` apps. Changing a public header that apps use changes this file, and the API version must be bumped when that happens. Build output tells you when the CSV is out of sync.
- **`lib/`**: protocol and support libraries (`subghz`, `nfc`, `infrared`, `lfrfid`, `ibutton`, `mbedtls`, `mjs` JS engine, `flipper_format`, `toolbox`, …). `lib/momentum/` holds fork-level settings (`settings.c`), asset pack loading (`asset_packs.c`), and name spoofing.
- **`applications/`**: every app/service is declared with an `application.fam` manifest (see `documentation/AppManifests.md`). `apptype` decides whether it is linked into the firmware (`SERVICE`, `APP`, `SETTINGS`, `STARTUP`) or built as an SD-card `.fap` (`EXTERNAL`, `PLUGIN`). Which internal apps are included is set by `FIRMWARE_APPS` in `fbt_options.py`.
  - `services/`: long-running system services (gui, desktop, storage, bt, rpc, cli, dolphin, loader, …)
  - `main/`: built-in apps (subghz, nfc, infrared, bad_usb, `momentum_app` = the fork's settings UI)
  - `settings/`, `system/`: settings apps and system apps (e.g. updater, JS app)
  - `external/`: submodule containing more than 180 bundled `.fap` apps
  - `debug/`: dev and test apps, including `unit_tests`
  - `applications_user/`: place for local custom apps; these are picked up automatically
- **GUI apps** use the `ViewDispatcher` + `SceneManager` pattern: `scenes/*_scene_config.h` lists scenes, and each scene has `on_enter/on_event/on_exit` handlers. Copy an existing app's structure when you add a new one.
- **`assets/`**: `icons/` (PNG → generated C via `./fbt icons`), `dolphin/` (animations), `protobuf/` (RPC schema submodule), `packs/` (asset packs compiled into `.bm` files under `build/.../resources/asset_packs/`).

## Sentinel asset pack workflow

- Source: `asset_pack_sentinel/` (`meta.txt`, `Anims/manifest.txt`, per-animation `frame_N.png` + `meta.txt`, `Icons/`, `Fonts/`)
- `./install_assets.sh` copies it to `assets/packs/Sentinel/`, which is what the build actually compiles
- `generate_sentinel_assets.py` / `generate_sentinel_icons*.py` generate frames and icons procedurally (PIL). `diagnose_animations.py` and `visualize_sentinel_animations.py` check the frames and render previews (GIF and contact sheet).
- Frames must be 1-bit monochrome 128x64 PNGs. The frame count in `meta.txt` must match the files on disk.
- The desktop loads every frame of the active animation into RAM. Keep each animation under ~40 KB of compiled `.bm` frames, or the device crashes at boot (`out of memory`).
- fbt only recompiles asset packs with `FORCE=1` (`./fbt FORCE=1 updater_package`). Otherwise stale frames ship silently.
- `generate_sentinel_assets.py` still generates the old v1 `*_128x64` animations, not the current ones.
- Original artwork and full-length animations live in `asset_pack_sentinel/_originals/` (gitignored). Keep them out of `assets/`.
- The pack must be selected on the device before it is used. See ANIMACIONES_DIAGNOSTICO.md for how to select it and for "failed to load animation" troubleshooting.

## Conventions

- Commit messages: `<type>: <subject>` with type in `feat|fix|docs|style|refactor|test|chore|security`. Branches: `feature/…`, `fix/…`, `security/…`, etc.
- C style per CODING_STYLE.md: `snake_case`, K&R braces, Doxygen comments on public APIs. Run `./fbt format` before committing.
- Update CHANGELOG.md for significant changes.
- `build/`, `dist/`, `toolchain/`, `venv/` are generated; don't edit them.
