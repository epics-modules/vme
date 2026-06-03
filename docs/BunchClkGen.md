---
layout: default
title: Bunch Clock Generator
nav_order: 8
---


# APS BCG-100 Bunch Clock Generator

Author: Frank Lenkszus (1995), David M. Kline (2005)

The BCG-100 is a VMEbus bunch clock generator module used at the Advanced
Photon Source. This module provides combined driver and device support for
controlling the BCG-100 in the A16 VME address space. It supports ai, ao, bi,
bo, and waveform record types.

A State Notation Language (SNL) sequence program (`getFilledBuckets.st`) is
included that loads the APS storage ring fill pattern into the bunch clock
generator hardware.

## Platform Support

vxWorks only.

## Source Files

| File | Description |
|------|-------------|
| `vmeApp/src/devBunchClkGen.c` | Combined driver and device support |
| `vmeApp/src/getFilledBuckets.st` | SNL sequence program for fill pattern loading |

## Configuration

Call `BunchClkGenConfigure` in the IOC startup script before `iocInit`:

```
BunchClkGenConfigure(card, base)
```

| Argument | Description |
|----------|-------------|
| `card` | Card number |
| `base` | VME A16 base address |

**Example:**

```
BunchClkGenConfigure(0, 0x7000)
```

After `iocInit`, start the fill pattern sequence program:

```
seq &getFillPat, "unit=$(PREFIX)$(INSTANCE)"
```

## Supported Record Types

All use VME_IO link type (`#C<card> S<signal> @`).

| Record Type | DTYP String |
|-------------|-------------|
| ai | `"Bunch Clk Gen"` |
| ao | `"Bunch Clk Gen"` |
| bi | `"Bunch Clk Gen"` |
| bo | `"Bunch Clk Gen"` |
| waveform | `"Bunch Clk Gen"` |

## Database Files

| File | Description |
|------|-------------|
| `vmeApp/Db/BunchClkGen.db` | Bunch clock generator records (delays, bucket control, enable/disable, waveforms) |
| `vmeApp/Db/BunchClkGenA.db` | Alternate database with all fields explicitly set |
| `vmeApp/Db/BunchClkGenA.req` | Autosave request file for delay values |

### Database Macros

| Macro | Description |
|-------|-------------|
| `$(UNIT)` | PV prefix (typically `$(PREFIX)$(INSTANCE)`) |
| `$(C)` | Card number |

## iocsh Script

The provided iocsh script handles configuration, database loading, and
sequencer startup:

```
< $(VME)/iocsh/BunchClkGen.iocsh
```

### Script Macros

| Macro | Required | Default | Description |
|-------|----------|---------|-------------|
| `PREFIX` | Yes | -- | IOC prefix |
| `INSTANCE` | Yes | -- | Instance prefix |
| `ADDRESS` | Yes | -- | A16 card address |
| `VME` | Yes | -- | Location of vme module |
| `CARD` | No | 0 | Card number |

## Operator Displays

| File | Format |
|------|--------|
| `vmeApp/op/adl/BunchClkGen.adl` | MEDM |
| `vmeApp/op/adl/BunchClkGenA.adl` | MEDM |

Autoconverted displays are available in the `bob/`, `edl/`, `opi/`, and `ui/`
subdirectories.

## Debug Variables

```
var drvBunchClkGenDebug 0
var devBunchClkGenDebug 0
```

| Variable | Level | Output |
|----------|-------|--------|
| `drvBunchClkGenDebug` | > 0 | Driver messages |
| `devBunchClkGenDebug` | > 0 | Device support messages |

## Dependencies

The fill pattern sequence program requires the **SNCSEQ** (sequencer) module.
