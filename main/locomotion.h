/**
 * @file locomotion.h
 * @author Liangde Li
 * @date 16 July 2019
 * @brief The interface for leds and locomotion system
 *
 * @note 
 * */
#pragma once
#include "hardware.h"

/**
 * @brief Set up all leds
 *
 * @param 
 *
 * @return 
 *
 * @note Two leds setup including red and green
 * */
void leds_setup(void);

/**
 * @brief Set up locomotion system
 *
 * @param 
 *
 * @return 
 *
 * @note Including two motors, two encoders and their four pins
 * */
void locomotion_setup(void);
