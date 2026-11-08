# Desarrollo de Sentinel Firmware

## Alcance y arquitectura

Este repositorio contiene firmware para Flipper Zero basado en Momentum. La IA
asiste al desarrollo; no se incorpora un servicio de IA al firmware salvo petición
expresa. Las características anunciadas en ReadMe.md o ROADMAP.md deben comprobarse
en el código antes de tratarlas como implementadas.

- `applications/`: aplicaciones, servicios, interfaz y pruebas del dispositivo.
- `furi/`: primitivas del sistema; `targets/`: plataforma y HAL.
- `lib/`: bibliotecas propias y dependencias; respetar los submódulos.
- `assets/` y `asset_pack_sentinel/`: recursos gráficos y animaciones.
- `scripts/`, `site_scons/`, `SConstruct`: herramientas Python y compilación FBT.

## Cambios

Consultar CODING_STYLE.md y CONTRIBUTING.md y seguir los patrones del módulo.
Usar cuatro espacios, nombres descriptivos y comprobar retornos, ownership y
liberación de recursos. Revisar memoria, sincronización y duración de callbacks
en cambios de C/C++. No reescribir cambios locales ajenos ni regenerar recursos
sin identificar los archivos afectados. Mantener los cambios enfocados.

## Verificación

- Registrar `git status --short` antes de modificar archivos.
- Compilación: `FBT_NO_SYNC=1 ./fbt updater_package` para utilizar los submódulos
  presentes sin actualizarlos implícitamente. Comprobar antes su disponibilidad.
- Validación de CI: `FBT_NO_SYNC=1 ./fbt lint_all`; informar por separado de errores
  del código, recursos y restricciones del entorno.
- No ejecutar formateo global sobre cambios ajenos. Comprobar el diff después
  del formateo de los archivos modificados.
- Las pruebas en `applications/debug/unit_tests` incluyen componentes que necesitan
  firmware y dispositivo. Compilar no equivale a ejecutarlas.
- Para cambios de interfaz, almacenamiento, temporización o HAL, documentar la
  prueba en hardware y si realmente se ejecutó. No flashear como simple comprobación
  de compilación.
- En animaciones, comprobar dimensiones uniformes de fotogramas y formato de
  imagen requerido por las herramientas. Conservar originales fuera de las
  carpetas que el compilador interpreta como animaciones.

## Entrega

Explicar qué cambió, qué pruebas se ejecutaron y qué queda pendiente. No afirmar
que una compilación, prueba en hardware o comparación entre modelos pasó sin
evidencia. No modificar la configuración global de modelos del usuario.
