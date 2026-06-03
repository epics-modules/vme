---
layout: default
title: Generic A32 VME
nav_order: 4
---


# Generic A32/D32 VME Device Support

Author: Ned D. Arnold (1997)

This device support module provides generic access to any VME register in the
A32/D32 address space. It supports ai, ao, bi, bo, longin, longout, mbbi, and
mbbo record types. Arbitrary bit fields within 32-bit registers can be read and
written using the PARM field to specify the starting bit and width.

## Platform Support

vxWorks only.

## Source Files

| File | Description |
|------|-------------|
| `vmeApp/src/devA32Vme.c` | Device support for all record types |

## Configuration

Call `devA32VmeConfig` in the IOC startup script before `iocInit`:

```
devA32VmeConfig(card, a32base, nreg, iVector, iLevel)
```

| Argument | Description |
|----------|-------------|
| `card` | Card number (0-3) |
| `a32base` | Base address of card in A32 VME space (max `0xf0000000`) |
| `nreg` | Number of 32-bit registers on this card (max 64) |
| `iVector` | Interrupt vector (MRD100 only; use 0 for no interrupts) |
| `iLevel` | Interrupt level (MRD100 only) |

**Example:**

```
devA32VmeConfig(0, 0x80000000, 44, 0x3e, 5)
```

## Supported Record Types

All record types use DTYP `"Generic A32 VME"` with VME_IO link type.

### Link Format

```
field(INP, "#C<card> S<register> @<parm>")
field(OUT, "#C<card> S<register> @<parm>")
```

Where:
- `card` is the card number configured by `devA32VmeConfig`
- `register` is the register offset (0, 1, 2, ...) -- not a byte address
- `parm` specifies the bit field of interest (format varies by record type)

### PARM Field Formats

| Record Type | PARM Format | Description |
|-------------|-------------|-------------|
| ai, ao | `lsb, width, type` | `lsb`=starting bit (0-31), `width`=number of bits, `type`=0 for unipolar, 1 for bipolar (2's complement) |
| bi, bo | `bit` | Single bit number (0-31) |
| longin, longout | `lsb, width` | `lsb`=starting bit, `width`=number of bits |
| mbbi, mbbo | `lsb, width` | `lsb`=starting bit, `width`=number of bits |

### Examples

```
# Read bits 0-15 of register 5 on card 0 as an analog input (unipolar)
record(ai, "$(P)reading") {
    field(DTYP, "Generic A32 VME")
    field(INP,  "#C0 S5 @0,16,0")
}

# Write to bit 3 of register 10 on card 0
record(bo, "$(P)control") {
    field(DTYP, "Generic A32 VME")
    field(OUT,  "#C0 S10 @3")
}
```

## Database Files

| File | Description |
|------|-------------|
| `vmeApp/Db/VME_DAC.db` | Generic DAC template (works with any DTYP including `"Generic A32 VME"`) |

## Debug Variable

```
var devA32VmeDebug 0
```

| Level | Output |
|-------|--------|
| 0 | No messages |
| >= 5 | Hardware initialization information |
| >= 10 | Record initialization information |
| >= 15 | Write commands |
| >= 20 | Read commands |
