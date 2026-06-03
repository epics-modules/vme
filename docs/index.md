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

The module also includes a generic VME record for performing arbitrary VME bus
read/write operations, and a generic A32/D32 device support layer for accessing
any register-mapped VME card.

## Supported Hardware

| Device | Type | Documentation |
|--------|------|---------------|
| [Generic VME Record](vmeRecord.md) | Arbitrary VME I/O (A16/A24/A32, D8/D16/D32) | Record support |
| [Generic A32 VME](devA32Vme.md) | A32/D32 register access for any VME card | Device support |
| [Acromag AVME-9210](Acromag_AVME9210.md) | 8-channel, 12-bit analog output (DAC) | Device + driver |
| [Acromag AVME-9440](Acromag_AVME9440.md) | 16-bit digital I/O with interrupts | Device support |
| [APS BCG-100](BunchClkGen.md) | Bunch clock generator | Device + driver |
| [CAEN V895](CAEN_v895.md) | 16-channel leading edge discriminator | asynPortDriver |
| [ESRF Varoc](Varoc.md) | SSI absolute encoder interface | Device + driver |
| [Heidenhain IK320](IK320_setup.md) | VME encoder counter/interpolator | Device + driver |
| [HP 10895A](HP_10895A.md) | Laser axis interferometer | Device support |
| [Joerger VSC/VS Scalers](Joerger_scaler.md) | 32-bit scaler/counter/timer modules | Device support |
| [MRD-100](MRD100.md) | Machine status link (accelerator data) | Device support |
| [VMIC VMI4116](VMI4116.md) | 8-channel, 16-bit DAC | Device support |

## Platform Support

Most device support in this module is **vxWorks only**. Exceptions:

- **Acromag AVME-9440**: OS independent (uses EPICS devLib)
- **Heidenhain IK320**: Builds on both vxWorks and RTEMS

## Dependencies

| Module | Purpose |
|--------|---------|
| EPICS Base | Required (7.0.4 or compatible) |
| [asyn](https://github.com/epics-modules/asyn) | Required for CAEN V895 driver (asynPortDriver) |
| [SNCSEQ](https://github.com/epics-modules/sequencer-2-2) | Required for bunch clock fill pattern sequencer |
| [SCALER](https://github.com/epics-modules/scaler) | Required for Joerger scaler record type |

## Additional Documentation

- [VME Record](vmeRecord.md) -- generic VME I/O record
- [Release Notes](vmeReleaseNotes.md) -- release history from 2-3 through 2-9-5

## Hardware Datasheets (included)

- [Acromag AVME-9210](AVME9210.pdf)
- [Acromag AVME-9440](AVME9440.pdf)
- [Joerger VS64](JOERGER_Vs64Mc.PDF)
- [VMIC VMI4116](VMICVMI4116.pdf)
