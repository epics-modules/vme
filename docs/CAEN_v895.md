---
layout: default
title: CAEN V895
parent: Device Support
nav_order: 5
---


# CAEN V895 16-Channel Discriminator

Author: Tim Mooney

The CAEN V895 is a 16-channel leading edge discriminator in a VME module. This
driver is implemented as an asynPortDriver subclass, providing access to the
per-channel threshold values (0-255) through standard asyn interfaces.

## Configuration

An iocsh script is provided that initializes the driver and loads all 16
channels:

```
iocshLoad("$(VME)/iocsh/CAEN_v895.iocsh", "PREFIX=xxx:, INSTANCE=disc:, VME=$(VME), ADDRESS=0x04000000, PORT=v895")
```

| Macro | Required | Description |
|-------|----------|-------------|
| `PREFIX` | Yes | IOC prefix |
| `INSTANCE` | Yes | Instance prefix |
| `ADDRESS` | Yes | VME base address |
| `PORT` | Yes | asyn port name |
| `VME` | Yes | Location of vme module |

The script calls `initCAEN_v895(portName, baseAddress)` and loads one instance
of `singleDiscrim.db` per channel (N=1-16, S=0-15). When configuring manually:

```
initCAEN_v895("v895", 0x04000000)
dbLoadRecords("$(VME)/vmeApp/Db/singleDiscrim.db", "P=$(PREFIX),D=$(INSTANCE),N=1,S=0")
# Repeat for N=2..16, S=1..15
```

## asyn Parameters

| Parameter String | asyn Interface | Description |
|-----------------|----------------|-------------|
| `DATA` | asynInt32 | Discriminator threshold in device units (0-255) |
| `DOUBLE_DATA` | asynFloat64 | Discriminator threshold as floating-point |

The driver is configured as `ASYN_MULTIDEVICE` with 16 channels (addresses
0-15).

**Note:** The CAEN V895 hardware is write-only. Read operations return 0; the
last written value is tracked via the asyn parameter library.

## Dependencies

Requires the **asyn** module (asynPortDriver base class).
