# Sentinel Asset Pack

**Professional Tactical Theme for Flipper Zero**

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![Status](https://img.shields.io/badge/status-pre--alpha-orange)
![Animations](https://img.shields.io/badge/animations-3-orange)
![Icons](https://img.shields.io/badge/icons-17-purple)

---

## 📋 Overview

The **Sentinel Asset Pack** is a professionally designed theme for Flipper Zero, tailored for security professionals, penetration testers, and tactical users. It features a minimalist, tactical aesthetic with enhanced readability and a cohesive visual identity.

### Design Philosophy

- **Tactical & Professional**: Military-inspired design with clean lines and functional aesthetics
- **Security-Focused**: Icons and animations emphasize security, scanning, and protection themes
- **Minimalist**: No unnecessary visual clutter - everything serves a purpose
- **Monochrome Excellence**: Optimized for the Flipper Zero's 128x64 monochrome display

---

## 🎨 Features

### Animations (3 Custom)

1. **Sentinel_Scanning** (64 frames, 4 fps): shown at low mood levels (max butthurt 6)
2. **Sentinel_Secure** (64 frames, 4 fps): shown at all mood levels (max butthurt 18)
3. **Sentinel_Security** (64 frames, 5 fps): shown at low to moderate mood levels (max butthurt 14)

> Each animation must stay under ~40 KB of compiled frames: the desktop loads every frame into RAM.
> See [ANIMACIONES_DIAGNOSTICO.md](../ANIMACIONES_DIAGNOSTICO.md).

### Icons (17 Custom)

#### Dolphin Category (5 icons)
- `DolphinDone_80x58.png` - Success with shield
- `DolphinSuccess_91x55.png` - Tactical success indicator
- `DolphinWait_59x54.png` - Scanning/radar mode
- `DolphinSaved_92x58.png` - Secure save with lock
- `WarningDolphin_45x42.png` - Alert mode

#### Interface Category (5 icons)
- `LockOpen_30x30.png` - Unlocked state
- `LockClosed_30x30.png` - Locked state
- `Security_32x32.png` - Shield security icon
- `Scan_32x32.png` - Tactical crosshair scanner
- `Terminal_32x32.png` - Command line interface

#### NFC Category (2 icons)
- `NFCSuccess_60x60.png` - Successful NFC read
- `NFCRead_50x50.png` - NFC reading in progress

#### SubGhz Category (2 icons)
- `SignalSuccess_64x64.png` - Signal received
- `SignalScan_64x64.png` - Scanning for signals

#### Settings Category (2 icons)
- `Security_40x40.png` - Security settings
- `System_40x40.png` - System settings with grid

#### Passport Category (1 icon)
- `ProfileSentinel_64x64.png` - Tactical profile portrait

### Fonts

Currently using default Flipper Zero fonts. Custom tactical fonts may be added in future versions.

---

## 🚀 Installation

### Automated Installation (Recommended)

```bash
# 1. Navigate to Sentinel Firmware directory
cd Sentinel-Firmware

# 2. Run installation script
./install_assets.sh

# 3. Build firmware with asset pack (FORCE=1 recompiles asset packs)
./fbt FORCE=1 updater_package

# 4. Flash to device
./fbt flash_usb_full
```

### Manual Installation

```bash
# 1. Copy asset pack to packs directory (replace Anims entirely)
rm -rf assets/packs/Sentinel/Anims
cp -r asset_pack_sentinel/Anims assets/packs/Sentinel/
cp -r asset_pack_sentinel/Icons assets/packs/Sentinel/

# 2. Build and flash
./fbt FORCE=1 updater_package
./fbt flash_usb_full
```

### Activating on Device

1. On your Flipper Zero, navigate to: **Momentum → Interface → Graphics → Asset Pack**
2. Select **Sentinel**
3. Restart if prompted

---

## 🛠️ Regenerating Assets

> ⚠️ `generate_sentinel_assets.py` generates the **old v1 animations** (`*_128x64`), not the current ones,
> and they are not listed in the current `manifest.txt`. Use it only to regenerate the icons, and check
> the result before committing.

After changing any asset:

```bash
./install_assets.sh
./fbt FORCE=1 updater_package
```

---

## 📊 Technical Specifications

### Animations

| Animation | Frames | FPS | Duration | Resolution | RAM when loaded |
|-----------|--------|-----|----------|------------|-----------------|
| Scanning  | 64     | 4   | 360s     | 128x64     | ~36.5KB |
| Secure    | 64     | 4   | 360s     | 128x64     | ~23KB   |
| Security  | 64     | 5   | 360s     | 128x64     | ~37KB   |

Only the active animation is loaded into RAM, so the per-animation size is what matters (keep it under ~40 KB).

### Icons

| Category  | Count | Total Size |
|-----------|-------|------------|
| Dolphin   | 5     | ~12KB      |
| Interface | 5     | ~8KB       |
| NFC       | 2     | ~5KB       |
| SubGhz    | 2     | ~6KB       |
| Settings  | 2     | ~4KB       |
| Passport  | 1     | ~3KB       |

**Total Icon Storage**: ~38KB (17 icons)

**Total Asset Pack Size**: ~98KB on the SD card (compiled `.bm`/`.bmx` files)

---

## 🎯 Design Elements

### Visual Motifs

- **Shields**: Security and protection
- **Crosshairs**: Precision and targeting
- **Radar**: Scanning and detection
- **Locks**: Access control and encryption
- **Terminal Lines**: Code and exploitation
- **Tactical Grid**: Military and professional aesthetic

### Color Scheme

- **Monochrome**: Black (#000000) and White (#FFFFFF)
- **High Contrast**: Optimized for OLED display readability
- **Clean Lines**: 2px stroke weight for primary elements
- **Minimalist Fills**: Strategic use of solid fills for emphasis

---

## 📁 File Structure

```
asset_pack_sentinel/
├── README.md                           # This file
├── STRUCTURE_INFO.md                   # Technical documentation
├── meta.txt                            # Pack metadata
│
├── Anims/                              # Animations
│   ├── manifest.txt                    # Animation manifest
│   ├── Sentinel_Scanning/
│   │   ├── meta.txt
│   │   └── frame_*.png (64 frames)
│   ├── Sentinel_Secure/
│   │   ├── meta.txt
│   │   └── frame_*.png (64 frames)
│   └── Sentinel_Security/
│       ├── meta.txt
│       └── frame_*.png (64 frames)
│
├── Icons/                              # Icons
│   ├── Dolphin/                        # 5 icons
│   ├── Interface/                      # 5 icons
│   ├── NFC/                            # 2 icons
│   ├── SubGhz/                         # 2 icons
│   ├── Settings/                       # 2 icons
│   └── Passport/                       # 1 icon
│
└── Fonts/                              # (Reserved for future use)
```

---

## 🔧 Customization

### Modifying Animations

Edit `generate_sentinel_assets.py` to customize:

- Animation frame count and timing
- Visual elements (radar speed, shield pulse, etc.)
- Dolphin design and accessories
- Background effects

### Adding New Icons

To add new icons:

1. Edit `generate_sentinel_assets.py`
2. Add new icon generation methods
3. Call methods in `generate_all_assets()`
4. Regenerate with `python3 generate_sentinel_assets.py`

### Creating Custom Fonts

To add custom fonts:

1. Create `.c` font files for Primary, Secondary, and Keyboard
2. Place in `Fonts/` directory
3. Follow Flipper Zero font format specifications

---

## 🤝 Contributing

Contributions to improve the Sentinel Asset Pack are welcome!

### Ideas for Contribution

- Additional animations (Sentinel in different scenarios)
- More category-specific icons
- Custom tactical fonts
- Seasonal or event-themed variants
- Performance optimizations

### Submission Guidelines

1. Follow existing design language (tactical, minimalist, monochrome)
2. Test on actual hardware when possible
3. Ensure proper file naming conventions
4. Include metadata files for animations
5. Document any new features

---

## 📜 License

This asset pack is part of the Sentinel Firmware project and is licensed under GPL-3.0.

### Attribution

- **Author**: Sentinel Team
- **Based on**: Momentum Firmware
- **Inspiration**: Military tactical aesthetics, security tooling, professional pentesting

---

## 🔗 Links

- **Sentinel Firmware**: [GitHub Repository](https://github.com/alejandropsan/Sentinel-Firmware)
- **Documentation**: See `STRUCTURE_INFO.md` for technical details
- **Issues**: Report bugs or request features on GitHub

---

## 📝 Changelog

### Version 1.0.0 (Current)

**Status**: not released yet (part of Sentinel pre-alpha). Animations reworked in November 2026.

**Features**:
- ✅ 3 custom tactical animations (64 frames each)
- ✅ 17 professional icons across 6 categories
- ✅ Complete manifest and metadata files
- ✅ Automated installation script
- ✅ Asset regeneration system

**Upcoming**:
- Custom tactical fonts
- Additional animations (stealth mode, breach mode)
- Expanded icon library
- Seasonal variants

---

## ⚠️ Notes

- **Display Optimization**: All assets are designed for 128x64 monochrome OLED
- **Performance**: Minimal impact on battery life and system performance
- **Compatibility**: Tested with Sentinel Firmware (based on Momentum)
- **SD Card**: Asset packs are typically stored on SD card (`SD/asset_packs/`)

---

**Enjoy your Sentinel-themed Flipper Zero!** 🛡️

*For security professionals, by security professionals.*
