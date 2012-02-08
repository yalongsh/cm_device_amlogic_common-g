#!/system/bin/sh

busybox-fbset echo "w 1076 0x30f" > /sys/class/i2c/cbus_reg
busybox-fbset echo "w 106d 0x232" > /sys/class/i2c/cbus_reg
