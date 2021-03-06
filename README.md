# Zephyr FPGA controller

Copyright (c) 2021-2022 [Antmicro](https://antmicro.com).

This README describes new FPGA-related subsystems which allow users to control FPGA chips using the Zephyr real-time operating system.

The main part is the FPGA controller itself.
It is a completely new Zephyr subsystem developed to enable communication between Zephyr and FPGAs.
The subsystem is described in more detail in the following sections.

In addition to that, a serial loader command has been added to Zephyr.
It allows users to load arbitrary data to the memory of a board running Zephyr over the serial interface.
It ties well with the FPGA subsystem - as described below, it can be used to easily transfer bitstream files into the memory, and later load them onto the FPGA using the new controller subsystem.

## The FPGA controller

The FPGA controller has been created to:
* have control over the FPGA from Zephyr
* enable bitstream loading into the FPGA

The work on the controller consisted of several parts:
* a completely new API, which enables the users to check the status of the FPGA chip, power it on or off, and change the contents of the reprogrammable logic
* FPGA drivers, the controller currently supports the EOS S3 chip with its embedded FPGA, and Xilinx US+ FPGAs (using [Antmicro's Mercury XU port](https://github.com/zephyrproject-rtos/zephyr/tree/main/boards/arm/mercury_xu))
* a new Zephyr shell command which allows users to check the status, control and reprogram the FPGA using the Zephyr shell subsystem

## The serial loader

The serial loader is a new Zephyr shell command that allows users to easily load arbitrary data into the device memory.
In this case it is used to load bitstream files, which can then be used in combination with the new ``fpga`` command to reprogram the FPGA chip.

## Usage
The FPGA subsystem for [QuickLogic QuickFeather board](https://github.com/QuickLogic-Corp/quick-feather-dev-board) is already mainlined.
In case of subsystem for Xilinx US+ FPGA (using [Antmicro's Mercury XU port](https://github.com/zephyrproject-rtos/zephyr/tree/main/boards/arm/mercury_xu)), it is still in the process of mainlining, for now use our fork of Zephyr.
You can find the fork containing all the relevant changes [here](https://github.com/antmicro/zephyr/tree/zynqmp-fpga-controller-with-sample).

# Example 1: programming the FPGA from C code (QuickFeather)
This example demonstrates how to use the FPGA driver API.
The described sample works with the [QuickLogic QuickFeather board](https://github.com/QuickLogic-Corp/quick-feather-dev-board).

## Cloning the repository and building

Please refer to Zephyr's official [getting started guide](https://docs.zephyrproject.org/latest/getting_started/index.html) for detailed setup instructions.

Then, build the sample for the `quick_feather` board:

```bash
cd zephyr
west build -b quick_feather samples/drivers/fpga/fpga_loader/quickfeather/
```

## Running
See [QuickFeather programming and debugging](https://docs.zephyrproject.org/latest/boards/arm/quick_feather/doc/index.html#programming-and-debugging) for instructions on how to load an image to the board.

## Expected behavior

Once the board is programmed, the LED should alternately flash red and green.
Note that the blinking is not controlled by software. There are two bistreams: one lights up the red LED, the other one lights up the green LED.
One bitstream replaces the other in a continuous loop to demonstrate dynamic FPGA reprogramming from Zephyr.

![Bitstream reprogramming demonstration quick_feather](https://user-images.githubusercontent.com/8438531/119640404-dcedda00-be18-11eb-8a16-49004a82d76f.gif)

# Example 2: programming the FPGA from C code (Mercury XU)
This example demonstrates how to use the FPGA driver API.
The described sample works with the [Antmicro's Mercury XU port](https://github.com/zephyrproject-rtos/zephyr/tree/main/boards/arm/mercury_xu)



## Initialization

Please refer to Zephyr's official [getting started guide](https://docs.zephyrproject.org/latest/getting_started/index.html) for detailed setup instructions.

The first step is to initialize the workspace folder (``workspace``) where
the app and all Zephyr modules will be cloned. You can do
that by running:
```
west init -m https://github.com/antmicro/zephyr-fpga-controller-examples --mr master workspace
cd workspace
west update
```

Then, build the sample for the `mercury_xu` board:

```bash
cd zephyr-fpga-controller-examples
west build -b mercury_xu app/
```

## Running
See [Zephyr RTOS and Cortex-R5 on Zynq UltraScale+](https://antmicro.com/blog/2019/09/zephyr-cortex-r5-on-ultrascale/) for instructions on how to load an image to the board.

## Expected behavior
Once the board is programmed, the LED should blink.
Note that the blinking is not controlled by software. There are two bitstreams: one blinks slower than the other one.
One bitstream replaces the other in a continuous loop to demonstrate dynamic FPGA reprogramming from Zephyr.

![Bitstream reprogramming demonstration mercury_xu](https://user-images.githubusercontent.com/56843331/164222142-acc51789-bf13-49c6-aeb8-10b6a553107d.gif)

# Example 3: programming the FPGA using the Zephyr shell

This sample demonstrates how to use the FPGA driver API along with the Serial loader subsystem in Zephyr Shell.
Currently, the sample works with the [QuickLogic QuickFeather board](https://github.com/QuickLogic-Corp/quick-feather-dev-board).

## Cloning the repository and building

Follow the instructions from the previous chapter to download the sources.

Then, build the sample for the QuickLogic QuickFeather board with the following command:
```bash
west build -b quick_feather samples/drivers/fpga/fpga_loader_shell
```

See [QuickFeather programming and debugging](https://docs.zephyrproject.org/latest/boards/arm/quick_feather/doc/index.html#programming-and-debugging) for instructions on how to load an image to the board.

## Running

After connecting to the UART console you should see the following output:

```bash
Address of bitstream (red): 0xADDR
Address of bitstream (green): 0xADDR
Size of bitstream (red): 75960
Size of bitstream (green): 75960

uart:~$
```
This sample is already prepared with two bitstreams.
After executing the sample, you can see at what address it is stored and its size in bytes.

The FPGA loader command can now be used (`fpga load <device> <size in bytes> <address>`):
```bash
uart:~$ fpga load FPGA 75960 0xADDR
FPGA: loading bitstream
```
The LED should start blinking (color depending on the selected bitstream).
To upload the bitstream again, you need to reset the FPGA:

```bash
uart:~$ fpga reset FPGA
FPGA: resseting FPGA
```
You can also use your own bitstream.
To load a bitstream into the device memory, use the `serial loader` subsystem.
It is important to use the -e option when sending the bitstream via `xxd`:
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
(It is important to use a plain-style hex dump)
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
