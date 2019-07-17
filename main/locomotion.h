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

#define MOTION_STOP    0
#define MOTION_FORWARD 1
#define MOTION_LEFT    2
#define MOTION_RIGHT   3
#define MOTION_BACK    4

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

/**
 * @brief Set locomotion mode to turning, including turn left/right/back
 *
 * @param 
 *
 * @return 
 *
 * @note Read the request from global variable motion_mode
 * */
void turning(void);