/**
 ******************************************************************************
 * @file           : rng_util.h
 * @brief          :
 *
 ******************************************************************************
 */

#ifndef __RNG_UTIL_H
#define __RNG_UTIL_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
// Xorshift RNG Constants
#define XORSHIFT_S1         13          // Shift constant 1
#define XORSHIFT_S2         17          // Shift constant 2
#define XORSHIFT_S3         5           // Shift constant 3

/* Function prototypes -------------------------------------------------------*/
uint32_t GetRandomNumber(void);
void XorshiftRandomNumber(uint32_t *rand_num);

#endif /* __RNG_UTIL_H */