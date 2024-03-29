#include "drvIK320ErrStat.h"
#include "drvIK320.h"
void	drvIK320RegErrStr(void)
{
  errSymbolAdd( S_drvIK320_cardBusy,
                "IK320 card busy");
  errSymbolAdd( S_drvIK320_invalidFunc,
                "IK320 invalid VME function call");
  errSymbolAdd( S_drvIK320_asyncStarted,
                "IK320 asynchronous processing started");
  errSymbolAdd( S_drvIK320_invalidParm,
                "IK320 invalid parameter");
  errSymbolAdd( S_drvIK320_HWreadError,
                "IK320 hardware read error");
  errSymbolAdd( S_drvIK320_needsRef,
                "IK320 reference not set");
  errSymbolAdd( S_drvIK320_noSignal,
                "IK320 signal level too low");
  errSymbolAdd( S_drvIK320_maskedChannel,
                "IK320 channel is deactivated");
  errSymbolAdd( S_drvIK320_needsPOST,
                "IK320 needs POST");
  errSymbolAdd( S_drvIK320_timeout,
                "IK320 command timeout");
  errSymbolAdd( S_drvIK320_xferSetX1,
                "xfer marker set on X1");
  errSymbolAdd( S_drvIK320_xferSetX2,
                "xfer marker set on X1");
  errSymbolAdd( S_drvIK320_xferSetX3,
                "xfer marker set on X1");
}
