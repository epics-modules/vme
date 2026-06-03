---
layout: default
title: CAEN V895
nav_order: 9
---


# CAEN V895 16-Channel Discriminator

Author: Tim Mooney

The CAEN V895 is a 16-channel leading edge discriminator in a VME module. This
driver is implemented as an asynPortDriver subclass, providing access to the
per-channel threshold values (0-255) through standard asyn interfaces.

## Platform Support

vxWorks only.

## Source Files

| File | Description |
|------|-------------|
| `vmeApp/src/drvCAEN_v895.cpp` | asynPortDriver subclass |

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

## Database Files

| File | Description |
|------|-------------|
| `vmeApp/Db/singleDiscrim.db` | Single discriminator channel with threshold and tweak controls |
| `vmeApp/Db/singleDiscrim_settings.req` | Autosave request file |

### Database Macros (singleDiscrim.db)

| Macro | Description |
|-------|-------------|
| `$(P)` | PV prefix |
| `$(D)` | Discriminator instance prefix |
| `$(N)` | Channel number (1-16, for PV naming) |
| `$(S)` | asyn address (0-15, maps to hardware channel) |

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

## Operator Displays

| File | Format |
|------|--------|
| `vmeApp/op/adl/CAEN_v895.adl` | MEDM |

Autoconverted displays are available in the `bob/`, `edl/`, `opi/`, and `ui/`
subdirectories.

## Dependencies

Requires the **asyn** module (asynPortDriver base class).
