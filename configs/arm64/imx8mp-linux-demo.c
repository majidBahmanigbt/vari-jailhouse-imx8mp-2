/*
 * iMX8MM target - linux-demo
 *
 * Copyright 2020 NXP
 *
 * Authors:
 *  Peng Fan <peng.fan@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

/*
+ * Boot 2nd Linux cmdline with console on J18-3 and J18-5:
+ * 1. Use recovery sd card to program emmc (install_yocto.sh)
+ * 2. Boot from emmc, with recovery sd card inserted
+ * 3. export PATH=$PATH:/usr/share/jailhouse/tools/
+ * 4. jailhouse cell linux /usr/share/jailhouse/cells/imx8mp-linux-demo.cell /boot/Image.gz -d /boot/imx8mp-var-som-inmate.dtb -c 'clk_ignore_unused console=ttymxc2,115200 earlycon=ec_imx6q,0x30a60000,115200 root=/dev/mmcblk1p1 rootwait rw'
 */
#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

struct {
	struct jailhouse_cell_desc cell;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[23];
	struct jailhouse_irqchip irqchips[2];
	struct jailhouse_pci_device pci_devices[4];
} __attribute__((packed)) config = {
	.cell = {
		.signature = JAILHOUSE_CELL_DESC_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.name = "linux-inmate-demo",
		.flags = JAILHOUSE_CELL_PASSIVE_COMMREG,

		.cpu_set_size = sizeof(config.cpus),
		.num_memory_regions = ARRAY_SIZE(config.mem_regions),
		.num_irqchips = ARRAY_SIZE(config.irqchips),
		.num_pci_devices = ARRAY_SIZE(config.pci_devices),
		.vpci_irq_base = 154, /* Not include 32 base */
	},

	.cpus = {
		0xc,
	},

	.mem_regions = {
		/* IVSHMEM shared memory region (virtio-blk front) */
		{
			.phys_start = 0x7ba00000,
			.virt_start = 0x7ba00000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0x7ba01000,
			.virt_start = 0x7ba01000,
			.size = 0xdf000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		{ 0 },
		{ 0 },
		/* IVSHMEM shared memory region (virtio-con front) */
		{
			.phys_start = 0x7bae0000,
			.virt_start = 0x7bae0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0x7bae1000,
			.virt_start = 0x7bae1000,
			.size = 0xf000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		{ 0 },
		{ 0 },
		/* IVHSMEM shared memory region for 00:00.0 (demo )*/
		{
			.phys_start = 0x7baf0000,
			.virt_start = 0x7baf0000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0x7baf1000,
			.virt_start = 0x7baf1000,
			.size = 0x9000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0x7bafa000,
			.virt_start = 0x7bafa000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0x7bafc000,
			.virt_start = 0x7bafc000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_ROOTSHARED,
		},
		{
			.phys_start = 0x7bafe000,
			.virt_start = 0x7bafe000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		/* IVSHMEM shared memory regions for 00:01.0 (networking) */
		JAILHOUSE_SHMEM_NET_REGIONS(0x7bb00000, 1),
		/* UART2 earlycon */ {
			.phys_start = 0x30a60000,
			.virt_start = 0x30a60000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* UART3 */ {
			.phys_start = 0x30880000,
			.virt_start = 0x30880000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* SHDC2 */ {
			.phys_start = 0x30b50000,
			.virt_start = 0x30b50000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* RAM: Top at 4GB Space */ {
			.phys_start = 0x7b700000,
			.virt_start = 0,
			.size = 0x10000, /* 64KB */
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
		/* RAM */ {
			/*
			 * We could not use 0x80000000 which conflicts with
			 * COMM_REGION_BASE
			 */
			.phys_start = 0x6fc00000,
			.virt_start = 0x6fc00000,
			.size = 0x08000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_DMA |
				JAILHOUSE_MEM_LOADABLE,
		},
		/* communication region */ {
			.virt_start = 0x80000000,
			.size = 0x00001000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_COMM_REGION,
		},
	},

	.irqchips = {
		/* uart2/sdhc1 */ {
			.address = 0x38800000,
			.pin_base = 32,
			.pin_bitmap = {
				(1 << (23 + 32 - 32)) | (1 << (28 + 32 - 32))
			},
		},
		/* IVSHMEM */ {
			.address = 0x38800000,
			.pin_base = 160,
			.pin_bitmap = {
				0xf << (154 + 32 - 160) /* SPI 154-157 */
			},
		},
	},

	.pci_devices = {
		{
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 2,
			.bdf = 2 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 1,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VIRTIO_FRONT +
				VIRTIO_DEV_BLOCK,
		},
		{
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 2,
			.bdf = 3 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 4,
			.shmem_dev_id = 1,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VIRTIO_FRONT +
				VIRTIO_DEV_CONSOLE,
		},
		{ /* IVSHMEM 00:00.0 (demo) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 2,
			.bdf = 0 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 8,
			.shmem_dev_id = 2,
			.shmem_peers = 3,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
		},
		{ /* IVSHMEM 00:01.0 (networking) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 2,
			.bdf = 1 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 13,
			.shmem_dev_id = 1,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
		},
	},
};
