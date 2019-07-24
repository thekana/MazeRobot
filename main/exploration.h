/**
 * @file exploration.h
 * @author Jawei Liu
 * @date 24 July 2019
 * @brief The interface for exploration function
 *
 * @note 
 * */
#pragma once

#include "cppQueue.h" 
#include "hardware.h"

/**
 * @brief Exploration the field to make motion decision
 *
 * @param 
 *
 * @return 
 *
 * @note 
 * */
void exploration(Queue& motion_queue, int* lfr);