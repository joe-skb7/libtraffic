// SPDX-License-Identifier: GPL-2.0
#include <trl.h>
#include <tools.h>

#include <libftdi1/ftdi.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIB_NAME	"libtrl"

/* USB "Vendor ID" and "Product ID" for FTDI chip */
#define FTDI_VID	0x0403
#define FTDI_PID	0x6001

#define PIN_TXD		0x01  /* Orange (on FTDI cable) */
#define PIN_RXD		0x02  /* Yellow (on FTDI cable) */
#define PIN_RTS		0x04  /* Green  (on FTDI cable) */
#define PIN_CTS		0x08  /* Brown  (on FTDI cable) */
#define PIN_DTR		0x10
#define PIN_DSR		0x20
#define PIN_DCD		0x40
#define PIN_RI		0x80

#define GPIO_DATA	PIN_TXD
#define GPIO_CLOCK	PIN_CTS
#define GPIO_LATCH	PIN_DTR

/* Number of shift registers */
#define REG_NUM		2

#define BIT(n)		(1 << (n))
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

#define TRL_LOG(f, fmt, ...)						\
	do {								\
		fprintf(f, "%s: %s: " fmt,				\
			LIB_NAME, __func__, ## __VA_ARGS__);		\
	} while (0)

struct trl {
	struct ftdi_context *ftdi;	/* libftdi object */
	uint16_t state;			/* current traffic lights states */
};

static struct trl trl;

/* TODO: Init everything with red on start */
int trl_init(void)
{
	int res, ret;

	assert(trl.ftdi == NULL);

	trl.ftdi = ftdi_new();
	if (trl.ftdi == NULL) {
		TRL_LOG(stderr, "Error: Can't allocate memory for FTDI obj\n");
		return -1;
	}

	res = ftdi_usb_open(trl.ftdi, FTDI_VID, FTDI_PID);
	if (res < 0) {
		TRL_LOG(stderr, "Error: Unable to open FTDI device: %d (%s)\n",
				res, ftdi_get_error_string(trl.ftdi));
		ret = -2;
		goto err1;
	}

	res = ftdi_set_bitmode(trl.ftdi, 0xff, BITMODE_BITBANG);
	if (res < 0) {
		TRL_LOG(stderr, "Error: Can't enable bitbang\n");
		ret = -3;
		goto err2;
	}

	return 0;

err2:
	ftdi_usb_close(trl.ftdi);
err1:
	ftdi_free(trl.ftdi);
	return ret;
}

/* TODO: Implement this one */
int trl_set_one(int num, int state)
{
	assert(trl.ftdi != NULL);

	/* XXX: Remove */
	TRL_LOG(stdout, "stub\n");
	(void)num;	/* Unused */
	(void)state;	/* Unused */
	return 0;
}

/* TODO: Implement this one */
int trl_set_burst(uint16_t mask)
{
	/*
	 * Buffer for output on FTDI. Each byte represents lines to set
	 * (each bit in byte is one line, like PIN_TXD). We issue all signals
	 * in one USB write.
	 *
	 * 8 clock pulses (LOW, HIGH) and latch pulse (for 74HC595 chips).
	 */
	unsigned char buf[2*8*REG_NUM + 1] = { 0 };
	/* Buffer iterator */
	unsigned char *b = buf;
	int res;
	int i;

	assert(trl.ftdi != NULL);

	/* Set all GPIO lines to zero */
	buf[0] = 0;
	res = ftdi_write_data(trl.ftdi, buf, 1);
	if (res < 0) {
		TRL_LOG(stderr, "Error: Write #1 failed: %s\n",
			ftdi_get_error_string(trl.ftdi));
		return -1;
	}

	b = buf;


	/* Count down, send data in reverse order */
	for (i = TRL_COUNT - 1; i >= 0; i--) {
		unsigned char state; /* lines state for current byte */

		state = (mask & BIT(i)) ? GPIO_DATA : 0;
		*b++ = state;
		state |= GPIO_CLOCK;
		*b++ = state;
	}
	*b++ = GPIO_LATCH;

	res = ftdi_write_data(trl.ftdi, buf, ARRAY_SIZE(buf));
	if (res < 0) {
		TRL_LOG(stderr, "Error: Write #2 failed: %s\n",
			ftdi_get_error_string(trl.ftdi));
		return -1;
	}

	return 0;
}

void trl_exit(void)
{
	assert(trl.ftdi != NULL);

	ftdi_disable_bitbang(trl.ftdi);
	ftdi_usb_close(trl.ftdi);
	ftdi_free(trl.ftdi);

	memset(&trl, 0, sizeof(struct trl));
}
