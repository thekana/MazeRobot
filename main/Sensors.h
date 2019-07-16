#pragma once
#include "external_VL6180X.h"
#include "external_I2CIO.h"
#include "Wire.h"
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "external_MPU6050_I2Cdev.h"
#include "hardware.h"
#include "hardware_definition.h"

#include "external_MPU6050_6Axis_MotionApps20.h"
//using namespace hardware;

#include "external_VL6180X.h"
#include "external_I2CIO.h"
#include "Wire.h"
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "external_MPU6050_I2Cdev.h"
#include "hardware.h"
#include "hardware_definition.h"

#include "external_MPU6050_6Axis_MotionApps20.h"
using namespace hardware;

void sensorSetup();
int orientation(int lfr[]);
void carWall(double distance_l, double distance_f, double distance_r, int *lfr);
void ESWNWall(double Yaw, int *lfr, int *ESWN, int *face_dir);  
