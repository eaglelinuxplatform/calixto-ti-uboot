/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com/
 */

#ifndef LPDDR4_K3_REG
#define LPDDR4_K3_REG


#define lpddr4_k3_readreg(k3_ddrss, block, shift, reg, pt) do {		\
		u32 offset = 0U;					\
		u32 result = 0U;					\
		TH_OFFSET_FROM_REG(reg, shift, offset);			\
		result = k3_ddrss->driverdt->readreg(&k3_ddrss->pd, block, offset, pt); \
		if (result > 0U) {					\
			printf("%s: Failed to read %s\n", __func__, xstr(reg));	\
			hang();						\
		}							\
	} while (0)

#define lpddr4_k3_writereg(k3_ddrss, block, shift, reg, value) do {	\
		u32 offset = 0U;					\
		u32 result = 0U;					\
		TH_OFFSET_FROM_REG(reg, shift, offset);			\
		result = k3_ddrss->driverdt->writereg(&k3_ddrss->pd, block, offset, value); \
		if (result > 0U) {					\
			printf("%s: Failed to write %s\n", __func__, xstr(reg)); \
			hang();						\
		}							\
	} while (0)

#define lpddr4_k3_readreg_ctl(ddrss, reg, pt) \
	lpddr4_k3_readreg(ddrss, LPDDR4_CTL_REGS, CTL_SHIFT, reg, pt)

#define lpddr4_k3_readreg_pi(ddrss, reg, pt) \
	lpddr4_k3_readreg(ddrss, LPDDR4_PHY_INDEP_REGS, PI_SHIFT, reg, pt)

#define lpddr4_k3_readreg_phy(ddrss, reg, pt) \
	lpddr4_k3_readreg(ddrss, LPDDR4_PHY_REGS, PHY_SHIFT, reg, pt)

#define lpddr4_k3_writereg_ctl(ddrss, reg, value) \
	lpddr4_k3_writereg(ddrss, LPDDR4_CTL_REGS, CTL_SHIFT, reg, value)

#define lpddr4_k3_writereg_pi(ddrss, reg, value) \
	lpddr4_k3_writereg(ddrss, LPDDR4_PHY_INDEP_REGS, PI_SHIFT, reg, value)

#define lpddr4_k3_writereg_phy(ddrss, reg, value) \
	lpddr4_k3_writereg(ddrss, LPDDR4_PHY_REGS, PHY_SHIFT, reg, value)

#define lpddr4_k3_set_ctl(k3_ddrss, reg, mask) do {	\
	u32 val;					\
	lpddr4_k3_readreg_ctl(ddrss, reg, &val);		\
	val |= mask;					\
	lpddr4_k3_writereg_ctl(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_clr_ctl(k3_ddrss, reg, mask) do {		\
		u32 val;					\
		lpddr4_k3_readreg_ctl(ddrss, reg, &val);		\
		val &= ~(mask);					\
		lpddr4_k3_writereg_ctl(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_set_pi(k3_ddrss, reg, mask) do {		\
		u32 val;					\
		lpddr4_k3_readreg_pi(ddrss, reg, &val);		\
		val |= mask;					\
		lpddr4_k3_writereg_pi(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_clr_pi(k3_ddrss, reg, mask) do {		\
		u32 val;					\
		lpddr4_k3_readreg_pi(ddrss, reg, &val);		\
		val &= ~(mask);					\
		lpddr4_k3_writereg_pi(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_set_phy(ddrss, reg, mask) do {		\
	u32 val;					\
	lpddr4_k3_readreg_phy(ddrss, reg, &val);		\
	val |= mask;					\
	lpddr4_k3_writereg_phy(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_clr_phy(ddrss, reg, mask) do {		\
	u32 val;					\
	lpddr4_k3_readreg_phy(ddrss, reg, &val);		\
	val &= ~(mask);					\
	lpddr4_k3_writereg_phy(ddrss, reg, val);		\
	} while (0)

#endif  /* LPDDR4_K3_REG */
