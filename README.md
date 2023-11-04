# RZ/G2 Door Phone Demo

## Table of contents

1. [About Door Phone demo](#about-door-phone-demo)
2. [Hardware](#hardware)
3. [Supported environments](#supported-environments)
4. [Sample MP4 movies](#sample-mp4-movies)
5. [How to compile the demo](#how-to-compile-the-demo)
6. [How to install the demo](#how-to-install-the-demo)
7. [How to run the demo (single board mode)](#how-to-run-the-demo-single-board-mode)
8. [How to run the demo (dual board mode)](#how-to-run-the-demo-dual-board-mode)
9. [RZ/G2E-EK874 only](#rzg2e-ek874-only)

## About Door Phone demo

This demo shows the multi-stream capabilities of the RZ/G2. One board is intended to be a camera on a front door that will encode as H.264. Another board is intended to be inside the house to view the camera stream as well as 3 other remote mp4 streams.

This demo requires 2 boards, called "outdoor" and "basephone".

* Outdoor sends a live video stream from MIPI/USB camera and 3 MP4 video streams.
* Basephone uses Qt framework to receive 4 streams and displays them on monitor. It includes 1 main screen and 3 sub-screens. These screens can change positions for each other.

## Hardware

### Target devices

* [RZ/G2M-HiHope, Revision 4, SoC 3.0](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rz-mpus/rzg2m-hihope-rzg2m-reference-board).
* [RZ/G2N-Hihope, Revision 4](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rz-mpus/rzg2n-hihope-musashi-rzg2n-reference-board).
* [RZ/G2E-EK874, Revision E](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rz-mpus/ek874-rzg2e-reference-board).
* [RZ/G2H, Revision 4](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rz-mpus/rzg2h-hihope-rzg2h-reference-board).

### MIPI camera support

* 96Boards AiStarVision MIPI Adapter v2.4 with OV5645.
* To connect MIPI adapter to RZ/G2E-EK874, please refer to [this link](https://renesas.info/wiki/RZ-G/RZG_camera#Connection_guide_for_Silicon_Linux_RZ/G2E_evaluation_kit_(EK874)).
* To connect MIPI adapter to RZ/G2M, RZ/G2N, and RZ/G2H, please refer to [this link](https://renesas.info/wiki/RZ-G/RZG_camera#Connection_guide_for_HiHope_RZ/G2M_platform_(hihope-rzg2m)).

### USB camera support

* Webcam Logitech C270 HD 720p, Logitech C930E 1080p, or Logitech C920.
* Before running the demo, please reinsert `uvcvideo` kernel module with `allocators` option set to `1`. This is done automatically by the `start.sh` script:

  ```bash
  root@<board>:~# rmmod uvcvideo
  root@<board>:~# modprobe uvcvideo allocators=1
  ```

* **Note:** Other webcams may work. Please use at your own risks.

### Others

* Full-HD LCD monitor with keyboard and mouse, or a touchscreen monitor.
* HDMI cable.
* Ethernet cable.
* USB-A to micro-USB cables.

## Supported environments

* [BSP 3.0.5 (core-image-qt)](https://github.com/renesas-rz/meta-renesas/tree/BSP-3.0.5).
* **Note:** Environments from BSP-3.0.0 to BSP-3.0.4 may not work. Please use at your own risks.

## Sample MP4 movies

* Please refer to the [Wiki section](https://github.com/renesas-rz/rzg2_doorphone_demo/wiki) to download the MP4 videos.

## How to compile the demo

* Generate SDK from `core-image-qt` environment. For instructions, please refer to [Build Instructions for SDK](https://github.com/renesas-rz/meta-renesas/tree/BSP-3.0.5#build-instructions-for-sdk):

  ```bash
  user@ubuntu:~$ bitbake core-image-qt -c populate_sdk
  ```

* Install the SDK. Then, source its environment setup script as below:

  ```bash
  user@ubuntu:~$ source /path/to/sdk/environment-setup-aarch64-poky-linux
  ```

* Use the `build.sh` script to compile the demo:

  ```bash
  user@ubuntu:~$ cd doorphone_rzg2

  user@ubuntu:~/doorphone_rzg2$ ./build.sh clean
  user@ubuntu:~/doorphone_rzg2$ ./build.sh
  ```

## How to install the demo

* Copy `start.sh` script, `basephone`, and `outdoor` binaries to [target device](#target-devices)'s rootfs:

  ```bash
  root@<board>:~# mkdir /home/root/doorphone_rzg2/
  ```

  ```bash
  user@ubuntu:~/doorphone_rzg2$ scp basephone/basephone outdoor/outdoor script/start.sh root@<IP address>:/home/root/doorphone_rzg2/
  ```

* [Copy MP4 videos](#sample-mp4-movies) to `/home/root/doorphone_rzg2/` directory as below:

  ```text
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

## How to run the demo (single board mode)

* For MIPI camera:

  ```bash
  root@<board>:~/doorphone_rzg2# ./start.sh mipi
  ```

* For USB camera:

  ```bash
  root@<board>:~/doorphone_rzg2# ./start.sh usb
  ```

## How to run the demo (dual board mode)

* Connect 2 boards (of the same type) using an Ethernet cable.
* On BOARD 1 (with camera), run commands:
  * For MIPI camera:

    ```bash
    root@<board>:~/doorphone_rzg2# ./start.sh mipi outdoor
    ```

  * For USB camera:

    ```bash
    root@<board>:~/doorphone_rzg2# ./start.sh usb outdoor
    ```

* On BOARD 2 (with LCD monitor and mouse), run commands:

  ```bash
  root@<board>:~/doorphone_rzg2# ifconfig eth0 192.168.5.180
  root@<board>:~/doorphone_rzg2# ./basephone &
  ```

## How to stop the demo

* Option 1 (recommended):

  ```bash
  root@<board>:~/doorphone_rzg2# ./start.sh stop
  ```

* Option 2:

  ```bash
  root@<board>:~/doorphone_rzg2# killall basephone
  root@<board>:~/doorphone_rzg2# killall outdoor
  ```

## RZ/G2E-EK874 only

### Increase global CMA area

* For [single board mode](#how-to-run-the-demo-single-board-mode) to work on [RZ/G2E-EK874](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rz-mpus/ek874-rzg2e-reference-board), global [CMA area](https://renesas.info/wiki/RZ-G/RZ-G2_BSP_MemoryMap#How_to_modify_kernel) must be increased to 384MB.
* There are 2 ways to do this:

#### Option 1: Re-build and re-install the Device Tree

* In Yocto environment, modify `global_cma` and `mmp_reserved` nodes in `…/build/tmp/work-shared/ek874/kernel-source/arch/arm64/boot/dts/renesas/cat874-common.dtsi`:

  ```diff
  diff --git a/arch/arm64/boot/dts/renesas/cat874-common.dtsi b/arch/arm64/boot/dts/  renesas/cat874-common.dtsi
  index 356d914310af..265ab28b9e97 100644
  --- a/arch/arm64/boot/dts/renesas/cat874-common.dtsi
  +++ b/arch/arm64/boot/dts/renesas/cat874-common.dtsi
  @@ -61,7 +61,7 @@ reserved-memory {
   		global_cma: linux,cma@58000000 {
   			compatible = "shared-dma-pool";
   			reusable;
  -			reg = <0x00000000 0x58000000 0x0 0x10000000>;
  +			reg = <0x00000000 0x58000000 0x0 0x18000000>;
   			linux,cma-default;
   		};
   
  @@ -69,7 +69,7 @@ global_cma: linux,cma@58000000 {
   		mmp_reserved: linux,multimedia {
   			compatible = "shared-dma-pool";
   			reusable;
  -			reg = <0x00000000 0x68000000 0x0 0x08000000>;
  +			reg = <0x00000000 0x70000000 0x0 0x08000000>;
   		};
   	};
   
  ```

* Recompile `linux-renesas` recipe:

  ```bash
  user@ubuntu:…/build$ bitbake linux-renesas -fC compile
  ```

* After that, the new Device Tree files can be found at `…/build/tmp/deploy/images/ek874/`:

  ```bash
  user@ubuntu:…/build/tmp/deploy/images/ek874$ ls Image-r8a774c0-*

  Image-r8a774c0-cat874.dtb            Image-r8a774c0-ek874-mipi-2.4.dtb           Image-r8a774c0-es10-cat874.dtb
  Image-r8a774c0-cat874-revc.dtb       Image-r8a774c0-ek874-revc.dtb               Image-r8a774c0-es10-ek874.dtb
  Image-r8a774c0-ek874.dtb             Image-r8a774c0-ek874-revc-idk-2121wr.dtb    Image-r8a774c0-es10-ek874-idk-2121wr.dtb
  Image-r8a774c0-ek874-idk-2121wr.dtb  Image-r8a774c0-ek874-revc-mipi-2.1.dtb      Image-r8a774c0-es10-ek874-mipi-2.1.dtb
  Image-r8a774c0-ek874-mipi-2.1.dtb    Image-r8a774c0-ek874-revc-mipi-2.4.dtb      Image-r8a774c0-es10-ek874-mipi-2.4.dtb
  ```

* In Linux environment, the `CmaTotal` must be `524288`KB:

  ```bash
  root@ek874:~# cat /proc/meminfo | grep CmaTotal
  CmaTotal:         524288 kB
  ```

### Option 2: Modify the Device Tree in DDR before booting (recommended)

* Run the below commands in u-boot:

  ```bash
  # Load kernel and device tree in DDR
  # The files can be different on your machine
  => fatload mmc 0:1 0x48080000 Image-ek874.bin
  => fatload mmc 0:1 0x48000000 Image-r8a774c0-ek874-mipi-2.4.dtb

  # Tell the fdt command where the device tree is located in DDR
  => fdt addr 0x48000000

  # Print /reserved-memory
  => fdt print /reserved-memory

  # Increase global cma area from 256M to 384M
  => fdt set /reserved-memory/linux,cma@58000000 reg <0x00000000 0x58000000 0x00000000 0x18000000>

  # Change start address of CMA area reserved for multimedia
  => fdt set /reserved-memory/linux,multimedia reg <0x00000000 0x70000000 0x00000000 0x08000000>

  # Print /reserved-memory
  => fdt print /reserved-memory

  # Boot to Linux
  => booti 0x48080000 - 0x48000000
  ```

* You can resize global CMA area and boot to Linux with a single command as below:

  ```bash
  # Create cma512_boot command
  # Note: Next time, no need to run these commands
  => setenv cma512_1 'fatload mmc 0:1 0x48080000 Image-ek874.bin; fatload mmc 0:1 0x48000000 Image-r8a774c0-ek874-mipi-2.4.dtb'
  => setenv cma512_2 'fdt addr 0x48000000; fdt set /reserved-memory/linux,cma@58000000 reg <0x00000000 0x58000000 0x00000000 0x18000000>'
  => setenv cma512_3 'fdt set /reserved-memory/linux,multimedia reg <0x00000000 0x70000000 0x00000000 0x08000000>'
  => setenv cma512_4 'booti 0x48080000 - 0x48000000'
  => setenv cma512_boot 'run cma512_1 cma512_2 cma512_3 cma512_4'
  => saveenv

  # Boot to Linux
  => run cma512_boot
  ```
