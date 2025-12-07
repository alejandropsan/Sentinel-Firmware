#!/bin/bash

# Sentinel Firmware - Asset Pack Installation Script
# This script installs the Sentinel custom asset pack

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ASSET_PACK_SOURCE="${SCRIPT_DIR}/asset_pack_sentinel"
ASSET_PACK_DEST="${SCRIPT_DIR}/assets/packs/Sentinel"

echo -e "${BLUE}================================================${NC}"
echo -e "${BLUE}  Sentinel Firmware - Asset Pack Installer${NC}"
echo -e "${BLUE}================================================${NC}"
echo ""

# Check if source asset pack exists
if [ ! -d "$ASSET_PACK_SOURCE" ]; then
    echo -e "${RED}ERROR: Asset pack source directory not found at:${NC}"
    echo -e "${RED}       $ASSET_PACK_SOURCE${NC}"
    exit 1
fi

# Check if meta.txt exists
if [ ! -f "$ASSET_PACK_SOURCE/meta.txt" ]; then
    echo -e "${RED}ERROR: meta.txt not found in asset pack${NC}"
    exit 1
fi

echo -e "${GREEN}✓${NC} Found asset pack source"
echo -e "  Source: ${ASSET_PACK_SOURCE}"
echo ""

# Display asset pack info
echo -e "${YELLOW}Asset Pack Information:${NC}"
grep -E "^Name:|^Description:|^Author:|^Version:" "$ASSET_PACK_SOURCE/meta.txt" | while read line; do
    echo -e "  $line"
done
echo ""

# Create destination directory if it doesn't exist
if [ ! -d "$ASSET_PACK_DEST" ]; then
    echo -e "${YELLOW}Creating destination directory...${NC}"
    mkdir -p "$ASSET_PACK_DEST"
fi

echo -e "${GREEN}✓${NC} Destination directory ready"
echo -e "  Destination: ${ASSET_PACK_DEST}"
echo ""

# Copy asset pack files
echo -e "${YELLOW}Installing asset pack...${NC}"

# Copy Anims folder
if [ -d "$ASSET_PACK_SOURCE/Anims" ]; then
    echo -e "  ${BLUE}→${NC} Copying animations..."
    cp -r "$ASSET_PACK_SOURCE/Anims" "$ASSET_PACK_DEST/"
    echo -e "  ${GREEN}✓${NC} Animations installed"
fi

# Copy Fonts folder
if [ -d "$ASSET_PACK_SOURCE/Fonts" ]; then
    echo -e "  ${BLUE}→${NC} Copying fonts..."
    cp -r "$ASSET_PACK_SOURCE/Fonts" "$ASSET_PACK_DEST/"
    echo -e "  ${GREEN}✓${NC} Fonts installed"
fi

# Copy Icons folder
if [ -d "$ASSET_PACK_SOURCE/Icons" ]; then
    echo -e "  ${BLUE}→${NC} Copying icons..."
    cp -r "$ASSET_PACK_SOURCE/Icons" "$ASSET_PACK_DEST/"
    echo -e "  ${GREEN}✓${NC} Icons installed"
fi

# Note: meta.txt is informational only, not copied to build

echo ""
echo -e "${GREEN}================================================${NC}"
echo -e "${GREEN}  Asset Pack Installation Complete!${NC}"
echo -e "${GREEN}================================================${NC}"
echo ""
echo -e "${YELLOW}Next Steps:${NC}"
echo -e "  1. Build the firmware: ${BLUE}./fbt updater_package${NC}"
echo -e "  2. Flash to device: ${BLUE}./fbt flash_usb_full${NC}"
echo -e "  3. On your Flipper Zero, go to:"
echo -e "     ${BLUE}Settings → Desktop → Dolphin${NC}"
echo -e "     and select the Sentinel theme"
echo ""
echo -e "${GREEN}✓${NC} Enjoy your Sentinel-themed Flipper Zero!"
echo ""
