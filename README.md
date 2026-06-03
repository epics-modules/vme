[![VME](https://github.com/epics-modules/vme/actions/workflows/ci-scripts-build.yml/badge.svg)](https://github.com/epics-modules/vme/actions/workflows/ci-scripts-build.yml)

# vme

APS BCDA synApps module: **vme**

The vme module provides EPICS record support, device support, and driver
support for a variety of VME bus hardware modules. It includes a generic VME
record for arbitrary VME I/O, generic A32/D32 device support, and dedicated
support for specific VME cards used at the Advanced Photon Source.

## Supported Hardware

| Device | Type |
|--------|------|
| Generic VME Record | Arbitrary VME I/O (A16/A24/A32, D8/D16/D32) |
| Generic A32 VME | A32/D32 register access for any VME card |
| Acromag AVME-9210 | 8-channel, 12-bit analog output (DAC) |
| Acromag AVME-9440 | 16-bit digital I/O with interrupts |
| APS BCG-100 | Bunch clock generator |
| CAEN V895 | 16-channel leading edge discriminator |
| ESRF Varoc | SSI absolute encoder interface |
| Heidenhain IK320 | VME encoder counter/interpolator |
| HP 10895A | Laser axis interferometer |
| Joerger VSC8/VSC16 | 8/16-channel, 32-bit scaler |
| Joerger VS64/VS32/VS16 | 32-bit read-on-the-fly scaler |
| MRD-100 | Machine status link (accelerator data) |
| VMIC VMI4116 | 8-channel, 16-bit DAC |

## Platform Support

Most device support is **vxWorks only**. Exceptions:

- **Acromag AVME-9440**: OS independent (uses EPICS devLib)
- **Heidenhain IK320**: Builds on both vxWorks and RTEMS

## Dependencies

| Module | Tested Version | Purpose |
|--------|---------------|---------|
| [EPICS Base](https://github.com/epics-base/epics-base) | 7.0.4 | Required |
| [asyn](https://github.com/epics-modules/asyn) | 4-41 | CAEN V895 driver |
| [SNCSEQ](https://github.com/epics-modules/sequencer-2-2) | 2-2-5 | Bunch clock fill pattern sequencer |
| [SCALER](https://github.com/epics-modules/scaler) | 4-0 | Joerger scaler record type |

## Building

1. Edit `configure/RELEASE` to set the paths for EPICS Base and the required
   support modules (asyn, SNCSEQ, SCALER).
2. Run `make` at the top level.

## Documentation

Full documentation is available on the
[GitHub Pages site](https://epics-modules.github.io/vme/) or in the
[docs/](docs/) directory.

## Links

- [Report an issue](https://github.com/epics-modules/vme/issues/new?title=%20ISSUE%20NAME%20HERE&body=**Describe%20the%20issue**%0A%0A**Steps%20to%20reproduce**%0A1.%20Step%20one%0A2.%20Step%20two%0A3.%20Step%20three%0A%0A**Expected%20behaviour**%0A%0A**Actual%20behaviour**%0A%0A**Build%20Environment**%0AArchitecture:%0AEpics%20Base%20Version:%0ADependent%20Module%20Versions:&labels=bug)
- [Request a feature](https://github.com/epics-modules/vme/issues/new?title=%20FEATURE%20SHORT%20DESCRIPTION&body=**Feature%20Long%20Description**%0A%0A**Why%20should%20this%20be%20added?**%0A&labels=enhancement)
- [synApps](https://github.com/EPICS-synApps)

## License

This software is distributed under the
[synApps license](LICENSE).
