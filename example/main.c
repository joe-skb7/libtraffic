#include <trl.h>

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int res;
	int ret = EXIT_SUCCESS;

	res = trl_init();
	if (res < 0)
		return EXIT_FAILURE;

	res = trl_set_burst(0xffff);
	if (res < 0) {
		ret = EXIT_FAILURE;
		goto err;
	}

	res = trl_set_one(0, TRL_STATE_GREEN);
	if (res < 0) {
		ret = EXIT_FAILURE;
		goto err;
	}

	trl_exit();

	printf("Success!\n");

err:
	return ret;
}
