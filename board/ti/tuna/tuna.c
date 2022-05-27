/*
 * (C) Copyright 2012
 * Alexander Tarasikov <alexander.tarasikov@gmail.com>
 *
 * based on panda board which is
 * (C) Copyright 2010
 * Texas Instruments Incorporated, <www.ti.com>
 * Steve Sakoman  <steve@sakoman.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <mmc.h>
#include <i2c.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/clocks.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>

#include "tuna_mux_data.h"

#include <asm/omap_musb.h>
#include <asm/errno.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/musb.h>

#ifdef CONFIG_USB_EHCI
#include <usb.h>
#include <asm/arch/ehci.h>
#include <asm/ehci-omap.h>
#endif

#ifdef CONFIG_TWL6030_POWER
#include <twl6030.h>
#endif
#include "omap4_debug.h"

#define TUNA_GPIO_USB3333_RESETB 159

DECLARE_GLOBAL_DATA_PTR;

const struct omap_sysinfo sysinfo = {
	"Board: OMAP4 Espresso\n"
};

struct omap4_scrm_regs *const scrm = (struct omap4_scrm_regs *)0x4a30a000;

/******************************************************************************
 * Framebuffer
 *****************************************************************************/
#ifdef CONFIG_VIDEO
#include <video_fb.h>

GraphicDevice gdev;

#define TUNA_FB 0xbef00000
#define TUNA_XRES 1024
#define TUNA_YRES 600

void *video_hw_init(void) {
	memset((void*)TUNA_FB, 0x00, TUNA_XRES * TUNA_YRES * 4);
	gdev.frameAdrs = TUNA_FB;
	gdev.winSizeX = TUNA_XRES;
	gdev.winSizeY = TUNA_YRES;
	gdev.gdfBytesPP = 4;
	gdev.gdfIndex = GDF_24BIT_888RGB;
	return &gdev;
}
#endif

/**
 * @brief board_init
 *
 * @return 0
 */

int board_init(void)
{
	//since we can boot either from uboot, Samsung SBL or natively,
	//we want to always reinit mux regardless of what
	//the code in arch/arm/cpu/armv7/omap-common is thinking
	set_muxconf_regs_essential();
	set_muxconf_regs_non_essential();
	gpmc_init();

	gd->bd->bi_arch_number = MACH_TYPE_TUNA;
	gd->bd->bi_boot_params = (0x80000000 + 0x100); /* boot param addr */
	gd->ram_size = 0x40000000;
	
	gd->bd->bi_dram[0].start = 0x80000000;
	gd->bd->bi_dram[0].size =  0x40000000;

	//get it before board-specific hardware initialization routines are called
	//gnex_get_revision();
	
	//indicate we're alive

	return 0;
}

/**
 * @brief misc_init_r - set up tuna misc hardware (currently ULPI PHY clock)
 * @return 0
 */
int misc_init_r(void)
{
	//musb_register(&musb_plat, &musb_board_data, (void *)MUSB_BASE);
	return 0;
}

void set_muxconf_regs_essential(void)
{
	do_set_mux(CONTROL_PADCONF_CORE, core_padconf_array_essential,
		   sizeof(core_padconf_array_essential) /
		   sizeof(struct pad_conf_entry));

	do_set_mux(CONTROL_PADCONF_WKUP, wkup_padconf_array_essential,
		   sizeof(wkup_padconf_array_essential) /
		   sizeof(struct pad_conf_entry));

	if (omap_revision() >= OMAP4460_ES1_0)
		do_set_mux(CONTROL_PADCONF_WKUP,
				 wkup_padconf_array_essential_4460,
				 sizeof(wkup_padconf_array_essential_4460) /
				 sizeof(struct pad_conf_entry));
}

void set_muxconf_regs_non_essential(void)
{
	do_set_mux(CONTROL_PADCONF_CORE, core_padconf_array_non_essential,
		   sizeof(core_padconf_array_non_essential) /
		   sizeof(struct pad_conf_entry));
	
	if (omap_revision() < OMAP4460_ES1_0)
		do_set_mux(CONTROL_PADCONF_CORE,
				core_padconf_array_non_essential_4430,
				sizeof(core_padconf_array_non_essential_4430) /
				sizeof(struct pad_conf_entry));
	else
		do_set_mux(CONTROL_PADCONF_CORE,
				core_padconf_array_non_essential_4460,
				sizeof(core_padconf_array_non_essential_4460) /
				sizeof(struct pad_conf_entry));

	do_set_mux(CONTROL_PADCONF_WKUP, wkup_padconf_array_non_essential,
		   sizeof(wkup_padconf_array_non_essential) /
		   sizeof(struct pad_conf_entry));

	if (omap_revision() < OMAP4460_ES1_0)
		do_set_mux(CONTROL_PADCONF_WKUP,
				wkup_padconf_array_non_essential_4430,
				sizeof(wkup_padconf_array_non_essential_4430) /
				sizeof(struct pad_conf_entry));
}


//#if !defined(CONFIG_SPL_BUILD) && defined(CONFIG_GENERIC_MMC)
//int board_mmc_init(bd_t *bis)
//{
//	//unlock TWL6030 PMIC for writing
//	gpio_direction_output(6, 1);
//	gpio_set_value(6, 1);
//
//	//enable EMMC power
//	gpio_direction_output(158, 1);
//	gpio_set_value(158, 1);
//
//	i2c_set_bus_num(0);
//	omap_mmc_init(0, 0, 0);
//	return 0;
//}
//#endif

//void board_usb_init(void) {
//}

/*
 * get_board_rev() - get board revision
 */
u32 get_board_rev(void)
{
	//gnex_get_revision();
	return 0;
}
