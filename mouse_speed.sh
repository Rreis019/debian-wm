#!/bin/bash

echo "Listando dispositivos de entrada (mouses):"

mapfile -t devices < <(xinput list --name-only | grep -iE 'mouse|pointer')

for i in "${!devices[@]}"; do
  echo "$((i+1))) ${devices[$i]}"
done

read -rp "Número do dispositivo: " choice

if ! [[ "$choice" =~ ^[0-9]+$ ]] || (( choice < 1 || choice > ${#devices[@]} )); then
  echo "Escolha inválida."
  exit 1
fi

device="${devices[$((choice-1))]}"
echo "Dispositivo selecionado: $device"

read -rp "Digite o valor de velocidade/ aceleração (ex: -1.0 a 1.0, onde 0 é padrão): " speed

# Validação simples do valor: -1.0 a 1.0 (libinput aceita essa faixa)
if ! [[ "$speed" =~ ^-?[0-9]+(\.[0-9]+)?$ ]] || (( $(echo "$speed < -1.0" | bc -l) )) || (( $(echo "$speed > 1.0" | bc -l) )); then
  echo "Valor inválido. Deve estar entre -1.0 e 1.0"
  exit 1
fi

# Procura a propriedade libinput Accel Speed
prop_libinput=$(xinput list-props "$device" | grep -i "libinput Accel Speed (" | awk -F'[()]' '{print $2}')

if [ -n "$prop_libinput" ]; then
  echo "Aplicando velocidade $speed usando libinput Accel Speed..."
  xinput set-prop "$device" "$prop_libinput" "$speed"
  echo "Configuração aplicada!"
  exit 0
fi

# Se não existir, tenta Device Accel Constant Deceleration (inverte valor)
prop_decel=$(xinput list-props "$device" | grep -i "Accel Constant Deceleration (" | awk -F'[()]' '{print $2}')

if [ -n "$prop_decel" ]; then
  deceleration=$(awk "BEGIN {print 1/$speed}")
  echo "Aplicando velocidade $speed invertido para deceleração $deceleration usando Device Accel Constant Deceleration..."
  xinput set-prop "$device" "$prop_decel" "$deceleration"
  echo "Configuração aplicada!"
  exit 0
fi

echo "Nenhuma propriedade de aceleração encontrada para este dispositivo."
exit 1
