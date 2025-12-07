# Sentinel Asset Pack - Structure Documentation

## Overview
This document explains the structure needed for a complete Flipper Zero asset pack based on the investigation of existing packs (Momentum and WatchDogs).

## Asset Pack Location
Asset packs are stored in: `assets/packs/<PackName>/`

## Required Structure

```
asset_pack_sentinel/
├── Anims/                      # Desktop animations
│   ├── manifest.txt            # Animation manifest file
│   └── <AnimName>_128x64/      # Animation folders (128x64 resolution)
│       ├── meta.txt            # Animation metadata
│       ├── frame_0.png
│       ├── frame_1.png
│       └── ... (multiple frames)
│
├── Fonts/                      # Custom fonts (optional)
│   ├── Primary.c               # Main system font
│   ├── Secondary.c             # Secondary font
│   └── Keyboard.c              # On-screen keyboard font
│
├── Icons/                      # System icons
│   ├── Animations/             # Loading/progress animations
│   ├── BLE/                    # Bluetooth icons
│   ├── Dolphin/                # Dolphin mascot icons
│   ├── iButton/                # iButton app icons
│   ├── Infrared/               # IR app icons
│   ├── Interface/              # UI elements
│   ├── NFC/                    # NFC app icons
│   ├── Passport/               # Passport/profile icons
│   ├── RFID/                   # RFID app icons
│   ├── Settings/               # Settings icons
│   ├── SubGhz/                 # Sub-GHz app icons
│   └── U2F/                    # U2F security icons
│
└── meta.txt                    # Pack metadata (not used by WatchDogs/Momentum)

```

## File Formats

### 1. Animation Manifest (`Anims/manifest.txt`)
```
Filetype: Flipper Animation Manifest
Version: 1

Name: <AnimationName>_128x64
Min butthurt: 0
Max butthurt: 18
Min level: 1
Max level: 30
Weight: 3
```

**Parameters:**
- `Name`: Animation folder name
- `Min/Max butthurt`: Dolphin mood range (0-18)
- `Min/Max level`: Dolphin level range (1-30)
- `Weight`: Animation selection probability

### 2. Animation Meta (`Anims/<AnimName>/meta.txt`)
```
Filetype: Flipper Animation
Version: 1

Width: 128
Height: 64
Passive frames: 16
Active frames: 83
Frames order: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15...
Active cycles: 1
Frame rate: 6
Duration: 360
Active cooldown: 3

Bubble slots: 0
```

**Parameters:**
- `Width/Height`: Animation dimensions (always 128x64)
- `Passive frames`: Number of idle animation frames
- `Active frames`: Number of active animation frames
- `Frames order`: Sequence of frame playback
- `Frame rate`: Frames per second
- `Duration`: Total animation duration in seconds
- `Active cooldown`: Cooldown time between animations
- `Bubble slots`: Number of speech bubble slots

### 3. Animation Frames
- Format: PNG images
- Naming: `frame_0.png`, `frame_1.png`, etc.
- Resolution: 128x64 pixels
- Color: Monochrome (1-bit)

### 4. Icons
- Format: PNG images
- Naming: `<IconName>_<Width>x<Height>.png`
- Examples:
  - `DolphinDone_80x58.png`
  - `WarningDolphin_45x42.png`
- Color: Monochrome (1-bit)

### 5. Fonts (Optional)
- Format: C source files
- Files needed:
  - `Primary.c` - Main system font
  - `Secondary.c` - Secondary/small font
  - `Keyboard.c` - On-screen keyboard font

## Current Sentinel Pack Status

### ✅ Created
- `meta.txt` - Pack metadata (info only, not compiled)
- `Anims/manifest.txt` - Empty manifest structure
- Directory structure

### ❌ Missing
- **Animations**: No PNG frames or complete meta.txt files
- **Icons**: Empty directories (0 icons)
- **Fonts**: Empty directory (optional, can use default)

## Existing Packs Comparison

### Momentum Pack
- **Anims**: 3 custom animations by Kuronons
- **Icons**: 36 PNG icons across 12 categories
- **Fonts**: Uses default fonts (no custom fonts)

### WatchDogs Pack
- **Anims**: 20+ custom Watch_Dogs themed animations
- **Icons**: Custom icons for 5 categories
- **Fonts**: 3 custom fonts (Hacked, Tiny5, 3x5im)

## Build Process

Asset packs are compiled using:
```bash
./fbt icons proto dolphin_internal dolphin_blocking dolphin_ext resources
```

Compiled packs are placed in: `SD/asset_packs/` on the device

## Installation on Device

1. Build firmware with asset pack
2. Flash to device or copy to SD card
3. Navigate to: **Settings → Desktop → Dolphin**
4. Select the asset pack

## Next Steps for Sentinel Pack

To create a complete Sentinel asset pack, you need:

1. **Create Animations** (minimum 1-3):
   - Design 128x64 monochrome PNG frames
   - Create meta.txt with frame sequences
   - Add to manifest.txt

2. **Create Icons** (recommended 30-40):
   - Design monochrome PNG icons
   - Follow naming convention: `Name_WxH.png`
   - Organize by category (Dolphin, Interface, Apps, etc.)

3. **Fonts** (optional):
   - Can use default fonts
   - Or create custom C font files

4. **Theme**: Define a cohesive visual style
   - Security/tactical aesthetic
   - Dark/minimalist design
   - Professional appearance

## Resources

- [Flipper Animation Guide](https://docs.flipperzero.one/development/animations)
- [Asset Naming Rules](https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/AssetNaming/)
- Existing packs in `assets/packs/` for reference
