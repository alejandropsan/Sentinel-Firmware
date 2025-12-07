##### ESPAÑOL / SPANISH #####

# 🗺️ Hoja de ruta del firmware Sentinel

## Fase 1: Núcleo de seguridad (meses 1-3) ✅ EN CURSO

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

- [ ] **Registro de auditoría**
  - [ ] Niveles de registro configurables
  - [ ] Marca de tiempo en todas las operaciones
  - [ ] Exportación a CSV
  - [ ] Registro remoto a través de WiFi
  - [ ] Rotación y límites de registros

- [ ] **Arranque seguro**
  - [ ] Verificación de la suma de comprobación del firmware
  - [ ] Detección de manipulaciones
  - [ ] Alternancia entre modo desarrollador y modo seguro

### Infraestructura
- [x] Repositorio Fork
- [x] Configuración de CI/CD
- [x] Creación del paquete de activos Sentinel
- [ ] Sitio de documentación (MkDocs)
- [ ] Servidor Discord de la comunidad

**Lanzamiento previsto**: Alpha v0.1.0 (mes 3)

---

## Fase 2: Programación avanzada (meses 4-6)

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

**Lanzamiento previsto**: Beta v0.2.0 (mes 6)

---

## Fase 3: Análisis de señales (meses 7-10)

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

**Fecha de lanzamiento prevista**: RC v0.3.0 (mes 10)

---

## Fase 4: Pulido y comunidad (meses 11-12)

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

**Lanzamiento previsto**: v1.0.0 Estable (mes 12)

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

**Objetivos para el primer año**:
- Más de 1000 estrellas en GitHub
- Más de 5000 instalaciones activas
- Más de 50 colaboradores
- Más de 20 aplicaciones exclusivas
- 0 vulnerabilidades de seguridad críticas
- Tasa de fallos inferior al 1 %

**Lista de verificación de diferenciación**:
- ✅ Único firmware con PIN de coacción
- ✅ Único firmware con MicroPython
- ✅ Único firmware con analizador de protocolos
- ✅ Único firmware con DuckyScript 3.0
- ✅ La mejor documentación del ecosistema






##### INGLÉS // ENGLISH #####

# 🗺️ Sentinel Firmware Roadmap

## Phase 1: Security Core (Months 1-3) ✅ IN PROGRESS

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

- [ ] **Audit Logging**
  - [ ] Configurable logging levels
  - [ ] Timestamp all operations
  - [ ] Export to CSV
  - [ ] Remote logging via WiFi
  - [ ] Log rotation and limits

- [ ] **Secure Boot**
  - [ ] Firmware checksum verification
  - [ ] Tamper detection
  - [ ] Developer vs Secure mode toggle

### Infrastructure
- [x] Fork repository
- [x] Setup CI/CD
- [x] Create Sentinel Asset Pack
- [ ] Documentation site (MkDocs)
- [ ] Community Discord server

**Target Release**: Alpha v0.1.0 (Month 3)

---

## Phase 2: Advanced Scripting (Months 4-6)

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

**Target Release**: Beta v0.2.0 (Month 6)

---

## Phase 3: Signal Analysis (Months 7-10)

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

**Target Release**: RC v0.3.0 (Month 10)

---

## Phase 4: Polish & Community (Months 11-12)

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

**Target Release**: v1.0.0 Stable (Month 12)

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

**Year 1 Targets**:
- 1,000+ GitHub stars
- 5,000+ active installations
- 50+ contributors
- 20+ exclusive apps
- 0 critical security vulnerabilities
- Sub-1% crash rate

**Differentiation Checklist**:
- ✅ Only firmware with Duress PIN
- ✅ Only firmware with MicroPython
- ✅ Only firmware with Protocol Analyzer
- ✅ Only firmware with DuckyScript 3.0
- ✅ Best documentation in ecosystem
