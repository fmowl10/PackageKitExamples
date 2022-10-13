#!/usr/bin/env sh

fullFileName=$1
filename="${fullFileName%.*}"

gcc -g `pkg-config --cflags glib-2.0 packagekit-glib2 gio-2.0`  $fullFileName -o $filename \
`pkg-config --libs glib-2.0 packagekit-glib2 gio-2.0`
