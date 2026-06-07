---
layout: default
title: Acromag AVME-9440
parent: Device Support
nav_order: 3
---


# Acromag AVME-9440 Digital I/O
{: .no_toc}

Author: Greg Nawrocki (1993), David M. Kline (2005)

## Table of contents
{: .no_toc .text-delta }

- TOC
{:toc}

The Acromag AVME-9440 is a 16-bit binary input and output VME board. This
device support provides EPICS access to all 16 input and 16 output channels
via bi, bo, mbbi, and mbbo records. Change-of-state I/O interrupts are
available for binary input signals 0-7 only, and only for the bi record type.

{: .note }
> This device support is OS independent (uses EPICS devLib).

## Configuration

An iocsh script is provided that handles configuration and database loading:

```
iocshLoad("$(VME)/iocsh/Acromag_AVME9440.iocsh", "PREFIX=xxx:, INSTANCE=avme:, VME=$(VME), ADDRESS=0x400, INT_VEC=0x78")
```

| Macro | Required | Default | Description |
|-------|----------|---------|-------------|
| `PREFIX` | Yes | -- | IOC prefix |
| `INSTANCE` | Yes | -- | Instance prefix |
| `VME` | Yes | -- | Location of vme module |
| `ADDRESS` | First call | -- | A16 base address |
| `INT_VEC` | First call | -- | Interrupt vector |
| `MAX_CARDS` | No | 1 | Total number of cards |
| `CARD` | No | 0 | Card number |

The script calls `devAvme9440Config(ncards, base, vector)` and loads
`Acromag_16IO.db`. When configuring manually:

```
devAvme9440Config(1, 0x400, 0x78)
dbLoadRecords("$(VME)/vmeApp/Db/Acromag_16IO.db", "P=$(PREFIX),A=$(INSTANCE),C=0")
```

## Supported Record Types

### Signal Assignments

| Signal Range | DTYP | Direction | Description |
|--------------|------|-----------|-------------|
| 0-15 | `"AVME9440 O"` | Output | Binary outputs |
| 0-15 | `"AVME9440 I"` | Input | Binary inputs |
| 16-31 | `"AVME9440 I"` | Input (readback) | Binary output readback |

All use VME_IO link type (`#C<card> S<signal> @`).

### Record Types

| Record Type | DTYP Strings |
|-------------|-------------|
| bi | `"AVME9440 I"` |
| bo | `"AVME9440 O"` |
| mbbi | `"AVME9440 I"` |
| mbbo | `"AVME9440 O"` |

### I/O Interrupt Scanning

I/O Intr scanning is available for bi records on input signals 0-7 only. If
interrupts are needed for mbbi-type values, use bi records with `SCAN="I/O Intr"`
linked to calc records.

## Debug Variable

```
var devAvme9440Debug 0
```

| Level | Output |
|-------|--------|
| 0 | No messages |
| >= 5 | Hardware initialization information |
| >= 10 | Record initialization information |
| >= 15 | Write commands |
| >= 20 | Read commands |

## Hardware Reference

See [AVME9440.pdf](AVME9440.pdf) for the Acromag AVME-9440 datasheet.
