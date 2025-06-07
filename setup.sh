#!/bin/bash

sudo apt install i3 xorg chromium pulseaudio alsa-utils pavucontrol lxappearance
#wallpaper manager
sudo apt install nitrogen
# file explorer
sudo apt install thunar
# bluetooth
sudo apt install bluetooth blueman bluez
#screenshot
sudo apt install maim xclip
# Monitor Manager
sudo apt install arandr
# compositor
sudo apt install picom

# Rofi
sudo apt install rofi
º
# Copy wallpaper to ~/Pictures
mkdir -p ~/Pictures
cp wallpaper_samurai.jpg ~/Pictures/wallpaper_samurai.jpg

# Change wallpaper in nitrogen
nitrogen --set-zoom-fill ~/Pictures/wallpaper_samurai.jpg

echo "exec i3" > ~/.xinitrc

#Copy Configs Files #################################################

mkdir -p ~/.config/picom
cp /etc/xdg/picom.conf ~/.config/picom/picom.conf

mkdir -p ~/.config/i3
cp ~/.i3/config ~/.config/i3/config

mkdir -p ~/.config/i3status
cp /etc/i3status.conf ~/.config/i3status/config

mkdir -p ~/.config/rofi
cp config.rasi ~/.config/rofi/config.rasi

cp .Xresources ~/.Xresources

#######################################################

# Run set-bibata-cursor.sh
sudo apt install bibata-cursor-theme
bash set-bibata-cursor.sh

# Apply .XResources 
xrdb -merge ~/.Xresources

