# Animaciones Sentinel: guía y diagnóstico

Estado verificado en hardware en noviembre de 2026: las 3 animaciones cargan sin errores y el Flipper
arranca sin cuelgues.

## Animaciones actuales

| Animación | Fotogramas | FPS | RAM al cargar | Max butthurt | Nivel |
|---|---|---|---|---|---|
| `Sentinel_Scanning` | 64 (pasivos) | 4 | ~36,5 KB | 6 | 1-30 |
| `Sentinel_Secure` | 64 (pasivos) | 4 | ~23 KB | 18 | 1-30 |
| `Sentinel_Security` | 64 (pasivos) | 5 | ~37 KB | 14 | 1-30 |

Las tres tienen peso 4, duración de 360 s y sin bocadillos (`Bubble slots: 0`). Las versiones completas
de 128 fotogramas están en `asset_pack_sentinel/_originals/anims/*_128f/`, una carpeta excluida de git.

## Límite de memoria (importante)

El escritorio carga en RAM **todos los fotogramas** de la animación activa a la vez. Con el Flipper en
reposo quedan unos 136-142 KB de heap libre. Al arrancar hay bastante menos, porque los servicios
todavía se están iniciando.

- **Mantén cada animación por debajo de ~40 KB** de fotogramas compilados (`.bm`). La mayor animación de
  serie ronda los 49 KB.
- Con 128 fotogramas (~73-75 KB) el Flipper se colgaba al arrancar con `out of memory` o
  `NULL pointer dereference`.

Para medir lo que ocupa cada animación una vez compilada:

```bash
python3 -c "
import glob, os
for d in sorted(glob.glob('build/f7-firmware-C/resources/asset_packs/Sentinel/Anims/*/')):
    sz = [os.path.getsize(f) for f in glob.glob(d + '*.bm')]
    print(f'{d.split(\"/\")[-2]:20} {len(sz):4} fotogramas  ~{sum((s + 15) // 8 * 8 for s in sz) / 1024:.1f} KB')
"
```

## Flujo de trabajo

1. Edita la fuente en `asset_pack_sentinel/Anims/<Nombre>/`:
   - `frame_N.png`: 128×64, 1 bit (modo `1`), numerados desde 0 y sin saltos.
   - `meta.txt`: `Passive frames` + `Active frames` debe coincidir con el número de entradas de
     `Frames order`. Sin espacios al principio de las líneas y con `Frames order` en una sola línea.
   - No dejes otros archivos (GIF, previsualizaciones) dentro de la carpeta de la animación.
2. Añade la animación a `asset_pack_sentinel/Anims/manifest.txt`.
3. Copia el pack a la carpeta que compila el build: `./install_assets.sh`, que sustituye
   `assets/packs/Sentinel/Anims` por completo.
4. Compila **con `FORCE=1`**. Sin esa opción, fbt no recompila los asset packs y el paquete sigue
   llevando los fotogramas anteriores:
   ```bash
   FBT_NO_SYNC=1 ./fbt FORCE=1 updater_package
   ```
5. Comprueba el tamaño con el script de arriba e instala: `./fbt flash_usb_full`.

## Activar el pack en el Flipper

**Momentum → Interface → Graphics → Asset Pack → Sentinel**

Cada animación aparece según el humor (butthurt) y el nivel del delfín, así que no salen todas a la vez.
Van rotando.

## Problemas frecuentes

| Síntoma | Causa probable | Solución |
|---|---|---|
| `out of memory` o `NULL pointer dereference` al arrancar o al volver al escritorio | Animación demasiado grande para la RAM | Reducir fotogramas hasta quedar por debajo de ~40 KB |
| "failed to load animation" | `meta.txt` con formato inválido, o con un número de fotogramas que no coincide con `Frames order` o con los archivos | Revisar `meta.txt` (ver el flujo de trabajo) |
| Una animación no aparece nunca | No está en `manifest.txt`, o el humor y el nivel del delfín no entran en su rango | Revisar `manifest.txt` |
| Los cambios no se ven en el Flipper | El build reutilizó el pack anterior | Compilar con `FORCE=1` |
| Sigue saliendo el delfín de serie | El pack Sentinel no está seleccionado | Seleccionarlo en Momentum → Interface → Graphics |

### Ver el log del Flipper

Con el Flipper conectado por USB, abre `./fbt cli` y ejecuta `log debug`. Al cambiar de animación verás
líneas como estas:

```
[I][AnimationManager] Load animation 'Sentinel_Scanning'
[I][Loader] Application stopped. Free heap: 136016
```

Si hay un cuelgue, el Flipper imprime un bloque `[CRASH]` con el hilo, los registros y el heap libre
antes de reiniciarse.
