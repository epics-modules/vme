---
layout: default
title: Release Notes
nav_order: 3
---


vme Release Notes
-----------------

Release 2-9-5 (Jun 7, 2023)
---------------------------

Include file fix to build under newer versions of base

Documentation moved to github pages

Release 2-9-4 (Jun 11, 2021)
----------------------------

Update to use SCALER module instead of STD

Release 2-9-3 (Oct 5, 2020)
---------------------------

- iocsh files now installed to top-level folder from vmeApp/iocsh
- bob files added, ui and edl files updated

Release 2-9-2 (Jun 14, 2019)
----------------------------

- Req files now installed to top level db folder.

Release 2-9-1 (May 2, 2019)
---------------------------

- IOC shell script for CAEN v895
- MRD\_100 shell script now allows user to separately define ID1 and ID2 macros

Release 2-9 (Mar. 26, 2018)
---------------------------

- add iocsh bindings for commands and debug variables
- Added op/ui/autoconvert and op/opi/autoconvert directories
- Enable Travis CI
- Added support for CAEN V895 16 channel discriminator

Release 2-8-2 (Jul. ,2014)
--------------------------

- device support for Joerger scaler reports whether card is configured for TTL or NIM termination.
- new translations for .adl files to .ui and .opi files
- getFilledBuckets.st: delay after stored beam and before loading bunch pattern was implemented in a way that didn't work.

Release 2-8-1 (Apr. 17,2013)
----------------------------

- Added displays for CSS-BOY and caQtDM
- MRD100\_CantedID.db - New file for canted undulator

Release 2-8 (Aug. 26,2011)
--------------------------

- devScaler.c, devScaler\_debug.c: check that scaler\_state exists at init.   
    Replace VxWorks specific rebootHookAdd() calls with EPICS OSI epicsAtExit() calls.
- Modified RELEASE; deleted RELEASE.arch
- Added .opi display files for CSS-BOY

Release 2-7 (Mar. 30,2010)
--------------------------

- fixes for EPICS 3.14.11
- devScaler.c, devScaler\_debug.c - check that scaler\_state exists at init.

Release 2-6 (Dec. 19,2008)
--------------------------

- removed iocBoot example, and associated build in src
- use devScaler\_debug.c for VSC-series scalers, because many of the new series have hardware trouble.

Release 2-4-4 (Dec. 5, 2006)
----------------------------

Changed:

- The following changes were made to files:   
    1\) devScaler.c, devScaler\_VS.c – changed to new interface between scaler record and device support.   
    2\) Db/Jscaler\* – deleted, these are obsolete and stdApp/Db/scaler\* should be used instead.

Release 2-4-3
-------------

Changed:

- The following changes were made to files:   
    1\) Acromag\_16IO.db – card number is now a macro.   
    2\) devScaler.c – removed debug macro definitions with a variable number of arguments   
    3\) devScaler\_VS.c – removed debug macro definitions with a variable number of arguments   
    4\) vmeRecord.c – removed debug macro definitions with a variable number of arguments

Release 2-4-2
-------------

Changed:

- The following changes were made to files:  
    1\) getFilledBuckets.st – To allow multiple instances.  
    2\) vmeVXSupport.db – Included vmsSupport.dbd.  
    3\) Acro\_bi\_scan.adl – Added “9440” to PV names,  
    4\) BunchClkGen.db – To allow multiple instances.
- The following changes were made to the Heidenhain IK320 device driver.  
    1\) Elminated the need to call drvIK320RegErrStr() from the vxWorks st.cmd file.  
    2\) taskDelay(1)'s and semTake(\*\*, 1)'s with delay of 1 tick changed to 5ms.  
    3\) Allow reading encoder without referencing.

Release 2-4-1
-------------

Changed:

- The devAvme9440 module is OS independent.

Release 2-4-0
-------------

Tested using base 3.14.6 and 3.14.7, seq 2.0.8, calc 2.3, sscan 2.3, std 2.3, and asyn 1.2.

New support:

- MRD-100:  
    1\) devAvmeMRD – Provides new device support and is the first step in moving away from using devA32Vme.  
    2\) msl\_mrd100.db – Modified to reflect new device support module.  
    3\) msl\_mrd101.db – Modified to reflect new device support module.
- Added Joerger VSxx scaler.
- Added support for Acromag 9210 and 9440
- Added new vmeTest test application. It's a menu-driven application that provides simple VME read/write operations.

Release 2-3
-----------

This is the first release of the synApps vme module. Version numbering for this module begins with 2.3 because this module was split from version 2.2 of the std module, and I wanted to retain the CVS histories of module contents.

This version is intended to build with EPICS base 3.14.5. Differences from software as previously released in std 2.2:

- Converted to EPICS 3.14.
- VME\_DAC\_SETTINGS.req, VME\_DAC\_POSITIONS.req - New files

Suggestions and Comments to:   
[David Kline ](mailto:dkline@aps.anl.gov): (dkline@aps.anl.gov)   
Last modified: May 10, 2006
