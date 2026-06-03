---
layout: default
title: Joerger Scalers
nav_order: 12
---


# Joerger Scalers

Author: Tim Mooney (1995, 2003)

The vme module provides device support for two families of Joerger VME scaler
modules:

- **VSC series** (VSC8, VSC16): 8- or 16-channel, 32-bit scalers with
  interrupt-driven count completion (originally Lecroy 1151). Located in A32
  address space.
- **VS series** (VS64, VS32, VS16): 32-bit read-on-the-fly scalers in multiple
  variants (TTL, ECL, NIM, and "D" versions). Located in A16 address space.

Both families interface with the EPICS scaler record from the **SCALER** module
(formerly part of STD).

## Platform Support

vxWorks only (with OSI portability via S. Kate Feng, 2003).

## Source Files

### VSC Series (VSC8, VSC16)

| File | Description |
|------|-------------|
| `vmeApp/src/devScaler_debug.c` | Device support (debug-enhanced version, actively used) |
| `vmeApp/src/devScaler.c` | Original device support (currently not compiled) |

### VS Series (VS64, VS32, VS16)

| File | Description |
|------|-------------|
| `vmeApp/src/devScaler_VS.c` | Device support |

## Configuration

### VSC Series

Call `VSCSetup` in the IOC startup script before `iocInit`:

```
VSCSetup(num_cards, addrs, vector)
```

| Argument | Description | Default |
|----------|-------------|---------|
| `num_cards` | Maximum number of cards in crate | -- |
| `addrs` | A32 base address (256-byte boundary, range `0x100`-`0xffffff00`) | `0x00900000` |
| `vector` | Interrupt vector (64-255, or 0 for non-interrupting) | 0 |

**Example:**

```
VSCSetup(1, 0x00900000, 200)
```

### VS Series

Call `scalerVS_Setup` in the IOC startup script before `iocInit`:

```
scalerVS_Setup(num_cards, addrs, vector, intlevel)
```

| Argument | Description | Default |
|----------|-------------|---------|
| `num_cards` | Maximum number of cards in crate | -- |
| `addrs` | A16 base address (2048-byte boundary, range `0x800`-`0xf800`) | `0xd000` |
| `vector` | Interrupt vector (64-255, or 0 for none) | 0 |
| `intlevel` | Interrupt level (masked to 3 bits) | 5 |

**Example:**

```
scalerVS_Setup(1, 0xd000, 210, 5)
```

## Supported Record Types

### VSC Series

| Record Type | DTYP String |
|-------------|-------------|
| scaler | `"Joerger VSC8/16"` |

### VS Series

| Record Type | DTYP String |
|-------------|-------------|
| scaler | `"Joerger VS"` |

## Diagnostic Functions

### VSC Series

```
VSCscaler_show(card)
VSCscaler_debug(card, numReads, waitLoops)
```

### VS Series

```
scalerVS_show(card, level)
scalerVS_regShow(card, level)
```

## Debug Variables

### VSC Series

```
var devScalerDebug 0
```

| Level | Output |
|-------|--------|
| 0 | No messages |
| >= 5 | Initialization messages |
| >= 19+N | Per-channel detail for channel N |

### VS Series

| Variable | Default | Description |
|----------|---------|-------------|
| `devScaler_VSDebug` | 0 | Debug level (>= 5: init info, >= 10: detail) |
| `devScaler_VS_check_trig` | 1 | Enable trigger verification |
| `devScaler_VS_trig_retries` | 10000 | Max retries when verifying trigger |
| `devScaler_VS_trig_retry_report` | 100 | Report threshold for retry count |
| `devScaler_VS_trig_reads` | 5 | Number of consistent reads required |

## Notes

- The VSC-series `devScaler_debug.c` is used instead of `devScaler.c` because
  many newer VSC-series cards have hardware issues that the debug version works
  around.
- The VSC device support reports at initialization whether the card is
  configured for TTL or NIM termination.
- Both families require the **SCALER** module (provides the scaler record type).

## Hardware Reference

See [JOERGER_Vs64Mc.PDF](JOERGER_Vs64Mc.PDF) for the Joerger VS series
datasheet.
