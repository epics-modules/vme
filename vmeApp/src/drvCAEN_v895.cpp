/* drvCAEN_v895.cpp
 * Driver for CAEN v895 discriminator using asynPortDriver base class
 * Tim Mooney
 * Based on (almost copied verbatim from) code from dac128V.cpp, in Mark Rivers' dac128V module.
 */

#include <vxWorks.h>
#include <vxLib.h>
#include <types.h>
#include <stdlib.h>
#include <stdioLib.h>
#include <lstLib.h>
#include <string.h>
#include <sysLib.h>
#include <vme.h>

#include <asynPortDriver.h>
#include <iocsh.h>
#include <epicsExport.h>

#define MAX_CHANNELS 16

static const char *driverName = "CAEN_v895";

/** This is the class definition for the CAEN_v895 class
  */
class CAEN_v895 : public asynPortDriver {
public:
    CAEN_v895(const char *portName, int baseAddress);

    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    virtual asynStatus getBounds(asynUser *pasynUser, epicsInt32 *low, epicsInt32 *high);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
    virtual void report(FILE *fp, int details);

protected:
    int CAEN_v895_Data;          /**< (asynInt32, r/w) discriminator value in device units */
    #define FIRST_PARAM CAEN_v895_Data
    int CAEN_v895_DoubleData;    /**< (asynFloat64, r/w) discriminator value in device units but double */
    #define LAST_PARAM CAEN_v895_DoubleData
    
private:
    int lastChan;
    int maxValue;
    volatile unsigned short* regs;    
};


#define CAEN_v895DataString        "DATA"
#define CAEN_v895DoubleDataString  "DOUBLE_DATA"

#define NUM_PARAMS (&LAST_PARAM - &FIRST_PARAM + 1)

CAEN_v895::CAEN_v895(const char *portName, int baseAddress)
    : asynPortDriver(portName, MAX_CHANNELS, NUM_PARAMS, 
          asynInt32Mask | asynFloat64Mask | asynDrvUserMask,
          asynInt32Mask | asynFloat64Mask, 
          ASYN_MULTIDEVICE, 1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=1, autoConnect=1 */
          0, 0)  /* Default priority and stack size */
{
    static const char *functionName = "CAEN_v895";
	char *vmeAddress;

    createParam(CAEN_v895DataString,       asynParamInt32,   &CAEN_v895_Data);
    createParam(CAEN_v895DoubleDataString, asynParamFloat64, &CAEN_v895_DoubleData);
    
    if (sysBusToLocalAdrs(VME_AM_EXT_SUP_DATA,
          (char*)baseAddress, (char**)&vmeAddress) != OK){
        printf("Addressing error in CAEN_v895 driver initialization \n");
        return;
    }

    this->regs = (unsigned short *) vmeAddress;

    /* lastChan and maxValue could be set by looking at "model" in the future
     * if models with more channels or more bits are available */
    this->lastChan = MAX_CHANNELS-1;
    this->maxValue = 255;
}

asynStatus CAEN_v895::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int channel;
    static const char *functionName = "writeInt32";

    this->getAddress(pasynUser, &channel);
    if(value<0 || value>this->maxValue || channel<0 || channel>this->lastChan)
        return(asynError);
    setIntegerParam(channel, CAEN_v895_Data, value);
    this->regs[channel] = value;
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
              "%s:%s, port %s, wrote %d to channel %d\n",
              driverName, functionName, this->portName, value, channel);
    return(asynSuccess);
}

asynStatus CAEN_v895::getBounds(asynUser *pasynUser, epicsInt32 *low, epicsInt32 *high)
{
    *low = 0;
    *high = this->maxValue;
    return(asynSuccess);
}

asynStatus CAEN_v895::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    return(this->writeInt32(pasynUser, (epicsInt32) value));
}

asynStatus CAEN_v895::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
    int channel;
    static const char *functionName = "readInt32";

    this->getAddress(pasynUser, &channel);
    if(channel<0 || channel>this->lastChan) return(asynError);
    //*value=this->regs[channel];  module is write only
    *value=0;
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
              "%s:%s, port %s, read %d from channel %d\n",
              driverName, functionName, this->portName, *value, channel);
    return(asynSuccess);
}

asynStatus CAEN_v895::readFloat64(asynUser *pasynUser, epicsFloat64 *value)
{
    epicsInt32 ivalue;
    asynStatus status;

    status = this->readInt32(pasynUser, &ivalue);
    *value = (epicsFloat64)ivalue;
    return(status);
}

/* Report  parameters */
void CAEN_v895::report(FILE *fp, int details)
{
    asynPortDriver::report(fp, details);
    fprintf(fp, "  Port: %s, address %p\n", this->portName, this->regs);
    if (details >= 1) {
        fprintf(fp, "  lastChan=%d, maxValue=%d\n", 
                this->lastChan, this->maxValue);
    }
}

/** Configuration command, called directly or from iocsh */
extern "C" int initCAEN_v895(const char *portName, int baseAddress)
{
    new CAEN_v895(portName,baseAddress);
    return(asynSuccess);
}


static const iocshArg initArg0 = { "Port name",iocshArgString};
static const iocshArg initArg1 = { "Base Address",iocshArgInt};
static const iocshArg * const initArgs[2] = {&initArg0,
                                             &initArg1};
static const iocshFuncDef initFuncDef = {"initCAEN_v895",2,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    initCAEN_v895(args[0].sval, args[1].ival);
}

void drvCAEN_v895Register(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
    epicsExportRegistrar(drvCAEN_v895Register);
}
