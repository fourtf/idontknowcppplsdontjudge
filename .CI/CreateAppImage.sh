#!/bin/sh

set -e

if [ ! -f ./bin/chatterino ] || [ ! -x ./bin/chatterino ]; then
    echo "ERROR: No chatterino binary file found. This script must be run in the build folder, and chatterino must be built first."
    exit 1
fi

script_path=$(readlink -f "$0")
script_dir=$(dirname "$script_path")
chatterino_dir=$(dirname "$script_dir")

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/qt512/lib/

ldd ./bin/chatterino
make INSTALL_ROOT=appdir -j"$(nproc)" install ; find appdir/
cp "$chatterino_dir"/resources/icon.png ./appdir/chatterino.png

linuxdeployqt_path="linuxdeployqt-6-x86_64.AppImage"
linuxdeployqt_url="https://github.com/probonopd/linuxdeployqt/releases/download/6/linuxdeployqt-6-x86_64.AppImage"

if [ ! -f "$linuxdeployqt_path" ]; then
    wget -nv "$linuxdeployqt_url"
    chmod a+x "$linuxdeployqt_path"
fi
if [ ! -f appimagetool-x86_64.AppImage ]; then
    wget -nv "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
    chmod a+x appimagetool-x86_64.AppImage
fi
./"$linuxdeployqt_path" \
    appdir/usr/share/applications/*.desktop \
    -no-translations \
    -bundle-non-qt-libs \
    -unsupported-allow-new-glibc \
    -qmake=/opt/qt512/bin/qmake

rm -rf appdir/home
rm appdir/AppRun

# shellcheck disable=SC2016
echo '#!/bin/sh
here="$(dirname "$(readlink -f "${0}")")"
export QT_QPA_PLATFORM_PLUGIN_PATH="$here/usr/plugins"
cd "$here/usr"
exec "$here/usr/bin/chatterino" "$@"' > appdir/AppRun
chmod a+x appdir/AppRun

./appimagetool-x86_64.AppImage appdir
