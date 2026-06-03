---
layout: default
title: Acromag AVME-9210
nav_order: 7
---


# Acromag AVME-9210 Analog Output

Author: Joe White (SLAC, 1995)

The Acromag AVME-9210 is an 8-channel, 12-bit DAC analog output VME module in
A16 short I/O address space. This module provides device support and a
low-level driver for controlling the AVME-9210.

## Platform Support

vxWorks only.

## Configuration

Call `avme9210Config` in the IOC startup script before `iocInit`:

```
avme9210Config(maxCards, maxChan, address)
```

| Argument | Description | Default |
|----------|-------------|---------|
| `maxCards` | Maximum number of cards | 0 (none assumed unless called) |
| `maxChan` | Maximum channels per card | 8 |
| `address` | A16 base address | `0x3000` |

**Example:**

```
avme9210Config(1, 8, 0x3000)
dbLoadRecords("$(VME)/vmeApp/Db/VME_DAC.db", "P=$(PREFIX),D=1,N=chan1,DTYP=AVME-9210,C=0,S=0,H=10,L=-10")
```

The `VME_DAC.db` template provides a rate-of-change-limited DAC output with
tweaking and can be used with this device support by setting
`DTYP="AVME-9210"`.

## Supported Record Types

| Record Type | DTYP String |
|-------------|-------------|
| ao | `"AVME-9210"` |

Link type: VME_IO (`#C<card> S<signal> @`)

## Usage Notes

- The card is assumed to be in **straight binary** (not 2's complement) jumper
  configuration.
- Output voltage range jumpers on the card must match the EGUF/EGUL fields in
  the ao record.
- Linear conversion slope: `(EGUF - EGUL) / 4095.0`
- The card cannot be meaningfully read back (reads return `0xFFFF`).
- Card identity is verified at initialization via the module ID PROM string
  `"VMEIDACR9210"`.

## Hardware Reference

See [AVME9210.pdf](AVME9210.pdf) for the Acromag AVME-9210 datasheet.
