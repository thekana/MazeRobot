#include "Sensors.h"
#include "hardware.h"
#include "hardware_definition.h"

using namespace hardware;
using namespace hardware::pins;

void sensorSetup() {
	#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
	sonar<sonar_trigger, sonar_echo>::enable();
	// setup lidar
	left_lidar_enable::config_io_mode(io_mode::output);
	right_lidar_enable::config_io_mode(io_mode::output);

	left_lidar_enable::write(logic_level::low);
	right_lidar_enable::write(logic_level::low);
	lidar<lidar_tag<0>>::enable(); //left lidar
	delay(100);
	lidar<lidar_tag<1>>::enable();  //right lidar
	//setup imu
	if (imu::enable()) {
		Serial.println("IMU success");
    Serial.println("IMU stablizing...");
    imu::stabilize();
    Serial.println("IMU stablize");
	}
	else {
		Serial.println("IMU failed");
	}
}

int orientation(int lfr[]) {
	int dir = 1;  //direction number indicate the should move front(1), turn right(2), turn left(3),turn back(4)
	if (lfr[0] == 1 && lfr[2] == 1) {
		dir = 1;
	}
	if (lfr[0] == 1 && lfr[1] == 1) {
		dir = 2;
	}
	if (lfr[1] == 1 && lfr[2] == 1) {
		dir = 3;
	}
	if (lfr[0] == 1 && lfr[1] == 1 && lfr[2] == 1) {
		dir = 4;
	}
	return dir;
}

void carWall(double distance_l, double distance_f, double distance_r, int *lfr) {
	if (distance_f < 100) {
		lfr[1] = 1;
	}
	else {
		lfr[1] = 0;
	}

	if (distance_l < 100) {
		lfr[0] = 1;
	}
	else {
		lfr[0] = 0;
	}
	//  delay(20);

	if (distance_r < 100) {
		lfr[2] = 1;
	}
	else {
		lfr[2] = 0;
	}
}

void ESWNWall(double Yaw, int *lfr, int *ESWN, int *face_dir) {
	if (Yaw > -22.5 && Yaw < 22.5) {
		//      Serial.println("North");
		*face_dir = 4;
		if (lfr[1] != 0) {
			ESWN[3] = 1;
		}
		else {
			ESWN[3] = 0;
		}
		if (lfr[0] != 0) {
			ESWN[2] = 1;
		}
		else {
			ESWN[2] = 0;
		}
		if (lfr[2] != 0) {
			ESWN[0] = 1;
		}
		else {
			ESWN[0] = 0;
		}
		ESWN[1] = 0;
	}
	else if (Yaw > 67.5 && Yaw < 112.5) {
		//      Serial.println("East");
		*face_dir = 1;
		if (lfr[1] != 0) {
			ESWN[0] = 1;
		}
		else {
			ESWN[0] = 0;
		}
		if (lfr[0] != 0) {
			ESWN[3] = 1;
		}
		else {
			ESWN[3] = 0;
		}
		if (lfr[2] != 0) {
			ESWN[1] = 1;
		}
		else {
			ESWN[1] = 0;
		}
		ESWN[2] = 0;
	}
	else if (Yaw < -67.5 && Yaw > -112.5) {
		//      Serial.println("West");
		*face_dir = 3;
		if (lfr[1] != 0) {
			ESWN[2] = 1;
		}
		else {
			ESWN[2] = 0;
		}
		if (lfr[0] != 0) {
			ESWN[1] = 1;
		}
		else {
			ESWN[1] = 0;
		}
		if (lfr[2] != 0) {
			ESWN[3] = 1;
		}
		else {
			ESWN[3] = 0;
		}
		ESWN[0] = 0;
	}
	else if (Yaw > 157.5 || Yaw < -157.5) {
		//      Serial.println("South");
		*face_dir = 2;
		if (lfr[1] != 0) {
			ESWN[1] = 1;
		}
		else {
			ESWN[1] = 0;
		}
		if (lfr[0] != 0) {
			ESWN[0] = 1;
		}
		else {
			ESWN[0] = 0;
		}
		if (lfr[2] != 0) {
			ESWN[1] = 1;
		}
		else {
			ESWN[1] = 0;
		}
		ESWN[3] = 0;
	}
}
