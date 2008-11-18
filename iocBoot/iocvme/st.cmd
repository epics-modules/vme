# vxWorks startup script


cd ""
< ../nfsCommands
< cdCommands

################################################################################
cd topbin

# If the VxWorks kernel was built using the project facility, the following must
# be added before any C++ code is loaded (see SPR #28980).
sysCplusEnable=1

### Load custom EPICS software from user tree and from share
ld < vme.munch

cd startup
################################################################################
# Tell EPICS all about the record types, device-support modules, drivers,
# etc. in the software we just loaded (vme.munch)
dbLoadDatabase("../../dbd/iocvme.dbd")
iocvme_registerRecordDeviceDriver(pdbbase)

##############################################################################

### Scalers: Joerger VSC8/16
dbLoadRecords("$(VME)/stdApp/Db/Jscaler.db","P=vme:,S=scaler1,C=0")
# Joerger VSC setup parameters: 
#     (1)cards, (2)base address(ext, 256-byte boundary), 
#     (3)interrupt vector (0=disable or  64 - 255)
VSCSetup(1, 0x90000000, 200)

# Heidenhain IK320 VME encoder interpolator
#dbLoadRecords("$(VME)/stdApp/Db/IK320card.db","P=vme:,sw2=card0:,axis=1,switches=41344,irq=3")
#dbLoadRecords("$(VME)/stdApp/Db/IK320card.db","P=vme:,sw2=card0:,axis=2,switches=41344,irq=3")
#dbLoadRecords("$(VME)/stdApp/Db/IK320group.db","P=vme:,group=5")
#drvIK320RegErrStr()

# vme test record
dbLoadRecords("$(VME)/stdApp/Db/vme.db", "P=vme:,Q=vme1")

# Hewlett-Packard 10895A Laser Axis (interferometer)
#dbLoadRecords("$(VME)/stdApp/Db/HPLaserAxis.db", "P=vme:,Q=HPLaser1, C=0")
# hardware configuration
# example: devHP10895LaserAxisConfig(ncards,a16base)
#devHPLaserAxisConfig(2,0x1000)

# Acromag AVME9440
#####################################################
# devAvme9440Config(ncards,a16base,intvec)
#    ncards  = number of cards
#    a16base = a16 base address
#    intvec  = interrupt vector
# For example:
#    devAvme9440Config(1, 0x0400, 0x78)
#
#devAvme9440Config(1, 0x400, 0x78 )
#dbLoadRecords("$(VME)/vmeApp/Db/Acromag_16IO.db","P=vme:,A=1,C=0")

# Bunch Clock Generator (BCG) board (BCG 100)
#####################################################
# BunchClkGenConfigure(card, address)
#   case = card number
#   base = base address of card
# For example
#   BunchClkGenConfigure(0, 0x7000)
#####################################################
#BunchClkGenConfigure(0, 0x7000)
#dbLoadRecords("$(VME)/stdApp/Db/BunchClkGen.db","P=vme:")
#dbLoadRecords("$(VME)/stdApp/Db/BunchClkGenA.db", "UNIT=vme")

# Machine Status Link (MSL) board (MRD 100)
#####################################################
# devAvmeMRDConfig( base, vector, level )                 
#    base   = base address of card              
#    vector = interrupt vector
#    level  = interrupt level
# For Example                                     
#    devAvmeMRDConfig(0xA0000200, 0xA0, 5)             
#####################################################
#  Configure the MSL MRD 100 module.....
#devAvmeMRDConfig(0xA0000200, 0xA0, 5)             
#dbLoadRecords("../../vmeApp/Db/msl_mrd100.db","C=0,S=01,ID1=00,ID2=00us")


###############################################################################
# Set shell prompt (otherwise it is left at mv167 or mv162)
shellPromptSet "iocvme> "
iocLogDisable=1
iocInit

# Bunch clock generator
#seq &getFillPat, "unit=vme"

