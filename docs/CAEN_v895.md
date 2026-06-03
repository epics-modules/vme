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

## Platform Support

vxWorks only.

## Configuration

Call `initCAEN_v895` in the IOC startup script before `iocInit`:

```
initCAEN_v895(portName, baseAddress)
```

| Argument | Description |
|----------|-------------|
| `portName` | asyn port name |
| `baseAddress` | VME base address (A32 extended supervisory data) |

**Example:**

```
initCAEN_v895("v895", 0x04000000)
dbLoadRecords("$(VME)/vmeApp/Db/singleDiscrim.db", "P=$(PREFIX),D=$(INSTANCE),N=1,S=0")
```

Load one instance of `singleDiscrim.db` per channel (N=1-16, S=0-15).

## asyn Parameters

| Parameter String | asyn Interface | Description |
|-----------------|----------------|-------------|
| `DATA` | asynInt32 | Discriminator threshold in device units (0-255) |
| `DOUBLE_DATA` | asynFloat64 | Discriminator threshold as floating-point |

The driver is configured as `ASYN_MULTIDEVICE` with 16 channels (addresses
0-15).

**Note:** The CAEN V895 hardware is write-only. Read operations return 0; the
last written value is tracked via the asyn parameter library.

## iocsh Script

The provided iocsh script initializes the driver and loads all 16 channels:

```
< $(VME)/iocsh/CAEN_v895.iocsh
```

### Script Macros

| Macro | Required | Description |
|-------|----------|-------------|
| `PREFIX` | Yes | IOC prefix |
| `INSTANCE` | Yes | Instance prefix |
| `ADDRESS` | Yes | VME base address |
| `PORT` | Yes | asyn port name |
| `VME` | Yes | Location of vme module |

## Dependencies

Requires the **asyn** module (asynPortDriver base class).
