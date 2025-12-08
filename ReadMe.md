##### ESPAÑOL // SPANISH #####

# 🔱 Firmware Sentinel

**Firmware Flipper Zero centrado en la seguridad con scripting avanzado y análisis de señales profesional**

[![Build](https://img.shields.io/github/actions/workflow/status/alejandropsan/Sentinel-Firmware/build.yml?branch=dev&label=Build&logo=github)](https://github.com/alejandropsan/Sentinel-Firmware/actions)
[![Licencia](https://img.shields.io/github/license/alejandropsan/Sentinel-Firmware?color=blue)](https://github.com/alejandropsan/Sentinel-Firmware/blob/dev/LICENSE)
[![Descargas](https://img.shields.io/github/downloads/alejandropsan/Sentinel-Firmware/total?color=success)](https://github.com/alejandropsan/Sentinel-Firmware/releases)

## Noticias y Novedades
- ** 07/12/2025 Comienzo del proyecto: Hemos iniciado el fork y la creación del repositorio.**
- ** 08/12/2025 Creación de servidor en Discord: https://discord.gg/eQHHB6SSRU.

## Filosofía

Sentinel llena el vacío entre la estabilidad y la innovación en el ecosistema Flipper Zero:

- ** La seguridad es lo primero**: PIN de coacción, almacenamiento cifrado, registro de auditoría
- ** Fácil de usar para los desarrolladores**: DuckyScript 3.0, MicroPython, macros visuales
- ** Análisis profesional**: analizador de espectro, decodificador de protocolos, demodulación avanzada
- ** Rendimiento estable**: basado en la probada base Momentum

## Características principales

### Seguridad y privacidad
- **PIN de emergencia**: borrado de emergencia en caso de introducción de un PIN comprometido
- **Almacenamiento cifrado**: cifrado AES-256 para archivos confidenciales
- **Registro de auditoría**: seguimiento completo de las operaciones
- **Cadena de arranque segura**: verificación de la integridad del firmware

### Scripting avanzado
- **DuckyScript 3.0**: variables, condicionales, bucles, funciones
- **MicroPython Runtime**: scripting Python completo en el dispositivo
- **Editor visual de macros**: automatización basada en nodos (aplicación móvil)

### Análisis de señales
- **Analizador de espectro**: visualización en tiempo real con pantalla en cascada
- **Decodificador de protocolos**: análisis automático de protocolos desconocidos.
- **Demodulación avanzada**: compatibilidad con 4FSK, PSK y QAM.
- **Herramientas de exportación**: formatos GNURadio, URH, CSV y WAV.

### Heredado de Momentum
- Sistema Asset Packs para una personalización completa.
- Frecuencias SubGHz ampliadas (281-962 MHz).
- Bad-KB con suplantación de Bluetooth
- 8 estilos de interfaz, incluido CoverFlow
- 183 aplicaciones integradas

## Instalación

### Instalador web (recomendado)
1. Visite [sentinel-fw.dev](https://alejandropsan.github.io/oscp/flipperzero/Sentinel-firmware.html) (próximamente)
2. Conecte Flipper Zero a través de USB
3. Haga clic en «Instalar»

### Instalación manual
1. Descargue el último archivo `.tgz` de [Versiones](https://github.com/alejandropsan/Sentinel-Firmware/releases)
2. Abra qFlipper
3. Instale desde el archivo

## 🛠️ Compilación desde el código fuente

```bash
git clone --recursive https://github.com/alejandropsan/Sentinel-Firmware.git
cd Sentinel-Firmware
./fbt flash_usb_full
```

## Hoja de ruta

- [x] Fase 1: Núcleo de seguridad (PIN de coacción, almacenamiento cifrado, registro de auditoría)
- [ ] Fase 2: Scripting avanzado (DuckyScript 3.0, MicroPython)
- [ ] Fase 3: Análisis de señales (analizador de espectro, decodificador de protocolos)
- [ ] Fase 4: Comunidad y pulido

Consulte [ROADMAP.md](ROADMAP.md) para ver el calendario detallado.

## Contribuciones

Sentinel es de código abierto (GPL-3.0) y agradece las contribuciones.

- **Informes de errores**: [Abrir un problema](https://github.com/alejandropsan/Sentinel-Firmware/issues)
- **Solicitudes de funciones**: [Discusiones](https://github.com/alejandropsan/Sentinel-Firmware/discussions)
- **Solicitudes de incorporación de cambios**: Consulte [CONTRIBUTING.md](CONTRIBUTING.md)

## Créditos

Creado sobre la excelente base del [firmware Momentum](https://github.com/Next-Flip/Momentum-Firmware) de WillyJL y su equipo.

Agradecimientos especiales a:
- El equipo de Momentum por el código base
- El equipo de Unleashed por las implementaciones de protocolo
- Flipper Devices por el increíble hardware
- Toda la comunidad Flipper

## Licencia

Licencia GPL-3.0 - Consulte [LICENSE](LICENSE)

## Enlaces

- **Sitio web**: https://alejandropsan.github.io/oscp/flipper-zero/Sentinel-Firmware.html (próximamente)
- **Discord**: https://discord.gg/...... (próximamente)
- **Documentación**: https://docs..... (próximamente)

---

** Descargo de responsabilidad**: Úselo de forma responsable y ética. Pruébelo solo en dispositivos que sean de su propiedad o para los que tenga permiso explícito para probarlos.







##### INGLÉS // ENGLISH #####

# Sentinel Firmware

**Security-focused Flipper Zero firmware with advanced scripting and professional signal analysis**

[![Build](https://img.shields.io/github/actions/workflow/status/alejandropsan/Sentinel-Firmware/build.yml?branch=dev&label=Build&logo=github)](https://github.com/alejandropsan/Sentinel-Firmware/actions)
[![License](https://img.shields.io/github/license/alejandropsan/Sentinel-Firmware?color=blue)](https://github.com/alejandropsan/Sentinel-Firmware/blob/dev/LICENSE)
[![Downloads](https://img.shields.io/github/downloads/alejandropsan/Sentinel-Firmware/total?color=success)](https://github.com/alejandropsan/Sentinel-Firmware/releases)

## News and Updates
- ** 07/12/2025 Project start date: We have begun the fork and created the repository.**
- ** 08/12/2025 Discord server creation: https://discord.gg/eQHHB6SSRU.

## Philosophy

Sentinel fills the gap between stability and innovation in the Flipper Zero ecosystem:

- ** Security First**: Duress PIN, encrypted storage, audit logging
- ** Developer Friendly**: DuckyScript 3.0, MicroPython, visual macros
- ** Professional Analysis**: Spectrum analyzer, protocol decoder, advanced demodulation
- **⚡ Stable Performance**: Built on proven Momentum foundation

## Key Features

### Security & Privacy
- **Duress PIN**: Emergency wipe on compromised PIN entry
- **Encrypted Storage**: AES-256 encryption for sensitive files
- **Audit Logging**: Comprehensive operation tracking
- **Secure Boot Chain**: Firmware integrity verification

### Advanced Scripting
- **DuckyScript 3.0**: Variables, conditionals, loops, functions
- **MicroPython Runtime**: Full Python scripting on-device
- **Visual Macro Editor**: Node-based automation (mobile app)

### Signal Analysis
- **Spectrum Analyzer**: Real-time visualization with waterfall display
- **Protocol Decoder**: Automatic analysis of unknown protocols
- **Advanced Demodulation**: 4FSK, PSK, QAM support
- **Export Tools**: GNURadio, URH, CSV, WAV formats

### Inherited from Momentum
- Asset Packs system for full customization
- Extended SubGHz frequencies (281-962 MHz)
- Bad-KB with Bluetooth spoofing
- 8 interface styles including CoverFlow
- 183 built-in applications

## Installation

### Web Installer (Recommended)
1. Visit [sentinel-fw.dev](https://alejandropsan.github.io/oscp/flipperzero/Sentinel-firmware.html) (coming soon)
2. Connect Flipper Zero via USB
3. Click "Install"

### Manual Installation
1. Download latest `.tgz` from [Releases](https://github.com/alejandropsan/Sentinel-Firmware/releases)
2. Open qFlipper
3. Install from file

## Building from Source

```bash
git clone --recursive https://github.com/alejandropsan/Sentinel-Firmware.git
cd Sentinel-Firmware
./fbt flash_usb_full
```

## Roadmap

- [x] Phase 1: Security Core (Duress PIN, Encrypted Storage, Audit Log)
- [ ] Phase 2: Advanced Scripting (DuckyScript 3.0, MicroPython)
- [ ] Phase 3: Signal Analysis (Spectrum Analyzer, Protocol Decoder)
- [ ] Phase 4: Community & Polish

See [ROADMAP.md](ROADMAP.md) for detailed timeline.

## Contributing

Sentinel is open source (GPL-3.0) and welcomes contributions!

- **Bug Reports**: [Open an issue](https://github.com/alejandropsan/Sentinel-Firmware/issues)
- **Feature Requests**: [Discussions](https://github.com/alejandropsan/Sentinel-Firmware/discussions)
- **Pull Requests**: See [CONTRIBUTING.md](CONTRIBUTING.md)

## Credits

Built on the excellent foundation of [Momentum Firmware](https://github.com/Next-Flip/Momentum-Firmware) by WillyJL and team.

Special thanks to:
- Momentum team for the codebase
- Unleashed team for protocol implementations
- Flipper Devices for the amazing hardware
- The entire Flipper community

## License

GPL-3.0 License - See [LICENSE](LICENSE)

## Links

- **Website**: https://alejandropsan.github.io/oscp/flipper-zero/Sentinel-Firmware.html (coming soon)
- **Discord**: https://discord.gg/...... (coming soon)
- **Documentation**: https://docs..... (coming soon)

---

**Disclaimer**: Use responsibly and ethically. Only test on devices you own or have explicit permission to test.
