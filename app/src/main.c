/*
 * Copyright (c) 2021-2022 Antmicro <www.antmicro.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <drivers/fpga.h>
#include <sys/printk.h>
#include "led1.h"
#include "led2.h"

/*
 * This example demonstrates how to use the FPGA controller API.
 * Once the board is programmed, the LED should blink.
 * There are two bitstreams: one blinks slower than the other one.
 * One bitstream replaces the other in a continuous loop to
 * demonstrate dynamic FPGA reprogramming from Zephyr.
 */

const struct device *fpga;

void main(void)
{
	fpga = DEVICE_DT_GET_ANY(xlnx_fpga);

	if (!fpga) {
		printk("unable to find fpga device\n");
	}

	printk("FPGA info: %s\n", fpga_get_info(fpga));

	while (1) {
		fpga_load(fpga, (uint32_t *)led1_bit, (uint32_t)sizeof(led1_bit));
		k_sleep(K_SECONDS(20)); /* LED is blinking */
		fpga_reset(fpga);
		fpga_load(fpga, (uint32_t *)led2_bit, (uint32_t)sizeof(led2_bit));
		k_sleep(K_SECONDS(20)); /* LED is blinking faster*/
		fpga_reset(fpga);
	}
}
