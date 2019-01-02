// SPDX-License-Identifier: GPL-2.0
#include <trl.h>
#include <tools.h>

#include <libftdi1/ftdi.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIB_NAME	"libtrl"

/* USB "Vendor ID" and "Product ID" numbers for FTDI chip */
#define FTDI_VID	0x0403
#define FTDI_PID	0x6001

#define PIN_TXD		0x01 /* Orange (on FTDI cable) */
#define PIN_RXD		0x02 /* Yellow (on FTDI cable) */
#define PIN_RTS		0x04 /* Green  (on FTDI cable) */
#define PIN_CTS		0x08 /* Brown  (on FTDI cable) */
#define PIN_DTR		0x10
#define PIN_DSR		0x20
#define PIN_DCD		0x40
#define PIN_RI		0x80

#define GPIO_DATA	PIN_TXD
#define GPIO_CLOCK	PIN_CTS
#define GPIO_LATCH	PIN_DTR

/*
 * When FTDI chip is in bitbang mode, the actual speed will be BAUDRATE * 16.
 * So, for BAUDRATE = 9600, we will get 9600 * 16 = 153600 bytes per second,
 * which gives us pulse width of 6.5 usec, which is sufficient for using
 * 74HC595 shift registers (must be >= 20 nsec by datasheet).
 */
#define BAUDRATE	9600

/* Number of shift registers */
#define REG_NUM		2

#define TRL_LOG(f, fmt, ...)					\
	fprintf(f, "%s: %s: " fmt,				\
		LIB_NAME, __func__, ##__VA_ARGS__)		\

struct trl {
	struct ftdi_context *ftdi;	/* libftdi object */
	uint16_t state;			/* current traffic lights states */
};

static struct trl trl;

int trl_init(void)
{
	int res, ret;

	assert(!trl.ftdi);

	trl.ftdi = ftdi_new();
	if (!trl.ftdi) {
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

	res = ftdi_set_baudrate(trl.ftdi, BAUDRATE);
	if (res < 0) {
		TRL_LOG(stderr, "Error: Can't set baudrate\n");
		ret = -4;
		goto err3;
	}

	/* Set all traffic lights to red state by default */
	trl_set_burst(0x0000);

	return 0;

err3:
	ftdi_disable_bitbang(trl.ftdi);
err2:
	ftdi_usb_close(trl.ftdi);
err1:
	ftdi_free(trl.ftdi);
	return ret;
}

int trl_set_one(int num, int state)
{
	uint16_t new_state;

	assert(num >= 0 && num < TRL_COUNT);
	assert(state == TRL_STATE_RED || state == TRL_STATE_GREEN);

	new_state = set_bit(trl.state, num, state);

	/* trl_set_burst() will set trl.state to new value for us */
	return trl_set_burst(new_state);
}

int trl_set_burst(uint16_t mask)
{
	/*
	 * FTDI output buffer:
	 *   - each byte of this array represents all FTDI GPIO lines
	 *   - each bit represents the state of one GPIO line, like PIN_TXD
	 *
	 * We issue all signals in one USB transmission, to achieve maximal
	 * possible transmission rate.
	 *
	 * This buffer contains next pulses:
	 *   - 8 clock+data pulses (1 pulse = 2 bytes, for LOW/HIGH states),
	 *     per one shift register
	 *   - 1 latch pulse (1 byte, only for HIGH state), for all shift
	 *     registers
	 */
	unsigned char buf[2 * 8 * REG_NUM + 1] = { 0 };
	/* Buffer iterator */
	unsigned char *b;
	int res, i;

	assert(trl.ftdi);

	/* Set all GPIO lines to zero */
	buf[0] = 0;
	res = ftdi_write_data(trl.ftdi, buf, 1);
	if (res < 0) {
		TRL_LOG(stderr, "Error: Write #1 failed: %s\n",
			ftdi_get_error_string(trl.ftdi));
		return -1;
	}

	/*
	 * Prepare FTDI buffer for sending signals to shift registers.
	 * We are sending data to shift registers in reverse order, because
	 * shift register is basically a "Last In First Out".
	 */
	b = buf;
	for (i = TRL_COUNT - 1; i >= 0; i--) {
		unsigned char state = 0; /* GPIO lines state for current byte */

		/*
		 * - for "green" state, issue logic "0" on GPIO_DATA line
		 * - for "red" state, issue logic "1" on GPIO_DATA line
		 */
		if ((mask & BIT(i)) == TRL_STATE_GREEN)
			state &= ~GPIO_DATA;
		else
			state |= GPIO_DATA;

		*b++ = state;
		state |= GPIO_CLOCK;
		*b++ = state;
	}
	*b++ = GPIO_LATCH;

	/* Send all signals in one USB transaction, to speed things up */
	res = ftdi_write_data(trl.ftdi, buf, ARRAY_SIZE(buf));
	if (res < 0) {
		TRL_LOG(stderr, "Error: Write #2 failed: %s\n",
			ftdi_get_error_string(trl.ftdi));
		return -1;
	}

	trl.state = mask;

	return 0;
}

void trl_exit(void)
{
	assert(trl.ftdi);

	/*
	 * NOTE: Without this sleep last ftdi_write_data() doesn't finish
	 *       correctly.
	 */
	msleep(100);

	ftdi_disable_bitbang(trl.ftdi);
	ftdi_usb_close(trl.ftdi);
	ftdi_free(trl.ftdi);

	memset(&trl, 0, sizeof(struct trl));
}
