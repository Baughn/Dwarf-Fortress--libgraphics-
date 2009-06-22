#!/bin/sh
set -e

mkdir df_linux
mkdir df_linux/libs
scons
cp libs/libgraphics.so df_linux/libs
tar cjf df-40d12-head.tar.bz2 df_linux
scp df-40d12-head.tar.bz2 brage.info:public_html/
rm df-40d12-head.tar.bz2
rm -r df_linux
