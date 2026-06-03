---
layout: default
title: Home
nav_order: 1
---


# vme -- EPICS VME Device Support Module

The **vme** module is part of the
[APS BCDA synApps](https://github.com/epics-modules) suite. It provides EPICS
record support, device support, and driver support for a variety of VME
(Versa Module Europa) bus hardware modules used at the Advanced Photon Source
(APS) at Argonne National Laboratory.

The module includes a [generic VME record](vmeRecord.md) for performing
arbitrary VME bus read/write operations, and
[device support](device_support.md) for 11 specific VME hardware modules.

## Platform Support

Most device support in this module is **vxWorks only**. Exceptions:

- **Acromag AVME-9440**: OS independent (uses EPICS devLib)
- **Heidenhain IK320**: Builds on both vxWorks and RTEMS

## Dependencies

| Module | Purpose |
|--------|---------|
| [EPICS Base](https://github.com/epics-base/epics-base) | Required (7.0.4 or compatible) |
| [asyn](https://github.com/epics-modules/asyn) | Required for CAEN V895 driver (asynPortDriver) |
| [seq](https://github.com/epics-modules/sequencer-2-2) | Required for bunch clock fill pattern sequencer |
| [scaler](https://github.com/epics-modules/scaler) | Required for Joerger scaler record type |

