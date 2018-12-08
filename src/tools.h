/* SPDX-License-Identifier: GPL-2.0 */
#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>

#define BIT(n)		(1 << (n))
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

enum str2int_err {
	STR2INT_SUCCESS		= 0,
	STR2INT_OVERFLOW	= -1,
	STR2INT_UNDERFLOW	= -2,
	STR2INT_INCONVERTIBLE	= -3
};

uint16_t set_bit(uint16_t var, unsigned int offset, unsigned int value);
void msleep(int msec);
int str2int(int *out, char *s, int base);

#endif /* TOOLS_H */
