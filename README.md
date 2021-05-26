# Zephyr FPGA loader
Copyright (c) 2021 [Antmicro](https://antmicro.com).

The Zephyr FPGA loader and the serial loader add support for FPGAs in Zephyr.
It adds new API to FPGA drivers, a driver for QuickLogic's EOS S3 SoC with eFPGA and a serial loader subsystem.

## FPGA loader
FPGA loader was created to:
* have control over the FPGA from Zephyr
* enable bitstream loading into the FPGA

Now you can easily check the status of the FPGA, as well as enable, disable or reset it.
A bitstream can be loaded with a single command.
This allows you to program the FPGA at any time without having to reset the entire board.

## Serial loader
Serial loader is a sub-system to the Zephyr shell that allows you to easily load a bitstream into the device memory.
It can also be used to load other things into the device memory as it supports loading any amount of data from the keyboard or by sending it to the appropriate /dev/tty.

## Usage
The FPGA subsystem for Zephyr is in the process of mainlining, for now use our fork of Zephyr.
You can find the fork containing all the relevant changes [here](https://github.com/antmicro/zephyr/tree/quickfeather-fpga-loader).

# Example 1: programming the FPGA from C code
This example demonstrates how to use the FPGA driver API.
Currently, the sample works with the [QuickLogic QuickFeather board](https://github.com/QuickLogic-Corp/quick-feather-dev-board).

## Requirements
* Zephyr RTOS
* [QuickLogic QuickFeather board](https://github.com/QuickLogic-Corp/quick-feather-dev-board)

## Cloning the repository and building
First, replace the `/zephyr` folder with the Zephyr version that supports FPGA loader and serial loader.
```
git clone --single-branch --branch quickfeather-fpga-loader https://github.com/antmicro/zephyr zephyr
```
Then, build the sample for the QuickLogic QuickFeather board:
```bash
west build -b quick_feather samples/drivers/fpga/fpga_loader
```

## Running
See [QuickFeather programming and debugging](https://docs.zephyrproject.org/latest/boards/arm/quick_feather/doc/index.html#programming-and-debugging) for instructions on how to load an image to the board.

## Expected behaviour
Once the board is programmed, the LED should alternately flash red and green.
Note that the blinking is not controlled by software, there are two bistreams, one lights up the red LED, the other one lights up the green LED.
The bitstream are continusly changed in a loop to demonstrate dynamic FPGA reprogramming from Zephyr.

![unnamed](https://user-images.githubusercontent.com/8438531/119640404-dcedda00-be18-11eb-8a16-49004a82d76f.gif)

# Example 2: programming the FPGA using the Zephyr shell
This sample demonstrates how to use the FPGA driver API along with the Serial loader subsystem in Zephyr Shell.
Currently the sample works with [Quicklogic Quickfeather board](https://github.com/QuickLogic-Corp/quick-feather-dev-board).

## Requirements
* Zephyr RTOS with shell subsystem enabled
* [QuickLogic QuickFeather board](https://github.com/QuickLogic-Corp/quick-feather-dev-board)

## Cloning the repository and building
First, replace the `/zephyr` folder with the Zephyr version that supports FPGA loader and serial loader.
```
git clone --single-branch --branch quickfeather-fpga-loader https://github.com/antmicro/zephyr zephyr
```

Then, build the sample for the QuickLogic QuickFeather board:
```bash
west build -b quick_feather samples/drivers/fpga/fpga_loader_shell
```

See [QuickFeather programming and debugging](https://docs.zephyrproject.org/latest/boards/arm/quick_feather/doc/index.html#programming-and-debugging) for instructions on how to load an image to the board.

## Running
After connecting to the UART console you should see the following output:

```bash
*** Booting Zephyr OS build v2.5.0-rc1-125-g25a1c4394db9  ***
Address of bitstream (red): 0xADDR
Address of bitstream (green): 0xADDR
Size of bitstream (red): 75960
Size of bitstream (green): 75960

uart:~$
```
This sample is already prepared with bitstreams.
After executing the sample, you can see at what address it is stored and its size in bytes.

The FPGA loader command can now be used (`fpga load <device> <size in bytes> <address>`):
```bash
uart:~$ fpga load FPGA 75960 0xADDR
FPGA: loading bitstream
```
The LED should start blinking (color depending on the selected bitstream).
To upload the bitstream again you need to reset the FPGA:

```bash
uart:~$ fpga reset FPGA
FPGA: resseting FPGA
```
You can also use your own bitstream. To load a bitstream into the device memory, use the `serial loader` subsystem.
It is important to use the -e option when sending a bistream via `xxd`:
```bash
uart:~$ serial load -e 0x10000
Loading...
Press ctrl+d to stop
```
Now, the serial loader is waiting for data.
You can either type it directly from the console or send it from the host PC (replace `ttyX` with the appropriate one for your UART console):
```bash
xxd -p data > /dev/ttyX
```
(It is important to use plain-style hex dump)
Once the data is transferred, use `ctrl+d` to quit the loader.
It will print the sum of the read bytes and return to the shell:
```bash
sum of bytes read: 75960
uart:~$
```
Now the bitstream can be uploaded again.
```bash
uart:~$ fpga load FPGA 75960 0x10000
FPGA: loading bitstream
```
