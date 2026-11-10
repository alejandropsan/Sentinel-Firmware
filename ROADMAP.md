##### ESPAÑOL / SPANISH #####

# 🗺️ Hoja de ruta del firmware Sentinel

> **Estado actual (noviembre 2026):** Fase 0 completada. La Fase 1 aún no ha empezado y ninguna función de seguridad está implementada.
> Las fases no tienen fechas comprometidas: los objetivos de lanzamiento se cumplirán al completar cada fase.

## Fase 0: Base del proyecto ✅ COMPLETADA

- [x] Fork de Momentum (mntm-011)
- [x] Marca Sentinel (`FIRMWARE_ORIGIN`)
- [x] Configuración de CI/CD (GitHub Actions)
- [x] Asset pack Sentinel: 3 animaciones e iconos propios
- [x] Iconos Sentinel en el menú principal y en el menú de bloqueo
- [x] Servidor de Discord de la comunidad

## Fase 1: Núcleo de seguridad ⏳ SIGUIENTE (sin empezar)

### Funciones de seguridad
- [ ] **Sistema PIN de coacción**
  - [ ] Múltiples perfiles de borrado (todo, solo aplicaciones, solo SubGHz)
  - [ ] Activación silenciosa (sin indicación visual)
  - [ ] Activación remota del borrado (a través de la placa de desarrollo WiFi)
  - [ ] Protección del modo de recuperación

- [ ] **Almacenamiento cifrado**
  - [ ] Cifrado AES-256 con mbedTLS
  - [ ] Directorios cifrados: `/secure/subghz`, `/secure/nfc`, `/secure/badusb`
  - [ ] Derivación de claves basada en contraseña (PBKDF2)
  - [ ] Cifrado/descifrado transparente

- [x] **Registro de auditoría** (v1; diseño en `documentation/sentinel/audit-log-design.md`)
  - [x] Niveles de registro configurables (`audit level`)
  - [x] Marca de tiempo en todas las operaciones
  - [x] Exportación a CSV (`audit export`)
  - [x] Cadena MAC con la clave del dispositivo para detectar manipulaciones (`audit verify`)
  - [x] Rotación y límites de registros
  - [ ] Registro remoto a través de WiFi (v2)
  - [ ] Cifrado del registro (llegará con el almacenamiento cifrado)

- [ ] **Arranque seguro**
  - [ ] Verificación de la suma de comprobación del firmware
  - [ ] Detección de manipulaciones
  - [ ] Alternancia entre modo desarrollador y modo seguro

### Infraestructura
- [ ] Sitio de documentación (MkDocs)

**Lanzamiento previsto**: Alpha v0.1.0 (al completar la Fase 1)

---

## Fase 2: Programación avanzada

### DuckyScript 3.0
- [ ] Implementación del analizador sintáctico (flex/bison)
- [ ] Compatibilidad con variables
- [ ] Condicionales (IF/ELSE)
- [ ] Bucles (WHILE/FOR)
- [ ] Funciones (DEFINE/CALL)
- [ ] Detección del sistema operativo
- [ ] Retrasos aleatorios
- [ ] Operaciones con cadenas

### Tiempo de ejecución de MicroPython
- [ ] Compilación mínima de MicroPython (<100 KB)
- [ ] Enlaces API Flipper:
  - [ ] Módulo `flipper.subghz`
  - [ ] Módulo `flipper.nfc`
  - [ ] Módulo `flipper.gpio`
  - [ ] Módulo `flipper.storage`
  - [ ] Módulo `flipper.notification`
- [ ] Gestión de memoria y ajuste de GC
- [ ] Editor de scripts en la aplicación móvil
- [ ] Biblioteca de scripts de ejemplo

### Sistema de macros visuales
- [ ] Formato de macros basado en JSON
- [ ] Motor de tiempo de ejecución de macros
- [ ] Interfaz de usuario de la aplicación móvil (React Native)
- [ ] Tipos de nodos:
  - [ ] Escaneo/reproducción SubGHz
  - [ ] Lectura/escritura NFC
  - [ ] Control GPIO
  - [ ] Retrasos y esperas
  - [ ] Condicionales
  - [ ] Notificaciones

**Lanzamiento previsto**: Beta v0.2.0 (al completar la fase)

---

## Fase 3: Análisis de señales

### Analizador de espectro
- [ ] Visualización FFT en tiempo real
- [ ] Opción de visualización en cascada
- [ ] Detección automática de picos
- [ ] Intervalo y RBW configurables
- [ ] Marcadores de frecuencia
- [ ] Capacidad de captura de pantalla/exportación

### Analizador de protocolos
- [ ] Detección automática de modulación
- [ ] Extracción de patrones de sincronización
- [ ] Reconocimiento de patrones de bits
- [ ] Detección de CRC/suma de comprobación
- [ ] Estructura de protocolo sugerida
- [ ] Exportación a Universal Radio Hacker

### Demodulación avanzada
- [ ] Implementación de 4FSK
- [ ] Demodulación PSK
- [ ] Detección de chirp LoRa
- [ ] Parámetros de demodulación configurables
- [ ] Métricas de calidad de la señal

### Exportación e integración
- [ ] Formato .complex (GNURadio)
- [ ] Exportación .wav
- [ ] Datos sin procesar .csv
- [ ] Archivos de proyecto .urh
- [ ] Transferencia directa a herramientas de PC

**Lanzamiento previsto**: RC v0.3.0 (al completar la fase)

---

## Fase 4: Pulido y comunidad

### Documentación
- [ ] Documentación completa de la API (Doxygen)
- [ ] Guías de usuario para todas las funciones
- [ ] Tutoriales en vídeo
- [ ] Guías para desarrolladores
- [ ] Sección de preguntas frecuentes

### Aplicaciones de la comunidad
- [ ] Portar aplicaciones seleccionadas de RogueMaster:
  - [ ] Laser Tag
  - [ ] Protocolo X10
  - [ ] Calculadoras avanzadas
  - [ ] Herramientas de red
- [ ] Desarrollar aplicaciones exclusivas:
  - [ ] Kit de herramientas de pruebas de penetración
  - [ ] Fuzzer de protocolos
  - [ ] Generador de señales
- [ ] Sistema de envío de la comunidad

### Estabilidad y rendimiento
- [ ] Perfilado y optimización de la memoria
- [ ] Pruebas de duración de la batería
- [ ] Pruebas de estrés de todas las funciones
- [ ] Programa de pruebas beta
- [ ] Programa de recompensa por errores
- [ ] Comparativas de rendimiento con otros firmwares

### Lanzamiento
- [ ] Instalador web en sentinel-fw.dev
- [ ] Galería de paquetes de activos
- [ ] Lanzamiento de la comunidad Discord
- [ ] Anuncio en r/flipperzero
- [ ] Lanzamiento del sitio de documentación

**Lanzamiento previsto**: v1.0.0 Estable (al completar la fase)

---

## Consideraciones futuras (después de la versión 1.0)

- Análisis de protocolos asistido por IA (aprendizaje automático en el dispositivo)
- Sincronización en la nube para archivos cifrados
- Funciones de colaboración en equipo
- Compatibilidad con complementos de hardware (placas personalizadas)
- Funciones de radio avanzadas (capacidades SDR)
- Integración con herramientas Kali Linux
- Funcionamiento remoto a través de API

---

## Métricas de éxito

**Objetivos para el primer año tras la versión 1.0**:
- Más de 1000 estrellas en GitHub
- Más de 5000 instalaciones activas
- Más de 50 colaboradores
- Más de 20 aplicaciones exclusivas
- 0 vulnerabilidades de seguridad críticas
- Tasa de fallos inferior al 1 %

**Objetivos de diferenciación** (ninguno conseguido todavía):
- [ ] Único firmware con PIN de coacción
- [ ] Único firmware con MicroPython
- [ ] Único firmware con analizador de protocolos
- [ ] Único firmware con DuckyScript 3.0
- [ ] La mejor documentación del ecosistema






##### INGLÉS // ENGLISH #####

# 🗺️ Sentinel Firmware Roadmap

> **Current status (November 2026):** Phase 0 complete. Phase 1 has not started and no security feature is implemented yet.
> Phases have no committed dates: release targets are reached when each phase is complete.

## Phase 0: Project Foundation ✅ COMPLETE

- [x] Fork of Momentum (mntm-011)
- [x] Sentinel branding (`FIRMWARE_ORIGIN`)
- [x] CI/CD setup (GitHub Actions)
- [x] Sentinel asset pack: 3 animations and custom icons
- [x] Sentinel icons in the main menu and lock menu
- [x] Community Discord server

## Phase 1: Security Core ⏳ NEXT (not started)

### Security Features
- [ ] **Duress PIN System**
  - [ ] Multiple wipe profiles (all, apps only, SubGHz only)
  - [ ] Silent activation (no visual indication)
  - [ ] Remote wipe trigger (via WiFi Dev Board)
  - [ ] Recovery mode protection

- [ ] **Encrypted Storage**
  - [ ] AES-256 encryption using mbedTLS
  - [ ] Encrypted directories: `/secure/subghz`, `/secure/nfc`, `/secure/badusb`
  - [ ] Password-based key derivation (PBKDF2)
  - [ ] Transparent encryption/decryption

- [x] **Audit Logging** (v1; design in `documentation/sentinel/audit-log-design.md`)
  - [x] Configurable logging levels (`audit level`)
  - [x] Timestamp all operations
  - [x] Export to CSV (`audit export`)
  - [x] MAC chain with the device key to detect tampering (`audit verify`)
  - [x] Log rotation and limits
  - [ ] Remote logging via WiFi (v2)
  - [ ] Log encryption (comes with encrypted storage)

- [ ] **Secure Boot**
  - [ ] Firmware checksum verification
  - [ ] Tamper detection
  - [ ] Developer vs Secure mode toggle

### Infrastructure
- [ ] Documentation site (MkDocs)

**Target Release**: Alpha v0.1.0 (when Phase 1 is complete)

---

## Phase 2: Advanced Scripting

### DuckyScript 3.0
- [ ] Parser implementation (flex/bison)
- [ ] Variables support
- [ ] Conditionals (IF/ELSE)
- [ ] Loops (WHILE/FOR)
- [ ] Functions (DEFINE/CALL)
- [ ] OS detection
- [ ] Random delays
- [ ] String operations

### MicroPython Runtime
- [ ] Minimal MicroPython build (<100KB)
- [ ] Flipper API bindings:
  - [ ] `flipper.subghz` module
  - [ ] `flipper.nfc` module
  - [ ] `flipper.gpio` module
  - [ ] `flipper.storage` module
  - [ ] `flipper.notification` module
- [ ] Memory management and GC tuning
- [ ] Script editor in mobile app
- [ ] Example scripts library

### Visual Macro System
- [ ] JSON-based macro format
- [ ] Macro runtime engine
- [ ] Mobile app UI (React Native)
- [ ] Node types:
  - [ ] SubGHz scan/replay
  - [ ] NFC read/write
  - [ ] GPIO control
  - [ ] Delays and waits
  - [ ] Conditionals
  - [ ] Notifications

**Target Release**: Beta v0.2.0 (when the phase is complete)

---

## Phase 3: Signal Analysis

### Spectrum Analyzer
- [ ] Real-time FFT visualization
- [ ] Waterfall display option
- [ ] Automatic peak detection
- [ ] Configurable span and RBW
- [ ] Frequency markers
- [ ] Screenshot/export capability

### Protocol Analyzer
- [ ] Automatic modulation detection
- [ ] Timing pattern extraction
- [ ] Bit pattern recognition
- [ ] CRC/checksum detection
- [ ] Suggested protocol structure
- [ ] Export to Universal Radio Hacker

### Advanced Demodulation
- [ ] 4FSK implementation
- [ ] PSK demodulation
- [ ] LoRa chirp detection
- [ ] Configurable demod parameters
- [ ] Signal quality metrics

### Export & Integration
- [ ] .complex format (GNURadio)
- [ ] .wav export
- [ ] .csv raw data
- [ ] .urh project files
- [ ] Direct transfer to PC tools

**Target Release**: RC v0.3.0 (when the phase is complete)

---

## Phase 4: Polish & Community

### Documentation
- [ ] Complete API documentation (Doxygen)
- [ ] User guides for all features
- [ ] Video tutorials
- [ ] Developer guides
- [ ] FAQ section

### Community Apps
- [ ] Port curated apps from RogueMaster:
  - [ ] Laser Tag
  - [ ] X10 Protocol
  - [ ] Advanced calculators
  - [ ] Network tools
- [ ] Develop exclusive apps:
  - [ ] Pentesting toolkit
  - [ ] Protocol fuzzer
  - [ ] Signal generator
- [ ] Community submission system

### Stability & Performance
- [ ] Memory profiling and optimization
- [ ] Battery life testing
- [ ] Stress testing all features
- [ ] Beta testing program
- [ ] Bug bounty program
- [ ] Performance benchmarks vs other firmwares

### Release
- [ ] Web installer at sentinel-fw.dev
- [ ] Asset Pack gallery
- [ ] Discord community launch
- [ ] Announcement on r/flipperzero
- [ ] Documentation site launch

**Target Release**: v1.0.0 Stable (when the phase is complete)

---

## Future Considerations (Post v1.0)

- AI-assisted protocol analysis (on-device ML)
- Cloud sync for encrypted files
- Team collaboration features
- Hardware add-on support (custom boards)
- Advanced radio features (SDR capabilities)
- Integration with Kali Linux tools
- Remote operation via API

---

## Success Metrics

**First-year targets after v1.0**:
- 1,000+ GitHub stars
- 5,000+ active installations
- 50+ contributors
- 20+ exclusive apps
- 0 critical security vulnerabilities
- Sub-1% crash rate

**Differentiation goals** (none achieved yet):
- [ ] Only firmware with Duress PIN
- [ ] Only firmware with MicroPython
- [ ] Only firmware with Protocol Analyzer
- [ ] Only firmware with DuckyScript 3.0
- [ ] Best documentation in ecosystem
