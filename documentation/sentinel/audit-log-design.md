# Diseño: registro de auditoría (Fase 1)

Estado: **aprobado** (08/11/2026; decisiones en la sección 12). No hay código todavía.

## 1. Objetivo

Guardar en el propio Flipper un registro de los eventos relevantes para la seguridad: quién desbloqueó
el dispositivo, PIN fallidos, qué apps se ejecutaron y cuándo se conectó un ordenador. El registro debe
**dejar en evidencia la manipulación**, aunque no pueda impedirla. También es la base que usarán el PIN
de coacción y el almacenamiento cifrado.

### Alcance de la v1
- Servicio de firmware que recibe eventos y los escribe en la SD sin bloquear a quien los emite.
- Eventos del sistema: arranque, bloqueo y desbloqueo, PIN, apps, SD y sesiones RPC.
- Formato CSV legible, con niveles, marca de tiempo y cadena de hashes.
- Rotación de archivos con un tamaño máximo.
- Comandos CLI para leer, verificar, exportar y borrar el registro.

### Fuera de la v1
- Cifrado del registro: llegará con el almacenamiento cifrado (sección 8).
- Envío remoto por WiFi.
- Interfaz gráfica de configuración.
- API pública para las apps `.fap`, que obligaría a cambiar `api_symbols.csv` y a subir la versión de la API.

## 2. Modelo de amenazas

| Escenario | ¿Lo cubre la v1? |
|---|---|
| Alguien usa el Flipper desbloqueado sin permiso | Sí: quedan registradas las apps y las sesiones RPC/qFlipper |
| Intentos de adivinar el PIN | Sí: cada fallo queda registrado, con el contador |
| Alguien edita o borra líneas del registro desde otro equipo | **Se detecta** con la cadena de hashes (`audit verify`), pero no se impide |
| Alguien borra el archivo entero o formatea la SD | No se detecta en la v1; ver el anclaje en RTC (sección 9) |
| Alguien lee el registro sacando la SD | No se impide: el registro va en texto plano hasta que exista el cifrado |

Hechos del código que condicionan el diseño:
- **No hay almacenamiento interno persistente.** `/int` se redirige a `/ext/.int`, en la SD
  (`storage_processing.c`). Todo lo que guardemos acaba en la SD.
- **Momentum ya puede formatear la SD tras 10 PIN fallidos** (`momentum_settings.bad_pins_format`,
  en `desktop_scene_pin_input.c`). Si esa opción está activa, el formateo se lleva el registro por
  delante.
- **Nunca se registran secretos**: ni PIN, ni claves, ni contenido de archivos.

## 3. Eventos de la v1

| Evento | Nivel | Origen | ¿Toca código de upstream? |
|---|---|---|---|
| `system.boot` (versión del firmware y mensaje del último crash, si lo hubo) | info | Arranque del servicio, `furi_hal_rtc_get_fault_data()` | No |
| `desktop.lock` / `desktop.unlock` | info | Pubsub de estado del escritorio (`desktop_api_get_status_pubsub`) | No |
| `pin.fail` (con el número de fallos) / `pin.ok` | warning / info | `desktop_scene_pin_input.c` | Sí: 2 líneas |
| `app.start` / `app.stop` / `app.load_failed` | info | Pubsub del loader | No (ver la nota) |
| `sd.mount` / `sd.unmount` / `sd.error` | info / warning | Pubsub de almacenamiento | No |
| `rpc.open` / `rpc.close` (USB, BLE o UART) | warning | `rpc_session_open` / `rpc_session_close` en `rpc.c` | Sí: 2 líneas |
| `audit.start`, `audit.rotate`, `audit.cleared`, `audit.dropped`, `audit.config` | info / warning | El propio servicio | No |

**Nota sobre el loader.** `LoaderEvent` solo lleva el tipo de evento, no el nombre de la app. Además, los
callbacks del pubsub se ejecutan dentro del hilo del loader, así que llamar desde ahí a
`loader_get_application_name()` lo bloquearía. El callback solo encolará el evento. El hilo de
auditoría preguntará después el nombre con `loader_get_application_launch_path()`. Hay que comprobar en
hardware que el nombre ya está disponible en ese momento. Si no lo está, la alternativa es añadir el
nombre a `LoaderEvent`, lo que cambia la API y obliga a subir su versión.

Para la v2 quedan las sesiones de la CLI por USB, los cambios de ajustes y el estado del Bluetooth.
El Bluetooth tiene un solo callback (`bt_set_status_changed_callback`) y otras partes ya lo usan.

## 4. Formato

Una línea CSV por evento, en UTF-8 y con fin de línea `\n`:

```
seq,time,uptime_ms,level,event,detail,chain
```

- `seq`: contador que crece de uno en uno y no se reinicia al rotar archivos.
- `time`: hora del RTC en formato ISO 8601 (`2026-11-08T01:07:30`). El RTC puede estar mal puesto,
  así que también se guarda `uptime_ms`.
- `detail`: texto libre, de 96 bytes como máximo. Las comas, comillas y saltos de línea se sustituyen
  por `_`, de modo que nunca hay que escapar nada.
- `chain`: los primeros 16 caracteres hexadecimales del
  `SHA-256(chain_anterior || línea_sin_chain)` (mbedTLS ya está en `lib/`). La primera línea de cada
  archivo encadena con el último `chain` del archivo anterior.

La primera línea de cada archivo es una cabecera con la versión del formato:

```
# sentinel-audit v1
seq,time,uptime_ms,level,event,detail,chain
1042,2026-11-08T01:07:30,5321,info,system.boot,fw=mntm-dev-45cdcf0a fault=none,3f9a0c17b2e4d851
1043,2026-11-08T01:07:41,16012,warning,pin.fail,count=1,a17c55e0942bd3f0
```

## 5. Almacenamiento y rotación

- Ruta: `/ext/.sentinel/audit/audit.csv`. Al rotar pasa a `audit.1.csv`, luego a `audit.2.csv`, y así
  sucesivamente.
- Valores por defecto: 64 KB por archivo y 4 archivos (256 KB en total, unas 3.000 líneas).
- Escritura: el servicio agrupa los eventos pendientes y hace abrir, añadir y cerrar en cada lote, para
  no perder datos si se saca la SD.
- **Antes de que se monte la SD** (al arrancar), los eventos esperan en un búfer en RAM de 16 entradas.
  Se vuelcan cuando llega `sd.mount`. Si el búfer se llena, se registra `audit.dropped` con el número
  de eventos perdidos.

## 6. Arquitectura

```
applications/services/audit/
├── application.fam      # SERVICE "audit", record RECORD_AUDIT
├── audit.h              # API interna del firmware
├── audit.c              # hilo, cola, suscripciones a pubsub
├── audit_storage.c      # formato, cadena de hashes, rotación
└── audit_cli.c          # comandos CLI (plugin, como cli_neofetch)
```

- **API**: `audit_log(level, event, fmt, ...)`. Formatea el texto en el hilo de quien llama y lo mete en
  la cola con espera 0. **Nunca bloquea**: si la cola está llena, aumenta el contador de eventos
  perdidos. Así se puede llamar desde el escritorio o el RPC sin riesgo de interbloqueos.
- Para los cambios en código de upstream, un helper `sentinel_audit(...)` comprueba primero si el
  servicio existe (`furi_record_exists`). Con el servicio desactivado, esas líneas no hacen nada.
- **Presupuesto de RAM**: unos 6 KB en total (pila de 2 KB, cola de 32 entradas de unos 100 B y el búfer
  de arranque). Después del problema de memoria con las animaciones, se medirá el heap libre antes y
  después con el log de depuración.

### Cambios en código de upstream (deben ser mínimos)
- `applications/services/desktop/scenes/desktop_scene_pin_input.c`: `pin.ok` y `pin.fail`.
- `applications/services/rpc/rpc.c`: `rpc.open` y `rpc.close`.
- `fbt_options.py` o `application.fam`: incluir el servicio en el firmware.

Nada más. Todo lo demás va en carpetas nuevas.

## 7. Configuración y CLI

Los ajustes se guardan en `/ext/.sentinel/audit/audit.settings` (FlipperFormat), aparte de
`momentum_settings`, para no tocar `lib/momentum`:

| Ajuste | Valor por defecto |
|---|---|
| `enabled` | true |
| `min_level` | info (debug / info / warning / critical) |
| `max_file_kb` | 64 |
| `max_files` | 4 |

Comandos CLI:

```
audit tail [n]        # últimos n eventos (20 por defecto)
audit verify          # comprueba la cadena de hashes de todos los archivos
audit export          # vuelca todos los archivos en orden, en CSV
audit clear           # borra el registro; la primera línea del archivo nuevo es audit.cleared
audit level <nivel>   # cambia min_level y registra audit.config
```

## 8. Relación con otras funciones de la Fase 1

- **PIN de coacción**: si el uso del PIN de coacción quedara registrado en texto plano, quien lea la SD
  sabría que se usó. **Decidido:** se registra un `desktop.unlock` normal y ningún evento específico de
  coacción. Cuando exista el cifrado se valorará un evento propio dentro del registro cifrado.
- **Almacenamiento cifrado**: cuando exista, el registro pasará a escribirse cifrado. El formato no
  cambia: solo cambia dónde y cómo se escribe.
- **`bad_pins_format` de Momentum**: si se formatea la SD, el registro se pierde. Hay que dejarlo
  documentado para el usuario.

## 9. Mejoras opcionales (v1.1)

- **Anclaje en RTC**: guardar `seq` y 32 bits del último `chain` en registros de respaldo del RTC que
  estén libres (el STM32WB tiene 20 y el firmware usa unos 9). Así se detectaría que se ha borrado el
  archivo entero o se ha truncado su final. Se pierde si se queda sin batería, y obliga a tocar
  `furi_hal_rtc.h`.
  **Decidido:** se deja para la v1.1. El reparto de los registros libres del RTC se diseñará junto con
  el PIN de coacción y el arranque seguro, que también los necesitarán.

## 10. Pruebas

- **Tests unitarios** (nueva suite `audit` en `applications/debug/unit_tests`): formato y saneado de
  `detail`, cálculo y verificación de la cadena, verificación que detecta una línea alterada, rotación
  en un directorio temporal y desbordamiento de la cola.
- **En hardware**, con el log serie:
  - arranque, bloqueo y desbloqueo, PIN fallido y PIN correcto;
  - abrir y cerrar apps (comprobar que sale el nombre);
  - conectar qFlipper y sacar y volver a meter la SD;
  - medir el heap libre con el servicio activo y sin él.

## 11. Plan de implementación (PR pequeños)

1. Servicio, formato, rotación y `audit tail` / `audit export`, con eventos `system.boot` y
   `audit.*`. Incluye los tests unitarios del formato y la rotación.
2. Eventos por pubsub: escritorio, loader y SD.
3. Cambios mínimos en upstream: PIN y RPC.
4. Cadena de hashes y `audit verify`.
5. Ajustes en archivo y `audit level` / `audit clear`.

## 12. Decisiones (08/11/2026)

| # | Pregunta | Decisión | Motivo |
|---|---|---|---|
| 1 | ¿Cómo se registra el uso del PIN de coacción? | **Como un desbloqueo normal** (`desktop.unlock`) | La coacción solo sirve si no se nota, y el registro está en la SD. Cuando exista el cifrado se valorará un evento propio dentro del registro cifrado |
| 2 | ¿Texto plano en la v1? | **Sí**, dejándolo documentado | Permite probar el registro ya. El formato no cambiará con el cifrado. Lo que revela ya se deduce de los demás archivos de la SD |
| 3 | ¿Anclaje en el RTC? | **En la v1.1** | Solo quedan unos 10 registros libres del RTC. Se repartirán junto con el PIN de coacción y el arranque seguro |
| 4 | ¿Dónde va la interfaz de ajustes? | **App propia "Sentinel Security"**, creada con el PIN de coacción. En la v1, solo CLI | Sin conflictos con upstream. Reunirá los ajustes de toda la Fase 1 |
