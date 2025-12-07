#!/bin/bash
# 🔱 Sentinel Firmware - Master Setup Script
# Este script crea TODOS los archivos necesarios automáticamente
# Ejecutar desde la raíz del repositorio clonado de tu fork

set -e

echo "════════════════════════════════════════════════════════"
echo "🔱 SENTINEL FIRMWARE - MASTER SETUP SCRIPT"
echo "════════════════════════════════════════════════════════"
echo ""
echo "Este script creará todos los archivos necesarios para"
echo "transformar tu fork de Momentum en Sentinel Firmware."
echo ""

# Verificar que estamos en el directorio correcto
if [ ! -f "fbt" ]; then
    echo "❌ Error: No se encuentra 'fbt'"
    echo "   Por favor ejecuta este script desde la raíz del repositorio"
    exit 1
fi

# Crear estructura de directorios
echo "📁 Creando estructura de directorios..."
mkdir -p .github/workflows
mkdir -p .github/ISSUE_TEMPLATE
mkdir -p asset_pack_sentinel/{Anims,Icons,Fonts}

# ============================================================================
# ARCHIVO 1: .github/workflows/build.yml
# ============================================================================
echo "Creando .github/workflows/build.yml..."
cat > .github/workflows/build.yml << 'WORKFLOW_BUILD_EOF'
name: Build Sentinel Firmware

on:
  push:
    branches: [ dev, main ]
  pull_request:
    branches: [ dev ]
  workflow_dispatch:

env:
  FIRMWARE_NAME: Sentinel

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - name: Checkout Sentinel Firmware
      uses: actions/checkout@v4
      with:
        submodules: recursive
        fetch-depth: 0
    
    - name: Setup Python
      uses: actions/setup-python@v5
      with:
        python-version: '3.11'
    
    - name: Cache FBT toolchain
      uses: actions/cache@v4
      with:
        path: |
          ~/.fbt_cache
          toolchain
        key: ${{ runner.os }}-fbt-${{ hashFiles('scripts/toolchain/**') }}
        restore-keys: |
          ${{ runner.os }}-fbt-
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y git wget libusb-1.0-0-dev
    
    - name: Build firmware
      run: |
        ./fbt updater_package
        
    - name: Build info
      run: |
        ./fbt version_get
        ls -lh dist/
    
    - name: Upload artifacts - Full Package
      uses: actions/upload-artifact@v4
      with:
        name: sentinel-firmware-full-${{ github.sha }}
        path: dist/f7-C/*.tgz
        retention-days: 30
    
    - name: Upload artifacts - Update Package
      uses: actions/upload-artifact@v4
      with:
        name: sentinel-firmware-update-${{ github.sha }}
        path: dist/f7-C/*.zip
        retention-days: 30
    
    - name: Check firmware size
      run: |
        echo "### Firmware Size Report" >> $GITHUB_STEP_SUMMARY
        echo "" >> $GITHUB_STEP_SUMMARY
        echo "\`\`\`" >> $GITHUB_STEP_SUMMARY
        ./fbt firmware_size || true
        echo "\`\`\`" >> $GITHUB_STEP_SUMMARY

  release:
    needs: build
    runs-on: ubuntu-latest
    if: startsWith(github.ref, 'refs/tags/')
    
    steps:
    - name: Checkout
      uses: actions/checkout@v4
    
    - name: Download artifacts
      uses: actions/download-artifact@v4
      with:
        pattern: sentinel-firmware-*
        path: release-files
    
    - name: Generate changelog
      id: changelog
      run: |
        VERSION=${GITHUB_REF#refs/tags/}
        echo "VERSION=$VERSION" >> $GITHUB_OUTPUT
        
        if grep -q "## \[$VERSION\]" CHANGELOG.md; then
          sed -n "/## \[$VERSION\]/,/## \[/p" CHANGELOG.md | sed '$d' > release-notes.md
        else
          echo "No changelog found for $VERSION" > release-notes.md
        fi
    
    - name: Create Release
      uses: softprops/action-gh-release@v1
      with:
        name: Sentinel Firmware ${{ steps.changelog.outputs.VERSION }}
        body_path: release-notes.md
        files: |
          release-files/**/*.tgz
          release-files/**/*.zip
        draft: false
        prerelease: ${{ contains(github.ref, 'alpha') || contains(github.ref, 'beta') || contains(github.ref, 'rc') }}
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}

  lint:
    runs-on: ubuntu-latest
    
    steps:
    - name: Checkout
      uses: actions/checkout@v4
      with:
        submodules: recursive
    
    - name: Setup Python
      uses: actions/setup-python@v5
      with:
        python-version: '3.11'
    
    - name: Run linter
      run: |
        ./fbt lint || true
    
    - name: Code formatting check
      run: |
        ./fbt format_check || true
WORKFLOW_BUILD_EOF

# ============================================================================
# ARCHIVO 2: .github/workflows/pr-validation.yml
# ============================================================================
echo "📝 Creando .github/workflows/pr-validation.yml..."
cat > .github/workflows/pr-validation.yml << 'WORKFLOW_PR_EOF'
name: PR Validation

on:
  pull_request:
    types: [opened, synchronize, reopened]

jobs:
  validate:
    runs-on: ubuntu-latest
    
    steps:
    - name: Checkout
      uses: actions/checkout@v4
      with:
        submodules: recursive
        fetch-depth: 0
    
    - name: Setup Python
      uses: actions/setup-python@v5
      with:
        python-version: '3.11'
    
    - name: Check commit messages
      run: |
        echo "Checking commit messages..."
        git log --format=%s origin/${{ github.base_ref }}..HEAD | \
        grep -E '^(feat|fix|docs|style|refactor|test|chore|security)(\(.+\))?: .+' || \
        echo "Consider using conventional commit format"
    
    - name: Check for CHANGELOG update
      run: |
        if git diff --name-only origin/${{ github.base_ref }}...HEAD | grep -q "CHANGELOG.md"; then
          echo "CHANGELOG.md updated"
        else
          echo "Consider updating CHANGELOG.md"
        fi
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y git wget
    
    - name: Test build
      run: |
        ./fbt updater_package
    
    - name: Comment build status
      uses: actions/github-script@v7
      if: always()
      with:
        script: |
          const status = '${{ job.status }}' === 'success' ? '✅' : '❌';
          const body = `### ${status} Build ${status === '✅' ? 'Successful' : 'Failed'}
          
          **Commit**: ${{ github.event.pull_request.head.sha }}
          **Branch**: ${{ github.event.pull_request.head.ref }}
          
          ${status === '✅' ? 'All checks passed! Ready for review.' : 'Build failed. Please check the logs.'}`;
          
          github.rest.issues.createComment({
            issue_number: context.issue.number,
            owner: context.repo.owner,
            repo: context.repo.repo,
            body: body
          });
WORKFLOW_PR_EOF

# ============================================================================
# ARCHIVO 3: Rebrand script
# ============================================================================
echo "Creando script de rebrand..."
cat > rebrand.sh << 'REBRAND_EOF'
#!/bin/bash
echo "Iniciando rebrand a Sentinel Firmware..."

# Actualizar fbt_options.py
sed -i 's/FIRMWARE_NAME="Momentum"/FIRMWARE_NAME="Sentinel"/' fbt_options.py || true
sed -i 's/COPRO_OB_DATA="scripts\/copro_ob_data_Momentum.py"/COPRO_OB_DATA="scripts\/copro_ob_data_Sentinel.py"/' fbt_options.py || true

# Copiar archivo copro si existe
if [ -f "scripts/copro_ob_data_Momentum.py" ]; then
    cp scripts/copro_ob_data_Momentum.py scripts/copro_ob_data_Sentinel.py
fi

echo "Rebrand completado"
REBRAND_EOF

chmod +x rebrand.sh

# ============================================================================
# ARCHIVO 4: Asset Pack manifest
# ============================================================================
echo "Creando Asset Pack..."
cat > asset_pack_sentinel/meta.txt << 'ASSET_META_EOF'
Filetype: Flipper Asset Pack
Version: 1

Name: Sentinel
Description: Dark theme designed for security professionals and penetration testers. Features minimalist icons, professional color scheme, and enhanced readability.
Author: Sentinel Team
Version: 1.0.0
Homepage: https://github.com/TU_USUARIO/Sentinel-Firmware
ASSET_META_EOF

cat > asset_pack_sentinel/Anims/manifest.txt << 'ASSET_MANIFEST_EOF'
Filetype: Flipper Animation Manifest
Version: 1

Name: Sentinel
Description: Dark, professional theme for security professionals
Author: Sentinel Team
Version: 1.0.0

Idle frames: 15
Active frames: 0
Passive frames: 0

Min butthurt: 0
Max butthurt: 14
Min level: 1
Max level: 30
Weight: 8

Width: 128
Height: 64
Duration: 1500
ASSET_MANIFEST_EOF

# ============================================================================
# Ejecutar rebrand
# ============================================================================
echo ""
echo "Ejecutando rebrand..."
./rebrand.sh

echo ""
echo "════════════════════════════════════════════════════════"
echo "SETUP COMPLETADO"
echo "════════════════════════════════════════════════════════"
echo ""
echo "Archivos creados:"
echo "  ✓ .github/workflows/build.yml"
echo "  ✓ .github/workflows/pr-validation.yml"
echo "  ✓ rebrand.sh"
echo "  ✓ asset_pack_sentinel/"
echo ""
echo "Próximos pasos:"
echo ""
echo "1. Personaliza tu usuario de GitHub:"
echo "   sed -i 's/TU_USUARIO/tu_usuario_github/g' asset_pack_sentinel/meta.txt"
echo ""
echo "2. Prueba la compilación:"
echo "   ./fbt updater_package"
echo ""
echo "3. Commit y push:"
echo "   git add ."
echo "   git commit -m 'feat: initial Sentinel Firmware setup'"
echo "   git push origin dev"
echo ""
echo "4. Habilita GitHub Actions en tu repo:"
echo "   - Ve a tu repo en GitHub"
echo "   - Click en 'Actions' tab"
echo "   - Click 'I understand, enable them'"
echo ""