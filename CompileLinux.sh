#!/bin/bash
set -e

PROJECT_NAME=SyncHell
BUILD_DIR=build
APPDIR=${PROJECT_NAME}.AppDir
BINARY=${BUILD_DIR}/${PROJECT_NAME}
ASSETS_DIR=assets
DESKTOP_FILE=SyncHell.desktop
ICON_FILE=icon.png
LINUXDEPLOY=linuxdeploy-x86_64.AppImage

# ==== Проверка linuxdeploy ====
if [ ! -f "$LINUXDEPLOY" ]; then
    echo "Скачиваем linuxdeploy..."
    wget -O $LINUXDEPLOY "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
    chmod +x $LINUXDEPLOY
fi

# ==== Сборка проекта ====
echo "Сборка проекта..."
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ..

# ==== Создание AppDir ====
echo "Создаём AppDir..."
rm -rf ${APPDIR}
mkdir -p ${APPDIR}/usr/bin
mkdir -p ${APPDIR}/usr/lib
mkdir -p ${APPDIR}/usr/share

# Копируем бинарник
cp ${BINARY} ${APPDIR}/usr/bin/

# Копируем assets
cp -r ${ASSETS_DIR} ${APPDIR}/usr/share/assets

# ==== Создание AppRun ====
echo "Создаём AppRun..."
cat > ${APPDIR}/AppRun << 'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
exec "${HERE}/usr/bin/SyncHell" "$@"
EOF
chmod +x ${APPDIR}/AppRun

# ==== Копирование .desktop и иконки ====
if [ ! -f "$DESKTOP_FILE" ]; then
    echo "Создаём SyncHell.desktop..."
    cat > $DESKTOP_FILE << EOF
[Desktop Entry]
Name=SyncHell
Exec=SyncHell
Icon=icon
Type=Application
Categories=Game;
Comment=SyncHell game offline build
EOF
fi
cp $DESKTOP_FILE ${APPDIR}/

if [ ! -f "$ICON_FILE" ]; then
    echo "Создаём заглушку icon.png..."
    convert -size 256x256 xc:red $ICON_FILE || echo "Установите ImageMagick для создания иконки"
fi
cp $ICON_FILE ${APPDIR}/

# ==== Сборка AppImage ====
echo "Сборка AppImage..."
./${LINUXDEPLOY} --appdir ${APPDIR} --output appimage

echo "Готово! AppImage находится в текущей папке."

