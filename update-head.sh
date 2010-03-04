#!/bin/sh

(
set -e

if [ ! -d /opt/arch32 ]; then
  SCHROOT=""
else
  SCHROOT="schroot --"
fi

$SCHROOT scons -j2
mkdir -p df_linux/libs
mkdir -p df_linux/data/init
cp -d libs/* df_linux/libs/
cp dwarfort.exe df_linux/
cp data/shader.* df_linux/data/
cp data/init/init.txt df_linux/data/init/
$SCHROOT cp /usr/lib/libstdc++.so.6 df_linux/libs/
tar cjf df-40d19-head.tar.bz2 df_linux
scp df-40d19-head.tar.bz2 brage.info:public_html/
rm df-40d19-head.tar.bz2
)
rm -r df_linux
