#!/bin/bash

if [ "$EUID" -ne 0 ]; then
  echo "Run with sudo : sudo $0"
  exit 1
fi

# Atualiza e instala pacotes base
apt update
apt install -y \
  task-standard i3 xorg xterm pulseaudio alsa-utils pavucontrol \
  thunar lxappearance bluetooth blueman bluez arandr picom \
  rofi chromium nitrogen maim xclip bibata-cursor-theme neofetch \

echo "Base installation complete."
