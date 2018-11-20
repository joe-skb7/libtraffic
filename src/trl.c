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

#define GPIO0		PIN_CTS
#define GPIO1		PIN_TXD
#define GPIO2		PIN_RXD
/* On SparkFun breakout board we have DTR in place of RTS, so let's use both */
#define GPIO3		(PIN_RTS | PIN_DTR)

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
	int res;
	int i;
	unsigned char data = 0;

	assert(trl.ftdi != NULL);

	(void)mask; /* Unused */

	for (i = 0; i < 5; ++i) {
		int j;

		for (j = 0; j < 4; ++j) {
			switch (j) {
			case 0:
				data = GPIO0;
				break;
			case 1:
				data = GPIO1;
				break;
			case 2:
				data = GPIO2;
				break;
			case 3:
				data = GPIO3;
				break;
			}

			res = ftdi_write_data(trl.ftdi, &data, 1);
			if (res < 0) {
				TRL_LOG(stderr,
					"Error: Write failed for 0x%x: %s\n",
					data, ftdi_get_error_string(trl.ftdi));
				return -1;
			}

			msleep(125);
		}
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
