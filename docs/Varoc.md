---
layout: default
title: ESRF Varoc Encoder
parent: Device Support
nav_order: 6
---


# ESRF Varoc SSI Absolute Encoder Interface

Authors: Karen J. Coulter (1993), Robert A. Popper (1994)

The Varoc (ZIO085) is a VMEbus SSI (Synchronous Serial Interface) absolute
encoder interface card originally developed at ESRF. It supports up to 16
encoder channels per card. This module provides device support and a low-level
driver for reading absolute encoder positions via EPICS ai records.

## Platform Support

vxWorks only.

## Configuration

There is no user-callable configuration function. The driver uses compile-time
constants:

| Constant | Value | Description |
|----------|-------|-------------|
| `MAX_VAROC_CARDS` | 1 | Maximum number of cards supported |
| `ADDR` | `0x0800` | A16 base address |

To change the base address or number of cards, modify the constants in
`drvVarocB.c` and rebuild.

## Supported Record Types

| Record Type | DTYP String |
|-------------|-------------|
| ai | `"ESRF Varoc SSI Encoder Iface"` |

Link type: VME_IO (`#C<card> S<channel> @<parm>`)

### PARM Field Format

The PARM field specifies the encoder bit count and encoding type:

```
<numbits><code>
```

Where:
- `numbits` is the number of encoder bits (7-25)
- `code` is `G` for Gray-code or `S` for serial (straight binary)

**Examples:**

```
# 23-bit Gray-code encoder on card 0, channel 3
record(ai, "$(P)encoder3") {
    field(DTYP, "ESRF Varoc SSI Encoder Iface")
    field(INP,  "#C0 S3 @23G")
}

# 16-bit straight binary encoder on card 0, channel 0
record(ai, "$(P)encoder0") {
    field(DTYP, "ESRF Varoc SSI Encoder Iface")
    field(INP,  "#C0 S0 @16S")
}
```

## Usage Notes

- Supports up to 16 channels per card.
- Gray-code to binary conversion is handled in hardware (CMD0 bit 5).
- Linear conversion: `ESLO=1`, `ROFF = -(EGUL / ASLO)`
