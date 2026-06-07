---
layout: default
title: Release Notes
nav_order: 2
---


# vme Release Notes
{: .no_toc}

## Table of contents
{: .no_toc .text-delta }

- TOC
{:toc}

## Release 2-9-5 (Jun 7, 2023)

- Include file fix to build under newer versions of base
- Documentation moved to GitHub Pages

## Release 2-9-4 (Jun 11, 2021)

- Update to use SCALER module instead of STD

## Release 2-9-3 (Oct 5, 2020)

- iocsh files now installed to top-level folder from vmeApp/iocsh
- bob files added, ui and edl files updated

## Release 2-9-2 (Jun 14, 2019)

- Req files now installed to top level db folder

## Release 2-9-1 (May 2, 2019)

- IOC shell script for CAEN v895
- MRD_100 shell script now allows user to separately define ID1 and ID2 macros

## Release 2-9 (Mar 26, 2018)

- Added iocsh bindings for commands and debug variables
- Added op/ui/autoconvert and op/opi/autoconvert directories
- Enabled Travis CI
- Added support for CAEN V895 16-channel discriminator

## Release 2-8-2 (Jul 2014)

- Device support for Joerger scaler reports whether card is configured for TTL or NIM termination
- New translations for .adl files to .ui and .opi files
- getFilledBuckets.st: delay after stored beam and before loading bunch pattern was implemented in a way that didn't work

## Release 2-8-1 (Apr 17, 2013)

- Added displays for CSS-BOY and caQtDM
- MRD100_CantedID.db: new file for canted undulator

## Release 2-8 (Aug 26, 2011)

- devScaler.c, devScaler_debug.c: check that scaler_state exists at init.
  Replace vxWorks-specific rebootHookAdd() calls with EPICS OSI epicsAtExit() calls.
- Modified RELEASE; deleted RELEASE.arch
- Added .opi display files for CSS-BOY

## Release 2-7 (Mar 30, 2010)

- Fixes for EPICS 3.14.11
- devScaler.c, devScaler_debug.c: check that scaler_state exists at init

## Release 2-6 (Dec 19, 2008)

- Removed iocBoot example and associated build in src
- Use devScaler_debug.c for VSC-series scalers, because many of the new series have hardware trouble

## Release 2-4-4 (Dec 5, 2006)

- devScaler.c, devScaler_VS.c: changed to new interface between scaler record and device support
- Deleted Db/Jscaler* (obsolete; stdApp/Db/scaler* should be used instead)

## Release 2-4-3

- Acromag_16IO.db: card number is now a macro
- devScaler.c: removed debug macro definitions with a variable number of arguments
- devScaler_VS.c: removed debug macro definitions with a variable number of arguments
- vmeRecord.c: removed debug macro definitions with a variable number of arguments

## Release 2-4-2

- getFilledBuckets.st: modified to allow multiple instances
- vmeVXSupport.dbd: included vmeSupport.dbd
- Acro_bi_scan.adl: added "9440" to PV names
- BunchClkGen.db: modified to allow multiple instances
- Heidenhain IK320 device driver changes:
  - Eliminated the need to call drvIK320RegErrStr() from the vxWorks st.cmd file
  - taskDelay(1) and semTake(**, 1) with delay of 1 tick changed to 5 ms
  - Allow reading encoder without referencing

## Release 2-4-1

- The devAvme9440 module is OS independent

## Release 2-4-0

Tested using base 3.14.6 and 3.14.7, seq 2.0.8, calc 2.3, sscan 2.3, std 2.3, and asyn 1.2.

New support:

- MRD-100:
  - devAvmeMRD: provides new device support and is the first step in moving away from using devA32Vme
  - msl_mrd100.db: modified to reflect new device support module
  - msl_mrd101.db: modified to reflect new device support module
- Added Joerger VSxx scaler support
- Added support for Acromag AVME-9210 and AVME-9440
- Added new vmeTest test application: a menu-driven application that provides simple VME read/write operations

## Release 2-3

This is the first release of the synApps vme module. Version numbering for this
module begins with 2.3 because this module was split from version 2.2 of the std
module, and the CVS histories of module contents were retained.

This version is intended to build with EPICS base 3.14.5. Differences from
software as previously released in std 2.2:

- Converted to EPICS 3.14
- VME_DAC_SETTINGS.req, VME_DAC_POSITIONS.req: new files
