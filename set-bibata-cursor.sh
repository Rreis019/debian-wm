#!/bin/bash

# Define o tema de cursor
CURSOR_THEME="Bibata-Modern-Classic"
CURSOR_SIZE="24"

# Adiciona as configurações no ~/.Xresources e ~/.Xdefaults
grep -q "Xcursor.theme" ~/.Xresources || echo "Xcursor.theme: $CURSOR_THEME" >> ~/.Xresources
grep -q "Xcursor.size" ~/.Xresources || echo "Xcursor.size: $CURSOR_SIZE" >> ~/.Xresources

grep -q "Xcursor.theme" ~/.Xdefaults 2>/dev/null || echo "Xcursor.theme: $CURSOR_THEME" >> ~/.Xdefaults
grep -q "Xcursor.size" ~/.Xdefaults 2>/dev/null || echo "Xcursor.size: $CURSOR_SIZE" >> ~/.Xdefaults

# Cria ~/.xsession com comando para setar cursor (substitui se já existir)
cat > ~/.xsession <<EOF
#!/bin/sh
xsetroot -cursor_name left_ptr
exec i3
EOF
chmod +x ~/.xsession

# Cria configuração GTK3 para aplicar o cursor no ~/.config/gtk-3.0/settings.ini
mkdir -p ~/.config/gtk-3.0
cat > ~/.config/gtk-3.0/settings.ini <<EOF
[Settings]
gtk-cursor-theme-name=$CURSOR_THEME
gtk-cursor-theme-size=$CURSOR_SIZE
EOF

# Copia o tema Adwaita para ~/.icons (opcional, pode ajustar para Bibata)
if [ ! -d ~/.icons/Adwaita ]; then
    cp -r /usr/share/icons/Adwaita ~/.icons/ 2>/dev/null || echo "Tema Adwaita não encontrado em /usr/share/icons/"
fi

echo "Configurações aplicadas! Reinicie o X para ver efeito."
