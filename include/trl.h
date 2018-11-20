/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __TRL_H__
#define __TRL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORT __attribute__((visibility("default")))

/* Traffic lights count */
#define TRL_COUNT	16
#define TRL_STATE_RED	0
#define TRL_STATE_GREEN	1

/**
 * Initialize traffic light library.
 *
 * Call this function before using the rest of API.
 *
 * @return 0 on success or negative value on error.
 */
EXPORT int trl_init(void);

/**
 * Change the state of one specific traffic light.
 *
 * If you run this function from different threads, make sure to provide
 * necessary locking in your code.
 *
 * @param num Traffic light number; can be 0..TRL_COUNT-1
 * @param state Can be either TRL_STATE_RED or TRL_STATE_GREEN
 *
 * @return 0 on success or negative value on error.
 */
EXPORT int trl_set_one(int num, int state);

/**
 * Change the state of all traffic lights in one call.
 *
 * If you run this function from different threads, make sure to provide
 * necessary locking in your code.
 *
 * @param mask Bit mask of traffic lights states; e.g.:
 *             0xffff - all traffic lights are green
 *             0x0000 - all traffic lights are red
 *             0x0001 - only traffic light #0 is green
 *
 * @return 0 on success or negative value on error.
 */
EXPORT int trl_set_burst(uint16_t mask);

/**
 * De-initialize traffic light library.
 *
 * Call this function when traffic light library is not needed anymore.
 * After this call you can't call any other API.
 */
EXPORT void trl_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* __TRL_H__ */
