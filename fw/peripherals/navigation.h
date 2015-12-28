/*
 * navigation.h
 *
 *  Created on: Oct 24, 2015
 *      Author: Eloi Benvenuti
 */

#ifndef FW_PERIPHERALS_NAVIGATION_H_
#define FW_PERIPHERALS_NAVIGATION_H_

#define TARGET_REACHED_DISTANCE	5 //meters

#define N_TARGETS_MAX	20
typedef struct _target_list_t{
	float lat[N_TARGETS_MAX];
	float lon[N_TARGETS_MAX];
	enum _target_list_state{
		INVALID = 0,
		VALID,
		DONE,
		CURRENT}
	state[N_TARGETS_MAX];
	// circular buffer index variables
	uint8_t current_index;
	uint8_t last_index;} target_list_t;

//Compute angle and distance from the rover to the target
float nav_get_distance(float lat_current, float lon_current, float lat_target, float lon_target);
float nav_get_angle(float lat_current, float lon_current, float lat_target, float lon_target);

//"Main" task of the file
void navigation_task();

#endif /* FW_PERIPHERALS_NAVIGATION_H_ */
