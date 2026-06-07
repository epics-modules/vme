---
layout: default
title: Heidenhain IK320
parent: Device Support
nav_order: 7
---


# Heidenhain IK320 Encoder Setup
{: .no_toc}

Author: Kurt Goetze (goetze@aps.anl.gov)

## Table of contents
{: .no_toc .text-delta }

- TOC
{:toc}

The Heidenhain IK320 is a VME counter/interpolator for linear and angular
encoders. This page describes how to configure the IK320 for use with EPICS
(synApps). The IK320 device support and driver were written by Till Straumann
of PTB at BESSY.

{: .note }
> The IK320 builds on both **vxWorks** and **RTEMS**.

## Supported Record Types

| Record Type | DTYP | Purpose |
|-------------|------|---------|
| mbbo | `Heidenhain IK320 Command` | Function command (e.g., read, reference search) |
| mbbo | `Heidenhain IK320 Direction` | Counting direction |
| mbbo | `Heidenhain IK320 X3 Mode` | X3 interpolation mode |
| stringout | `Heidenhain IK320 Param` | Parameter get/set |
| ai | `Heidenhain IK320` | Single-axis reading |
| ai | `Heidenhain IK320 Group` | Group trigger reading |

## IK320card.db Configuration

Here is an example of a record from the `IK320card.db` database:

```
record(mbbo, "$(P)IK320:$(sw2)$(axis)function")
{
    field(DTYP, "Heidenhain IK320 Command")
    field(OUT,  "#C$(switches) S$(irq) @$(axis)")
}
```

Here is an example of loading `IK320card.db` in the startup command file:

```
dbLoadRecords("$(VME)/vmeApp/Db/IK320card.db", "P=kag:,sw2=card0:,axis=1,switches=41344,irq=3")
```

The macros that need additional explanation are `$(switches)` and `$(irq)`.

### Calculating the switches value

The `switches` argument is derived by taking the hexadecimal value of the S1/S2
switch banks and converting it to decimal. A switch in the OFF position is a 1.

For example:

```
Switch S1               Switch S2
---------               ---------
1000 0101               0000 0001
        ^- MSB                  ^- MSB
```

Rewriting in conventional notation (MSB on the left):

```
S1: 1010 0001 = 0xA1
S2: 1000 0000 = 0x80
```

Combined: `0xA180 = 41344` (decimal), hence `switches=41344` in the
`dbLoadRecords()` call above.

### VME Address Calculation

The IK320 occupies space in both the A16 and A24 VMEbus address spaces. The
formulas for determining the addresses based on the switch settings are:

**A24 base address:**

```
A24 base = 0x00C00000 + (0x4000 * S2)
```

In the example above, S2 = 0x80:

```
A24 base = 0x00C00000 + (0x4000 * 0x80) = 0xE00000
```

**A16 base address:**

```
A16 base = (S1 & 0xE0) * 0x100
```

In the example above, S1 = 0xA1:

```
A16 base = (0xA1 & 0xE0) * 0x100 = 0xA0 * 0x100 = 0xA000
```

### IRQ level

The `irq` value should be the IRQ level set by jumpers J1 and J2 on the IK320
board. According to the manual, these levels **must** be the same. Typically
they are left at the factory default of **3**.

### J3 Jumper

Most VME CPUs do not support the ADO cycle. It is recommended that J3 be
jumpered to generate a DTACK, per page 14 of the IK320 manual. For a group of
IK320 boards, **one** of the boards should have J3 jumpered.

## IK320group.db Configuration

Here is an example of loading `IK320group.db` in the startup command file:

```
dbLoadRecords("$(VME)/vmeApp/Db/IK320group.db", "P=kag:,group=5")
```

In order to use more than one channel, even if it is on the same board, a copy
of the `IK320group.db` database must be loaded. This database contains one AI
record that must be processed in order for readings to be taken. The `$(group)`
macro is derived by taking the decimal value of bits 8, 7, and 6 on S1. For
example, if these bits were set to `1 0 1`, then `group=5`.


