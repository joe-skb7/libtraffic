// SPDX-License-Identifier: GPL-2.0
#include <trl.h>
#include <tools.h>
#include <libftdi1/ftdi.h>
#include <stdio.h>
#include <stdlib.h>

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

int trl_init(void)
{
	/* TODO: Implement this one */
	printf("libtrl: trl_init() stub\n");
	return 0;
}

int trl_set_one(int num, int state)
{
	/* TODO: Implement this one */
	printf("libtrl: trl_set_one() stub\n");
	(void)num;
	(void)state;
	return 0;
}

int trl_set_burst(uint16_t mask)
{
	/* TODO: Implement this one */
	printf("libtrl: trl_set_burst() stub\n");
	(void)mask;
	return 0;
}

void trl_exit(void)
{
	/* TODO: Implement this one */
	printf("libtrl: trl_exit() stub\n");
}

#if 0
int main(void)
{
	struct ftdi_context *ftdi;
	int res, ret;
	int i;
	unsigned char data = 0;

	ftdi = ftdi_new();
	if (ftdi == NULL) {
		fprintf(stderr, "Error: Can't allocate memory for FTDI\n");
		return EXIT_FAILURE;
	}

	res = ftdi_usb_open(ftdi, FTDI_VID, FTDI_PID);
	if (res < 0) {
		fprintf(stderr, "Error: Unable to open FTDI device: %d (%s)\n",
				res, ftdi_get_error_string(ftdi));
		ret = EXIT_FAILURE;
		goto err1;
	}

	res = ftdi_set_bitmode(ftdi, 0xff, BITMODE_BITBANG);
	if (res < 0) {
		fprintf(stderr, "Error: Can't enable bitbang\n");
		ret = EXIT_FAILURE;
		goto err2;
	}

	for (i = 0; i < 10; ++i) {
		data ^= LED;
		res = ftdi_write_data(ftdi, &data, 1);
		if (res < 0) {
			fprintf(stderr, "Error: Write failed for 0x%x: %s\n",
					data, ftdi_get_error_string(ftdi));
			ret = EXIT_FAILURE;
			goto err3;
		}
		msleep(DELAY);
	}

err3:
	ftdi_disable_bitbang(ftdi);
err2:
	ftdi_usb_close(ftdi);
err1:
	ftdi_free(ftdi);
	return ret;
}
#endif
