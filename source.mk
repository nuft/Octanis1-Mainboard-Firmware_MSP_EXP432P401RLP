CSRC += \
	./fw/tests/minunit.c \
	./fw/protobuf/rover_status.pb.c \
	./fw/protobuf/simple.pb.c \
	./fw/peripherals/gps.c \
	./fw/peripherals/imu.c \
	./fw/peripherals/navigation.c \
	./fw/peripherals/vision.c \
	./fw/peripherals/weather.c \
	./fw/peripherals/hal/SHT2x.c \
	./fw/peripherals/hal/SHT2xi2c.c \
	./fw/peripherals/hal/bme280.c \
	./fw/peripherals/hal/bme280i2c.c \
	./fw/peripherals/hal/bmp180.c \
	./fw/peripherals/hal/bmp180i2c.c \
	./fw/peripherals/hal/bno055.c \
	./fw/peripherals/hal/bno055_support.c \
	./fw/peripherals/hal/hx1.c \
	./fw/peripherals/hal/i2c_helper.c \
	./fw/peripherals/hal/motors.c \
	./fw/peripherals/hal/rn2483.c \
	./fw/peripherals/hal/rockblock.c \
	./fw/peripherals/hal/ublox_6.c \
	./fw/peripherals/hal/ultrasonic.c \
	./fw/peripherals/hal/windsensor.c \
	./fw/main.c \
	./fw/lib/printf.c \
	./fw/lib/nanopb/pb_common.c \
	./fw/lib/nanopb/pb_decode.c \
	./fw/lib/nanopb/pb_encode.c \
	./fw/lib/minmea/minmea.c \
	./fw/core/cron.c \
	./fw/core/interrupts.c \
	./fw/core/log.c \
	./fw/core/system.c \

	# ./fw/core/cli.c \
#	./MSP_EXP432P401RLP.c \
#	./fw/peripherals/comm.c \
