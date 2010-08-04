#!/bin/bash
for f in `ldd dwarfort.exe | sed -re 's/.* => //' -e 's/\(.*//'`; do
  if [ -f $f ]; then
    cp $f libs/
  fi
done
