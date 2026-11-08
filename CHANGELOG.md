##### ESPAÑOL // SPANISH #####

# Registro de cambios

Todos los cambios importantes en el firmware de Sentinel se documentarán en este archivo.

El formato se basa en [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
y este proyecto se adhiere a [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Sin publicar]

### Añadido
- Bifurcación inicial del firmware Momentum mntm-011.
- Marca Sentinel (`FIRMWARE_ORIGIN = "Sentinel"`).
- Asset pack Sentinel con 3 animaciones (Scanning, Secure, Security) e iconos propios.
- Iconos Sentinel en el menú principal (`A_Sentinel_14`) y en el menú de bloqueo (`CC_Sentinel_16x16`), que sustituyen a los de Momentum.
- Canalización CI/CD con GitHub Actions.
- Hoja de ruta del proyecto y estructura de la documentación.
- Guías para el desarrollo asistido por IA (`CLAUDE.md`, `AGENTS.md`).

### Corregido
- Cuelgue al arrancar (`out of memory` / `NULL pointer dereference`) con el pack Sentinel: las animaciones se reducen de 128 a 64 fotogramas para no superar ~37 KB de RAM cada una.
- La compilación fallaba al generar los iconos porque había imágenes de distinto tamaño en `Sentinel_14`.
- Estado de símbolos no válido en `targets/f7/api_symbols.csv`.
- `meta.txt` de la animación `Sentinel_Security` con formato inválido.
- `lint_img` fallaba por el logo del README, que se ha movido a `.github/assets/`.

### Cambiado
- La documentación (ReadMe, ROADMAP) refleja el estado real: las funciones de seguridad están planificadas, todavía no implementadas.

---

## Orígenes del proyecto

El firmware Sentinel se basa en [Momentum Firmware](https://github.com/Next-Flip/Momentum-Firmware),
que a su vez evolucionó a partir de Xtreme Firmware. Agradecemos a todos los colaboradores que construyeron esta base.

**Créditos de Momentum**: WillyJL, xMasterX, Dmitry422, 956MB y más de 144 colaboradores.
**Créditos de Xtreme**: ClaraCrazy y el equipo original de Xtreme.






##### INGLÉS // ENGLISH #####

# Changelog

All notable changes to Sentinel Firmware will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial fork from Momentum Firmware mntm-011
- Sentinel branding (`FIRMWARE_ORIGIN = "Sentinel"`)
- Sentinel asset pack with 3 animations (Scanning, Secure, Security) and custom icons
- Sentinel icons in the main menu (`A_Sentinel_14`) and lock menu (`CC_Sentinel_16x16`), replacing the Momentum ones
- CI/CD pipeline with GitHub Actions
- Project roadmap and documentation structure
- Guides for AI-assisted development (`CLAUDE.md`, `AGENTS.md`)

### Fixed
- Boot crash (`out of memory` / `NULL pointer dereference`) with the Sentinel pack: animations reduced from 128 to 64 frames so each one stays under ~37 KB of RAM
- Build failed during icon generation because `Sentinel_14` contained images of different sizes
- Invalid symbol status in `targets/f7/api_symbols.csv`
- Invalid `meta.txt` format in the `Sentinel_Security` animation
- `lint_img` failed because of the README logo, now moved to `.github/assets/`

### Changed
- Documentation (ReadMe, ROADMAP) reflects the real status: security features are planned, not implemented yet

---

## Project Origins

Sentinel Firmware is built on [Momentum Firmware](https://github.com/Next-Flip/Momentum-Firmware),
which itself evolved from Xtreme Firmware. We are grateful to all contributors who built this foundation.

**Momentum Credits**: WillyJL, xMasterX, Dmitry422, 956MB, and 144+ other contributors
**Xtreme Credits**: ClaraCrazy and the original Xtreme team
