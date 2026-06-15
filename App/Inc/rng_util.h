/**
 ******************************************************************************
 * @file           : rng_util.h
 * @brief          : Header file for Hardware Random Number Generator (RNG) utility
 * functions. This module defines the interface for generating hardware-based
 * random numbers used across various display and scene modules on the
 * STM32F4 platform.
 ******************************************************************************
 */

#ifndef __RNG_UTIL_H
#define __RNG_UTIL_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
// Xorshift RNG Constants
#define XORSHIFT_S1 13 // Shift constant 1
#define XORSHIFT_S2 17 // Shift constant 2
#define XORSHIFT_S3 5  // Shift constant 3

/* Function prototypes -------------------------------------------------------*/
uint8_t GetRandomNumber(uint32_t *value);
void XorshiftRandomNumber(uint32_t *rand_num);

#endif /* __RNG_UTIL_H */