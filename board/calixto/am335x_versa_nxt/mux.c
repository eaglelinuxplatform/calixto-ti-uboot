/*
 * mux.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <config.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/hardware.h>
#include <asm/arch/mux.h>
#include <asm/io.h>
#include <i2c.h>
#include "board.h"



static struct module_pin_mux uart0_pin_mux[] = {
	{OFFSET(uart0_rxd), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* UART0_RXD */
	{OFFSET(uart0_txd), (MODE(0) | PULLUDEN)},		        /* UART0_TXD */
	{-1},
};
static struct module_pin_mux mmc0_pin_mux[] = {
	{OFFSET(mmc0_dat3), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT3 */
	{OFFSET(mmc0_dat2), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT2 */
	{OFFSET(mmc0_dat1), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT1 */
	{OFFSET(mmc0_dat0), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT0 */
	{OFFSET(mmc0_clk), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CLK */
	{OFFSET(mmc0_cmd), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CMD */
	{OFFSET(mcasp0_aclkx), (MODE(7) | RXACTIVE | PULLUP_EN)},	/* mcasp0_aclkx */
	{-1},
};

static struct module_pin_mux spi0_pin_mux[] = {
	{OFFSET(spi0_sclk), (MODE(0) | RXACTIVE | PULLUDEN)},	/* SPI0_SCLK */
	{OFFSET(spi0_d0), (MODE(0) | RXACTIVE | PULLUDEN | 
	PULLUP_EN)},			/* SPI0_D0 */
	{OFFSET(spi0_d1), (MODE(0) | RXACTIVE | PULLUDEN)},	        /* SPI0_D1 */
	{OFFSET(spi0_cs0), (MODE(0) | RXACTIVE | PULLUDEN | 
	PULLUP_EN)},			/* SPI0_CS0 */
	{-1},
};

#if defined(CONFIG_eMMC)
static struct module_pin_mux emmc_mmc2_pin_mux[] = {
        {OFFSET(gpmc_ad15), (MODE(3) | RXACTIVE | PULLUP_EN)},  /* MMC2_DAT3 */
        {OFFSET(gpmc_ad14), (MODE(3) | RXACTIVE | PULLUP_EN)},  /* MMC2_DAT2 */
        {OFFSET(gpmc_ad13), (MODE(3) | RXACTIVE | PULLUP_EN)},  /* MMC2_DAT1 */
        {OFFSET(gpmc_ad12), (MODE(3) | RXACTIVE | PULLUP_EN)},  /* MMC2_DAT0 */
        {OFFSET(gpmc_clk),  (MODE(3) | RXACTIVE | PULLUP_EN)},  /* MMC2_CLK */
        {OFFSET(gpmc_csn3), (MODE(3) | RXACTIVE | PULLUP_EN)},  /* MMC2_CMD */
        {-1},
};
#endif

static struct module_pin_mux rmii1_pin_mux[] = {
	{OFFSET(mii1_crs), MODE(1) | RXACTIVE},	        /* RMII1_CRS */
	{OFFSET(mii1_rxerr), MODE(1) | RXACTIVE},		/* RMII1_RXERR */
	{OFFSET(mii1_txen), MODE(1)},				/* RMII1_TXEN */
	{OFFSET(mii1_txd1), MODE(1)},				/* RMII1_TXD1 */
	{OFFSET(mii1_txd0), MODE(1)},				/* RMII1_TXD0 */
	{OFFSET(mii1_rxd1), MODE(1) | RXACTIVE},		/* RMII1_RXD1 */
	{OFFSET(mii1_rxd0), MODE(1) | RXACTIVE},		/* RMII1_RXD0 */
	{OFFSET(mdio_data), MODE(0) | RXACTIVE | PULLUP_EN},  /* MDIO_DATA */
	{OFFSET(mdio_clk), MODE(0) | PULLUP_EN},	        /* MDIO_CLK */
	{OFFSET(rmii1_refclk), MODE(0) | RXACTIVE},		/* RMII1_REFCLK */
	{-1},
};

static struct module_pin_mux nand_pin_mux[] = {
        {OFFSET(gpmc_ad0),      (MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD0  */
        {OFFSET(gpmc_ad1),      (MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD1  */
        {OFFSET(gpmc_ad2),      (MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD2  */
        {OFFSET(gpmc_ad3),      (MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD3  */
        {OFFSET(gpmc_ad4),      (MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD4  */
        {OFFSET(gpmc_ad5),      (MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD5  */
        {OFFSET(gpmc_ad6),      (MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD6  */
        {OFFSET(gpmc_ad7),      (MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD7  */
        {OFFSET(gpmc_wait0),    (MODE(0) | PULLUP_EN | RXACTIVE)}, /* nWAIT */
        {OFFSET(gpmc_wpn),      (MODE(7) | PULLUP_EN)},            /* nWP */
        {OFFSET(gpmc_csn0),     (MODE(0) | PULLUP_EN)},            /* nCS */
        {OFFSET(gpmc_wen),      (MODE(0) | PULLDOWN_EN)},          /* WEN */
        {OFFSET(gpmc_oen_ren),  (MODE(0) | PULLDOWN_EN)},          /* OE */
        {OFFSET(gpmc_advn_ale), (MODE(0) | PULLDOWN_EN)},          /* ADV_ALE */
        {OFFSET(gpmc_be0n_cle), (MODE(0) | PULLDOWN_EN)},          /* BE_CLE */
        {-1},
};

void enable_uart0_pin_mux(void)
{
	configure_module_pin_mux(uart0_pin_mux);
}

void enable_board_pin_mux(void)
{
        /* Calixto Module Pinmux */
        configure_module_pin_mux(mmc0_pin_mux);
        configure_module_pin_mux(rmii1_pin_mux);
        configure_module_pin_mux(spi0_pin_mux);
        configure_module_pin_mux(nand_pin_mux);
        #if defined(CONFIG_eMMC)
		configure_module_pin_mux(emmc_mmc2_pin_mux);
	#endif
}


/* CPLD registers */
#define I2C_CPLD_ADDR	0x35
#define CFG_REG		0x10
