---
layout: default
title: Generic A32 VME
parent: Device Support
nav_order: 1
---


# Generic A32/D32 VME Device Support
{: .no_toc}

Author: Ned D. Arnold (1997)

## Table of contents
{: .no_toc .text-delta }

- TOC
{:toc}

This device support module provides generic access to any VME register in the
A32/D32 address space. It supports ai, ao, bi, bo, longin, longout, mbbi, and
mbbo record types. Arbitrary bit fields within 32-bit registers can be read and
written using the PARM field to specify the starting bit and width.

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
dbLoadRecords("$(VME)/vmeApp/Db/VME_DAC.db", "P=$(PREFIX),D=1,N=chan1,DTYP=Generic A32 VME,C=0,S=0,H=10,L=-10")
```

The `VME_DAC.db` template provides a rate-of-change-limited DAC output with
tweaking and can be used with this device support by setting
`DTYP="Generic A32 VME"`.

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
