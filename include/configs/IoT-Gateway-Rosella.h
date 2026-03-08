/*
 * calixto nxt versa 512 test
 */

#ifndef __CONFIG_IOT_GATEWAY_ROSELLA_H
#define __CONFIG_IOT_GATEWAY_ROSELLA_H

#include <configs/ti_am335x_common.h>
#include <linux/sizes.h>

/* Clock Defines */
#define V_OSCK				24000000  /* Clock output from T2 */
#define V_SCLK				(V_OSCK)


//#ifndef CONFIG_XPL_BUILD
//#include <env/ti/dfu.h>

#define CFG_EXTRA_ENV_SETTINGS \
	"loadaddr=0x80200000\0" \
	"fdtaddr=0x80F80000\0" \
	"fdt_high=0xa0000000\0" \
	"boot_fdt=try\0" \
	"rdaddr=0x81000000\0" \
	"bootpart=0:2\0" \
	"bootdir=/boot\0" \
	"bootfile=zImage\0" \
	"fdtfile=iot-gateway-rosella.dtb\0" \
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
	"rootpath=/export/rootfs\0" \
	"nfsopts=nolock\0" \
	"netargs=setenv bootargs console=${console} " \
		"${optargs} " \
		"root=/dev/nfs " \
		"nfsroot=${serverip}:${rootpath},${nfsopts} rw " \
		"ip=dhcp\0" \
	"bootenv=uEnv.txt\0" \
	"loadbootenv=load mmc ${mmcdev} ${loadaddr} ${bootenv}\0" \
	"importbootenv=echo Importing environment from mmc ...; " \
		"env import -t $loadaddr $filesize\0" \
	"reflash_spi=echo Reflashing SPI Flash with New Boot Loader...; " \
			"mmc rescan; " \
			"sf probe 0; " \
			"echo Erasing SPI Boot Sector...; " \
			"sf erase 0x0 0xC0000; " \
			"fatload mmc 0 ${loadaddr} MLO.byteswap; " \
			"sf write ${loadaddr} 0 ${filesize}; " \
			"fatload mmc 0 ${loadaddr} u-boot.img; " \
			"sf write ${loadaddr} 0x20000 ${filesize}\0" \
	"spierase_env=echo removing enviroment settings from SPI flash..; " \
                        "sf probe 0; " \
                        "sf erase 0xA0000 0xC0000\0" \
	"enable_emmc_rstn=mmc dev 1;mmc rstn 1;\0" \
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
        "emmcbootos=" \
                "if run loadimage; then " \
                        "run mmcloados; " \
                "else " \
                        "echo Running SD Boot using uEnv.txt; " \
                        "run sdbootenv; " \
                "fi;\0" \
	"ramargs=setenv bootargs console=${console} ip=dhcp rw\0" \
	"bootcmd_mmc=echo Booting from SD Card ...; "\
		"setenv mmcdev 0;" \
		"setenv bootpart 0:2;" \
		"setenv mmcroot /dev/mmcblk0p2;" \
        	"run mmcbootos;\0" \
	"bootcmd_emmc=echo Booting from eMMC ...; "\
		"setenv mmcdev 1;" \
		"setenv bootpart 1:2;" \
                "if mmc rescan; then " \
			"setenv mmcroot /dev/mmcblk1p2;" \
                "else " \
			"setenv mmcroot /dev/mmcblk0p2;" \
                "fi;" \
        	"run emmcbootos;\0" \
	"boot_targets=" \
		"mmc " \
		"\0"

//#endif

#define CONFIG_BOOTCOMMAND \
	"for target in ${boot_targets}; do run bootcmd_${target}; done"

/* NS16550 Configuration */
#define CFG_SYS_NS16550_COM1		0x44e09000	/* Base EVM has UART0 */
#define CFG_SYS_NS16550_COM2		0x48022000	/* UART1 */
#define CFG_SYS_NS16550_COM3		0x48024000	/* UART2 */
#define CFG_SYS_NS16550_COM4		0x481a6000	/* UART3 */
#define CFG_SYS_NS16550_COM5		0x481a8000	/* UART4 */
#define CFG_SYS_NS16550_COM6		0x481aa000	/* UART5 */


/* SPI flash. */
#define CONFIG_SPL_SPI_SUPPORT
//#define CONFIG_SPL_SPI_FLASH_SUPPORT
//define CONFIG_SPL_SPI_LOAD
#define CONFIG_SPL_SPI_BUS		0


  // CHECK HERERERERERERE
// name changed due to undeclaration
//#define CONFIG_SPL_SPI_CS		0
#define CONFIG_ENV_SPI_CS 		0

//#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x20000





/*
 * Default to using SPI for environment, etc.
 * 0x000000 - 0x020000 : SPL (128KiB)
 * 0x020000 - 0x0A0000 : U-Boot (512KiB)
 * 0x0A0000 - 0x0AFFFF : First copy of U-Boot Environment (64KiB)
 * 0x0B0000 - 0x0BFFFF : Second copy of U-Boot Environment (64KiB)
 */
//#if (defined(CONFIG_SPI_FLASH) || defined(CONFIG_SPI_BOOT))
//#define CONFIG_ENV_IS_IN_SPI_FLASH
//#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SF_DEFAULT_SPEED
#define CONFIG_ENV_SECT_SIZE		(0x10000) /* 64 KB sectors */




 //  CHECK HERERERERERERE
   
// name changed due to undeclaration
//#define CONFIG_ENV_OFFSET		(0xA0000) /* 768 KiB in */
//#define CONFIG_ENV_ADDR			(0xA0000) /* 768 KiB in */

//#define CONFIG_ENV_OFFSET_REDUND	(0xB0000) /* 896 KiB in */



//#endif


/* SPI flash. */


#if defined(FLASH_TYPE_SST)
	#define CONFIG_SPI_FLASH_SST
#endif
#if defined(FLASH_TYPE_MACRONIX)
	#define CONFIG_SPI_FLASH_MACRONIX
#endif
#if defined(FLASH_TYPE_WINBOND)
	#define CONFIG_SPI_FLASH_WINBOND
#endif




#ifdef CONFIG_MTD_RAW_NAND
/* NAND: device related configs */
/* NAND: driver related configs */
#define CFG_SYS_NAND_ECCPOS		{ 2, 3, 4, 5, 6, 7, 8, 9, \
					 10, 11, 12, 13, 14, 15, 16, 17, \
					 18, 19, 20, 21, 22, 23, 24, 25, \
					 26, 27, 28, 29, 30, 31, 32, 33, \
					 34, 35, 36, 37, 38, 39, 40, 41, \
					 42, 43, 44, 45, 46, 47, 48, 49, \
					 50, 51, 52, 53, 54, 55, 56, 57, }

#define CFG_SYS_NAND_ECCSIZE		512
#define CFG_SYS_NAND_ECCBYTES	14
#endif /* !CONFIG_MTD_RAW_NAND */

/*
 * NOR Size = 16 MiB
 * Number of Sectors/Blocks = 128
 * Sector Size = 128 KiB
 * Word length = 16 bits
 * Default layout:
 * 0x000000 - 0x07FFFF : U-Boot (512 KiB)
 * 0x080000 - 0x09FFFF : First copy of U-Boot Environment (128 KiB)
 * 0x0A0000 - 0x0BFFFF : Second copy of U-Boot Environment (128 KiB)
 * 0x0C0000 - 0x4BFFFF : Linux Kernel (4 MiB)
 * 0x4C0000 - 0xFFFFFF : Userland (11 MiB + 256 KiB)
 */
#if defined(CONFIG_NOR)
#define CFG_SYS_FLASH_BASE		(0x08000000)
#define CFG_SYS_FLASH_SIZE		0x01000000
#endif  /* NOR support */

#endif	/* ! __CONFIG_AM335X_EVM_H */
