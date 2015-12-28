/*
 * navigation.c
 *
 *  Created on: Oct 24, 2015
 *      Author: Eloi Benvenuti
 *
 *  File description: Module for computing the path from position to target and sending command to motors.
 *  distances are in meters and angles in radian.
 */

#include "../../Board.h"
#include "gps.h"
#include "imu.h"
#include "navigation.h"
#include "hal/motors.h" //also contains all sorts of geometries (wheel radius etc)
#include <math.h>

#define MATH_PI 3.14159265358979323846
#define EARTH_RADIUS 6356752

//Target for testing, just outside of hackuarium
#define TARGET_LAT 	46.532476
#define TARGET_LON 	6.590315
#define INIT_LAT		46.5252206
#define INIT_LON 	6.6296219

//Other, hopefully closer
#define ESP_LAT 46.520216
#define ESP_LON 6.565783
#define HACK_LAT 46.532483
#define HACK_LON 6.590315
#define EL_LAT 46.519832
#define EL_LON 6.564925
#define ROLEX_LAT 46.518996
#define ROLEX_LON 6.568278
#define PLASMA_LAT 46.517241
#define PLASMA_LON 6.565021



typedef struct _navigation_status{
	float lat_rover;
	float lon_rover;
	float heading_rover;
	float lat_target;
	float lon_target;
	float distance_to_target;
	float angle_to_target;
	enum _current_state{
		STOP=0,
		GO_TO_TARGET,
		AVOID_OBSTACLE,
	} current_state;
} navigation_status_t;

static navigation_status_t navigation_status;

static target_list_t navigation_targets;


float getDistanceToTarget(float lat_current, float lon_current, float lat_target, float lon_target){

	/*About this code: here we use the haversin formula
	 * As I undersood, we may run into problems if our robot goes past the 180/-180 degree line
	 * As I get it, we have a 0.5% error with this formula, even with us being at the pole
	 * However it would be nice to double check that
	 * 0.5% error means that, on a 5m distance, we are 25cm imprecise.
	 * Since the gps is probably 5m off, it's not a big deal.
	 */
	float distance = 0;

	distance = 2*EARTH_RADIUS*asinf(sqrtf(powf(sinf((lat_target-lat_current)/2),2) + cosf(lat_current)*cosf(lat_target)*powf(sinf((lon_target-lon_current)/2),2)));

	return distance;
}


/**
 * Params: lat1, long1 => Latitude and Longitude of current point
 *         lat2, long2 => Latitude and Longitude of target  point
 *
 *         headX       => x-Value of built-in compass
 *
 * Returns the degree of a direction from current point to target point (between -180, 180).
 * Note: a negative result means the target is towards the left of the rover. positive to the right
 *
 */
float getAngleToTarget(float lat1, float lon1, float lat2, float lon2, float headX) {
    float dLon = MATH_PI/180*(lon2-lon1);

    lat1 = MATH_PI/180*(lat1);
    lat2 = MATH_PI/180*(lat2);

    float y = sinf(dLon) * cosf(lat2);
    float x = cosf(lat1) * sinf(lat2) - sinf(lat1)*cosf(lat2)*cosf(dLon);
    float brng = 180/MATH_PI*(atan2f(y, x));

    brng = brng - headX;

    // bound result between -180 and 180
    if(brng < -180.0)
    		brng = 360 + brng;

    return brng;
}

/* fetch the next target to move to from the queue */
void navigation_update_target()
{
	if(navigation_targets.state[navigation_targets.current_index] == DONE)
	{
		navigation_targets.current_index = (navigation_targets.current_index + 1) % N_TARGETS_MAX;
	}

	if(navigation_targets.state[navigation_targets.current_index] == VALID)
	{
		navigation_status.lon_target = navigation_targets.lon[navigation_targets.current_index];
		navigation_status.lat_target = navigation_targets.lat[navigation_targets.current_index];
		navigation_targets.state[navigation_targets.current_index] = CURRENT;
	}
	else if(navigation_targets.state[navigation_targets.current_index] == INVALID &&
			navigation_targets.current_index != navigation_targets.last_index)
	{//search queue for valid goals, until whole buffer is cycled
		navigation_targets.current_index = (navigation_targets.current_index + 1) % N_TARGETS_MAX;
	}
}

void navigation_update_position()
{
	if(gps_update_new_position(&(navigation_status.lat_rover), &(navigation_status.lon_rover)))
	{
		//we get a new gps position
	}
	else
	{
		//we didn't get a new gps position --> update position using odometry.
		//TODO
		motors_wheels_update_distance();
	}

	// recalculate heading angle
	navigation_status.heading_rover = imu_get_fheading();
	navigation_status.angle_to_target = getAngleToTarget(navigation_status.lat_rover,navigation_status.lon_rover,
			navigation_status.lat_target, navigation_status.lon_target, navigation_status.heading_rover);
	// recalculate distance to target
	navigation_status.distance_to_target = getDistanceToTarget(navigation_status.lat_rover,navigation_status.lon_rover,
			navigation_status.lat_target, navigation_status.lon_target);
}


void navigation_update_state()
{
	if(navigation_status.current_state == AVOID_OBSTACLE)
	{
		//TODO: check if obstacle is gone.
	}
	else
	{
		if(navigation_targets.state[navigation_targets.current_index] == CURRENT)
		{ //TODO: add conditions that may stop from changing state, for example low battery.
			navigation_status.current_state = GO_TO_TARGET;
		}
		else if(navigation_status.current_state == GO_TO_TARGET)
		{
			if(navigation_status.distance_to_target < TARGET_REACHED_DISTANCE)
			{
				navigation_targets.state[navigation_targets.current_index] = DONE;
				navigation_status.current_state = STOP;
			}
		}
	}
}


void navigation_move()
{
	static uint16_t lspeed, rspeed;
	if(navigation_status.current_state == GO_TO_TARGET)
	{
		if(navigation_status.angle_to_target < -90) //turn on spot to the left
		{
			lspeed = -PWM_SPEED_80;
			rspeed = PWM_SPEED_80;
		}
		else if(navigation_status.angle_to_target > 90) //turn on spot to the right
		{
			lspeed = PWM_SPEED_80;
			rspeed = -PWM_SPEED_80;
		}
		else if(navigation_status.angle_to_target < -5) //go to the left
		{
			lspeed = PWM_SPEED_60;
			rspeed = PWM_SPEED_100;
		}
		else if(navigation_status.angle_to_target > 5) //go to the right
		{
			lspeed = PWM_SPEED_100;
			rspeed = PWM_SPEED_60;
		}
		else //go straight
		{
			lspeed = PWM_SPEED_100;
			rspeed = PWM_SPEED_100;
		}

		motors_wheels_move(lspeed, rspeed, lspeed, rspeed);
	}
	else if(navigation_status.current_state == STOP)
	{
		motors_wheels_stop();
	}
	else if(navigation_status.current_state == AVOID_OBSTACLE)
	{
		// TODO: obstacle avoidance
	}
}




void navigation_init()
{
	motors_init();
	navigation_status.lat_rover = INIT_LAT;
	navigation_status.lon_rover = INIT_LON;
	navigation_status.heading_rover = 0.0;
	navigation_status.lat_target = TARGET_LAT;
	navigation_status.lon_target = TARGET_LON;
	navigation_status.distance_to_target = 0.0;
	navigation_status.angle_to_target = 0.0;
	navigation_status.current_state = STOP;
}


void navigation_task(){

//	float toPlasma=0;
//	float toEL=0;
//	float toRolex=0;
//	float toHackuarium=0;
//	float toEsplanade=0;

	navigation_init();
	Task_sleep(1000);

	while(1){

		navigation_update_target();
		navigation_update_position();
		navigation_update_state();
		navigation_move();

//		GPIO_write(Board_LED_RED, Board_LED_OFF);
//		struct navigation_status pos_var;
//
//		//Commented for debug reason because oterwise we never go into the else
//		//We check if the gps is already working
//		if (gps_get_validity() == 0){
//			//error
//		}
//		else{
//			pos_var.lat_current = gps_get_lat();
//			pos_var.lon_current = gps_get_lon();
//
//			toPlasma= nav_get_distance(pos_var.lat_current, pos_var.lon_current, PLASMA_LAT, PLASMA_LON);
//			toEL= nav_get_distance(pos_var.lat_current, pos_var.lon_current, EL_LAT, EL_LON);
//			toRolex= nav_get_distance(pos_var.lat_current, pos_var.lon_current, ROLEX_LAT, ROLEX_LON);
//			toHackuarium= nav_get_distance(pos_var.lat_current, pos_var.lon_current, HACK_LAT, HACK_LON);
//			toEsplanade= nav_get_distance(pos_var.lat_current, pos_var.lon_current, ESP_LAT, ESP_LON);
//
//			//We check if we are at the location
//			if(toEsplanade <= 5){
//				System_printf("Esplanade reached\n");
//				    /* SysMin will only print to the console when you call flush or exit */
//				System_flush();
//			}
//			else if (toHackuarium <= 5){
//				System_printf("Hackuarium reached\n");
//								    /* SysMin will only print to the console when you call flush or exit */
//				System_flush();
//			}
//			else if (toEL <= 5){
//				System_printf("EL reached\n");
//								    /* SysMin will only print to the console when you call flush or exit */
//				System_flush();
//			}
//			else if (toRolex <= 5){
//				System_printf("RLC reached\n");
//								    /* SysMin will only print to the console when you call flush or exit */
//				System_flush();
//			}
//			else if (toPlasma <= 5){
//				System_printf("Plasma center reached\n");
//								    /* SysMin will only print to the console when you call flush or exit */
//				System_flush();
//			}
//			else{
//				System_printf("To EL = %f\n To Esplanade = %f\n To Rolex = %f\n To Plasma = %f\n To Hackuarium = %f\n", toEL, toEsplanade, toRolex, toPlasma, toHackuarium);
//				    /* SysMin will only print to the console when you call flush or exit */
//				System_flush();
//			}
//
//			pos_var.angle = nav_get_angle(pos_var.lat_current, pos_var.lon_current, HACK_LAT, HACK_LON);
//
//			//Tell the motor to move accordingly
//		}
		Task_sleep(1500);

	}
}

