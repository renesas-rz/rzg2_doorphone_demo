# RZ/G2 Door Phone Demo

## About Door Phone demo
This demo shows the multi-stream capabilities of the RZ/G2. One board is intended to be a camera on a front door that will encode as H.264. Another board is intended to be inside the house to view the camera stream as well as 3 other remote mp4 streams.

 * This demo requires 2 boards, called "outdoor" and "basephone".
 * Outdoor sends a live video stream from MIPI/USB camera and 3 MP4 video streams.
 * Basephone uses Qt framework to receive 4 streams and displays them on monitor. It includes 1 main screen and 3 sub-screens. These screens can change positions for each other.


## Hardware
 * RZ/G2E EK874, RZ/G2M/G2N/G2H HiHope
 * LCD Monitor and mouse, or a Touchscreen Monitor
 * MIPI camera includes MIPI Mezzanine Adapter and OV5645 camera.
 * Please prepare RZ/G2E revision 2 if you would like to use MIPI camera on this platform.
 * Webcam Logitech C270 HD 720p, Logitech C930E 1080p, Logitech C920 is recommended to use in this demo.
 * MIPI camera and USB camera cannot be used at the same time. (This use case was not tested.)

## RZ/G2 Root File system
Use "core-image-qt" or "core-image-hmi" as the base image since Qt libraries are required.

## MIPI Camera Support
No changes to the default device tree is needed. Simple attached the MIPI camera as documented.

## USB Camera
By default in the BSP, the USB Camera driver is built as a module but not enabled.
Therefore, use this command to enable the driver.
```
$ modprobe uvcvideo
```
This is done automatically by the start.sh script

## Building
You must install a Yocto SDK that contains the Qt Libraries such as "core-image-qt-sdk".
Use the build.sh script provided.
```
$ source /opt/poky/2.4.3/environment-setup-aarch64-poky-linux
$ ./build.sh clean
$ ./build.sh
```

## Sample MP4 movies
Please refer to the Wiki section to download the MP4 video.
https://github.com/renesas-rz/rzg2_doorphone_demo/wiki

## Install
On the target boards, copy the files to create the follow directory structure under the directory /home/root/doorphone_rzg2/
```
home/
└──root/
   └──doorphone_rzg2/
      ├── hd_videos/
      │   ├── h264-hd-30.mp4
      │   ├── sintel_trailer-720p.mp4
      │   └── Sita_Sings_the_Blues_720p.mp4
      ├── vga_videos/
      │   ├── elephants_dream_480p_1500kbps.mp4
      │   ├── RenesasElectronicsVGA_1500kbps.mp4
      │   └── Sita_Sings_the_Blues_480p_1500kbps.mp4
      ├── basephone
      ├── outdoor
      └── start.sh
```

## Run the demo (single board mode)
Execute the following commands:
```
$ cd /home/root/doorphone_rzg2
$ ./start.sh mipi
```

## Run the demo (dual board mode)
Connect the boards directly together directly using an Ethernet cable.
On BOARD 1 (with camera), enter:
```
$ cd /home/root/doorphone_rzg2
$ ./start.sh mipi outdoor
```
On BOARD 2 (with LCD Monitor and mouse), enter:
```
$ ifconfig eth0 192.168.5.180
$ cd /home/root/doorphone_rzg2
$ ./basephone
```
## Stop the demo
```
$ ./start.sh stop
```

## RZ/G2E EK874 Only
### CMA area must be increased
For the RZ/G2E eval board  (EK874), the MIPI camera will not work unless the reserved memory area ("CMA") is
increased to 512MB.
There are 2 ways to do this. Option 2 is easier.

### Option 1. Modify the Device Tree, re-build and re-instal on the SD card.
```
	diff --git a/arch/arm64/boot/dts/renesas/r8a774c0-es10-cat874.dts b/arch/arm64/boot/dts/renesas/r8a774c0-es10-cat874.dts
	index aa6d0a5..4bcce45 100644
	--- a/arch/arm64/boot/dts/renesas/r8a774c0-es10-cat874.dts
	+++ b/arch/arm64/boot/dts/renesas/r8a774c0-es10-cat874.dts
	@@ -68,7 +68,7 @@
	 		linux,cma@58000000 {
	 			compatible = "shared-dma-pool";
	 			reusable;
	-			reg = <0x00000000 0x58000000 0x0 0x10000000>;
	+			reg = <0x00000000 0x58000000 0x0 0x18000000>;
	 			linux,cma-default;
	 		};

	@@ -76,7 +76,7 @@
	 		mmp_reserved: linux,multimedia {
	 			compatible = "shared-dma-pool";
	 			reusable;
	-			reg = <0x00000000 0x68000000 0x0 0x08000000>;
	+			reg = <0x00000000 0x70000000 0x0 0x8000000>;
	 		};
	 	};

	--
```

### Option 2. Use the command below in u-boot to modify the Device Tree in DDR before booting.
Enter the commands in u-boot:
```
	# load kernel and device tree in DDR
	=> fatload mmc 0:1 0x48080000 Image-ek874.bin; fatload mmc 0:1 0x48000000 Image-r8a774c0-es10-ek874.dtb

	# tell the fdt command where the device tree is located in DDR
	=> fdt addr 0x48000000

	# print current value:
	=> fdt print /reserved-memory

	# increase cma from 256M to 384M
	=> fdt set /reserved-memory/linux,cma@58000000 reg <0x00000000 0x58000000 0x0 0x18000000>

	# increase multimedia from 128M to 256M
	=> fdt set /reserved-memory/linux,multimedia reg <0x00000000 0x70000000 0x0 0x8000000>

	# confirm new values:
	=> fdt print /reserved-memory

	# boot
	=> booti 0x48080000 - 0x48000000
```
You can create and save single command to boot with on your RZ/G2E boards:
```
	=> setenv cma512_1 'fatload mmc 0:1 0x48080000 Image-ek874.bin; fatload mmc 0:1 0x48000000 Image-r8a774c0-es10-ek874.dtb'
	=> setenv cma512_2 'fdt addr 0x48000000 ; fdt set /reserved-memory/linux,cma@58000000 reg <0x00000000 0x58000000 0x0 0x18000000>'
	=> setenv cma512_3 'fdt set /reserved-memory/linux,multimedia reg <0x00000000 0x70000000 0x0 0x8000000>'
	=> setenv cma512_4 'booti 0x48080000 - 0x48000000'
	=> setenv cma512_boot 'run cma512_1 cma512_2 cma512_3 cma512_4'
	=> saveenv
```
Then use this command to boot:
```
	=> run cma512_boot
```



