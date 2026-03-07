// SPDX-License-Identifier: GPL-2.0+
/*
 * Board specific initialization for AM62x platforms
 *
 * Copyright (C) 2020-2022 Texas Instruments Incorporated - https://www.ti.com/
 *	Suman Anna <s-anna@ti.com>
 *
 */

#include <efi_loader.h>
#include <env.h>
#include <spl.h>
#include <init.h>
#include <video.h>
#include <splash.h>
#include <cpu_func.h>
#include <k3-ddrss.h>
#include <fdt_support.h>
#include <fdt_simplefb.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <dm/uclass.h>
#include <asm/arch/k3-ddr.h>

#include "../common/board_detect.h"
#include "../common/fdt_ops.h"

#include "../common/k3-ddr.h"

#define board_is_am62x_skevm()  (board_ti_k3_is("AM62-SKEVM") || \
				 board_ti_k3_is("AM62B-SKEVM"))
#define board_is_am62b_p1_skevm() board_ti_k3_is("AM62B-SKEVM-P1")
#define board_is_am62x_lp_skevm()  board_ti_k3_is("AM62-LP-SKEVM")
#define board_is_am62x_sip_skevm()  board_ti_k3_is("AM62SIP-SKEVM")
#define board_is_am62x_play()	board_ti_k3_is("BEAGLEPLAY-A0-")

DECLARE_GLOBAL_DATA_PTR;

#if CONFIG_IS_ENABLED(SPLASH_SCREEN)
static struct splash_location default_splash_locations[] = {
	{
		.name = "sf",
		.storage = SPLASH_STORAGE_SF,
		.flags = SPLASH_STORAGE_RAW,
		.offset = 0x700000,
	},
	{
		.name		= "mmc",
		.storage	= SPLASH_STORAGE_MMC,
		.flags		= SPLASH_STORAGE_FS,
		.devpart	= "1:1",
	},
};

int splash_screen_prepare(void)
{
	return splash_source_load(default_splash_locations,
				ARRAY_SIZE(default_splash_locations));
}
#endif

struct efi_fw_image fw_images[] = {
	{
		.image_type_id = AM62X_SK_TIBOOT3_IMAGE_GUID,
		.fw_name = u"AM62X_SK_TIBOOT3",
		.image_index = 1,
	},
	{
		.image_type_id = AM62X_SK_SPL_IMAGE_GUID,
		.fw_name = u"AM62X_SK_SPL",
		.image_index = 2,
	},
	{
		.image_type_id = AM62X_SK_UBOOT_IMAGE_GUID,
		.fw_name = u"AM62X_SK_UBOOT",
		.image_index = 3,
	}
};

struct efi_capsule_update_info update_info = {
	.dfu_string = "sf 0:0=tiboot3.bin raw 0 80000;"
	"tispl.bin raw 80000 200000;u-boot.img raw 280000 400000",
	.num_images = ARRAY_SIZE(fw_images),
	.images = fw_images,
};

#if IS_ENABLED(CONFIG_SET_DFU_ALT_INFO)
void set_dfu_alt_info(char *interface, char *devstr)
{
	if (IS_ENABLED(CONFIG_EFI_HAVE_CAPSULE_SUPPORT))
		env_set("dfu_alt_info", update_info.dfu_string);
}
#endif

int board_init(void)
{
	return 0;
}

#if CONFIG_IS_ENABLED(TI_I2C_BOARD_DETECT)
int do_board_detect(void)
{
	int ret;

	ret = ti_i2c_eeprom_am6_get_base(CONFIG_EEPROM_BUS_ADDRESS,
					 CONFIG_EEPROM_CHIP_ADDRESS);
	if (ret) {
		printf("EEPROM not available at 0x%02x, trying to read at 0x%02x\n",
		       CONFIG_EEPROM_CHIP_ADDRESS, CONFIG_EEPROM_CHIP_ADDRESS + 1);
		ret = ti_i2c_eeprom_am6_get_base(CONFIG_EEPROM_BUS_ADDRESS,
						 CONFIG_EEPROM_CHIP_ADDRESS + 1);
		if (ret)
			pr_err("Reading on-board EEPROM at 0x%02x failed %d\n",
			       CONFIG_EEPROM_CHIP_ADDRESS + 1, ret);
	}

	return ret;
}

int checkboard(void)
{
	struct ti_am6_eeprom *ep = TI_AM6_EEPROM_DATA;

	if (!do_board_detect())
		printf("Board: %s rev %s\n", ep->name, ep->version);

	return 0;
}

#if CONFIG_IS_ENABLED(BOARD_LATE_INIT)
static void setup_board_eeprom_env(void)
{
	char *name = "am62x_skevm";

	if (do_board_detect())
		goto invalid_eeprom;

	if (board_is_am62x_skevm())
		name = "am62x_skevm";
	else if (board_is_am62b_p1_skevm())
		name = "am62b_p1_skevm";
	else if (board_is_am62x_lp_skevm())
		name = "am62x_lp_skevm";
	else if (board_is_am62x_sip_skevm())
		name = "am62x_sip_skevm";
	else if (board_is_am62x_play())
		name = "am62x_beagleplay";
	else
		printf("Unidentified board claims %s in eeprom header\n",
		       board_ti_get_name());

invalid_eeprom:
	set_board_info_env_am6(name);
}

static void setup_serial(void)
{
	struct ti_am6_eeprom *ep = TI_AM6_EEPROM_DATA;
	unsigned long board_serial;
	char *endp;
	char serial_string[17] = { 0 };

	if (env_get("serial#"))
		return;

	board_serial = simple_strtoul(ep->serial, &endp, 16);
	if (*endp != '\0') {
		pr_err("Error: Can't set serial# to %s\n", ep->serial);
		return;
	}

	snprintf(serial_string, sizeof(serial_string), "%016lx", board_serial);
	env_set("serial#", serial_string);
}
#endif
#endif

#if CONFIG_IS_ENABLED(BOARD_LATE_INIT)
int board_late_init(void)
{
	if (IS_ENABLED(CONFIG_TI_I2C_BOARD_DETECT)) {
		setup_board_eeprom_env();
		setup_serial();
	}

	ti_set_fdt_env(NULL, NULL);
	return 0;
}
#endif

#if defined(CONFIG_XPL_BUILD)
void spl_board_init(void)
{
	u32 val;

	/* We have 32k crystal, so lets enable it */
	val = readl(MCU_CTRL_LFXOSC_CTRL);
	val &= ~(MCU_CTRL_LFXOSC_32K_DISABLE_VAL);
	writel(val, MCU_CTRL_LFXOSC_CTRL);
	/* Add any TRIM needed for the crystal here.. */
	/* Make sure to mux up to take the SoC 32k from the crystal */
	writel(MCU_CTRL_DEVICE_CLKOUT_LFOSC_SELECT_VAL,
	       MCU_CTRL_DEVICE_CLKOUT_32K_CTRL);

	enable_caches();
	if (IS_ENABLED(CONFIG_SPL_SPLASH_SCREEN) && IS_ENABLED(CONFIG_SPL_BMP))
		splash_display();
}

void spl_perform_fixups(struct spl_image_info *spl_image)
{
	if (IS_ENABLED(CONFIG_K3_DDRSS)) {
		if (IS_ENABLED(CONFIG_K3_INLINE_ECC))
			fixup_ddr_driver_for_ecc(spl_image);
	} else {
		fixup_memory_node(spl_image);
	}
}
#endif

#if defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, struct bd_info *bd)
{
	int ret = -1;

	if (IS_ENABLED(CONFIG_FDT_SIMPLEFB))
		ret = fdt_simplefb_enable_and_mem_rsv(blob);

	/* If simplefb is not enabled and video is active, then at least reserve
	 * the framebuffer region to preserve the splash screen while OS is booting
	 */
	if (IS_ENABLED(CONFIG_VIDEO) && IS_ENABLED(CONFIG_OF_LIBFDT)) {
		if (ret && video_is_active())
			return fdt_add_fb_mem_rsv(blob);
	}

	return 0;
}
#endif
