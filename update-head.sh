#!/bin/sh

(
set -e

if [ ! -d /opt/arch32 ]; then
  SCHROOT=""
else
  SCHROOT="schroot --"
fi

mkdir df_linux
mkdir df_linux/libs
$SCHROOT scons -j2
cp -d libs/* df_linux/libs/
$SCHROOT cp /usr/lib/libstdc++.so.6 df_linux/libs/
tar cjf df-40d17-head.tar.bz2 df_linux
scp df-40d17-head.tar.bz2 brage.info:public_html/
rm df-40d17-head.tar.bz2
)
rm -r df_linux
