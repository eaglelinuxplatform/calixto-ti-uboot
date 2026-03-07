// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 Texas Instruments Incorporated - https://www.ti.com/
 *	Dave Gerlach <d-gerlach@ti.com>
 */

#include <dm.h>
#include <soc.h>

#include <asm/arch/hardware.h>
#include <asm/io.h>

#define CTRLMMR_WKUP_GP_SW1_REG		4
#define GP_SW1_VARIANT_MOD_OPR			16

struct soc_ti_k3_plat {
	const char *family;
	const char *revision;
};

static const char *get_family_string(u32 idreg)
{
	const char *family;
	u32 jtag_dev_id;
	u32 pkg;
	u32 soc;

	jtag_dev_id = readl(CTRLMMR_WKUP_JTAG_DEVICE_ID);

	soc = (idreg & JTAG_ID_PARTNO_MASK) >> JTAG_ID_PARTNO_SHIFT;

	switch (soc) {
	case JTAG_ID_PARTNO_AM62X:
		family = "AM62X";
		break;
	case JTAG_ID_PARTNO_AM62AX:
		family = "AM62AX";
		break;
	case JTAG_ID_PARTNO_AM62PX:
		family = "AM62PX";
		break;
	case JTAG_ID_PARTNO_AM62LX:
		family = "AM62LX";
		break;
	case JTAG_ID_PARTNO_AM64X:
		family = "AM64X";
		break;
	case JTAG_ID_PARTNO_AM65X:
		family = "AM65X";
		break;
	case JTAG_ID_PARTNO_J7200:
		family = "J7200";
		break;
	case JTAG_ID_PARTNO_J721E:
		family = "J721E";
		break;
	case JTAG_ID_PARTNO_J721S2:
		family = "J721S2";
		break;
	case JTAG_ID_PARTNO_J722S:
		family = "J722S";
		break;
	case JTAG_ID_PARTNO_J784S4:
		{
			/* Keep default family as J784S4 */
			family = "J784S4";

			pkg = (jtag_dev_id & JTAG_DEV_J742S2_PKG_MASK) >> JTAG_DEV_J742S2_PKG_SHIFT;
			if (pkg == JTAG_ID_PKG_J742S2)
				family = "J742S2";

			break;
		}
	default:
		family = "Unknown Silicon";
	};

	return family;
}

static char *j721e_rev_string_map[] = {
	"1.0", "1.1", "2.0",
};

static char *am62lx_rev_string_map[] = {
	"1.0", "1.1",
};

static char *am62p_gpsw_rev_string_map[] = {
	"1.0", "1.1", "1.2",
};

static char *typical_rev_string_map[] = {
	"1.0", "2.0", "3.0",
};

static int
soc_ti_k3_get_gpsw_variant(struct udevice *dev)
{
	u32 gpsw_val = 0;
	void *gpsw_addr;

	gpsw_addr = dev_read_addr_index_ptr(dev, 1);
	if (!gpsw_addr)
		return -EINVAL;

	gpsw_val = readl(gpsw_addr + CTRLMMR_WKUP_GP_SW1_REG);

	return gpsw_val % GP_SW1_VARIANT_MOD_OPR;
}

static const char *get_rev_string(struct udevice *dev, u32 idreg)
{
	u32 gpsw_variant;
	u32 rev;
	u32 soc;

	rev = (idreg & JTAG_ID_VARIANT_MASK) >> JTAG_ID_VARIANT_SHIFT;
	soc = (idreg & JTAG_ID_PARTNO_MASK) >> JTAG_ID_PARTNO_SHIFT;

	switch (soc) {
	case JTAG_ID_PARTNO_J721E:
		if (rev >= ARRAY_SIZE(j721e_rev_string_map))
			goto bail;
		return j721e_rev_string_map[rev];
	case JTAG_ID_PARTNO_AM62LX:
		if (rev >= ARRAY_SIZE(am62lx_rev_string_map))
			goto bail;
		return am62lx_rev_string_map[rev];
	case JTAG_ID_PARTNO_AM62PX:
		gpsw_variant = soc_ti_k3_get_gpsw_variant(dev);
		if ((gpsw_variant >= ARRAY_SIZE(am62p_gpsw_rev_string_map)) || gpsw_variant < 0)
			goto bail;
		return am62p_gpsw_rev_string_map[gpsw_variant];
	default:
		if (rev >= ARRAY_SIZE(typical_rev_string_map))
			goto bail;
		return typical_rev_string_map[rev];
	};

bail:
	return "Unknown Revision";
}

static int soc_ti_k3_get_family(struct udevice *dev, char *buf, int size)
{
	struct soc_ti_k3_plat *plat = dev_get_plat(dev);

	snprintf(buf, size, "%s", plat->family);

	return 0;
}

static int soc_ti_k3_get_revision(struct udevice *dev, char *buf, int size)
{
	struct soc_ti_k3_plat *plat = dev_get_plat(dev);

	snprintf(buf, size, "SR%s", plat->revision);

	return 0;
}

static const struct soc_ops soc_ti_k3_ops = {
	.get_family = soc_ti_k3_get_family,
	.get_revision = soc_ti_k3_get_revision,
};

int soc_ti_k3_probe(struct udevice *dev)
{
	struct soc_ti_k3_plat *plat = dev_get_plat(dev);
	void *idreg_addr;
	u32 idreg;

	idreg_addr = dev_read_addr_index_ptr(dev, 0);
	if (!idreg_addr)
		return -EINVAL;

	idreg = readl(idreg_addr);

	plat->family = get_family_string(idreg);
	plat->revision = get_rev_string(dev, idreg);

	return 0;
}

static const struct udevice_id soc_ti_k3_ids[] = {
	{ .compatible = "ti,am654-chipid" },
	{ }
};

U_BOOT_DRIVER(soc_ti_k3) = {
	.name           = "soc_ti_k3",
	.id             = UCLASS_SOC,
	.ops		= &soc_ti_k3_ops,
	.of_match       = soc_ti_k3_ids,
	.probe          = soc_ti_k3_probe,
	.plat_auto	= sizeof(struct soc_ti_k3_plat),
};
