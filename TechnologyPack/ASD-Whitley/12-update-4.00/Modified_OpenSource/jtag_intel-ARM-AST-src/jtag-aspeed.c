// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2018 Mellanox Technologies. All rights reserved.
// Copyright (c) 2018 Oleksandr Shamray <oleksandrs@mellanox.com>
// Copyright (c) 2019 Intel Corporation

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/jtag.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <uapi/linux/jtag.h>

#define ASPEED_SCU_RESET_JTAG		BIT(22)

#define ASPEED_JTAG_DATA		0x00
#define ASPEED_JTAG_INST		0x04
#define ASPEED_JTAG_CTRL		0x08
#define ASPEED_JTAG_ISR			0x0C
#define ASPEED_JTAG_SW			0x10
#define ASPEED_JTAG_TCK			0x14
#define ASPEED_JTAG_EC			0x18

#define ASPEED_JTAG_DATA_MSB		0x01
#define ASPEED_JTAG_DATA_CHUNK_SIZE	0x20

/* ASPEED_JTAG_CTRL: Engine Control */
#define ASPEED_JTAG_CTL_ENG_EN		BIT(31)
#define ASPEED_JTAG_CTL_ENG_OUT_EN	BIT(30)
#define ASPEED_JTAG_CTL_FORCE_TMS	BIT(29)
#define ASPEED_JTAG_CTL_IR_UPDATE	BIT(26)
#define ASPEED_JTAG_CTL_INST_LEN(x)	((x) << 20)
#define ASPEED_JTAG_CTL_LASPEED_INST	BIT(17)
#define ASPEED_JTAG_CTL_INST_EN		BIT(16)
#define ASPEED_JTAG_CTL_DR_UPDATE	BIT(10)
#define ASPEED_JTAG_CTL_DATA_LEN(x)	((x) << 4)
#define ASPEED_JTAG_CTL_LASPEED_DATA	BIT(1)
#define ASPEED_JTAG_CTL_DATA_EN		BIT(0)

/* ASPEED_JTAG_ISR : Interrupt status and enable */
#define ASPEED_JTAG_ISR_INST_PAUSE	BIT(19)
#define ASPEED_JTAG_ISR_INST_COMPLETE	BIT(18)
#define ASPEED_JTAG_ISR_DATA_PAUSE	BIT(17)
#define ASPEED_JTAG_ISR_DATA_COMPLETE	BIT(16)
#define ASPEED_JTAG_ISR_INST_PAUSE_EN	BIT(3)
#define ASPEED_JTAG_ISR_INST_COMPLETE_EN BIT(2)
#define ASPEED_JTAG_ISR_DATA_PAUSE_EN	BIT(1)
#define ASPEED_JTAG_ISR_DATA_COMPLETE_EN BIT(0)
#define ASPEED_JTAG_ISR_INT_EN_MASK	GENMASK(3, 0)
#define ASPEED_JTAG_ISR_INT_MASK	GENMASK(19, 16)

/* ASPEED_JTAG_SW : Software Mode and Status */
#define ASPEED_JTAG_SW_MODE_EN		BIT(19)
#define ASPEED_JTAG_SW_MODE_TCK		BIT(18)
#define ASPEED_JTAG_SW_MODE_TMS		BIT(17)
#define ASPEED_JTAG_SW_MODE_TDIO	BIT(16)

/* ASPEED_JTAG_TCK : TCK Control */
#define ASPEED_JTAG_TCK_DIVISOR_MASK	GENMASK(10, 0)
#define ASPEED_JTAG_TCK_GET_DIV(x)	((x) & ASPEED_JTAG_TCK_DIVISOR_MASK)

/* ASPEED_JTAG_EC : Controller set for go to IDLE */
#define ASPEED_JTAG_EC_GO_IDLE		BIT(0)

#define ASPEED_JTAG_IOUT_LEN(len) \
	(ASPEED_JTAG_CTL_ENG_EN | \
	 ASPEED_JTAG_CTL_ENG_OUT_EN | \
	 ASPEED_JTAG_CTL_INST_LEN(len))

#define ASPEED_JTAG_DOUT_LEN(len) \
	(ASPEED_JTAG_CTL_ENG_EN | \
	 ASPEED_JTAG_CTL_ENG_OUT_EN | \
	 ASPEED_JTAG_CTL_DATA_LEN(len))

#define ASPEED_JTAG_SW_TDIO (ASPEED_JTAG_SW_MODE_EN | ASPEED_JTAG_SW_MODE_TDIO)

#define ASPEED_JTAG_GET_TDI(direction, byte) \
	(((direction) & JTAG_WRITE_XFER) ? byte : UINT_MAX)

#define ASPEED_JTAG_TCK_WAIT		10
#define ASPEED_JTAG_RESET_CNTR		10
#define WAIT_ITERATIONS		75

/*#define USE_INTERRUPTS*/
/*#define DEBUG_JTAG*/

static const char * const regnames[] = {
	[ASPEED_JTAG_DATA] = "ASPEED_JTAG_DATA",
	[ASPEED_JTAG_INST] = "ASPEED_JTAG_INST",
	[ASPEED_JTAG_CTRL] = "ASPEED_JTAG_CTRL",
	[ASPEED_JTAG_ISR]  = "ASPEED_JTAG_ISR",
	[ASPEED_JTAG_SW]   = "ASPEED_JTAG_SW",
	[ASPEED_JTAG_TCK]  = "ASPEED_JTAG_TCK",
	[ASPEED_JTAG_EC]   = "ASPEED_JTAG_EC",
};

#define ASPEED_JTAG_NAME		"jtag-aspeed"

struct aspeed_jtag {
	void __iomem			*reg_base;
	struct device			*dev;
	struct clk			*pclk;
	enum jtag_tapstate		status;
	int				irq;
	struct reset_control		*rst;
	u32				flag;
	wait_queue_head_t		jtag_wq;
	u32				mode;
};

/*
 * This structure represents a TMS cycle, as expressed in a set of bits and a
 * count of bits (note: there are no start->end state transitions that require
 * more than 1 byte of TMS cycles)
 */
struct tms_cycle {
	unsigned char		tmsbits;
	unsigned char		count;
};

/*
 * This is the complete set TMS cycles for going from any TAP state to any
 * other TAP state, following a "shortest path" rule.
 */
static const struct tms_cycle _tms_cycle_lookup[][16] = {
/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* TLR  */{{0x00, 0}, {0x00, 1}, {0x02, 2}, {0x02, 3}, {0x02, 4}, {0x0a, 4},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x0a, 5}, {0x2a, 6}, {0x1a, 5}, {0x06, 3}, {0x06, 4}, {0x06, 5},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x16, 5}, {0x16, 6}, {0x56, 7}, {0x36, 6} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* RTI  */{{0x07, 3}, {0x00, 0}, {0x01, 1}, {0x01, 2}, {0x01, 3}, {0x05, 3},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x05, 4}, {0x15, 5}, {0x0d, 4}, {0x03, 2}, {0x03, 3}, {0x03, 4},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x0b, 4}, {0x0b, 5}, {0x2b, 6}, {0x1b, 5} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* SelDR*/{{0x03, 2}, {0x03, 3}, {0x00, 0}, {0x00, 1}, {0x00, 2}, {0x02, 2},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x02, 3}, {0x0a, 4}, {0x06, 3}, {0x01, 1}, {0x01, 2}, {0x01, 3},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x05, 3}, {0x05, 4}, {0x15, 5}, {0x0d, 4} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* CapDR*/{{0x1f, 5}, {0x03, 3}, {0x07, 3}, {0x00, 0}, {0x00, 1}, {0x01, 1},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x01, 2}, {0x05, 3}, {0x03, 2}, {0x0f, 4}, {0x0f, 5}, {0x0f, 6},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x2f, 6}, {0x2f, 7}, {0xaf, 8}, {0x6f, 7} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* SDR  */{{0x1f, 5}, {0x03, 3}, {0x07, 3}, {0x07, 4}, {0x00, 0}, {0x01, 1},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x01, 2}, {0x05, 3}, {0x03, 2}, {0x0f, 4}, {0x0f, 5}, {0x0f, 6},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x2f, 6}, {0x2f, 7}, {0xaf, 8}, {0x6f, 7} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* Ex1DR*/{{0x0f, 4}, {0x01, 2}, {0x03, 2}, {0x03, 3}, {0x02, 3}, {0x00, 0},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x00, 1}, {0x02, 2}, {0x01, 1}, {0x07, 3}, {0x07, 4}, {0x07, 5},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x17, 5}, {0x17, 6}, {0x57, 7}, {0x37, 6} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* PDR  */{{0x1f, 5}, {0x03, 3}, {0x07, 3}, {0x07, 4}, {0x01, 2}, {0x05, 3},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x00, 0}, {0x01, 1}, {0x03, 2}, {0x0f, 4}, {0x0f, 5}, {0x0f, 6},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x2f, 6}, {0x2f, 7}, {0xaf, 8}, {0x6f, 7} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* Ex2DR*/{{0x0f, 4}, {0x01, 2}, {0x03, 2}, {0x03, 3}, {0x00, 1}, {0x02, 2},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x02, 3}, {0x00, 0}, {0x01, 1}, {0x07, 3}, {0x07, 4}, {0x07, 5},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x17, 5}, {0x17, 6}, {0x57, 7}, {0x37, 6} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* UpdDR*/{{0x07, 3}, {0x00, 1}, {0x01, 1}, {0x01, 2}, {0x01, 3}, {0x05, 3},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x05, 4}, {0x15, 5}, {0x00, 0}, {0x03, 2}, {0x03, 3}, {0x03, 4},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x0b, 4}, {0x0b, 5}, {0x2b, 6}, {0x1b, 5} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* SelIR*/{{0x01, 1}, {0x01, 2}, {0x05, 3}, {0x05, 4}, {0x05, 5}, {0x15, 5},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x15, 6}, {0x55, 7}, {0x35, 6}, {0x00, 0}, {0x00, 1}, {0x00, 2},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x02, 2}, {0x02, 3}, {0x0a, 4}, {0x06, 3} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* CapIR*/{{0x1f, 5}, {0x03, 3}, {0x07, 3}, {0x07, 4}, {0x07, 5}, {0x17, 5},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x17, 6}, {0x57, 7}, {0x37, 6}, {0x0f, 4}, {0x00, 0}, {0x00, 1},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x01, 1}, {0x01, 2}, {0x05, 3}, {0x03, 2} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* SIR  */{{0x1f, 5}, {0x03, 3}, {0x07, 3}, {0x07, 4}, {0x07, 5}, {0x17, 5},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x17, 6}, {0x57, 7}, {0x37, 6}, {0x0f, 4}, {0x0f, 5}, {0x00, 0},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x01, 1}, {0x01, 2}, {0x05, 3}, {0x03, 2} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* Ex1IR*/{{0x0f, 4}, {0x01, 2}, {0x03, 2}, {0x03, 3}, {0x03, 4}, {0x0b, 4},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x0b, 5}, {0x2b, 6}, {0x1b, 5}, {0x07, 3}, {0x07, 4}, {0x02, 3},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x00, 0}, {0x00, 1}, {0x02, 2}, {0x01, 1} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* PIR  */{{0x1f, 5}, {0x03, 3}, {0x07, 3}, {0x07, 4}, {0x07, 5}, {0x17, 5},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x17, 6}, {0x57, 7}, {0x37, 6}, {0x0f, 4}, {0x0f, 5}, {0x01, 2},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x05, 3}, {0x00, 0}, {0x01, 1}, {0x03, 2} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* Ex2IR*/{{0x0f, 4}, {0x01, 2}, {0x03, 2}, {0x03, 3}, {0x03, 4}, {0x0b, 4},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x0b, 5}, {0x2b, 6}, {0x1b, 5}, {0x07, 3}, {0x07, 4}, {0x00, 1},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x02, 2}, {0x02, 3}, {0x00, 0}, {0x01, 1} },

/*	    TLR        RTI        SelDR      CapDR      SDR        Ex1DR*/
/* UpdIR*/{{0x07, 3}, {0x00, 1}, {0x01, 1}, {0x01, 2}, {0x01, 3}, {0x05, 3},
/*	    PDR        Ex2DR      UpdDR      SelIR      CapIR      SIR*/
	    {0x05, 4}, {0x15, 5}, {0x0d, 4}, {0x03, 2}, {0x03, 3}, {0x03, 4},
/*	    Ex1IR      PIR        Ex2IR      UpdIR*/
	    {0x0b, 4}, {0x0b, 5}, {0x2b, 6}, {0x00, 0} },
};

#ifdef DEBUG_JTAG
static char *end_status_str[] = {
	"tlr", "idle", "selDR", "capDR", "sDR", "ex1DR", "pDR", "ex2DR",
	 "updDR", "selIR", "capIR", "sIR", "ex1IR", "pIR", "ex2IR", "updIR"
};
#endif

static u32 aspeed_jtag_read(struct aspeed_jtag *aspeed_jtag, u32 reg)
{
	u32 val = readl(aspeed_jtag->reg_base + reg);

#ifdef DEBUG_JTAG
	dev_dbg(aspeed_jtag->dev, "read:%s val = 0x%08x\n", regnames[reg], val);
#endif
	return val;
}

static void
aspeed_jtag_write(struct aspeed_jtag *aspeed_jtag, u32 val, u32 reg)
{
#ifdef DEBUG_JTAG
	dev_dbg(aspeed_jtag->dev, "write:%s val = 0x%08x\n",
		regnames[reg], val);
#endif
	writel(val, aspeed_jtag->reg_base + reg);
}

static int aspeed_jtag_freq_set(struct jtag *jtag, u32 freq)
{
	struct aspeed_jtag *aspeed_jtag = jtag_priv(jtag);
	unsigned long apb_frq;
	u32 tck_val;
	u16 div;

	apb_frq = clk_get_rate(aspeed_jtag->pclk);
	if (!apb_frq)
		return -ENOTSUPP;

	div = (apb_frq - 1) / freq;
	tck_val = aspeed_jtag_read(aspeed_jtag, ASPEED_JTAG_TCK);
	aspeed_jtag_write(aspeed_jtag,
			  (tck_val & ~ASPEED_JTAG_TCK_DIVISOR_MASK) | div,
			  ASPEED_JTAG_TCK);
	return 0;
}

static int aspeed_jtag_freq_get(struct jtag *jtag, u32 *frq)
{
	struct aspeed_jtag *aspeed_jtag = jtag_priv(jtag);
	u32 pclk;
	u32 tck;

	pclk = clk_get_rate(aspeed_jtag->pclk);
	tck = aspeed_jtag_read(aspeed_jtag, ASPEED_JTAG_TCK);
	*frq = pclk / (ASPEED_JTAG_TCK_GET_DIV(tck) + 1);

	return 0;
}

static inline void aspeed_jtag_output_disable(struct aspeed_jtag *aspeed_jtag)
{
	aspeed_jtag_write(aspeed_jtag, 0, ASPEED_JTAG_CTRL);
}

static inline void aspeed_jtag_master(struct aspeed_jtag *aspeed_jtag)
{
	aspeed_jtag_write(aspeed_jtag, (ASPEED_JTAG_CTL_ENG_EN |
					ASPEED_JTAG_CTL_ENG_OUT_EN),
					ASPEED_JTAG_CTRL);

	aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_SW_MODE_EN |
			ASPEED_JTAG_SW_MODE_TDIO,
			ASPEED_JTAG_SW);
	aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_ISR_INST_PAUSE |
			ASPEED_JTAG_ISR_INST_COMPLETE |
			ASPEED_JTAG_ISR_DATA_PAUSE |
			ASPEED_JTAG_ISR_DATA_COMPLETE |
			ASPEED_JTAG_ISR_INST_PAUSE_EN |
			ASPEED_JTAG_ISR_INST_COMPLETE_EN |
			ASPEED_JTAG_ISR_DATA_PAUSE_EN |
			ASPEED_JTAG_ISR_DATA_COMPLETE_EN,
			ASPEED_JTAG_ISR);  /* Enable Interrupt */
}

static int aspeed_jtag_mode_set(struct jtag *jtag, struct jtag_mode *jtag_mode)
{
	struct aspeed_jtag *aspeed_jtag = jtag_priv(jtag);

	switch (jtag_mode->feature) {
	case JTAG_XFER_MODE:
		aspeed_jtag->mode = jtag_mode->mode;
		break;
	case JTAG_CONTROL_MODE:
		if (jtag_mode->mode == JTAG_MASTER_OUTPUT_DISABLE)
			aspeed_jtag_output_disable(aspeed_jtag);
		else if (jtag_mode->mode == JTAG_MASTER_MODE)
			aspeed_jtag_master(aspeed_jtag);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static char aspeed_jtag_tck_cycle(struct aspeed_jtag *aspeed_jtag,
				  u8 tms, u8 tdi)
{
	char tdo = 0;

	/* TCK = 0 */
	aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_SW_MODE_EN |
			  (tms * ASPEED_JTAG_SW_MODE_TMS) |
			  (tdi * ASPEED_JTAG_SW_MODE_TDIO), ASPEED_JTAG_SW);

	aspeed_jtag_read(aspeed_jtag, ASPEED_JTAG_SW);

	/* TCK = 1 */
	aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_SW_MODE_EN |
			  ASPEED_JTAG_SW_MODE_TCK |
			  (tms * ASPEED_JTAG_SW_MODE_TMS) |
			  (tdi * ASPEED_JTAG_SW_MODE_TDIO), ASPEED_JTAG_SW);

	if (aspeed_jtag_read(aspeed_jtag, ASPEED_JTAG_SW) &
	    ASPEED_JTAG_SW_MODE_TDIO)
		tdo = 1;

	return tdo;
}

static int aspeed_jtag_bitbang(struct jtag *jtag,
			       struct bitbang_packet *bitbang,
			       struct tck_bitbang *bitbang_data)
{
	struct aspeed_jtag *aspeed_jtag = jtag_priv(jtag);
	int i = 0;

	for (i = 0; i < bitbang->length; i++) {
		bitbang_data[i].tdo =
		aspeed_jtag_tck_cycle(aspeed_jtag, bitbang_data[i].tms,
				      bitbang_data[i].tdi);
	}
	return 0;
}

static int aspeed_jtag_wait_instruction_pause(struct aspeed_jtag *aspeed_jtag)
{
	int res = 0;
#ifdef USE_INTERRUPTS
	res = wait_event_interruptible(aspeed_jtag->jtag_wq,
				       aspeed_jtag->flag &
				       ASPEED_JTAG_ISR_INST_PAUSE);
	aspeed_jtag->flag &= ~ASPEED_JTAG_ISR_INST_PAUSE;
#else
	u32 status = 0;
	u32 iterations = 0;

	while ((status & ASPEED_JTAG_ISR_INST_PAUSE) == 0) {
		status = aspeed_jtag_read(aspeed_jtag, ASPEED_JTAG_ISR);
#ifdef DEBUG_JTAG
		dev_dbg(aspeed_jtag->dev, "%s  = 0x%08x\n", __func__, status);
#endif
		iterations++;
		if (iterations > WAIT_ITERATIONS) {
			dev_err(aspeed_jtag->dev,
				"aspeed_jtag driver timed out waiting for instruction pause complete\n");
			res = -EFAULT;
			break;
		}
		if ((status & ASPEED_JTAG_ISR_DATA_COMPLETE) == 0) {
			if (iterations % 25 == 0)
				usleep_range(1, 5);
			else
				udelay(1);
		}
	}
	aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_ISR_INST_PAUSE |
			  (status & 0xf),
			  ASPEED_JTAG_ISR);
#endif
	return res;
}

static int
aspeed_jtag_wait_instruction_complete(struct aspeed_jtag *aspeed_jtag)
{
	int res = 0;
#ifdef USE_INTERRUPTS
	res = wait_event_interruptible(aspeed_jtag->jtag_wq,
				       aspeed_jtag->flag &
				       ASPEED_JTAG_ISR_INST_COMPLETE);
	aspeed_jtag->flag &= ~ASPEED_JTAG_ISR_INST_COMPLETE;
#else
	u32 status = 0;
	u32 iterations = 0;

	while ((status & ASPEED_JTAG_ISR_INST_COMPLETE) == 0) {
		status = aspeed_jtag_read(aspeed_jtag, ASPEED_JTAG_ISR);
#ifdef DEBUG_JTAG
		dev_dbg(aspeed_jtag->dev, "%s  = 0x%08x\n", __func__, status);
#endif
		iterations++;
		if (iterations > WAIT_ITERATIONS) {
			dev_err(aspeed_jtag->dev,
				"aspeed_jtag driver timed out waiting for instruction complete\n");
			res = -EFAULT;
			break;
		}
		if ((status & ASPEED_JTAG_ISR_DATA_COMPLETE) == 0) {
			if (iterations % 25 == 0)
				usleep_range(1, 5);
			else
				udelay(1);
		}
	}
	aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_ISR_INST_COMPLETE |
			  (status & 0xf),
			  ASPEED_JTAG_ISR);
#endif
	return res;
}

static int
aspeed_jtag_wait_data_pause_complete(struct aspeed_jtag *aspeed_jtag)
{
	int res = 0;
#ifdef USE_INTERRUPTS
	res = wait_event_interruptible(aspeed_jtag->jtag_wq,
				       aspeed_jtag->flag &
				       ASPEED_JTAG_ISR_DATA_PAUSE);
	aspeed_jtag->flag &= ~ASPEED_JTAG_ISR_DATA_PAUSE;
#else
	u32 status = 0;
	u32 iterations = 0;

	while ((status & ASPEED_JTAG_ISR_DATA_PAUSE) == 0) {
		status = aspeed_jtag_read(aspeed_jtag, ASPEED_JTAG_ISR);
#ifdef DEBUG_JTAG
		dev_dbg(aspeed_jtag->dev, "%s  = 0x%08x\n", __func__, status);
#endif
		iterations++;
		if (iterations > WAIT_ITERATIONS) {
			dev_err(aspeed_jtag->dev,
				"aspeed_jtag driver timed out waiting for data pause complete\n");
			res = -EFAULT;
			break;
		}
		if ((status & ASPEED_JTAG_ISR_DATA_COMPLETE) == 0) {
			if (iterations % 25 == 0)
				usleep_range(1, 5);
			else
				udelay(1);
		}
	}
	aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_ISR_DATA_PAUSE |
			  (status & 0xf), ASPEED_JTAG_ISR);
#endif
	return res;
}

static int aspeed_jtag_wait_data_complete(struct aspeed_jtag *aspeed_jtag)
{
	int res = 0;
#ifdef USE_INTERRUPTS
	res = wait_event_interruptible(aspeed_jtag->jtag_wq,
				       aspeed_jtag->flag &
				       ASPEED_JTAG_ISR_DATA_COMPLETE);
	aspeed_jtag->flag &= ~ASPEED_JTAG_ISR_DATA_COMPLETE;
#else
	u32 status = 0;
	u32 iterations = 0;

	while ((status & ASPEED_JTAG_ISR_DATA_COMPLETE) == 0) {
		status = aspeed_jtag_read(aspeed_jtag, ASPEED_JTAG_ISR);
#ifdef DEBUG_JTAG
		dev_dbg(aspeed_jtag->dev, "%s  = 0x%08x\n", __func__, status);
#endif
		iterations++;
		if (iterations > WAIT_ITERATIONS) {
			dev_err(aspeed_jtag->dev,
				"ast_jtag driver timed out waiting for data complete\n");
			res = -EFAULT;
			break;
		}
		if ((status & ASPEED_JTAG_ISR_DATA_COMPLETE) == 0) {
			if (iterations % 25 == 0)
				usleep_range(1, 5);
			else
				udelay(1);
		}
	}
	aspeed_jtag_write(aspeed_jtag,
			  ASPEED_JTAG_ISR_DATA_COMPLETE | (status & 0xf),
			  ASPEED_JTAG_ISR);
#endif
	return res;
}

static void aspeed_jtag_set_tap_state(struct aspeed_jtag *aspeed_jtag,
				      enum jtag_tapstate from_state,
				      enum jtag_tapstate end_state)
{
	int i = 0;
	enum jtag_tapstate from, to;

	from = from_state;
	to = end_state;

	if (from == JTAG_STATE_CURRENT)
		from = aspeed_jtag->status;

	for (i = 0; i < _tms_cycle_lookup[from][to].count; i++)
		aspeed_jtag_tck_cycle(aspeed_jtag,
			((_tms_cycle_lookup[from][to].tmsbits >> i) & 0x1), 0);
	aspeed_jtag->status = end_state;
}

static void aspeed_jtag_set_tap_state_sw(struct aspeed_jtag *aspeed_jtag,
					 struct jtag_tap_state *tapstate)
{
	/* SW mode from curent tap state -> to end_state */
	if (tapstate->reset) {
		int i = 0;

		for (i = 0; i < ASPEED_JTAG_RESET_CNTR; i++)
			aspeed_jtag_tck_cycle(aspeed_jtag, 1, 0);
		aspeed_jtag->status = JTAG_STATE_TLRESET;
	}

	aspeed_jtag_set_tap_state(aspeed_jtag, tapstate->from,
				  tapstate->endstate);
}

static int aspeed_jtag_status_set(struct jtag *jtag,
				  struct jtag_tap_state *tapstate)
{
	struct aspeed_jtag *aspeed_jtag = jtag_priv(jtag);

#ifdef DEBUG_JTAG
	dev_dbg(aspeed_jtag->dev, "Set TAP state: %s\n",
		end_status_str[tapstate->endstate]);
#endif

	if (!(aspeed_jtag->mode & JTAG_XFER_HW_MODE)) {
		aspeed_jtag_set_tap_state_sw(aspeed_jtag, tapstate);
		return 0;
	}

	/* x TMS high + 1 TMS low */
	if (tapstate->reset) {
		/* Disable sw mode */
		aspeed_jtag_write(aspeed_jtag, 0, ASPEED_JTAG_SW);
		mdelay(1);
		aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_CTL_ENG_EN |
				  ASPEED_JTAG_CTL_ENG_OUT_EN |
				  ASPEED_JTAG_CTL_FORCE_TMS, ASPEED_JTAG_CTRL);
		mdelay(1);
		aspeed_jtag_write(aspeed_jtag,
				  ASPEED_JTAG_SW_TDIO, ASPEED_JTAG_SW);
		aspeed_jtag->status = JTAG_STATE_TLRESET;
	}

	return 0;
}

static void aspeed_jtag_xfer_sw(struct aspeed_jtag *aspeed_jtag,
				struct jtag_xfer *xfer, u32 *data)
{
	unsigned long remain_xfer = xfer->length;
	unsigned long shift_bits = 0;
	unsigned long index = 0;
	unsigned long tdi;
	char tdo;

#ifdef DEBUG_JTAG
	dev_dbg(aspeed_jtag->dev, "SW JTAG SHIFT %s, length = %d\n",
		(xfer->type == JTAG_SIR_XFER) ? "IR" : "DR", xfer->length);
#endif

	if (xfer->type == JTAG_SIR_XFER)
		aspeed_jtag_set_tap_state(aspeed_jtag, xfer->from,
					  JTAG_STATE_SHIFTIR);
	else
		aspeed_jtag_set_tap_state(aspeed_jtag, xfer->from,
					  JTAG_STATE_SHIFTDR);

	tdi = ASPEED_JTAG_GET_TDI(xfer->direction, data[index]);
	data[index] = 0;
	while (remain_xfer > 1) {
		tdo = aspeed_jtag_tck_cycle(aspeed_jtag, 0,
					    tdi & ASPEED_JTAG_DATA_MSB);
		data[index] |= tdo << (shift_bits %
					    ASPEED_JTAG_DATA_CHUNK_SIZE);
		tdi >>= 1;
		shift_bits++;
		remain_xfer--;

		if (shift_bits % ASPEED_JTAG_DATA_CHUNK_SIZE == 0) {
			tdo = 0;
			index++;
			tdi = ASPEED_JTAG_GET_TDI(xfer->direction, data[index]);
			data[index] = 0;
		}
	}

	if ((xfer->endstate == (xfer->type == JTAG_SIR_XFER ?
				JTAG_STATE_SHIFTIR : JTAG_STATE_SHIFTDR))) {
		/* Stay in Shift IR/DR*/
		tdo = aspeed_jtag_tck_cycle(aspeed_jtag, 0,
					    tdi & ASPEED_JTAG_DATA_MSB);
		data[index] |= tdo << (shift_bits %
					ASPEED_JTAG_DATA_CHUNK_SIZE);
	} else  {
		/* Goto end state */
		tdo = aspeed_jtag_tck_cycle(aspeed_jtag, 1,
					    tdi & ASPEED_JTAG_DATA_MSB);
		data[index] |= tdo << (shift_bits %
				       ASPEED_JTAG_DATA_CHUNK_SIZE);
		aspeed_jtag->status = (xfer->type == JTAG_SIR_XFER) ?
				       JTAG_STATE_EXIT1IR : JTAG_STATE_EXIT1DR;
		aspeed_jtag_set_tap_state(aspeed_jtag, aspeed_jtag->status,
					  xfer->endstate);
	}
}

static int aspeed_jtag_xfer_push_data(struct aspeed_jtag *aspeed_jtag,
				      enum jtag_xfer_type type, u32 bits_len)
{
	int res = 0;

	if (type == JTAG_SIR_XFER) {
		aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_IOUT_LEN(bits_len),
				  ASPEED_JTAG_CTRL);
		aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_IOUT_LEN(bits_len) |
				  ASPEED_JTAG_CTL_INST_EN, ASPEED_JTAG_CTRL);
		res = aspeed_jtag_wait_instruction_pause(aspeed_jtag);
	} else {
		aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_DOUT_LEN(bits_len),
				  ASPEED_JTAG_CTRL);
		aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_DOUT_LEN(bits_len) |
				  ASPEED_JTAG_CTL_DATA_EN, ASPEED_JTAG_CTRL);
		res = aspeed_jtag_wait_data_pause_complete(aspeed_jtag);
	}
	return res;
}

static int aspeed_jtag_xfer_push_data_last(struct aspeed_jtag *aspeed_jtag,
					   enum jtag_xfer_type type,
					   u32 shift_bits)
{
	int res = 0;

	if (type == JTAG_SIR_XFER) {
		aspeed_jtag_write(aspeed_jtag,
				  ASPEED_JTAG_IOUT_LEN(shift_bits) |
				  ASPEED_JTAG_CTL_LASPEED_INST,
				  ASPEED_JTAG_CTRL);
		aspeed_jtag_write(aspeed_jtag,
				  ASPEED_JTAG_IOUT_LEN(shift_bits) |
				  ASPEED_JTAG_CTL_LASPEED_INST |
				  ASPEED_JTAG_CTL_INST_EN,
				  ASPEED_JTAG_CTRL);
		res = aspeed_jtag_wait_instruction_complete(aspeed_jtag);
	} else {
		aspeed_jtag_write(aspeed_jtag,
				  ASPEED_JTAG_DOUT_LEN(shift_bits) |
				  ASPEED_JTAG_CTL_LASPEED_DATA,
				  ASPEED_JTAG_CTRL);
		aspeed_jtag_write(aspeed_jtag,
				  ASPEED_JTAG_DOUT_LEN(shift_bits) |
				  ASPEED_JTAG_CTL_LASPEED_DATA |
				  ASPEED_JTAG_CTL_DATA_EN,
				  ASPEED_JTAG_CTRL);
		res = aspeed_jtag_wait_data_complete(aspeed_jtag);
	}
	return res;
}

static int aspeed_jtag_xfer_hw(struct aspeed_jtag *aspeed_jtag,
			       struct jtag_xfer *xfer, u32 *data)
{
	unsigned long remain_xfer = xfer->length;
	unsigned long index = 0;
	char shift_bits;
	u32 data_reg;
	u32 scan_end;

#ifdef DEBUG_JTAG
	dev_dbg(aspeed_jtag->dev, "HW JTAG SHIFT %s, length = %d\n",
		(xfer->type == JTAG_SIR_XFER) ? "IR" : "DR", xfer->length);
#endif
	data_reg = xfer->type == JTAG_SIR_XFER ?
		   ASPEED_JTAG_INST : ASPEED_JTAG_DATA;
	if (xfer->endstate == JTAG_STATE_SHIFTIR ||
	    xfer->endstate == JTAG_STATE_SHIFTDR ||
	    xfer->endstate == JTAG_STATE_PAUSEIR ||
	    xfer->endstate == JTAG_STATE_PAUSEDR) {
		scan_end = 0;
	} else {
		scan_end = 1;
	}

	while (remain_xfer) {
		if (xfer->direction & JTAG_WRITE_XFER)
			aspeed_jtag_write(aspeed_jtag, data[index], data_reg);
		else
			aspeed_jtag_write(aspeed_jtag, 0, data_reg);

		if (remain_xfer > ASPEED_JTAG_DATA_CHUNK_SIZE) {
#ifdef DEBUG_JTAG
			dev_dbg(aspeed_jtag->dev,
				"Chunk len=%d chunk_size=%d remain_xfer=%lu\n",
				xfer->length, ASPEED_JTAG_DATA_CHUNK_SIZE,
				remain_xfer);
#endif
			shift_bits = ASPEED_JTAG_DATA_CHUNK_SIZE;

			/*
			 * Transmit bytes that were not equals to column length
			 * and after the transfer go to Pause IR/DR.
			 */
			if (aspeed_jtag_xfer_push_data(aspeed_jtag, xfer->type,
						       shift_bits) != 0) {
				return -EFAULT;
			}
		} else {
			/*
			 * Read bytes equals to column length
			 */
			shift_bits = remain_xfer;
			if (scan_end) {
				/*
				 * If this data is the end of the transmission
				 * send remaining bits and go to endstate
				 */
#ifdef DEBUG_JTAG
				dev_dbg(aspeed_jtag->dev,
				"Last len=%d chunk_size=%d remain_xfer=%lu\n",
					xfer->length,
					ASPEED_JTAG_DATA_CHUNK_SIZE,
					remain_xfer);
#endif
				if (aspeed_jtag_xfer_push_data_last(
							aspeed_jtag,
							xfer->type,
							shift_bits) != 0) {
					return -EFAULT;
				}
			} else {
				/*
				 * If transmission is waiting for additional
				 * data send remaining bits and then go to
				 * Pause IR/DR.
				 */
#ifdef DEBUG_JTAG
				dev_dbg(aspeed_jtag->dev,
				"Tail len=%d chunk_size=%d remain_xfer=%lu\n",
					xfer->length,
					ASPEED_JTAG_DATA_CHUNK_SIZE,
					remain_xfer);
#endif
				if (aspeed_jtag_xfer_push_data(aspeed_jtag,
							       xfer->type,
							       shift_bits)
							       != 0) {
					return -EFAULT;
				}
			}
		}

		if (xfer->direction & JTAG_READ_XFER) {
			if (shift_bits < ASPEED_JTAG_DATA_CHUNK_SIZE) {
				data[index] = aspeed_jtag_read(aspeed_jtag,
							       data_reg);

				data[index] >>= ASPEED_JTAG_DATA_CHUNK_SIZE -
								shift_bits;
			} else {
				data[index] = aspeed_jtag_read(aspeed_jtag,
							       data_reg);
			}
		}

		remain_xfer = remain_xfer - shift_bits;
		index++;
	}
	return 0;
}

static int aspeed_jtag_xfer(struct jtag *jtag, struct jtag_xfer *xfer,
			    u8 *xfer_data)
{
	struct aspeed_jtag *aspeed_jtag = jtag_priv(jtag);

	if (!(aspeed_jtag->mode & JTAG_XFER_HW_MODE)) {
		/* SW mode */
		aspeed_jtag_write(aspeed_jtag, ASPEED_JTAG_SW_TDIO,
				  ASPEED_JTAG_SW);

		aspeed_jtag_xfer_sw(aspeed_jtag, xfer, (u32 *)xfer_data);
	} else {
		/* HW mode */
		aspeed_jtag_write(aspeed_jtag, 0, ASPEED_JTAG_SW);
		if (aspeed_jtag_xfer_hw(aspeed_jtag, xfer,
					(u32 *)xfer_data) != 0)
			return -EFAULT;
	}

	aspeed_jtag->status = xfer->endstate;
	return 0;
}

static int aspeed_jtag_status_get(struct jtag *jtag, u32 *status)
{
	struct aspeed_jtag *aspeed_jtag = jtag_priv(jtag);

	*status = aspeed_jtag->status;
	return 0;
}

#ifdef USE_INTERRUPTS
static irqreturn_t aspeed_jtag_interrupt(s32 this_irq, void *dev_id)
{
	struct aspeed_jtag *aspeed_jtag = dev_id;
	irqreturn_t ret = IRQ_HANDLED;
	u32 status;

	status = aspeed_jtag_read(aspeed_jtag, ASPEED_JTAG_ISR);

	if (status & ASPEED_JTAG_ISR_INT_MASK) {
		aspeed_jtag_write(aspeed_jtag,
				  (status & ASPEED_JTAG_ISR_INT_MASK)
				  | (status & ASPEED_JTAG_ISR_INT_EN_MASK),
				  ASPEED_JTAG_ISR);
		aspeed_jtag->flag |= status & ASPEED_JTAG_ISR_INT_MASK;
	}

	if (aspeed_jtag->flag) {
		wake_up_interruptible(&aspeed_jtag->jtag_wq);
		ret = IRQ_HANDLED;
	} else {
		dev_err(aspeed_jtag->dev, "irq status:%x\n",
			status);
		ret = IRQ_NONE;
	}
	return ret;
}
#endif

static int aspeed_jtag_enable(struct jtag *jtag)
{
	struct aspeed_jtag *aspeed_jtag = jtag_priv(jtag);

	aspeed_jtag_master(aspeed_jtag);
	return 0;
}

static int aspeed_jtag_disable(struct jtag *jtag)
{
	struct aspeed_jtag *aspeed_jtag = jtag_priv(jtag);

	aspeed_jtag_output_disable(aspeed_jtag);
	return 0;
}

static int aspeed_jtag_init(struct platform_device *pdev,
			    struct aspeed_jtag *aspeed_jtag)
{
	struct resource *res;
#ifdef USE_INTERRUPTS
	int err;
#endif

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	aspeed_jtag->reg_base = devm_ioremap_resource(aspeed_jtag->dev, res);
	if (IS_ERR(aspeed_jtag->reg_base))
		return -ENOMEM;

	aspeed_jtag->pclk = devm_clk_get(aspeed_jtag->dev, NULL);
	if (IS_ERR(aspeed_jtag->pclk)) {
		dev_err(aspeed_jtag->dev, "devm_clk_get failed\n");
		return PTR_ERR(aspeed_jtag->pclk);
	}

#ifdef USE_INTERRUPTS
	aspeed_jtag->irq = platform_get_irq(pdev, 0);
	if (aspeed_jtag->irq < 0) {
		dev_err(aspeed_jtag->dev, "no irq specified\n");
		return -ENOENT;
	}
#endif

	if (clk_prepare_enable(aspeed_jtag->pclk)) {
		dev_err(aspeed_jtag->dev, "no irq specified\n");
		return -ENOENT;
	}

	aspeed_jtag->rst = devm_reset_control_get_shared(&pdev->dev, NULL);
	if (IS_ERR(aspeed_jtag->rst)) {
		dev_err(aspeed_jtag->dev,
			"missing or invalid reset controller device tree entry");
		return PTR_ERR(aspeed_jtag->rst);
	}
	reset_control_deassert(aspeed_jtag->rst);

#ifdef USE_INTERRUPTS
	err = devm_request_irq(aspeed_jtag->dev, aspeed_jtag->irq,
			       aspeed_jtag_interrupt, 0,
			       "aspeed-jtag", aspeed_jtag);
	if (err) {
		dev_err(aspeed_jtag->dev, "unable to get IRQ");
		clk_disable_unprepare(aspeed_jtag->pclk);
		return err;
	}
#endif

	aspeed_jtag_output_disable(aspeed_jtag);

	aspeed_jtag->flag = 0;
	aspeed_jtag->mode = 0;
	init_waitqueue_head(&aspeed_jtag->jtag_wq);
	return 0;
}

static int aspeed_jtag_deinit(struct platform_device *pdev,
			      struct aspeed_jtag *aspeed_jtag)
{
	aspeed_jtag_write(aspeed_jtag, 0, ASPEED_JTAG_ISR);
	/* Disable clock */
	aspeed_jtag_write(aspeed_jtag, 0, ASPEED_JTAG_CTRL);
	reset_control_assert(aspeed_jtag->rst);
	clk_disable_unprepare(aspeed_jtag->pclk);
	return 0;
}

static const struct jtag_ops aspeed_jtag_ops = {
	.freq_get = aspeed_jtag_freq_get,
	.freq_set = aspeed_jtag_freq_set,
	.status_get = aspeed_jtag_status_get,
	.status_set = aspeed_jtag_status_set,
	.xfer = aspeed_jtag_xfer,
	.mode_set = aspeed_jtag_mode_set,
	.bitbang = aspeed_jtag_bitbang,
	.enable = aspeed_jtag_enable,
	.disable = aspeed_jtag_disable
};

static int aspeed_jtag_probe(struct platform_device *pdev)
{
	struct aspeed_jtag *aspeed_jtag;
	struct jtag *jtag;
	int err;

	jtag = jtag_alloc(&pdev->dev, sizeof(*aspeed_jtag), &aspeed_jtag_ops);
	if (!jtag)
		return -ENOMEM;

	platform_set_drvdata(pdev, jtag);
	aspeed_jtag = jtag_priv(jtag);
	aspeed_jtag->dev = &pdev->dev;

	/* Initialize device*/
	err = aspeed_jtag_init(pdev, aspeed_jtag);
	if (err)
		goto err_jtag_init;

	/* Initialize JTAG core structure*/
	err = devm_jtag_register(aspeed_jtag->dev, jtag);
	if (err)
		goto err_jtag_register;

	return 0;

err_jtag_register:
	aspeed_jtag_deinit(pdev, aspeed_jtag);
err_jtag_init:
	jtag_free(jtag);
	return err;
}

static int aspeed_jtag_remove(struct platform_device *pdev)
{
	struct jtag *jtag = platform_get_drvdata(pdev);

	aspeed_jtag_deinit(pdev, jtag_priv(jtag));
	return 0;
}

static const struct of_device_id aspeed_jtag_of_match[] = {
	{ .compatible = "aspeed,ast2400-jtag", },
	{ .compatible = "aspeed,ast2500-jtag", },
	{}
};

static struct platform_driver aspeed_jtag_driver = {
	.probe = aspeed_jtag_probe,
	.remove = aspeed_jtag_remove,
	.driver = {
		.name = ASPEED_JTAG_NAME,
//		.owner  = THIS_MODULE,
		.of_match_table = aspeed_jtag_of_match,
	},
};
module_platform_driver(aspeed_jtag_driver);

MODULE_AUTHOR("Oleksandr Shamray <oleksandrs@mellanox.com>");
MODULE_DESCRIPTION("ASPEED JTAG driver");
MODULE_LICENSE("GPL v2");
