#!/bin/bash

# Verifica se está em ambiente gráfico
if [ -z "$DISPLAY" ]; then
  echo "Este script deve ser corrido dentro do ambiente gráfico (i3, etc.)"
  exit 1
fi

# Copiar ficheiros de configuração
mkdir -p ~/.config/picom
cp picom.conf ~/.config/picom/picom.conf

mkdir -p ~/.config/i3
cp config ~/.config/i3/config

mkdir -p ~/.config/i3status
cp i3status.conf ~/.config/i3status/config

mkdir -p ~/.config/rofi
cp config.rasi ~/.config/rofi/config.rasi

cp .Xresources ~/.Xresources
echo "exec i3" > ~/.xinitrc

# Wallpaper
mkdir -p ~/Pictures
cp wallpaper_samurai.jpg ~/Pictures/
nitrogen --set-zoom-fill ~/Pictures/wallpaper_samurai.jpg

# Cursor
bash set-bibata-cursor.sh

# Aplicar tema
xrdb -merge ~/.Xresources

echo "Configuração do i3 concluída!"
