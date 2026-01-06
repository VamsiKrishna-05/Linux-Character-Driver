# ESD Wristband Monitoring – Linux Character Driver

## Overview
This project implements a Linux character device driver that models an
ESD wristband monitoring system. The driver exposes operator presence and
wristband grounding status to user space through a device node.

The current implementation uses simulated inputs to demonstrate driver
structure, kernel-to-user communication, and extensibility to real
GPIO or SPI-based hardware.

---

## What the Driver Does
- Registers a character device in the Linux kernel
- Creates `/dev/esd_monitor`
- Exposes ESD safety status via `read()`
- Allows status control using `write()`

---

## Example Output
```text
IR=1
WRISTBAND=CONNECTED
STATUS=SAFE
```
Build and Run
```
sudo insmod esd_monitor.ko
cat /dev/esd_monitor
sudo rmmod esd_monitor
```
## MAKEFILE
```
obj-m += esd_monitor.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```
