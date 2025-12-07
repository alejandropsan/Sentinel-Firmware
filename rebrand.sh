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
