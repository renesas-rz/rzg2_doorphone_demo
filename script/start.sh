#!/bin/bash

USAGE="\n\
usage:\n\
   ./start.sh mipi          - start demo using MIPI camera (single board)\n\
   ./start.sh usb           - start demo using USB camera (single board)\n\
   ./start.sh mipi outdoor  - start outdoor unit only using MIPI camera\n\
   ./start.sh usb outdoor   - start outdoor unit only using USB camera\n\
   ./start.sh stop          - stop demo\n\
"

if  [ "$1" == "" ] ; then
	echo -e "$USAGE"
	exit
fi
if [ "$1" != "mipi" ] && [ "$1" != "usb" ] && [ "$1" != "stop" ] ; then
	echo -e $USAGE
	exit
fi

if  [ "$1" == "stop" ] ; then

	# Stop basephone
	if ps | grep -v "grep" | grep "basephone" ; then
		PID=$(ps | grep -v "grep" | grep "basephone" | awk '{print $1}')
		echo kill -s USR1 $PID
		kill $PID
		sleep 1
	else
		echo "basephone not running"
	fi

	# Stop outdoor
	if ps | grep -v "grep" | grep "outdoor" ; then
		PID=$(ps | grep -v "grep" | grep "outdoor" | awk '{print $1}')
		echo kill -s USR1 $PID
		kill $PID
	else
		echo "outdoor not running"
	fi

	exit
fi

if grep "RZ/G2E" /sys/firmware/devicetree/base/model ; then
	IS_RZG2E=1
	VIDEO_DIR=vga_videos
else
	IS_RZG2E=0
	VIDEO_DIR=hd_videos
fi

# Detect MIPI camera
if [ "$1" == "mipi" ] ; then
	if [ "$IS_RZG2E" == "0" ] && [ -e /sys/bus/i2c/drivers/ov5645/2-003c ] ; then
		MIPI=1
	elif [ "$IS_RZG2E" == "1" ] && [ -e /sys/bus/i2c/drivers/ov5645/3-003c ] ; then
		MIPI=1
	else
		echo "ERROR: MIPI Camera not detected"
		exit
	fi
fi

# Detect USB Video Class driver and camera
if [ "$1" == "usb" ] ; then
	if [ ! -e /sys/bus/usb/drivers/uvcvideo ] ; then 
		# USB Video Class driver not installed
		# Try to install the module
		modprobe uvcvideo allocators=1
	else
		rmmod uvcvideo
		modprobe uvcvideo allocators=1
	fi
	# check again
	if [ ! -e /sys/bus/usb/drivers/uvcvideo ] ; then
		echo "ERROR: USB Video Class driver not installed "
		exit
	fi

	VIDEO=$( grep -rl "UVC" /sys/class/video4linux/*/name | awk -F '/' 'NR==1 { print $5 }' )

	if [ "$VIDEO" == "" ]; then
		echo "ERROR: USB Camera not detected"
		exit
	fi

	USB=1
	MIPI=0
fi

# Check to make sure the CMA memory was increased to 512M on RZ/G2E
if [ "$IS_RZG2E" == "1" ] ; then
	if ! cat /proc/meminfo | grep CmaTotal | grep 524288 ; then
		echo "CMA memory not set to 512M"
		exit
	fi
fi

if ! ifconfig | grep 192.168.5.182 ; then
	echo "Setting IP address"
	ifconfig eth0 192.168.5.182
	sleep 1
fi

# MIPI Camera
if [ "$MIPI" == "1" ] ; then
	CAMERA="-m"
	CAMERA_TEXT="MIPI"
fi
# USB Camera
if [ "$USB" == "1" ] ; then

	CAMERA="-u $VIDEO"
	CAMERA_TEXT="USB"
fi

# Run in background
echo "Starting streaming source unit using $CAMERA_TEXT camera"
echo ./outdoor -d $(pwd)/$VIDEO_DIR $CAMERA -p 5001
./outdoor -d $(pwd)/$VIDEO_DIR $CAMERA -p 5001 &
sleep 3

if [ "$2" == "outdoor" ] ; then
	exit
fi

echo "Starting GUI interface"
echo ./basephone
./basephone &

