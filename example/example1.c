// SPDX-License-Identifier: GPL-2.0

/*
 * Tiny app to demonstrate libtrl usage (Traffic Light Library).
 */

#include <trl.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int res;

	/* Initialize libtrl */
	res = trl_init();
	if (res < 0)
		goto err;

	/* Set all traffic lights to "green" state */
	res = trl_set_burst(0xffff);
	if (res < 0)
		goto err;

	/* Set traffic light #0 to "red" state */
	res = trl_set_one(0, TRL_STATE_RED);
	if (res < 0)
		goto err;

	/* De-initialize libtrl */
	trl_exit();

	printf("Success!\n");
	return EXIT_SUCCESS;

err:
	fprintf(stderr, "Failure!\n");
	return EXIT_FAILURE;
}
