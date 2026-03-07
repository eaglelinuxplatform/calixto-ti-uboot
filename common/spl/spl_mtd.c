// SPDX-License-Identifier: GPL-2.0+
/*
 * FIT image loader using MTD read
 *
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 *	Apurva Nandan <a-nandan@ti.com>
 */
#include <config.h>
#include <image.h>
#include <log.h>
#include <spl.h>
#include <asm/io.h>
#include <mtd.h>

uint32_t __weak spl_mtd_get_uboot_offs(void)
{
	return CONFIG_SYS_MTD_U_BOOT_OFFS;
}

static ulong spl_mtd_read_skip_bad(struct mtd_info *mtd, loff_t offs,
				   size_t size, void *dst)
{
	size_t remaining = size;
	size_t ret_len;
	loff_t current_offs = offs;
	loff_t block_aligned_offs, next_block;
	u_char *buf = (u_char *)dst;
	int err;

	while (remaining > 0) {
		size_t read_size;
		size_t block_remaining;

		block_aligned_offs = current_offs & ~(mtd->erasesize - 1);

		if (mtd_block_isbad(mtd, block_aligned_offs)) {
			debug("SPL: Skipping bad block at 0x%llx, jumping to 0x%llx\n",
			      block_aligned_offs, next_block);

			next_block = block_aligned_offs + mtd->erasesize;
			current_offs = next_block;
			continue;
		}

		block_remaining = mtd->erasesize -
				  (current_offs & (mtd->erasesize - 1));
		read_size = remaining < block_remaining ? remaining : block_remaining;

		err = mtd_read(mtd, current_offs, read_size, &ret_len, buf);
		if (err) {
			printf("SPL: Read error at offset 0x%llx: %d\n",
			       current_offs, err);
			return size - remaining;
		}

		buf += ret_len;
		current_offs += ret_len;
		remaining -= ret_len;

		if (current_offs >= mtd->size) {
			printf("SPL: Reached end of device, read %zu/%zu bytes\n",
			       size - remaining, size);
			break;
		}
	}

	return size - remaining;
}

static ulong spl_mtd_fit_read(struct spl_load_info *load, ulong offs,
			      ulong size, void *dst)
{
	struct mtd_info *mtd = load->priv;
	size_t ret_len;

	ret_len = spl_mtd_read_skip_bad(mtd, offs, size, dst);
	if (ret_len > 0)
		return ret_len;

	return 0;
}

static int spl_mtd_load_image(struct spl_image_info *spl_image,
			      struct spl_boot_device *bootdev)
{
	int err;
	struct legacy_img_hdr *header;
	__maybe_unused struct spl_load_info load;
	struct mtd_info *mtd;
	size_t ret_len;

	mtd_probe_devices();

	switch (bootdev->boot_device) {
	case BOOT_DEVICE_SPINAND:
		mtd = get_mtd_device_nm("spi-nand0");
		if (IS_ERR_OR_NULL(mtd)) {
			printf("MTD device %s not found, ret %ld\n", "spi-nand",
			       PTR_ERR(mtd));
			err = PTR_ERR(mtd);
			goto remove_mtd_device;
		}
		break;
	default:
		puts(PHASE_PROMPT "Unsupported MTD Boot Device!\n");
		err = -EINVAL;
		goto remove_mtd_device;
	}

	header = spl_get_load_buffer(0, sizeof(*header));

	ret_len = spl_mtd_read_skip_bad(mtd, spl_mtd_get_uboot_offs(),
					sizeof(*header), (void *)header);
	if (ret_len != sizeof(*header)) {
		printf("SPL: Failed to read image header\n");
		err = -EIO;
		goto remove_mtd_device;
	}

	if (IS_ENABLED(CONFIG_SPL_LOAD_FIT) &&
	    image_get_magic(header) == FDT_MAGIC) {
		debug("Found FIT\n");
		load.priv = mtd;
		load.bl_len = 1;
		load.read = spl_mtd_fit_read;
		err = spl_load_simple_fit(spl_image, &load,
					  spl_mtd_get_uboot_offs(), header);
	} else if (IS_ENABLED(CONFIG_SPL_LOAD_IMX_CONTAINER)) {
		load.priv = mtd;
		load.bl_len = 1;
		load.read = spl_mtd_fit_read;
		err = spl_load_imx_container(spl_image, &load,
					     spl_mtd_get_uboot_offs());
	} else {
		err = spl_parse_image_header(spl_image, bootdev, header);
		if (err)
			goto remove_mtd_device;

		ret_len = spl_mtd_read_skip_bad(mtd, spl_mtd_get_uboot_offs(),
						spl_image->size,
						(void *)(ulong)spl_image->load_addr);
		if (ret_len != spl_image->size) {
			printf("SPL: Failed to read full image: %zu/%u bytes\n",
			       ret_len, spl_image->size);
			err = -EIO;
		}
	}

remove_mtd_device:
	mtd_remove(mtd);
	return err;
}

SPL_LOAD_IMAGE_METHOD("SPINAND", 0, BOOT_DEVICE_SPINAND, spl_mtd_load_image);
