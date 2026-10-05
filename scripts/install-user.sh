#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 || ! -f $1 ]]; then
    printf 'Usage: %s path/to/soundcurrent-studio_*.deb\n' "$0" >&2
    exit 2
fi
package=$1
if [[ $(dpkg-deb --field "$package" Package) != soundcurrent-studio ]]; then
    printf 'This is not a SoundCurrent Studio package.\n' >&2
    exit 2
fi

for tool in pipewire pactl pw-cli pw-dump; do
    if ! command -v "$tool" >/dev/null; then
        printf 'Missing runtime dependency: %s. Use sudo apt install ./package.deb to install dependencies.\n' "$tool" >&2
        exit 1
    fi
done

data_dir=${XDG_DATA_HOME:-$HOME/.local/share}
install_dir="$data_dir/soundcurrent-studio"
bin_dir="$HOME/.local/bin"
mkdir -p "$install_dir" "$bin_dir" "$data_dir/applications" "$data_dir/icons/hicolor/scalable/apps"
dpkg-deb --extract "$package" "$install_dir"
if ldd "$install_dir/usr/bin/soundcurrent-studio" | grep -q 'not found'; then
    printf 'A Qt runtime library is missing. Use sudo apt install ./package.deb to install dependencies.\n' >&2
    exit 1
fi

cat > "$bin_dir/soundcurrent-studio" <<EOF
#!/usr/bin/env bash
exec "$install_dir/usr/bin/soundcurrent-studio" "\$@"
EOF
chmod 755 "$bin_dir/soundcurrent-studio"
cp "$install_dir/usr/share/icons/hicolor/scalable/apps/io.github.rhamenator.SoundCurrentStudio.svg" \
   "$data_dir/icons/hicolor/scalable/apps/"
cat > "$data_dir/applications/io.github.rhamenator.SoundCurrentStudio.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=SoundCurrent Studio
Comment=Adjustable system-wide equalizer for PipeWire
Exec="$bin_dir/soundcurrent-studio"
Icon=io.github.rhamenator.SoundCurrentStudio
StartupWMClass=soundcurrent-studio
Categories=Audio;AudioVideo;
Terminal=false
EOF
printf 'Installed SoundCurrent Studio for %s. Launch it from the app menu.\n' "$USER"
