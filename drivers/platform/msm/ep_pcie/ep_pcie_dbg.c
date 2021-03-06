/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Debugging enhancement in MSM PCIe endpoint driver.
 */

#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include "ep_pcie_com.h"
#include "ep_pcie_phy.h"

static struct dentry *dent_ep_pcie;
static struct dentry *dfile_case;
static struct ep_pcie_dev_t *dev;

static void ep_pcie_phy_dump(struct ep_pcie_dev_t *dev)
{
	int i;
	int control_offset[6] = {0x60, 0x70, 0x80, 0xA0, 0xB0, 0xB0};

	EP_PCIE_DUMP(dev, "PCIe V%d: PHY testbus\n", dev->rev);

	for (i = 0; i < 6; i++) {
		switch (i) {
		case 3:
			ep_pcie_write_reg(dev->phy,
					QSERDES_COM_ATB_SEL2,
					0x10);
			EP_PCIE_DUMP(dev,
				"PCIe V%d: QSERDES_COM_ATB_SEL2: 0x%x\n",
				dev->rev,
				readl_relaxed(dev->phy + QSERDES_COM_ATB_SEL2));
			break;
		case 4:
			ep_pcie_write_reg(dev->phy,
					QSERDES_TX_SERDES_BYP_EN_OUT,
					0x10);
			EP_PCIE_DUMP(dev,
				"PCIe V%d: QSERDES_TX_SERDES_BYP_EN_OUT: 0x%x\n",
				dev->rev,
				readl_relaxed(dev->phy +
					QSERDES_TX_SERDES_BYP_EN_OUT));
			break;
		case 5:
			ep_pcie_write_reg(dev->phy,
					QSERDES_TX_SERDES_BYP_EN_OUT,
					0x30);
			EP_PCIE_DUMP(dev,
				"PCIe V%d: QSERDES_TX_SERDES_BYP_EN_OUT: 0x%x\n",
				dev->rev,
				readl_relaxed(dev->phy +
					QSERDES_TX_SERDES_BYP_EN_OUT));
			break;
		default:
			break;
		}

		ep_pcie_write_reg(dev->phy, PCIE_PHY_TEST_CONTROL,
			control_offset[i]);

		EP_PCIE_DUMP(dev,
			"PCIe V%d: PCIE_PHY_TEST_CONTROL: 0x%x\n",
			dev->rev,
			readl_relaxed(dev->phy + PCIE_PHY_TEST_CONTROL));
		EP_PCIE_DUMP(dev,
			"PCIe V%d: PCIE_PHY_DEBUG_BUS_0_STATUS: 0x%x\n",
			dev->rev,
			readl_relaxed(dev->phy + PCIE_PHY_DEBUG_BUS_0_STATUS));
		EP_PCIE_DUMP(dev,
			"PCIe V%d: PCIE_PHY_DEBUG_BUS_1_STATUS: 0x%x\n",
			dev->rev,
			readl_relaxed(dev->phy + PCIE_PHY_DEBUG_BUS_1_STATUS));
		EP_PCIE_DUMP(dev,
			"PCIe V%d: PCIE_PHY_DEBUG_BUS_2_STATUS: 0x%x\n",
			dev->rev,
			readl_relaxed(dev->phy + PCIE_PHY_DEBUG_BUS_2_STATUS));
		EP_PCIE_DUMP(dev,
			"PCIe V%d: PCIE_PHY_DEBUG_BUS_3_STATUS: 0x%x\n",
			dev->rev,
			readl_relaxed(dev->phy + PCIE_PHY_DEBUG_BUS_3_STATUS));
	}

	EP_PCIE_DUMP(dev, "PCIe V%d: PHY register dump\n", dev->rev);

	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_COM_PLL_VCO_HIGH: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_COM_PLL_VCO_HIGH));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_COM_RESET_SM: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_COM_RESET_SM));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_COM_MUXVAL: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_COM_MUXVAL));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_PI_CTRL1: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_PI_CTRL1));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_PI_CTRL2: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_PI_CTRL2));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_PI_QUAD: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_PI_QUAD));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_IDATA1: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_IDATA1));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_IDATA2: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_IDATA2));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_AUX_DATA1: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_AUX_DATA1));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_AUX_DATA2: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_AUX_DATA2));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_AC_JTAG_OUTP: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_AC_JTAG_OUTP));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_AC_JTAG_OUTN: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_AC_JTAG_OUTN));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_RX_SIGDET: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_RX_SIGDET));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_RX_VDCOFF: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_RX_VDCOFF));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_IDAC_CAL_ON: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_IDAC_CAL_ON));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_IDAC_STATUS_I: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_IDAC_STATUS_I));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_IDAC_STATUS_Q: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_IDAC_STATUS_Q));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_IDAC_STATUS_A: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_IDAC_STATUS_A));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_CALST_STATUS_I: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_CALST_STATUS_I));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_CALST_STATUS_Q: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_CALST_STATUS_Q));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_CALST_STATUS_A: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_CALST_STATUS_A));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS0: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS0));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS1: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS1));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS2: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS2));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS3: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS3));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS4: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS4));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS5: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS5));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS6: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS6));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS7: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS7));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS8: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS8));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_EOM_STATUS9: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_EOM_STATUS9));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_RX_ALOG_INTF_OBSV: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_RX_ALOG_INTF_OBSV));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_READ_EQCODE: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_READ_EQCODE));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_RX_READ_OFFSETCODE: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_RX_READ_OFFSETCODE));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_TX_BIST_STATUS: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_TX_BIST_STATUS));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_TX_BIST_ERROR_COUNT1: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_TX_BIST_ERROR_COUNT1));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_TX_BIST_ERROR_COUNT2: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_TX_BIST_ERROR_COUNT2));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_TX_TX_ALOG_INTF_OBSV: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_TX_TX_ALOG_INTF_OBSV));
	EP_PCIE_DUMP(dev, "PCIe V%d: QSERDES_TX_PWM_DEC_STATUS: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + QSERDES_TX_PWM_DEC_STATUS));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_BIST_CHK_ERR_CNT_L: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_BIST_CHK_ERR_CNT_L));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_BIST_CHK_ERR_CNT_H: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_BIST_CHK_ERR_CNT_H));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_BIST_CHK_STATUS: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_BIST_CHK_STATUS));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_LFPS_RXTERM_IRQ_SOURCE: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_LFPS_RXTERM_IRQ_SOURCE));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_PCS_STATUS: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_PCS_STATUS));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_PCS_STATUS2: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_PCS_STATUS2));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_REVISION_ID0: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_REVISION_ID0));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_REVISION_ID1: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_REVISION_ID1));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_REVISION_ID2: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_REVISION_ID2));
	EP_PCIE_DUMP(dev, "PCIe V%d: PCIE_PHY_REVISION_ID3: 0x%x\n",
	dev->rev, readl_relaxed(dev->phy + PCIE_PHY_REVISION_ID3));
}

void ep_pcie_reg_dump(struct ep_pcie_dev_t *dev, u32 sel, bool linkdown)
{
	int r, i;
	u32 original;
	u32 size;

	EP_PCIE_DBG(dev,
		"PCIe V%d: Dump PCIe reg for 0x%x %s linkdown.\n",
		dev->rev, sel, linkdown ? "with" : "without");

	if (!dev->power_on) {
		EP_PCIE_ERR(dev,
			"PCIe V%d: the power is already down; can't dump registers.\n",
			dev->rev);
		return;
	}

	if (linkdown) {
		EP_PCIE_DUMP(dev,
			"PCIe V%d: dump PARF registers for linkdown case.\n",
			dev->rev);

		original = readl_relaxed(dev->parf + PCIE20_PARF_SYS_CTRL);
		for (i = 1; i <= 0x1A; i++) {
			ep_pcie_write_mask(dev->parf + PCIE20_PARF_SYS_CTRL,
				0xFF0000, i << 16);
			EP_PCIE_DUMP(dev,
				"PCIe V%d: PARF_SYS_CTRL:0x%x PARF_TEST_BUS:0x%x\n",
				dev->rev,
				readl_relaxed(dev->parf + PCIE20_PARF_SYS_CTRL),
				readl_relaxed(dev->parf +
							PCIE20_PARF_TEST_BUS));
		}
		ep_pcie_write_reg(dev->parf, PCIE20_PARF_SYS_CTRL, original);
	}

	for (r = 0; r < EP_PCIE_MAX_RES; r++) {
		if (!(sel & BIT(r)))
			continue;

		if (r == EP_PCIE_RES_PHY)
			ep_pcie_phy_dump(dev);

		size = resource_size(dev->res[r].resource);
		EP_PCIE_DUMP(dev,
			"\nPCIe V%d: dump registers of %s.\n\n",
			dev->rev, dev->res[r].name);

		for (i = 0; i < size; i += 32) {
			EP_PCIE_DUMP(dev,
				"0x%04x %08x %08x %08x %08x %08x %08x %08x %08x\n",
				i, readl_relaxed(dev->res[r].base + i),
				readl_relaxed(dev->res[r].base + (i + 4)),
				readl_relaxed(dev->res[r].base + (i + 8)),
				readl_relaxed(dev->res[r].base + (i + 12)),
				readl_relaxed(dev->res[r].base + (i + 16)),
				readl_relaxed(dev->res[r].base + (i + 20)),
				readl_relaxed(dev->res[r].base + (i + 24)),
				readl_relaxed(dev->res[r].base + (i + 28)));
		}
	}
}

static void ep_pcie_show_status(struct ep_pcie_dev_t *dev)
{
	EP_PCIE_DBG_FS("PCIe: is %s enumerated\n",
		dev->enumerated ? "" : "not");
	EP_PCIE_DBG_FS("PCIe: link is %s\n",
		(dev->link_status == EP_PCIE_LINK_ENABLED)
		? "enabled" : "disabled");
	EP_PCIE_DBG_FS("the link is %s suspending\n",
		dev->suspending ? "" : "not");
	EP_PCIE_DBG_FS("the power is %s on\n",
		dev->power_on ? "" : "not");
	EP_PCIE_DBG_FS("bus_client: %d\n",
		dev->bus_client);
	EP_PCIE_DBG_FS("linkdown_counter: %lu\n",
		dev->linkdown_counter);
	EP_PCIE_DBG_FS("linkup_counter: %lu\n",
		dev->linkup_counter);
	EP_PCIE_DBG_FS("wake_counter: %lu\n",
		dev->wake_counter);
	EP_PCIE_DBG_FS("d0_counter: %lu\n",
		dev->d0_counter);
	EP_PCIE_DBG_FS("d3_counter: %lu\n",
		dev->d3_counter);
	EP_PCIE_DBG_FS("perst_ast_counter: %lu\n",
		dev->perst_ast_counter);
	EP_PCIE_DBG_FS("perst_deast_counter: %lu\n",
		dev->perst_deast_counter);
}

static ssize_t ep_pcie_cmd_debug(struct file *file,
				const char __user *buf,
				size_t count, loff_t *ppos)
{
	unsigned long ret;
	char str[MAX_MSG_LEN];
	unsigned int testcase = 0;
	struct ep_pcie_msi_config msi_cfg;
	int i;
	u32 device_id = 0;
	struct ep_pcie_hw *phandle = NULL;
	struct ep_pcie_iatu entries[2] = {
		{0x80000000, 0xbe7fffff, 0, 0},
		{0xb1440000, 0xb144ae1e, 0x31440000, 0}
	};
	struct ep_pcie_db_config chdb_cfg = {0x64, 0x6b, 0xfd4fa000};
	struct ep_pcie_db_config erdb_cfg = {0x64, 0x6b, 0xfd4fa080};

	if (dev->power_on) {
		device_id = readl_relaxed(dev->dm_core);
		phandle = ep_pcie_get_phandle(device_id);
	}

	memset(str, 0, sizeof(str));
	ret = copy_from_user(str, buf, sizeof(str));
	if (ret)
		return -EFAULT;

	for (i = 0; i < sizeof(str) && (str[i] >= '0') && (str[i] <= '9'); ++i)
		testcase = (testcase * 10) + (str[i] - '0');

	EP_PCIE_DBG_FS("PCIe: TEST: %d\n", testcase);


	switch (testcase) {
	case 0: /* output status */
		ep_pcie_show_status(dev);
		break;
	case 1: /* output PHY and PARF registers */
		ep_pcie_reg_dump(dev, BIT(EP_PCIE_RES_PHY) |
				BIT(EP_PCIE_RES_PARF), true);
		break;
	case 2: /* output core registers */
		ep_pcie_reg_dump(dev, BIT(EP_PCIE_RES_DM_CORE), false);
		break;
	case 3: /* output MMIO registers */
		ep_pcie_reg_dump(dev, BIT(EP_PCIE_RES_MMIO), false);
		break;
	case 4: /* output ELBI registers */
		ep_pcie_reg_dump(dev, BIT(EP_PCIE_RES_ELBI), false);
		break;
	case 5: /* output MSI registers */
		ep_pcie_reg_dump(dev, BIT(EP_PCIE_RES_MSI), false);
		break;
	case 6: /* turn on link */
		ep_pcie_enable_endpoint(phandle, EP_PCIE_OPT_ALL);
		break;
	case 7: /* enumeration */
		ep_pcie_enable_endpoint(phandle, EP_PCIE_OPT_ENUM);
		break;
	case 8: /* turn off link */
		ep_pcie_disable_endpoint(phandle);
		break;
	case 9: /* check MSI */
		ep_pcie_get_msi_config(phandle, &msi_cfg);
		break;
	case 10: /* trigger MSI */
		ep_pcie_trigger_msi(phandle, 0);
		break;
	case 11: /* indicate the status of PCIe link */
		EP_PCIE_DBG_FS("\nPCIe: link status is %d.\n\n",
			ep_pcie_get_linkstatus(phandle));
		break;
	case 12: /* configure outbound iATU */
		ep_pcie_config_outbound_iatu(phandle, entries, 2);
		break;
	case 13: /* wake up the host */
		ep_pcie_wakeup_host(phandle);
		break;
	case 14: /* Configure routing of doorbells */
		ep_pcie_config_db_routing(phandle, chdb_cfg, erdb_cfg);
		break;
	case 21: /* write D3 */
		EP_PCIE_DBG_FS("\nPCIe Testcase %d: write D3 to EP\n\n",
			testcase);
		EP_PCIE_DBG_FS("\nPCIe: 0x44 of EP is 0x%x before change\n\n",
			readl_relaxed(dev->dm_core + 0x44));
		ep_pcie_write_mask(dev->dm_core + 0x44, 0, 0x3);
		EP_PCIE_DBG_FS("\nPCIe: 0x44 of EP is 0x%x now\n\n",
			readl_relaxed(dev->dm_core + 0x44));
		break;
	case 22: /* write D0 */
		EP_PCIE_DBG_FS("\nPCIe Testcase %d: write D0 to EP\n\n",
			testcase);
		EP_PCIE_DBG_FS("\nPCIe: 0x44 of EP is 0x%x before change\n\n",
			readl_relaxed(dev->dm_core + 0x44));
		ep_pcie_write_mask(dev->dm_core + 0x44, 0x3, 0);
		EP_PCIE_DBG_FS("\nPCIe: 0x44 of EP is 0x%x now\n\n",
			readl_relaxed(dev->dm_core + 0x44));
		break;
	case 23: /* assert wake */
		EP_PCIE_DBG_FS("\nPCIe Testcase %d: assert wake\n\n",
			testcase);
		gpio_set_value(dev->gpio[EP_PCIE_GPIO_WAKE].num,
			dev->gpio[EP_PCIE_GPIO_WAKE].on);
		break;
	case 24: /* deassert wake */
		EP_PCIE_DBG_FS("\nPCIe Testcase %d: deassert wake\n\n",
			testcase);
		gpio_set_value(dev->gpio[EP_PCIE_GPIO_WAKE].num,
			1 - dev->gpio[EP_PCIE_GPIO_WAKE].on);
		break;
	case 25: /* output PERST# status */
		EP_PCIE_DBG_FS("\nPCIe: PERST# is %d.\n\n",
			gpio_get_value(dev->gpio[EP_PCIE_GPIO_PERST].num));
		break;
	case 26: /* output WAKE# status */
		EP_PCIE_DBG_FS("\nPCIe: WAKE# is %d.\n\n",
			gpio_get_value(dev->gpio[EP_PCIE_GPIO_WAKE].num));
		break;
	case 31: /* output core registers when D3 hot is set by host*/
		dev->dump_conf = true;
		break;
	case 32: /* do not output core registers when D3 hot is set by host*/
		dev->dump_conf = false;
		break;
	default:
		EP_PCIE_DBG_FS("PCIe: Invalid testcase: %d.\n", testcase);
		break;
	}

	if (ret == 0)
		return count;
	else
		return -EFAULT;
}

const struct file_operations ep_pcie_cmd_debug_ops = {
	.write = ep_pcie_cmd_debug,
};

void ep_pcie_debugfs_init(struct ep_pcie_dev_t *ep_dev)
{
	dev = ep_dev;
	dent_ep_pcie = debugfs_create_dir("pcie-ep", 0);
	if (IS_ERR(dent_ep_pcie)) {
		EP_PCIE_ERR(dev,
			"PCIe V%d: fail to create the folder for debug_fs.\n",
			dev->rev);
		return;
	}

	dfile_case = debugfs_create_file("case", 0664,
					dent_ep_pcie, 0,
					&ep_pcie_cmd_debug_ops);
	if (!dfile_case || IS_ERR(dfile_case)) {
		EP_PCIE_ERR(dev,
			"PCIe V%d: fail to create the file for case.\n",
			dev->rev);
		goto case_error;
	}

	EP_PCIE_DBG2(dev,
		"PCIe V%d: debugfs is enabled.\n",
		dev->rev);

	return;

case_error:
	debugfs_remove(dent_ep_pcie);
}

void ep_pcie_debugfs_exit(void)
{
	debugfs_remove(dfile_case);
	debugfs_remove(dent_ep_pcie);
}
