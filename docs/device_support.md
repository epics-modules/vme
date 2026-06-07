---
layout: default
title: Device Support
nav_order: 4
has_children: true
---


# Device Support
{: .no_toc}

The vme module provides device support and drivers for the following VME
hardware:

| Device | Type |
|--------|------|
| [Generic A32 VME](devA32Vme.md) | A32/D32 register access for any VME card |
| [Acromag AVME-9210](Acromag_AVME9210.md) | 8-channel, 12-bit analog output (DAC) |
| [Acromag AVME-9440](Acromag_AVME9440.md) | 16-bit digital I/O with interrupts |
| [APS BCG-100](BunchClkGen.md) | Bunch clock generator |
| [CAEN V895](CAEN_v895.md) | 16-channel leading edge discriminator |
| [ESRF Varoc](Varoc.md) | SSI absolute encoder interface |
| [Heidenhain IK320](IK320_setup.md) | VME encoder counter/interpolator |
| [HP 10895A](HP_10895A.md) | Laser axis interferometer |
| [Joerger VSC/VS Scalers](Joerger_scaler.md) | 32-bit scaler/counter/timer modules |
| [MRD-100](MRD100.md) | Machine status link (accelerator data) |
| [VMIC VMI4116](VMI4116.md) | 8-channel, 16-bit DAC |

Most device support is **vxWorks only**. Exceptions:

- **Acromag AVME-9440**: OS independent (uses EPICS devLib)
- **Heidenhain IK320**: Builds on both vxWorks and RTEMS
