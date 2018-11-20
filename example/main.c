/**
 * Tiny app that demonstrates working with libtrl (Traffic Light Library).
 */

#include <trl.h>

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int res;
	int ret = EXIT_SUCCESS;

	/* Power-on libtrl */
	res = trl_init();
	if (res < 0)
		return EXIT_FAILURE;

	/* Set all traffic lights to "green" state */
	res = trl_set_burst(0xffff);
	if (res < 0) {
		ret = EXIT_FAILURE;
		goto err;
	}

	/* Set traffic light #0 to "red" state */
	res = trl_set_one(0, TRL_STATE_RED);
	if (res < 0) {
		ret = EXIT_FAILURE;
		goto err;
	}

	/* Power-off libtrl */
	trl_exit();

err:
	if (ret == EXIT_SUCCESS)
		printf("Success!\n");
	else
		fprintf(stderr, "Failure!\n");

	return ret;
}
