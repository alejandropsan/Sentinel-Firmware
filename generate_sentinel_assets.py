#!/usr/bin/env python3
"""
Sentinel Asset Pack Generator
Creates tactical-themed animations and icons for Flipper Zero
"""

from PIL import Image, ImageDraw, ImageFont
import os
import math

# Sentinel color theme (monochrome)
BLACK = 0
WHITE = 1

class SentinelAssetGenerator:
    def __init__(self, base_path="asset_pack_sentinel"):
        self.base_path = base_path
        self.anims_path = os.path.join(base_path, "Anims")
        self.icons_path = os.path.join(base_path, "Icons")

    def create_monochrome_image(self, width, height):
        """Create a new monochrome image"""
        return Image.new('1', (width, height), WHITE)

    def draw_grid(self, draw, width, height, spacing=8):
        """Draw tactical grid background"""
        for x in range(0, width, spacing):
            draw.line([(x, 0), (x, height)], fill=BLACK, width=1)
        for y in range(0, height, spacing):
            draw.line([(0, y), (width, y)], fill=BLACK, width=1)

    def draw_crosshair(self, draw, x, y, size=10):
        """Draw tactical crosshair"""
        draw.line([(x-size, y), (x+size, y)], fill=BLACK, width=2)
        draw.line([(x, y-size), (x, y+size)], fill=BLACK, width=2)
        draw.ellipse([(x-size//2, y-size//2), (x+size//2, y+size//2)], outline=BLACK, width=2)

    def draw_shield(self, draw, x, y, width, height):
        """Draw security shield icon"""
        points = [
            (x + width//2, y),  # top
            (x + width, y + height//3),
            (x + width, y + 2*height//3),
            (x + width//2, y + height),  # bottom point
            (x, y + 2*height//3),
            (x, y + height//3)
        ]
        draw.polygon(points, outline=BLACK, fill=WHITE, width=2)
        # Inner shield design
        draw.line([(x + width//2, y + height//4), (x + width//2, y + 3*height//4)], fill=BLACK, width=2)
        draw.line([(x + width//4, y + height//2), (x + 3*width//4, y + height//2)], fill=BLACK, width=2)

    def draw_lock(self, draw, x, y, width, height):
        """Draw padlock icon"""
        # Lock body
        body_y = y + height//2
        draw.rectangle([(x, body_y), (x + width, y + height)], outline=BLACK, fill=WHITE, width=2)
        # Lock shackle
        draw.arc([(x + width//4, y), (x + 3*width//4, y + height//2)], 0, 180, fill=BLACK, width=2)
        # Keyhole
        key_y = body_y + height//6
        draw.ellipse([(x + width//2 - 3, key_y), (x + width//2 + 3, key_y + 6)], fill=BLACK)

    def draw_radar(self, draw, x, y, radius, angle=0):
        """Draw radar sweep animation"""
        draw.ellipse([(x-radius, y-radius), (x+radius, y+radius)], outline=BLACK, width=2)
        draw.ellipse([(x-radius//2, y-radius//2), (x+radius//2, y+radius//2)], outline=BLACK, width=1)
        # Radar sweep line
        end_x = x + int(radius * math.cos(math.radians(angle)))
        end_y = y + int(radius * math.sin(math.radians(angle)))
        draw.line([(x, y), (end_x, end_y)], fill=BLACK, width=2)

    def draw_terminal_lines(self, draw, x, y, width, num_lines=5, offset=0):
        """Draw terminal/code lines"""
        line_height = 6
        for i in range(num_lines):
            line_y = y + i * line_height + offset
            line_width = width - (i * 5) % 30  # Variable width for realism
            draw.rectangle([(x, line_y), (x + line_width, line_y + 3)], fill=BLACK)

    # ==================== ANIMATIONS ====================

    def create_sentinel_scanning_animation(self):
        """Create Sentinel scanning/analyzing animation"""
        anim_name = "Sentinel_Scanning_128x64"
        anim_path = os.path.join(self.anims_path, anim_name)
        os.makedirs(anim_path, exist_ok=True)

        print(f"Creating animation: {anim_name}")

        num_frames = 60
        for frame in range(num_frames):
            img = self.create_monochrome_image(128, 64)
            draw = ImageDraw.Draw(img)

            # Draw Sentinel dolphin silhouette (simplified tactical version)
            # Body
            body_x, body_y = 30, 25
            draw.ellipse([(body_x, body_y), (body_x + 35, body_y + 25)], outline=BLACK, fill=WHITE, width=2)

            # Head
            head_x, head_y = body_x + 25, body_y - 5
            draw.ellipse([(head_x, head_y), (head_x + 20, head_y + 15)], outline=BLACK, fill=WHITE, width=2)

            # Eye (tactical visor style)
            eye_x, eye_y = head_x + 12, head_y + 5
            draw.rectangle([(eye_x, eye_y), (eye_x + 6, eye_y + 4)], fill=BLACK)

            # Tail
            tail_points = [(body_x, body_y + 12), (body_x - 10, body_y + 5), (body_x - 8, body_y + 15)]
            draw.polygon(tail_points, outline=BLACK, fill=WHITE, width=2)

            # Flipper with tactical gear
            flipper_points = [(body_x + 15, body_y + 23), (body_x + 10, body_y + 35), (body_x + 20, body_y + 33)]
            draw.polygon(flipper_points, outline=BLACK, fill=WHITE, width=2)

            # Scanning radar effect (rotating)
            radar_angle = (frame * 360 / num_frames) % 360
            radar_x, radar_y = 90, 32
            self.draw_radar(draw, radar_x, radar_y, 25, radar_angle)

            # Scanning lines moving down
            scan_y = (frame * 2) % 64
            for i in range(3):
                y = (scan_y + i * 20) % 64
                draw.line([(0, y), (128, y)], fill=BLACK, width=1)

            # Save frame
            img.save(os.path.join(anim_path, f"frame_{frame}.png"))

        # Create meta.txt
        meta_content = f"""Filetype: Flipper Animation
Version: 1

Width: 128
Height: 64
Passive frames: 20
Active frames: 60
Frames order: {' '.join(map(str, range(20)))} {' '.join(map(str, range(20, 60)))}
Active cycles: 1
Frame rate: 12
Duration: 360
Active cooldown: 3

Bubble slots: 0
"""
        with open(os.path.join(anim_path, "meta.txt"), 'w') as f:
            f.write(meta_content)

        print(f"  ✓ Created {num_frames} frames")

    def create_sentinel_hacking_animation(self):
        """Create Sentinel hacking/coding animation"""
        anim_name = "Sentinel_Hacking_128x64"
        anim_path = os.path.join(self.anims_path, anim_name)
        os.makedirs(anim_path, exist_ok=True)

        print(f"Creating animation: {anim_name}")

        num_frames = 48
        for frame in range(num_frames):
            img = self.create_monochrome_image(128, 64)
            draw = ImageDraw.Draw(img)

            # Dolphin at terminal
            body_x, body_y = 15, 30
            draw.ellipse([(body_x, body_y), (body_x + 30, body_y + 22)], outline=BLACK, fill=WHITE, width=2)

            # Head looking at screen
            head_x, head_y = body_x + 20, body_y - 8
            draw.ellipse([(head_x, head_y), (head_x + 18, head_y + 14)], outline=BLACK, fill=WHITE, width=2)

            # Focused eye
            eye_x, eye_y = head_x + 10, head_y + 5
            draw.ellipse([(eye_x, eye_y), (eye_x + 4, eye_y + 3)], fill=BLACK)

            # Terminal screen
            screen_x, screen_y = 55, 10
            screen_w, screen_h = 65, 45
            draw.rectangle([(screen_x, screen_y), (screen_x + screen_w, screen_y + screen_h)], outline=BLACK, width=2)

            # Scrolling code lines
            scroll_offset = -(frame * 2) % 30
            self.draw_terminal_lines(draw, screen_x + 5, screen_y + 5, screen_w - 10, 7, scroll_offset)

            # Blinking cursor
            if frame % 20 < 10:
                cursor_y = screen_y + screen_h - 10
                draw.rectangle([(screen_x + 10, cursor_y), (screen_x + 15, cursor_y + 5)], fill=BLACK)

            # Keyboard
            kb_y = body_y + 25
            draw.rectangle([(body_x + 5, kb_y), (body_x + 40, kb_y + 8)], outline=BLACK, width=1)
            for i in range(6):
                draw.line([(body_x + 8 + i*6, kb_y), (body_x + 8 + i*6, kb_y + 8)], fill=BLACK, width=1)

            img.save(os.path.join(anim_path, f"frame_{frame}.png"))

        # Create meta.txt
        meta_content = f"""Filetype: Flipper Animation
Version: 1

Width: 128
Height: 64
Passive frames: 16
Active frames: 48
Frames order: {' '.join(map(str, range(16)))} {' '.join(map(str, range(16, 48)))}
Active cycles: 1
Frame rate: 10
Duration: 360
Active cooldown: 3

Bubble slots: 0
"""
        with open(os.path.join(anim_path, "meta.txt"), 'w') as f:
            f.write(meta_content)

        print(f"  ✓ Created {num_frames} frames")

    def create_sentinel_secure_animation(self):
        """Create Sentinel secure/protected animation"""
        anim_name = "Sentinel_Secure_128x64"
        anim_path = os.path.join(self.anims_path, anim_name)
        os.makedirs(anim_path, exist_ok=True)

        print(f"Creating animation: {anim_name}")

        num_frames = 40
        for frame in range(num_frames):
            img = self.create_monochrome_image(128, 64)
            draw = ImageDraw.Draw(img)

            # Central dolphin
            body_x, body_y = 45, 25
            draw.ellipse([(body_x, body_y), (body_x + 35, body_y + 25)], outline=BLACK, fill=WHITE, width=2)

            # Head
            head_x, head_y = body_x + 10, body_y - 6
            draw.ellipse([(head_x, head_y), (head_x + 18, head_y + 14)], outline=BLACK, fill=WHITE, width=2)

            # Confident eye
            eye_x, eye_y = head_x + 10, head_y + 5
            draw.line([(eye_x, eye_y), (eye_x + 5, eye_y)], fill=BLACK, width=2)

            # Shield around dolphin (pulsing)
            shield_offset = int(5 * math.sin(frame * math.pi / 20))
            shield_size = 50 + shield_offset
            shield_x = body_x + 17 - shield_size//2
            shield_y = body_y + 12 - shield_size//2
            self.draw_shield(draw, shield_x, shield_y, shield_size, shield_size + 10)

            # Security indicators (corners)
            corner_size = 8
            # Top-left
            draw.line([(0, 0), (corner_size, 0)], fill=BLACK, width=2)
            draw.line([(0, 0), (0, corner_size)], fill=BLACK, width=2)
            # Top-right
            draw.line([(128-corner_size, 0), (128, 0)], fill=BLACK, width=2)
            draw.line([(128, 0), (128, corner_size)], fill=BLACK, width=2)
            # Bottom-left
            draw.line([(0, 64), (corner_size, 64)], fill=BLACK, width=2)
            draw.line([(0, 64-corner_size), (0, 64)], fill=BLACK, width=2)
            # Bottom-right
            draw.line([(128-corner_size, 64), (128, 64)], fill=BLACK, width=2)
            draw.line([(128, 64-corner_size), (128, 64)], fill=BLACK, width=2)

            # Lock icons rotating
            if frame % 30 < 15:
                self.draw_lock(draw, 10, 20, 12, 16)
                self.draw_lock(draw, 106, 20, 12, 16)

            img.save(os.path.join(anim_path, f"frame_{frame}.png"))

        # Create meta.txt
        meta_content = f"""Filetype: Flipper Animation
Version: 1

Width: 128
Height: 64
Passive frames: 20
Active frames: 40
Frames order: {' '.join(map(str, range(20)))} {' '.join(map(str, range(20, 40)))}
Active cycles: 1
Frame rate: 8
Duration: 360
Active cooldown: 3

Bubble slots: 0
"""
        with open(os.path.join(anim_path, "meta.txt"), 'w') as f:
            f.write(meta_content)

        print(f"  ✓ Created {num_frames} frames")

    def create_animation_manifest(self):
        """Create manifest.txt for animations"""
        manifest_content = """Filetype: Flipper Animation Manifest
Version: 1

Name: Sentinel_Scanning_128x64
Min butthurt: 0
Max butthurt: 6
Min level: 1
Max level: 30
Weight: 4

Name: Sentinel_Hacking_128x64
Min butthurt: 0
Max butthurt: 10
Min level: 1
Max level: 30
Weight: 4

Name: Sentinel_Secure_128x64
Min butthurt: 0
Max butthurt: 18
Min level: 1
Max level: 30
Weight: 4
"""
        manifest_path = os.path.join(self.anims_path, "manifest.txt")
        with open(manifest_path, 'w') as f:
            f.write(manifest_content)
        print("✓ Created animation manifest")

    # ==================== ICONS ====================

    def create_dolphin_icons(self):
        """Create Dolphin category icons"""
        icon_path = os.path.join(self.icons_path, "Dolphin")
        os.makedirs(icon_path, exist_ok=True)

        print("Creating Dolphin icons...")

        # DolphinDone - Success with shield
        img = self.create_monochrome_image(80, 58)
        draw = ImageDraw.Draw(img)
        # Dolphin
        draw.ellipse([(15, 15), (50, 45)], outline=BLACK, fill=WHITE, width=2)
        draw.ellipse([(35, 10), (55, 25)], outline=BLACK, fill=WHITE, width=2)
        # Shield
        self.draw_shield(draw, 50, 20, 20, 25)
        # Checkmark
        draw.line([(55, 30), (60, 35), (70, 25)], fill=BLACK, width=3)
        img.save(os.path.join(icon_path, "DolphinDone_80x58.png"))

        # DolphinSuccess - Tactical success
        img = self.create_monochrome_image(91, 55)
        draw = ImageDraw.Draw(img)
        draw.ellipse([(20, 15), (60, 45)], outline=BLACK, fill=WHITE, width=2)
        draw.ellipse([(45, 8), (70, 28)], outline=BLACK, fill=WHITE, width=2)
        # Tactical visor eye
        draw.rectangle([(55, 15), (65, 20)], fill=BLACK)
        # Success indicator
        for i in range(3):
            y = 10 + i * 15
            draw.line([(75, y), (85, y)], fill=BLACK, width=2)
        img.save(os.path.join(icon_path, "DolphinSuccess_91x55.png"))

        # DolphinWait - Scanning mode
        img = self.create_monochrome_image(59, 54)
        draw = ImageDraw.Draw(img)
        draw.ellipse([(10, 15), (40, 40)], outline=BLACK, fill=WHITE, width=2)
        draw.ellipse([(28, 10), (48, 25)], outline=BLACK, fill=WHITE, width=2)
        # Radar eye
        self.draw_radar(draw, 38, 18, 8, 45)
        img.save(os.path.join(icon_path, "DolphinWait_59x54.png"))

        # DolphinSaved - Secure save
        img = self.create_monochrome_image(92, 58)
        draw = ImageDraw.Draw(img)
        draw.ellipse([(15, 15), (50, 45)], outline=BLACK, fill=WHITE, width=2)
        draw.ellipse([(35, 10), (55, 25)], outline=BLACK, fill=WHITE, width=2)
        # Lock icon
        self.draw_lock(draw, 60, 20, 20, 25)
        img.save(os.path.join(icon_path, "DolphinSaved_92x58.png"))

        # WarningDolphin - Alert mode
        img = self.create_monochrome_image(45, 42)
        draw = ImageDraw.Draw(img)
        draw.ellipse([(8, 10), (35, 32)], outline=BLACK, fill=WHITE, width=2)
        draw.ellipse([(22, 5), (40, 18)], outline=BLACK, fill=WHITE, width=2)
        # Alert eye
        draw.ellipse([(28, 10), (34, 14)], fill=BLACK)
        # Warning triangle
        points = [(5, 2), (0, 10), (10, 10)]
        draw.polygon(points, outline=BLACK, fill=WHITE, width=2)
        draw.text((3, 3), "!", fill=BLACK)
        img.save(os.path.join(icon_path, "WarningDolphin_45x42.png"))

        print("  ✓ Created 5 Dolphin icons")

    def create_interface_icons(self):
        """Create Interface category icons"""
        icon_path = os.path.join(self.icons_path, "Interface")
        os.makedirs(icon_path, exist_ok=True)

        print("Creating Interface icons...")

        # LockOpen - Unlocked state
        img = self.create_monochrome_image(30, 30)
        draw = ImageDraw.Draw(img)
        # Open shackle
        draw.arc([(5, 2), (18, 15)], 90, 270, fill=BLACK, width=2)
        # Body
        draw.rectangle([(3, 13), (20, 28)], outline=BLACK, fill=WHITE, width=2)
        draw.ellipse([(9, 17), (14, 22)], fill=BLACK)
        img.save(os.path.join(icon_path, "LockOpen_30x30.png"))

        # LockClosed - Locked state
        img = self.create_monochrome_image(30, 30)
        draw = ImageDraw.Draw(img)
        self.draw_lock(draw, 5, 5, 20, 25)
        img.save(os.path.join(icon_path, "LockClosed_30x30.png"))

        # Security - Shield icon
        img = self.create_monochrome_image(32, 32)
        draw = ImageDraw.Draw(img)
        self.draw_shield(draw, 4, 2, 24, 28)
        img.save(os.path.join(icon_path, "Security_32x32.png"))

        # Scan - Scanning crosshair
        img = self.create_monochrome_image(32, 32)
        draw = ImageDraw.Draw(img)
        self.draw_crosshair(draw, 16, 16, 12)
        # Corner brackets
        for x, y in [(2, 2), (26, 2), (2, 26), (26, 26)]:
            draw.rectangle([(x, y), (x+4, y+1)], fill=BLACK)
            draw.rectangle([(x, y), (x+1, y+4)], fill=BLACK)
        img.save(os.path.join(icon_path, "Scan_32x32.png"))

        # Terminal - Command line
        img = self.create_monochrome_image(32, 32)
        draw = ImageDraw.Draw(img)
        draw.rectangle([(2, 2), (30, 30)], outline=BLACK, width=2)
        # Terminal prompt
        draw.text((5, 8), ">_", fill=BLACK)
        for i in range(3):
            y = 16 + i * 5
            draw.rectangle([(5, y), (20, y+2)], fill=BLACK)
        img.save(os.path.join(icon_path, "Terminal_32x32.png"))

        print("  ✓ Created 5 Interface icons")

    def create_nfc_icons(self):
        """Create NFC category icons"""
        icon_path = os.path.join(self.icons_path, "NFC")
        os.makedirs(icon_path, exist_ok=True)

        print("Creating NFC icons...")

        # NFCSuccess - Successful NFC read
        img = self.create_monochrome_image(60, 60)
        draw = ImageDraw.Draw(img)
        # NFC waves
        center_x, center_y = 30, 30
        for radius in [10, 18, 26]:
            draw.arc([(center_x-radius, center_y-radius), (center_x+radius, center_y+radius)],
                    -45, 45, fill=BLACK, width=2)
        # Checkmark
        draw.line([(25, 30), (28, 35), (35, 25)], fill=BLACK, width=3)
        img.save(os.path.join(icon_path, "NFCSuccess_60x60.png"))

        # NFCRead - Reading NFC
        img = self.create_monochrome_image(50, 50)
        draw = ImageDraw.Draw(img)
        center_x, center_y = 25, 25
        # Animated waves (static version)
        for radius in [8, 15, 22]:
            draw.arc([(center_x-radius, center_y-radius), (center_x+radius, center_y+radius)],
                    -60, 60, fill=BLACK, width=2)
        # Center dot
        draw.ellipse([(center_x-3, center_y-3), (center_x+3, center_y+3)], fill=BLACK)
        img.save(os.path.join(icon_path, "NFCRead_50x50.png"))

        print("  ✓ Created 2 NFC icons")

    def create_subghz_icons(self):
        """Create SubGhz category icons"""
        icon_path = os.path.join(self.icons_path, "SubGhz")
        os.makedirs(icon_path, exist_ok=True)

        print("Creating SubGhz icons...")

        # SignalSuccess - Signal received
        img = self.create_monochrome_image(64, 64)
        draw = ImageDraw.Draw(img)
        # Signal waves
        center_x = 15
        for i, height in enumerate([10, 20, 30, 40, 50]):
            x = center_x + i * 10
            draw.rectangle([(x, 32 - height//2), (x + 6, 32 + height//2)], fill=BLACK)
        # Checkmark
        draw.line([(45, 25), (50, 32), (60, 18)], fill=BLACK, width=3)
        img.save(os.path.join(icon_path, "SignalSuccess_64x64.png"))

        # SignalScan - Scanning for signals
        img = self.create_monochrome_image(64, 64)
        draw = ImageDraw.Draw(img)
        # Radar dish
        draw.arc([(10, 15), (54, 50)], 0, 180, fill=BLACK, width=2)
        draw.arc([(18, 20), (46, 45)], 0, 180, fill=BLACK, width=2)
        # Scanning beam
        draw.line([(32, 32), (32, 5)], fill=BLACK, width=2)
        draw.line([(32, 32), (20, 15)], fill=BLACK, width=1)
        draw.line([(32, 32), (44, 15)], fill=BLACK, width=1)
        img.save(os.path.join(icon_path, "SignalScan_64x64.png"))

        print("  ✓ Created 2 SubGhz icons")

    def create_settings_icons(self):
        """Create Settings category icons"""
        icon_path = os.path.join(self.icons_path, "Settings")
        os.makedirs(icon_path, exist_ok=True)

        print("Creating Settings icons...")

        # Security - Security settings
        img = self.create_monochrome_image(40, 40)
        draw = ImageDraw.Draw(img)
        self.draw_shield(draw, 8, 5, 24, 30)
        # Gear overlay
        center_x, center_y = 20, 20
        draw.ellipse([(center_x-6, center_y-6), (center_x+6, center_y+6)], outline=BLACK, width=2)
        img.save(os.path.join(icon_path, "Security_40x40.png"))

        # System - System settings with tactical grid
        img = self.create_monochrome_image(40, 40)
        draw = ImageDraw.Draw(img)
        # Grid background
        for i in range(0, 40, 8):
            draw.line([(i, 0), (i, 40)], fill=BLACK, width=1)
            draw.line([(0, i), (40, i)], fill=BLACK, width=1)
        # Central system icon
        draw.rectangle([(12, 12), (28, 28)], outline=BLACK, fill=WHITE, width=2)
        draw.line([(20, 8), (20, 12)], fill=BLACK, width=2)
        draw.line([(20, 28), (20, 32)], fill=BLACK, width=2)
        draw.line([(8, 20), (12, 20)], fill=BLACK, width=2)
        draw.line([(28, 20), (32, 20)], fill=BLACK, width=2)
        img.save(os.path.join(icon_path, "System_40x40.png"))

        print("  ✓ Created 2 Settings icons")

    def create_passport_icons(self):
        """Create Passport category icons"""
        icon_path = os.path.join(self.icons_path, "Passport")
        os.makedirs(icon_path, exist_ok=True)

        print("Creating Passport icons...")

        # ProfileSentinel - Tactical profile
        img = self.create_monochrome_image(64, 64)
        draw = ImageDraw.Draw(img)
        # Border
        draw.rectangle([(5, 5), (59, 59)], outline=BLACK, width=2)
        # Tactical dolphin portrait
        draw.ellipse([(20, 15), (44, 35)], outline=BLACK, fill=WHITE, width=2)
        draw.ellipse([(35, 12), (50, 25)], outline=BLACK, fill=WHITE, width=2)
        # Tactical visor
        draw.rectangle([(40, 17), (48, 20)], fill=BLACK)
        # ID bars
        for i in range(3):
            y = 42 + i * 5
            draw.rectangle([(10, y), (35, y+2)], fill=BLACK)
        img.save(os.path.join(icon_path, "ProfileSentinel_64x64.png"))

        print("  ✓ Created 1 Passport icon")

    def generate_all_assets(self):
        """Generate all Sentinel assets"""
        print("=" * 60)
        print("SENTINEL ASSET PACK GENERATOR")
        print("=" * 60)
        print()

        print("[1/3] Generating Animations...")
        print("-" * 60)
        self.create_sentinel_scanning_animation()
        self.create_sentinel_hacking_animation()
        self.create_sentinel_secure_animation()
        self.create_animation_manifest()
        print()

        print("[2/3] Generating Icons...")
        print("-" * 60)
        self.create_dolphin_icons()
        self.create_interface_icons()
        self.create_nfc_icons()
        self.create_subghz_icons()
        self.create_settings_icons()
        self.create_passport_icons()
        print()

        print("[3/3] Summary...")
        print("-" * 60)

        # Count generated assets
        anim_count = len([d for d in os.listdir(self.anims_path) if os.path.isdir(os.path.join(self.anims_path, d))])

        icon_count = 0
        for root, dirs, files in os.walk(self.icons_path):
            icon_count += len([f for f in files if f.endswith('.png')])

        print(f"✓ Animations created: {anim_count}")
        print(f"✓ Icons created: {icon_count}")
        print()
        print("=" * 60)
        print("SENTINEL ASSET PACK GENERATION COMPLETE!")
        print("=" * 60)
        print()
        print("Next steps:")
        print("  1. Run: ./install_assets.sh")
        print("  2. Build: ./fbt updater_package")
        print("  3. Flash: ./fbt flash_usb_full")
        print()

if __name__ == "__main__":
    generator = SentinelAssetGenerator()
    generator.generate_all_assets()
