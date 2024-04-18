/*
 * calixto_stamp.h
 *
 * Copyright (C) 2015 Calixto Systems Pvt Ltd - http://www.calixto.co.in/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CONFIG_AM335X_STAMP_H
#define __CONFIG_AM335X_STAMP_H

#include <configs/ti_am335x_common.h>

#define FLASH_TYPE_SST
#define FLASH_TYPE_MACRONIX
#define FLASH_TYPE_WINBOND

#define MACH_TYPE_TIAM335EVM		3589	/* Until the next sync */
#define CONFIG_MACH_TYPE		MACH_TYPE_TIAM335EVM

/* Clock Defines */
#define V_OSCK				24000000  /* Clock output from T2 */
#define V_SCLK				(V_OSCK)

/* Custom script for NOR */
#define CONFIG_SYS_LDSCRIPT		"board/calixto/calixto_stamp/u-boot.lds"

/* Always 64 KiB env size */
#define CONFIG_ENV_SIZE			(0x10000)

#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG

#ifndef CONFIG_SPL_BUILD
#undef BOOTCMD_NAND
#undef BOOTCMD_COMMON
#define BOOTCMD_NAND \
	"mtdids=" MTDIDS_DEFAULT "\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"nandargs=setenv bootargs console=${console} " \
		"${optargs} " \
		"root=${nandroot} " \
		"rootfstype=${nandrootfstype}\0" \
	"nandroot=ubi0:calixto-stamp-rootfs rw ubi.mtd=NAND.file-system," \
		__stringify(CONFIG_SYS_NAND_PAGE_SIZE) "\0" \
	"nandrootfstype=ubifs rootwait=1\0" \
	"nandboot=echo Booting from nand ...; " \
		"run nandargs; " \
		"nand read ${fdtaddr} NAND.dtb; " \
		"nand read ${loadaddr} NAND.kernel1; " \
		"bootz ${loadaddr} - ${fdtaddr}\0"  \
	"bootcmd_nand=run nandboot;\0"

#define BOOTCMD_NET \
	"rootpath=/export/rootfs\0" \
	"nfsopts=nolock\0" \
	"netargs=setenv bootargs console=${console} " \
		"${optargs} " \
		"root=/dev/nfs " \
		"nfsroot=${serverip}:${rootpath},${nfsopts} rw " \
		"ip=dhcp\0" \
	"netboot=echo Booting from network ...; " \
		"setenv autoload no; " \
		"dhcp; " \
		"tftp ${loadaddr} ${bootfile}; " \
		"tftp ${fdtaddr} ${fdtfile}; " \
		"run netargs; " \
		"bootz ${loadaddr} - ${fdtaddr}\0" \
	"bootcmd_net=run netboot;\0"

#define BOOTCMD_FLASHER \
	"tftpboot=echo Booting from TFTP over Ethernet ...; " \
			"setenv autoload no; " \
			"dhcp; " \
			"if tftp 80000000 debrick.scr; then "	\
				"source 80000000; "		\
			"fi;\0" \
	"tftpbootusb=echo Booting from TFTP over USB RNDIS ...; " \
			"setenv autoload no; " \
			"setenv ethact usb_ether; "\
			"dhcp; " \
			"if tftp 80000000 debrick.scr; then "	\
				"source 80000000; "		\
			"fi;\0" \
	"bootcmd_flasher=run tftpboot;\0" \
	"bootcmd_rndis=run tftpbootusb;\0"

#define BOOTCMD_COMMON \
	"boot_targets=" \
		"nand " \
		"mmc " \
		"\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"loadaddr=0x82000000\0" \
	"fdtaddr=0x88000000\0" \
	"boot_fdt=try\0" \
	"bootpart=0:2\0" \
	"bootdir=/boot\0" \
	"bootfile=zImage\0" \
	"fdtfile=am335x-calixto-stamp.dtb\0" \
	"moduleconfig=csevm\0" \
        "fdtcustomfile=\0" \
	"console=ttyO0,115200n8\0" \
	"optargs=\0" \
	"mmcdev=0\0" \
	"mmcroot=/dev/mmcblk0p2 ro\0" \
	"mmcrootfstype=ext4 rootwait\0" \
	"mmcargs=setenv bootargs console=${console} " \
		"${optargs} " \
		"root=${mmcroot} " \
		"rootfstype=${mmcrootfstype}\0" \
	"bootenv=uEnv.txt\0" \
	"loadbootenv=load mmc ${mmcdev} ${loadaddr} ${bootenv}\0" \
	"importbootenv=echo Importing environment from mmc ...; " \
		"env import -t $loadaddr $filesize\0" \
	"reflash_spi=echo Reflashing SPI Flash with New Boot Loader...; " \
			"mmc rescan; " \
			"sf probe 0; " \
			"echo Erasing SPI Boot Sector...; " \
			"sf erase 0 0xC0000; " \
			"fatload mmc 0 ${loadaddr} MLO.byteswap; " \
			"sf write ${loadaddr} 0 ${filesize}; " \
			"fatload mmc 0 ${loadaddr} u-boot.img; " \
			"sf write ${loadaddr} 0x20000 ${filesize}\0" \
	"spierase_env=echo removing enviroment settings from SPI flash..; " \
                        "sf probe 0; " \
                        "sf erase 0xA0000 0xC0000\0" \
	"loadimage=load mmc ${bootpart} ${loadaddr} ${bootdir}/${bootfile}\0" \
	"loadfdt=load mmc ${bootpart} ${fdtaddr} ${bootdir}/${fdtfile}\0" \
	"mmcloados=run mmcargs; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if run loadfdt; then " \
				"bootz ${loadaddr} - ${fdtaddr}; " \
			"else " \
				"echo WARN: Cannot load the DT; " \
				"echo Running SD Boot using uEnv.txt; " \
				"run sdbootenv; " \
			"fi; " \
		"fi;\0" \
	"sdbootenv=mmc dev ${mmcdev}; " \
                "if mmc rescan; then " \
                        "echo SD/MMC found on device ${mmcdev};" \
                        "if run loadbootenv; then " \
                                "echo Loaded environment from ${bootenv};" \
                                "run importbootenv;" \
                        "fi;" \
                        "if test -n $uenvcmd; then " \
                                "echo Running uenvcmd ...;" \
                                "run uenvcmd;" \
                        "fi;" \
                "fi;\0" \
        "mmcbootos=mmc dev ${mmcdev}; " \
                "if mmc rescan; then " \
                        "echo SD/MMC found on device ${mmcdev};" \
                        "if run loadimage; then " \
                                "run mmcloados; " \
                        "else " \
                                "echo Running SD Boot using uEnv.txt; " \
                                "run sdbootenv; " \
                        "fi;" \
                "fi;\0" \
	"bootcmd_mmc="\
        	"run mmcbootos;\0" \
	BOOTCMD_NAND \
	BOOTCMD_NET \
	BOOTCMD_FLASHER \
	BOOTCMD_COMMON 
#endif

#define CONFIG_BOOTCOMMAND \
	"for target in ${boot_targets}; do run bootcmd_${target}; done"

/* NS16550 Configuration */
#define CONFIG_SYS_NS16550_COM1		0x44e09000	/* Base EVM has UART0 */
#define CONFIG_SYS_NS16550_COM2		0x48022000	/* UART1 */
#define CONFIG_SYS_NS16550_COM3		0x48024000	/* UART2 */
#define CONFIG_SYS_NS16550_COM4		0x481a6000	/* UART3 */
#define CONFIG_SYS_NS16550_COM5		0x481a8000	/* UART4 */
#define CONFIG_SYS_NS16550_COM6		0x481aa000	/* UART5 */
#define CONFIG_BAUDRATE			115200

/* SPL */
#ifndef CONFIG_NOR_BOOT
#define CONFIG_SPL_POWER_SUPPORT
#define CONFIG_SPL_YMODEM_SUPPORT

/* CPSW support */
#define CONFIG_SPL_ETH_SUPPORT

/* USB gadget RNDIS */
#define CONFIG_SPL_MUSB_NEW_SUPPORT

/* General network SPL, both CPSW and USB gadget RNDIS */
#define CONFIG_SPL_NET_SUPPORT
#define CONFIG_SPL_ENV_SUPPORT
#define CONFIG_SPL_NET_VCI_STRING	"AM335x U-Boot SPL"

/* SPI flash. */
#define CONFIG_SPL_SPI_SUPPORT
#define CONFIG_SPL_SPI_FLASH_SUPPORT
#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SPL_SPI_BUS		0
#define CONFIG_SPL_SPI_CS		0
#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x20000

#define CONFIG_SPL_LDSCRIPT		"$(CPUDIR)/am33xx/u-boot-spl.lds"
#endif

/* Enhance our eMMC support / experience. */
#define CONFIG_CMD_GPT
#define CONFIG_EFI_PARTITION
#define CONFIG_PARTITION_UUIDS
#define CONFIG_CMD_PART

/* NAND support */
#ifdef CONFIG_NAND
/* NAND: device related configs */
#define CONFIG_SYS_NAND_PAGE_SIZE		2048
#define CONFIG_SYS_NAND_OOBSIZE			64
#define CONFIG_SYS_NAND_BLOCK_SIZE		(128*1024)
#define CONFIG_SPL_NAND_DEVICE_WIDTH		8
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_PAGE_COUNT		(CONFIG_SYS_NAND_BLOCK_SIZE / \
						 CONFIG_SYS_NAND_PAGE_SIZE)
/* NAND: driver related configs */
#define CONFIG_NAND_OMAP_GPMC
#define CONFIG_NAND_OMAP_ELM
#define CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_BASE			0x8000000
#define CONFIG_SYS_MAX_NAND_DEVICE		1
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_SYS_NAND_BAD_BLOCK_POS		NAND_LARGE_BADBLOCK_POS
#define CONFIG_SYS_NAND_ECCPOS		      { 2, 3, 4, 5, 6, 7, 8, 9, \
					       10, 11, 12, 13, 14, 15, 16, 17, \
					       18, 19, 20, 21, 22, 23, 24, 25, \
					       26, 27, 28, 29, 30, 31, 32, 33, \
					       34, 35, 36, 37, 38, 39, 40, 41, \
					       42, 43, 44, 45, 46, 47, 48, 49, \
					       50, 51, 52, 53, 54, 55, 56, 57, }
#define CONFIG_SYS_NAND_ECCSIZE			512
#define CONFIG_SYS_NAND_ECCBYTES		14
#define CONFIG_NAND_OMAP_ECCSCHEME		OMAP_ECC_BCH8_CODE_HW

#define MTDIDS_DEFAULT			"nand0=nand.0,"
#define MTDPARTS_DEFAULT		"mtdparts=" \
					"nand.0:" \
					"128k(NAND.dtb)," \
					"8m(NAND.kernel1)," \
					"8m(NAND.kernel2)," \
					"-(NAND.file-system)"

/* NAND: SPL falcon mode related configs */
  #ifdef CONFIG_SPL_OS_BOOT
    #define CONFIG_CMD_SPL_NAND_OFS		0x00080000 /* os parameters */
    #define CONFIG_SYS_NAND_SPL_KERNEL_OFFS	0x00A00000 /* kernel offset */
    #define CONFIG_CMD_SPL_WRITE_SIZE		0x2000
  #endif
#else
#define NANDARGS ""
#endif /* !CONFIG_NAND */

/*
 * USB configuration.  We enable MUSB support, both for host and for
 * gadget.  We set USB0 as peripheral and USB1 as host, based on the
 * board schematic and physical port wired to each.  Then for host we
 * add mass storage support and for gadget we add both RNDIS ethernet
 * and DFU.
 */
#define CONFIG_USB_MUSB_DSPS
#define CONFIG_ARCH_MISC_INIT
#define CONFIG_MUSB_GADGET
#define CONFIG_MUSB_PIO_ONLY
#define CONFIG_MUSB_DISABLE_BULK_COMBINE_SPLIT
#define CONFIG_USB_GADGET
#define CONFIG_USBDOWNLOAD_GADGET
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_GADGET_VBUS_DRAW	2
#define CONFIG_MUSB_HOST
#define CONFIG_AM335X_USB0
#define CONFIG_AM335X_USB0_MODE	MUSB_PERIPHERAL
#define CONFIG_AM335X_USB1
#define CONFIG_AM335X_USB1_MODE MUSB_HOST

#ifdef CONFIG_MUSB_HOST
#define CONFIG_CMD_USB
#define CONFIG_USB_STORAGE
#endif

#ifdef CONFIG_MUSB_GADGET
#define CONFIG_USB_ETHER
#define CONFIG_USB_ETH_RNDIS
#define CONFIG_USBNET_HOST_ADDR	"de:ad:be:af:00:00"

/* USB TI's IDs */
#define CONFIG_G_DNL_VENDOR_NUM 0x0403
#define CONFIG_G_DNL_PRODUCT_NUM 0xBD00
#define CONFIG_G_DNL_MANUFACTURER "Texas Instruments"
#endif /* CONFIG_MUSB_GADGET */

#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_USBETH_SUPPORT)
/* disable host part of MUSB in SPL */
#undef CONFIG_MUSB_HOST
/*
 * Disable CPSW SPL support so we fit within the 101KiB limit.
 */
#undef CONFIG_SPL_ETH_SUPPORT
#endif

/*
 * Default to using SPI for environment, etc.
 * 0x000000 - 0x020000 : SPL (128KiB)
 * 0x020000 - 0x0A0000 : U-Boot (512KiB)
 * 0x0A0000 - 0x0AFFFF : First copy of U-Boot Environment (64KiB)
 * 0x0B0000 - 0x0BFFFF : Second copy of U-Boot Environment (64KiB)
 */
#if (defined(CONFIG_SPI_FLASH) || defined(CONFIG_SPI_BOOT))
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SF_DEFAULT_SPEED
#define CONFIG_ENV_SECT_SIZE		(0x10000) /* 64 KB sectors */
#define CONFIG_ENV_OFFSET		(0xA0000) /* 768 KiB in */
#define CONFIG_ENV_OFFSET_REDUND	(0xB0000) /* 896 KiB in */
#endif

/* SPI flash. */
#define CONFIG_CMD_SF

#if defined(FLASH_TYPE_SST)
	#define CONFIG_SPI_FLASH_SST
#endif

#if defined(FLASH_TYPE_MACRONIX)
	#define CONFIG_SPI_FLASH_MACRONIX
#endif

#if defined(FLASH_TYPE_WINBOND)
	#define CONFIG_SPI_FLASH_WINBOND
#endif

#define CONFIG_SF_DEFAULT_SPEED		24000000

/* Network. */
#define CONFIG_PHY_GIGE
#define CONFIG_PHYLIB
#define CONFIG_PHY_SMSC

#endif	/* ! __CONFIG_AM335X_EVM_H */
