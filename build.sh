#!/bin/bash

if [ "$TARGET_PREFIX" == "" ] ; then

  echo "Yocto SDK environment not set up"
  echo "source /opt/poky/2.4.3/environment-setup-aarch64-poky-linux"
  exit
fi

cd outdoor
make $1 $2 $3
cd ..

cd basephone
if [ ! -e Makefile ] && [ "$1" == "" ] ; then
	qmake -makefile -o Makefile basephone.pro
fi
make $1 $2 $3 
cd ..

