TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#PROD_HOST += caExample
#caExample_SRCS	+= caExample.c
#caExample_LIBS	+= ca
#caExample_LIBS	+= Com
#ca_DIR           = $(EPICS_BASE_LIB)
#Com_DIR          = $(EPICS_BASE_LIB)


#=============================
# xxxRecord.h will be created from xxxRecord.dbd

DBDINC += vmeRecord

# <name>.dbd will be created from <name>Include.dbd
DBD += vme.dbd
DBD += vmeSupport.dbd
DBD += vmeVX.dbd
DBD += vmeVXSupport.dbd

#=============================

LIBRARY_IOC += vme

vme_SRCS_vxWorks += devAiVaroc.c drvVarocB.c
vme_SRCS_vxWorks += devXxHP10895LaserAxis.c

# Heidenhain IK320 support
vme_SRCS_vxWorks += devIK320.c
vme_SRCS_vxWorks += drvIK320.c
vme_SRCS_vxWorks += drvIK320ErrStat.c

vme_SRCS_vxWorks += devScaler.c
vme_SRCS_vxWorks += devScaler_VS.c
vme_SRCS_vxWorks += vmeRecord.c
vme_SRCS_vxWorks += devBunchClkGen.c
vme_SRCS_vxWorks += devAoVMI4116.c
vme_SRCS_vxWorks += devAvme9440.c

# For machine-status board
vme_SRCS_vxWorks  += devA32Vme.c

### The following builds SNL programs as components of vme
# NOTE: for suffix .st, code is run through cpp; for .stt, it's not
vme_SRCS += getFilledBuckets.st

vme_LIBS += seq pv
seq_DIR    = $(SNCSEQ_LIB)
pv_DIR     = $(SNCSEQ_LIB)

vme_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application for testing

PROD_IOC = vmeApp

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
vmeApp_SRCS_DEFAULT += vme_registerRecordDeviceDriver.cpp
vmeApp_SRCS_DEFAULT += vmeMain.cpp
vmeApp_SRCS_vxWorks += vmeVX_registerRecordDeviceDriver.cpp

#The following adds support from base/src/vxWorks
vmeApp_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

vmeApp_LIBS += vme
vmeApp_LIBS += std calc sscan mpf
vmeApp_LIBS += seq pv
vmeApp_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

