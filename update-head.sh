#!/bin/sh

(
set -e
mkdir df_linux
mkdir df_linux/libs
schroot -- scons -j2
cp libs/libgraphics.so df_linux/libs/
schroot cp /usr/lib/libstdc++.so.6 df_linux/libs/
tar cjf df-40d14-head.tar.bz2 df_linux
scp df-40d14-head.tar.bz2 brage.info:public_html/
rm df-40d14-head.tar.bz2
)
rm -r df_linux
