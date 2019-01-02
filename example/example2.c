// SPDX-License-Identifier: GPL-2.0
/*
 * Test app for libtrl (Traffic Light Library).
 */

#include <tools.h>

#include <trl.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define EXIT_SIGNAL	128	/* terminated by signal */

static void sig_handler(int signum)
{
	if (signum == SIGINT) {
		trl_set_burst(0x0000); /* set all to red */
		trl_exit();
		exit(EXIT_SIGNAL + signum);
	}
}

int main(void)
{
	int res;

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		fprintf(stderr, "Warning: Can't catch SIGINT\n");

	/* Power-on libtrl */
	res = trl_init();
	if (res < 0)
		return EXIT_FAILURE;

	/* Sanity test traffic lights */
	for (;;) {
		int j;

		/* Set all traffic lights to "green" state */
		res = trl_set_burst(0xffff);
		if (res < 0)
			goto err;
		msleep(500);


		/* Set all traffic lights to "red" state */
		res = trl_set_burst(0x0000);
		if (res < 0)
			goto err;
		msleep(500);

		/* Set all traffic lights to pattern #1 */
		res = trl_set_burst(0xaaaa);
		if (res < 0)
			goto err;
		msleep(500);

		/* Set all traffic lights to pattern #2 */
		res = trl_set_burst(0x5555);
		if (res < 0)
			goto err;
		msleep(500);

		/* Set all traffic lights to "red" state */
		res = trl_set_burst(0x0000);
		if (res < 0)
			goto err;

		for (j = 0; j < 10; j++) {
			res = trl_set_one(j, TRL_STATE_GREEN);
			if (res < 0)
				goto err;
			msleep(100);
		}

		for (j = 9; j >= 0; j--) {
			res = trl_set_one(j, TRL_STATE_RED);
			if (res < 0)
				goto err;
			msleep(100);
		}
	}

	/* Does not return; exit happens in Ctrl-C handler */

err:
	fprintf(stderr, "Failure!\n");
	return EXIT_FAILURE;
}
