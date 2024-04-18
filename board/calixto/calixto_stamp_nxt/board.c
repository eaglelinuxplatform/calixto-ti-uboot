/*
 * board.c
 *
 * Board functions for Calixto AM335X STAMP-NXT SOM
 *
 * Copyright (C) 2015, Calixto Systems Pvt Ltd - http://www.calixto.co.in/
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <spl.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/omap.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mem.h>
#include <asm/io.h>
#include <asm/emif.h>
#include <asm/gpio.h>
#include <i2c.h>
#include <miiphy.h>
#include <cpsw.h>
#include "board.h"

DECLARE_GLOBAL_DATA_PTR;

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

#if defined(CONFIG_SPL_BUILD)

static const struct ddr_data ddr3_calixto_stamp_nxt_data = {
        .datardsratio0 = CALIXTO_DDR3_RD_DQS,
        .datawdsratio0 = CALIXTO_DDR3_WR_DQS,
        .datafwsratio0 = CALIXTO_DDR3_PHY_FIFO_WE,
        .datawrsratio0 = CALIXTO_DDR3_PHY_WR_DATA,
};

static const struct cmd_control ddr3_calixto_stamp_nxt_cmd_ctrl_data = {
        .cmd0csratio = CALIXTO_DDR3_RATIO,
        .cmd0iclkout = CALIXTO_DDR3_INVERT_CLKOUT,

        .cmd1csratio = CALIXTO_DDR3_RATIO,
        .cmd1iclkout = CALIXTO_DDR3_INVERT_CLKOUT,

        .cmd2csratio = CALIXTO_DDR3_RATIO,
        .cmd2iclkout = CALIXTO_DDR3_INVERT_CLKOUT,
};

static struct emif_regs ddr3_calixto1024_emif_reg_data = {
        .sdram_config = CALIXTO1024_DDR3_EMIF_SDCFG,
        .ref_ctrl = CALIXTO1024_DDR3_EMIF_SDREF,
        .sdram_tim1 = CALIXTO1024_DDR3_EMIF_TIM1,
        .sdram_tim2 = CALIXTO1024_DDR3_EMIF_TIM2,
        .sdram_tim3 = CALIXTO1024_DDR3_EMIF_TIM3,
        .zq_config = CALIXTO1024_DDR3_ZQ_CFG,
        .emif_ddr_phy_ctlr_1 = CALIXTO1024_DDR3_EMIF_READ_LATENCY,
};

static struct emif_regs ddr3_calixto_stamp_nxt_512_emif_reg_data = {
        .sdram_config = CALIXTO512_DDR3_EMIF_SDCFG,
        .ref_ctrl = CALIXTO512_DDR3_EMIF_SDREF,
        .sdram_tim1 = CALIXTO512_DDR3_EMIF_TIM1,
        .sdram_tim2 = CALIXTO512_DDR3_EMIF_TIM2,
        .sdram_tim3 = CALIXTO512_DDR3_EMIF_TIM3,
        .zq_config = CALIXTO512_DDR3_ZQ_CFG,
        .emif_ddr_phy_ctlr_1 = CALIXTO512_DDR3_EMIF_READ_LATENCY,
};

static struct emif_regs ddr3_calixto_stamp_nxt_256_emif_reg_data = {
        .sdram_config = CALIXTO256_DDR3_EMIF_SDCFG,
        .ref_ctrl = CALIXTO256_DDR3_EMIF_SDREF,
        .sdram_tim1 = CALIXTO256_DDR3_EMIF_TIM1,
        .sdram_tim2 = CALIXTO256_DDR3_EMIF_TIM2,
        .sdram_tim3 = CALIXTO256_DDR3_EMIF_TIM3,
        .zq_config = CALIXTO256_DDR3_ZQ_CFG,
        .emif_ddr_phy_ctlr_1 = CALIXTO256_DDR3_EMIF_READ_LATENCY,
};

#ifdef CONFIG_SPL_OS_BOOT
int spl_start_uboot(void)
{
	/* break into full u-boot on 'c' */
	return (serial_tstc() && serial_getc() == 'c');
}
#endif

#define OSC	(V_OSCK/1000000)
const struct dpll_params dpll_ddr_calixto_stamp_nxt = {
		303, OSC-1, 1, -1, -1, -1, -1};

void am33xx_spl_board_init(void)
{
	
	/* Get the frequency */
	dpll_mpu_opp100.m = am335x_get_efuse_mpu_max_freq(cdev);
	

	/* Setup Max Frequency */
        dpll_mpu_opp100.m = MPUPLL_M_720;

        /* Set CORE Frequencies to OPP100 */
        do_setup_dpll(&dpll_core_regs, &dpll_core_opp100);

        /* Set MPU Frequency to what we detected now that voltages are set */
        do_setup_dpll(&dpll_mpu_regs, &dpll_mpu_opp100);
}

const struct dpll_params *get_dpll_ddr_params(void)
{
	return &dpll_ddr_calixto_stamp_nxt;
}

void set_uart_mux_conf(void)
{
#ifdef CONFIG_SERIAL1
	enable_uart0_pin_mux();
#endif /* CONFIG_SERIAL1 */
}

void set_mux_conf_regs(void)
{
	enable_board_pin_mux();
}

void sdram_init(void)
{
#if defined(CONFIG_512DDR3)
	config_ddr(303, CALIXTO_DDR3_IOCTRL_VALUE, &ddr3_calixto_stamp_nxt_data,
		   	&ddr3_calixto_stamp_nxt_cmd_ctrl_data, &ddr3_calixto_stamp_nxt_512_emif_reg_data, 0);
#elif defined(CONFIG_256DDR3)
	config_ddr(303, CALIXTO_DDR3_IOCTRL_VALUE, &ddr3_calixto_stamp_nxt_data,
			&ddr3_calixto_stamp_nxt_cmd_ctrl_data, &ddr3_calixto_stamp_nxt_256_emif_reg_data, 0);
#elif defined(CONFIG_1024DDR3)
	config_ddr(303, CALIXTO_DDR3_IOCTRL_VALUE, &ddr3_calixto_stamp_nxt_data,
			&ddr3_calixto_stamp_nxt_cmd_ctrl_data, &ddr3_calixto1024_emif_reg_data, 0);

#endif
}
#endif

/*
 * Basic board specific setup.  Pinmux has been handled already.
 */
int board_init(void)
{
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
#ifdef CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
	setenv("board_name", "AM335-CALIXTO-STAMP-NXT");
	setenv("board_rev", "1.0");
#endif


#ifdef CONFIG_FLASHER
	setenv("boot_targets", "flasher");
#endif

#ifdef CONFIG_UARTLOAD
	setenv("boot_targets", "uart");
#endif
	return 0;
}
#endif

#if (defined(CONFIG_DRIVER_TI_CPSW) && !defined(CONFIG_SPL_BUILD)) || \
	(defined(CONFIG_SPL_ETH_SUPPORT) && defined(CONFIG_SPL_BUILD))
static void cpsw_control(int enabled)
{
	/* VTP can be added here */

	return;
}

static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,
		.phy_addr	= 0,
	},
	{
		.slave_reg_ofs	= 0x308,
		.sliver_reg_ofs	= 0xdc0,
		.phy_addr	= 3,
	},
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 1,
	.slave_data		= cpsw_slaves,
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.bd_ram_ofs		= 0x2000,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
};
#endif

#if defined(CONFIG_DRIVER_TI_CPSW) || \
	(defined(CONFIG_USB_ETHER) && defined(CONFIG_MUSB_GADGET))
int board_eth_init(bd_t *bis)
{
	int rv, n = 0;
	uint8_t mac_addr[6];
	uint32_t mac_hi, mac_lo;

	/* try reading mac address from efuse */
	mac_lo = readl(&cdev->macid0l);
	mac_hi = readl(&cdev->macid0h);
	mac_addr[0] = mac_hi & 0xFF;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
	mac_addr[4] = mac_lo & 0xFF;
	mac_addr[5] = (mac_lo & 0xFF00) >> 8;

#if (defined(CONFIG_DRIVER_TI_CPSW) && !defined(CONFIG_SPL_BUILD)) || \
	(defined(CONFIG_SPL_ETH_SUPPORT) && defined(CONFIG_SPL_BUILD))
	if (!getenv("ethaddr")) {
		printf("<ethaddr> not set. Validating first E-fuse MAC\n");

		if (is_valid_ether_addr(mac_addr))
			eth_setenv_enetaddr("ethaddr", mac_addr);
	}

#ifdef CONFIG_DRIVER_TI_CPSW

	mac_lo = readl(&cdev->macid1l);
	mac_hi = readl(&cdev->macid1h);
	mac_addr[0] = mac_hi & 0xFF;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
	mac_addr[4] = mac_lo & 0xFF;
	mac_addr[5] = (mac_lo & 0xFF00) >> 8;

	if (!getenv("eth1addr")) {
		if (is_valid_ether_addr(mac_addr))
			eth_setenv_enetaddr("eth1addr", mac_addr);
	}
	
	writel(CALIXTO_SOM_ETHERNET_EN, &cdev->miisel);
	cpsw_slaves[0].phy_if =	PHY_INTERFACE_MODE_RMII; 
	cpsw_slaves[1].phy_if =	PHY_INTERFACE_MODE_MII;

	rv = cpsw_register(&cpsw_data);
	if (rv < 0)
		printf("Error %d registering CPSW switch\n", rv);
	else
		n += rv;
#endif
#endif
#if defined(CONFIG_USB_ETHER) && \
	(!defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_USBETH_SUPPORT))
	if (is_valid_ether_addr(mac_addr))
		eth_setenv_enetaddr("usbnet_devaddr", mac_addr);

	rv = usb_eth_initialize(bis);
	if (rv < 0)
		printf("Error %d registering USB_ETHER\n", rv);
	else
		n += rv;
#endif
	return n;
}
#endif
