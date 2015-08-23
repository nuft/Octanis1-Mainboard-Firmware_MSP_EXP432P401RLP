/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.3 at Sun Aug 23 12:35:02 2015. */

#include "rover_status.pb.h"

#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t rover_status_fields[9] = {
    PB_FIELD(  1, INT32   , OPTIONAL, STATIC  , FIRST, rover_status, system_temp, system_temp, 0),
    PB_FIELD(  2, FLOAT   , OPTIONAL, STATIC  , OTHER, rover_status, bv, system_temp, 0),
    PB_FIELD(  3, FLOAT   , OPTIONAL, STATIC  , OTHER, rover_status, pv, bv, 0),
    PB_FIELD(  4, INT32   , OPTIONAL, STATIC  , OTHER, rover_status, cpu_load, pv, 0),
    PB_FIELD(  5, INT32   , OPTIONAL, STATIC  , OTHER, rover_status, gps_health, cpu_load, 0),
    PB_FIELD(  6, INT32   , OPTIONAL, STATIC  , OTHER, rover_status, radio_health, gps_health, 0),
    PB_FIELD(  7, INT32   , OPTIONAL, STATIC  , OTHER, rover_status, rockblock_health, radio_health, 0),
    PB_FIELD(  8, INT32   , OPTIONAL, STATIC  , OTHER, rover_status, watchdog_last_fired, rockblock_health, 0),
    PB_LAST_FIELD
};


