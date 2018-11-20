// SPDX-License-Identifier: GPL-2.0
#include <trl.h>
#include <tools.h>

#include <libftdi1/ftdi.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIB_NAME	"libtrl"

#define PIN_TX		0x01  /* Orange wire on FTDI cable */
#define PIX_RX		0x02  /* Yellow */
#define PIN_RTS		0x04  /* Green */
#define PIN_CTS		0x08  /* Brown */
#define PIN_DTR		0x10
#define PIN_DSR		0x20
#define PIN_DCD		0x40
#define PIN_RI		0x80

#define FTDI_VID	0x0403
#define FTDI_PID	0x6001
#define LED		PIN_CTS
#define DELAY		1000	/* msec */

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

	if (trl.ftdi != NULL) {
		TRL_LOG(stderr, "Error: trl_init() was already invoked\n");
		return -1;
	}

	trl.ftdi = ftdi_new();
	if (trl.ftdi == NULL) {
		TRL_LOG(stderr, "Error: Can't allocate memory for FTDI obj\n");
		return -2;
	}

	res = ftdi_usb_open(trl.ftdi, FTDI_VID, FTDI_PID);
	if (res < 0) {
		TRL_LOG(stderr, "Error: Unable to open FTDI device: %d (%s)\n",
				res, ftdi_get_error_string(trl.ftdi));
		ret = -3;
		goto err1;
	}

	res = ftdi_set_bitmode(trl.ftdi, 0xff, BITMODE_BITBANG);
	if (res < 0) {
		TRL_LOG(stderr, "Error: Can't enable bitbang\n");
		ret = -4;
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
	/* XXX: Remove */
	TRL_LOG(stdout, "stub\n");
	(void)num;
	(void)state;
	return 0;
}

/* TODO: Implement this one */
int trl_set_burst(uint16_t mask)
{
	/* XXX: Remove */
	TRL_LOG(stdout, "stub\n");
	(void)mask;
	return 0;
}

void trl_exit(void)
{
	ftdi_disable_bitbang(trl.ftdi);
	ftdi_usb_close(trl.ftdi);
	ftdi_free(trl.ftdi);
	memset(&trl, 0, sizeof(struct trl));
}

#if 0
int main(void)
{
	struct ftdi_context *ftdi;
	int res, ret;
	int i;
	unsigned char data = 0;

	for (i = 0; i < 10; ++i) {
		data ^= LED;
		res = ftdi_write_data(ftdi, &data, 1);
		if (res < 0) {
			TRL_LOG(stderr, "Error: Write failed for 0x%x: %s\n",
					data, ftdi_get_error_string(ftdi));
			ret = EXIT_FAILURE;
			goto err3;
		}
		msleep(DELAY);
	}
}
#endif
