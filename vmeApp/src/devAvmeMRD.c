/*+ devAvmeMRD.c

                          Argonne National Laboratory
                            APS Operations Division
                     Beamline Controls and Data Acquisition

                             MRD-100 Device Support

 Description
   This module provides device support for the MRD-100 VMEbus module. The
   module is found in the A32/D32 VME address space and supports the ai,
   bi, bo, longin, and mbbi record types. The VME configuration is specificed
   in the INP or OUT fields. Below shows the specific format for the given
   record types:

      ai       "Cn Sr @s,w,t"
      bi       "Cn Sr @s"
      bo       "Cn Sr @s"
      longin   "Cn Sr @s,w"
      mbbi     "Cn Sr @s,w"

      Where:
         n  - Card number (always set to 0).
         r  - Register number 'r'.
         s  - Data start bit number (0..32).
         w  - Data width (1..32).
         t  - Data type which can be either 0 for normal (unipolar) or
              1 (bipolar) for a twos complement signal.

   Diagnostic messages can be output by setting the variable devAvmeMRDDebug
   to the following values:

   When:
      devAvmeMRDDebug  =  0, outputs no messages.
      devAvmeMRDDebug >=  5, outputs hardware initialization messages (unused).
      devAvmeMRDDebug >= 10, outputs record initialization messages (unused).
      devAvmeMRDDebug >= 15, outputs write command messages.
      devAvmeMRDDebug >= 20, outputs read command messages.

   The method devAvmeMRDConfig is called from the startup script to specify the
   VME address and interrupt configuration. It must be called prior to iocInit().
   Below is the calling sequence:

      devAvmeMRDConfig( base, vector, level )

      Where:
         base     - VME A32/D32 address space.
         vector   - Interrupt vector (0..255).
         level    - Interrupt request level (1..7).

      For example:
         devAvmeMRDConfig(0xB0000200, 0xA0, 5)

 Developer notes:
   1) Conventions:
      - Public methods are prepended with 'devAvmeMRD'.
      - Local symbolic constants are prepended with 'MRD__' (two underscores).
        Local methods have '__' (two underscores) in the name.
      - Symbolic constants types:
        K   - constant.
        M   - mask.
        S   - size (in bytes).
        STS - status.
      - Typedefs:
        r   - is a struct.
        u   - is a union.

 =============================================================================
 History:
 Author: Ned D. Arnold, 97-11-21 (devA32VME)
 -----------------------------------------------------------------------------
 22-12-17   DMK   Taken from the existing devA32Vme device support. Reworked
                  driver for clarity and efficiency.
 -----------------------------------------------------------------------------

-*/


/* System related include files */
#include <vme.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* VxWorks related include files */
#ifdef vxWorks
   #include <iv.h>
   #include <vxLib.h>
   #include <sysLib.h>
   #include <intLib.h>
   #include <rebootLib.h>
#else
   #error "devAvmeMRD: Unsupported operating system"
#endif


/* EPICS system related include files */
#include <alarm.h>
#include <devSup.h>
#include <recGbl.h>
#include <epicsPrint.h>
#include <epicsExport.h>


/* EPICS record processing related include files */
#include <dbScan.h>
#include <aiRecord.h>
#include <boRecord.h>
#include <biRecord.h>
#include <mbbiRecord.h>
#include <longinRecord.h>


/* Define general symbolic constants */
#define MRD__K_ACTIVE      (  1 )         /* Record active */
#define MRD__K_INACTIVE    (  0 )         /* Record inactive */

#define MRD__K_OPRREGS     ( 27 )         /* # of operational registers */
#define MRD__K_MAXREGS     ( 30 )         /* Max. # of registers */
#define MRD__K_MAXBITS     ( 32 )         /* Max. width of data */
#define MRD__K_MAXADDR     ( 0xF0000000 ) /* Max. base address */

#define MRD__K_BIPOLAR     (  1 )         /* AI/AO biplor value */
#define MRD__K_UNIPOLAR    (  0 )         /* AI/AO unipolar value */

#define MRD__K_SHOWNONE    (  0 )         /* Diagnostic level indicators */
#define MRD__K_SHOWWRITE   ( 15 )
#define MRD__K_SHOWREAD    ( 20 )

#define MRD__K_MAXCOL      (  3 )         /* Max. # of columns in report */


/* Define symbolic constants for device-support method completion status */
#define MRD__STS_OK        (  0 )         /* OK */
#define MRD__STS_OKNOVAL   (  2 )         /* OK - do not modify VAL */
#define MRD__STS_ERROR     (  ERROR )     /* FAILURE */


/* Declare MRD-100 register map and symbolic constants */
#define MRD__M_IVEC        ( 0xFF )       /* Interrupt Vector mask */
#define MRD__M_ICR         ( 0x10 )       /* Interrupt Control Register mask */
#define MRD__M_IDC         ( 0x0F )       /* Interrupt Detect/Clear mask */

#define MRD__K_IVEC        ( 0xFF )       /* Interrupt Vector value */
#define MRD__K_ICR         ( 0x10 )       /* Interrupt Control Register value */
#define MRD__K_IDC         ( 0x0F )       /* Interrupt Detect/Clear value */

#define MRD__S_REGS        ( sizeof(uMRD__REGS) )

typedef union umrd__regs
{
   unsigned long data[MRD__K_MAXREGS];    /* Linear register map */

   struct rregs
   {
      unsigned long regs[MRD__K_OPRREGS]; /* Operational Registers   (0..26) */
      unsigned long IDC;                  /* Interrupt Dectect/Clear    (27) */
      unsigned long ICR;                  /* Interrupt Control          (28) */
      unsigned long IVEC;                 /* Interrupt Vector           (29) */
   } rREGS;

} uMRD__REGS;

/* Declare MRD info structure */
#define MRD__S_INFO        ( sizeof(rMRD__INFO) )

typedef struct rmrd__info
{
   uMRD__REGS*    base;          /* Base address */
   unsigned long  card;          /* Card number */
   epicsMutexId   lock;          /* Access sync. */
   unsigned long  instCount;     /* Instance count */
   IOSCANPVT      ioscanpvt;     /* scan IO event */

} rMRD__INFO;

/* Declare MRD record instance structure */
#define MRD__S_INST        ( sizeof(rMRD__INST) )

typedef struct rmrd__inst
{
   rMRD__INFO*    pmrd;          /* Pointer to MRD info */
   unsigned long* pdata;         /* Pointer to data register */
   unsigned long  dmask;         /* Data mask */
   unsigned long  dsbn;          /* Data start bit number */
   unsigned long  dwid;          /* Data width */
   unsigned long  dtyp;          /* Data type (0=unipolar / 1=bipolar) */

} rMRD__INST;


/* Declare DSET data structure */
typedef struct rmrd__dset
{
   long      number;             /* # of method pointers */
   DEVSUPFUN report;             /* Reports device support information */
   DEVSUPFUN init;               /* Device support initialization */
   DEVSUPFUN init_record;        /* Record support initialization */
   DEVSUPFUN get_ioint_info;     /* Associate interrupt source with record */
   DEVSUPFUN read_write;         /* Read/Write method */
   DEVSUPFUN specialLinconv;     /* Special processing for AO/AI records */

} rMRD__DSET;


/* Define local variants */
static rMRD__INFO    mrd__info;


/* Declare local forward references for device-processing methods */
static void mrd__isr( );
static long mrd__read( );
static long mrd__write( );
static void mrd__reboot( );
static long mrd__report( );


/* Declare local forward references for record-processing methods */
static long ai__init( );
static long ai__read( );

static long bi__init( );
static long bi__read( );
static long bi__getIntInfo( );

static long bo__init( );
static long bo__write( );

static long longin__init( );
static long longin__read( );

static long mbbi__init( );
static long mbbi__read( );


/* Define DSET structures */
static rMRD__DSET devAiAvmeMRD   = {6, NULL,        NULL, ai__init,      NULL,           ai__read,       NULL};
static rMRD__DSET devBiAvmeMRD   = {5, mrd__report, NULL, bi__init,      bi__getIntInfo, bi__read,       NULL};
static rMRD__DSET devBoAvmeMRD   = {5, NULL,        NULL, bo__init,      NULL,           bo__write,      NULL};
static rMRD__DSET devLiAvmeMRD   = {5, NULL,        NULL, longin__init,  NULL,           longin__read,   NULL};
static rMRD__DSET devMbbiAvmeMRD = {5, NULL,        NULL, mbbi__init,    NULL,           mbbi__read,     NULL};


/* Publish DSET structure references to EPICS */
epicsExportAddress(dset, devAiAvmeMRD);
epicsExportAddress(dset, devBiAvmeMRD);
epicsExportAddress(dset, devBoAvmeMRD);
epicsExportAddress(dset, devLiAvmeMRD);
epicsExportAddress(dset, devMbbiAvmeMRD);


/* Define global variants */
unsigned long devAvmeMRDDebug = 0;


/****************************************************************************
 * Define device-specific methods
 ****************************************************************************/


/*
 * mrd__isr()
 *
 * Description:
 *    This method is executed when an interrupt occurs from the MRD-100.
 *
 * Input Parameters:
 *    pmrd  - Address of MRD information structure.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    None.
 *
 * Developer notes:
 *
 */
static void mrd__isr( pmrd )
rMRD__INFO* pmrd;
{
   /* Process scan IO requests */
   scanIoRequest( pmrd->ioscanpvt );

   /* Reenable interrupts */
   pmrd->base->rREGS.IDC = MRD__K_IDC;
}


/*
 * mrd__reboot()
 *
 * Description:
 *    This method is executed when a 'reboot' command or ^X is entered from
 *    the VxWorks shell. It will disable all interrupts.
 *
 * Input Parameters:
 *    None.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    None.
 *
 * Developer notes:
 *
 */
static void mrd__reboot( void )
{
   mrd__info.base->rREGS.ICR = 0;
}


/*
 * mrd__report()
 *
 * Description:
 *    This method is called from the dbior shell command. It outputs
 *    information about the MRD-100 configuration.
 *
 * Input Parameters:
 *    level - Indicates interest level of information.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status: MRD__STS_OK
 *
 * Developer notes:
 *
 */
static long mrd__report( level )
int level;
{
unsigned int i, r;


   /* Output MRD-100 information */
   printf( "\nMRD-100 Configuration\n" );
   printf( "\tBase address                    - 0x%8.8X\n", (unsigned int)mrd__info.base );
   printf( "\tInterrupt vector                - 0x%4.4X\n", (unsigned int)mrd__info.base->rREGS.IVEC );
   printf( "\tInterrupt control register      - 0x%4.4X\n", (unsigned int)mrd__info.base->rREGS.ICR );
   printf( "\tInterrupt detect/clear register - 0x%4.4X\n", (unsigned int)mrd__info.base->rREGS.IDC );
   printf( "\tRecord instance count           - %d\n",      (unsigned int)mrd__info.instCount );

   printf( "\tOperational register contents:\n" );
   for( i = 0, r = 1; i < MRD__K_OPRREGS; ++i, ++r )
   {
      /* Output register contents */
      printf( "\t%2.2d - 0x%8.8X", i, (unsigned int)mrd__info.base->data[i] );

      /* Evaluate row break */
      if( (r % MRD__K_MAXCOL) == 0 )
      {
         printf( "\n" );
      }
   }
   printf( "\n" );

   /* Return completion status */
   return( MRD__STS_OK );

}


/*
 * mrd__write()
 *
 * Description:
 *    This method performs the longword write to the MRD-100 given the address.
 *
 * Input Parameters:
 *    paddr    - Address to write.
 *    mask     - Mask to preserve existing bits.
 *    value    - Value to write.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status: MRD__STS_OK
 *
 * Developer notes:
 *
 */
static long mrd__write( paddr, mask, value )
unsigned long* paddr;
unsigned long  mask;
unsigned long  value;
{

   /* Acquire access mutex */
   epicsMutexMustLock( mrd__info.lock );

   /* Write raw value given mask */
   *paddr = (*paddr & ~mask) | (value & mask);

   /* Release access mutex */
   epicsMutexUnlock( mrd__info.lock );

   /* Output diagnostic */
   if( devAvmeMRDDebug >= MRD__K_SHOWWRITE )
   {
      epicsPrintf( "devAvmeMRD:mrd__write(): Wrote 0x%8.8X to 0x%8.8X\n", (unsigned int)*paddr, (unsigned int)paddr );
   }

   /* Return completion status */
   return( MRD__STS_OK );

} /* end-method: mrd__write() */


/*
 * mrd__read()
 *
 * Description:
 *    This method performs the longword read from the MRD-100 given the address.
 *
 * Input Parameters:
 *    paddr    - Address to write.
 *    mask     - Mask to preserve existing bits.
 *
 * Output Parameters:
 *    value    - Address of value read.
 *
 * Returns:
 *    Completion status: MRD__STS_OK
 *
 * Developer notes:
 *
 */
static long mrd__read( paddr, mask, value )
unsigned long* paddr;
unsigned long  mask;
unsigned long* value;
{

   /* Read raw value from MRD and mask */
   *value = *paddr & mask;

   /* Output diagnostic */
   if( devAvmeMRDDebug >= MRD__K_SHOWREAD )
   {
      epicsPrintf( "devAvmeMRD:mrd__read(): Read 0x%8.8X from 0x%8.8X\n", (unsigned int)*value, (unsigned int)paddr );
   }

   /* Return completion status */
   return( MRD__STS_OK );

} /* end-method: mrd__read() */


/****************************************************************************
 * Define record-specific methods
 ****************************************************************************/


/*
 * ai__init()
 *
 * Description:
 *    This method performs the initialization for an AI record.
 *
 * Input Parameters:
 *    pai   - Address of aiRecord.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_ERROR
 *
 * Developer notes:
 *
 */
static long ai__init( pai )
struct aiRecord* pai;
{
long sts = MRD__STS_OK;


   /* Evaluate input type */
   switch( pai->inp.type )
   {
      case VME_IO:
      {
      unsigned long  type;
      unsigned long  start;
      unsigned long  width;
      rMRD__INST*    pinst;

         /* Evaluate base address */
         if( mrd__info.base == NULL )
         {
            epicsPrintf( "devAvmeMRD:ai__init(): Base address not specified for %s\n", pai->name );
            sts         = MRD__STS_ERROR;
            pai->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate card number */
         if( pai->inp.value.vmeio.card != mrd__info.card )
         {
            epicsPrintf( "devAvmeMRD:ai__init(): Invalid card number %d for %s\n", (unsigned int)pai->inp.value.vmeio.card, pai->name );
            sts         = MRD__STS_ERROR;
            pai->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate register number */
         if( pai->inp.value.vmeio.signal >= MRD__K_MAXREGS )
         {
            epicsPrintf( "devAvmeMRD:ai__init(): Invalid register number %d specified for %s\n", pai->inp.value.vmeio.signal, pai->name );
            sts         = MRD__STS_ERROR;
            pai->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Acquire data start bit number, width, and data type */
         sts = sscanf( pai->inp.value.vmeio.parm, "%d,%d,%d", (unsigned int*)&start, (unsigned int*)&width, (unsigned int*)&type );

         /* Evaluate completion status */
         if( sts != 3 )
         {
            epicsPrintf( "devAvmeMRD:ai__init(): Completion status failure %d from sscanf() for %s\n", (unsigned int)sts, pai->name );
            sts         = MRD__STS_ERROR;
            pai->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate starting bit number, width, and type */
         if( (start >= MRD__K_MAXBITS) || (width <= 0) || ((start + width) > MRD__K_MAXBITS) || (type < 0 || type > 1) )
         {
            epicsPrintf( "devAvmeMRD:ai__init(): Invalid bit number %d specified for %s\n", (unsigned int)start, pai->name );
            sts         = MRD__STS_ERROR;
            pai->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Allocate memory for MRD record instance */
         pinst = calloc( 1, MRD__S_INST );
         if( pinst == NULL )
         {
            epicsPrintf( "devAvmeMRD:ai__init(): Failure to allocate instance memory for %s\n", pai->name );
            sts         = MRD__STS_ERROR;
            pai->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Initialize MRD record instance structure */
         pinst->pmrd    = &mrd__info;
         pinst->pdata   = &mrd__info.base->data[pai->inp.value.vmeio.signal];
         pinst->dsbn    = start;
         pinst->dtyp    = type;
         pinst->dwid    = width;
         pinst->dmask   = ((1 << width) - 1) << start;

         /* Update AI record structure */
         pai->dpvt      = pinst;
         pai->eslo      = (pai->eguf - pai->egul) / (pow(2, width) - 1);

         if( type == MRD__K_BIPOLAR )
         {
            pai->roff = pow(2, (width - 1));
         }

         /* Increment record instance count */
         ++mrd__info.instCount;
         break;
      }

      default:
      {
         sts         = MRD__STS_ERROR;
         pai->pact   = MRD__K_ACTIVE;

         epicsPrintf( "devAvmeMRD:ai__init(): Invalid input specified for %s\n", pai->name );
         break;
      }

   } /* end-switch: ( pai->inp.type ) */

   /* Return completion status */
   return( sts );

} /* end-method: ai__init() */


/*
 * ai__read()
 *
 * Description:
 *    This method performs the read for an AI record.
 *
 * Input Parameters:
 *    pai   - Address of aiRecord.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_OKNOVAL
 *
 * Developer notes:
 *
 */
static long ai__read( pai )
struct aiRecord* pai;
{
long           sts;
unsigned long  readback;
rMRD__INST*    pinst;


   /* Initialize local variants */
   pinst = (rMRD__INST*)pai->dpvt;

   /* Read register value */
   sts = mrd__read( pinst->pdata, pinst->dmask, &readback );
   if( sts == MRD__STS_OK )
   {
      pai->rval   = readback >> pinst->dsbn;

      /* Process sign extensions for bipolar */
      if( pinst->dtyp == MRD__K_BIPOLAR )
      {
      unsigned long value = ( 2 << (pinst->dwid - 2) );

         if( pai->rval & value )
         {
            pai->rval   |= ((2 << 31) - value) * 2;
         }

      }

   }
   else
   {
      sts   = MRD__STS_OKNOVAL;

      /* Set an alarm for the record */
      recGblSetSevr( pai, READ_ALARM, INVALID_ALARM);
   }

   /* Return completion status */
   return( sts );

} /* end-method: ai__read() */


/*
 * bi__init()
 *
 * Description:
 *    This method performs the initialization for a BI record.
 *
 * Input Parameters:
 *    pbi   - Address of the BI record.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_ERROR
 *
 * Developer notes:
 *
 */
static long bi__init( pbi )
struct biRecord* pbi;
{
long sts = MRD__STS_OK;


   /* Evaluate input type */
   switch( pbi->inp.type )
   {
      case VME_IO:
      {
      unsigned long  sbn;
      unsigned long  readback;
      rMRD__INST*    pinst;

         /* Evaluate base address */
         if( mrd__info.base == NULL )
         {
            epicsPrintf( "devAvmeMRD:bi__init(): Base address not specified for %s\n", pbi->name );
            sts         = MRD__STS_ERROR;
            pbi->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate card number */
         if( pbi->inp.value.vmeio.card != mrd__info.card )
         {
            epicsPrintf( "devAvmeMRD:bi__init(): Invalid card number %d for %s\n", (unsigned int)pbi->inp.value.vmeio.card, pbi->name );
            sts         = MRD__STS_ERROR;
            pbi->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate register number */
         if( pbi->inp.value.vmeio.signal >= MRD__K_MAXREGS )
         {
            epicsPrintf( "devAvmeMRD:bi__init(): Invalid register number %d specified for %s\n", pbi->inp.value.vmeio.signal, pbi->name );
            sts         = MRD__STS_ERROR;
            pbi->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Acquire data start bit number */
         sts = sscanf( pbi->inp.value.vmeio.parm, "%d", (unsigned int*)&sbn );

         /* Evaluate completion status */
         if( sts != 1 )
         {
            epicsPrintf( "devAvmeMRD:bi__init(): Completion status failure %d from sscanf() for %s\n", (unsigned int)sts, pbi->name );
            sts         = MRD__STS_ERROR;
            pbi->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate starting bit number */
         if( sbn >= MRD__K_MAXBITS )
         {
            epicsPrintf( "devAvmeMRD:bi__init(): Invalid bit number %d specified for %s\n", (unsigned int)sbn, pbi->name );
            sts         = MRD__STS_ERROR;
            pbi->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Allocate memory for MRD record instance */
         pinst = calloc( 1, MRD__S_INST );
         if( pinst == NULL )
         {
            epicsPrintf( "devAvmeMRD:bi__init(): Failure to allocate instance memory for %s\n", pbi->name );
            sts         = MRD__STS_ERROR;
            pbi->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Initialize MRD record instance structure */
         pinst->pmrd    = &mrd__info;
         pinst->pdata   = &mrd__info.base->data[pbi->inp.value.vmeio.signal];
         pinst->dmask   = 1 << sbn;

         /* Update BI record structure */
         pbi->dpvt      = pinst;
         pbi->mask      = pinst->dmask;

         /* Initialize record raw input value */
         sts = mrd__read( pinst->pdata, pinst->dmask, &readback );
         if( sts == MRD__STS_OK )
         {
            pbi->rval   = readback;
         }
         else
         {
            sts         = MRD__STS_OKNOVAL;
         }

         /* Increment record instance count */
         ++mrd__info.instCount;
         break;
      }

      default:
      {
         sts         = MRD__STS_ERROR;
         pbi->pact   = MRD__K_ACTIVE;

         epicsPrintf( "devAvmeMRD:bi__init(): Invalid INP type specified for %s\n", pbi->name );
         break;
      }

   } /* end-switch: ( pbi->inp.type ) */

   /* Return completion status */
   return( sts );

} /* end-method: bi__init() */


/*
 * bi__read()
 *
 * Description:
 *    This method performs the read for a BI record.
 *
 * Input Parameters:
 *    pbi   - Address of the BI record.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_OKNOVAL
 *
 * Developer notes:
 *
 */
static long bi__read( pbi )
struct biRecord* pbi;
{
long           sts;
unsigned long  readback;
rMRD__INST*    pinst;


   /* Initialize local variants */
   pinst = (rMRD__INST*)pbi->dpvt;

   /* Read register value */
   sts = mrd__read( pinst->pdata, pinst->dmask, &readback );
   if( sts == MRD__STS_OK )
   {
      pbi->rval   = readback;
   }
   else
   {
      sts         = MRD__STS_OKNOVAL;

      /* Set an alarm for the record */
      recGblSetSevr( pbi, READ_ALARM, INVALID_ALARM);
   }

   /* Return completion status */
   return( sts );

} /* end-method: bi__read() */


/*
 * bi__getIntInfo()
 *
 * Description:
 *    This method is called by the IO interrupt scan task. It is passed a
 *    cmd value of 0 or 1 when the associated record is being put in or
 *    taken out of an IO scan list.
 *
 * Input Parameters:
 *    cmd   - Command.
 *    pbi   - Associated BI record.
 *    ppvt  - Address pointer to IO scan structure.
 *
 * Output Parameters:
 *    None.
 *
 * Developer notes:
 *
 */
static long bi__getIntInfo( cmd, pbi, ppvt )
int             cmd;
struct biRecord *pbi;
IOSCANPVT       *ppvt;
{
long sts;

   /* Acquire IO scan address */
   if( mrd__info.ioscanpvt )
   {
      sts   = MRD__STS_OK;

      *ppvt = mrd__info.ioscanpvt;
   }
   else
   {
      sts   = MRD__STS_ERROR;
   }

   /* Return completion status */
   return( sts );

} /* end-method: bi__getIntInfo() */


/*
 * bo__init()
 *
 * Description:
 *    This method performs the initialization for a BO record.
 *
 * Input Parameters:
 *    pbo   - Address of BO record.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_ERROR
 *
 * Developer notes:
 *
 */
static long bo__init( pbo )
struct boRecord *pbo;
{
long sts = MRD__STS_OK;


   /* Evaluate input type */
   switch( pbo->out.type )
   {
      case VME_IO:
      {
      unsigned long  sbn;
      unsigned long  readback;
      rMRD__INST*    pinst;

         /* Evaluate base address */
         if( mrd__info.base == NULL )
         {
            epicsPrintf( "devAvmeMRD:bo__init(): Base address not specified for %s\n", pbo->name );
            sts         = MRD__STS_ERROR;
            pbo->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate card number */
         if( pbo->out.value.vmeio.card != mrd__info.card )
         {
            epicsPrintf( "devAvmeMRD:bo__init(): Invalid card number %d for %s\n", (unsigned int)pbo->out.value.vmeio.card, pbo->name );
            sts         = MRD__STS_ERROR;
            pbo->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate register number */
         if( pbo->out.value.vmeio.signal >= MRD__K_MAXREGS )
         {
            epicsPrintf( "devAvmeMRD:bo__init(): Invalid register number %d specified for %s\n", pbo->out.value.vmeio.signal, pbo->name );
            sts         = MRD__STS_ERROR;
            pbo->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Acquire data start bit number */
         sts = sscanf( pbo->out.value.vmeio.parm, "%d", (unsigned int*)&sbn );

         /* Evaluate completion status */
         if( sts != 1 )
         {
            epicsPrintf( "devAvmeMRD:bo__init(): Completion status failure %d from sscanf() for %s\n", (unsigned int)sts, pbo->name );
            sts         = MRD__STS_ERROR;
            pbo->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate starting bit number */
         if( sbn >= MRD__K_MAXBITS )
         {
            epicsPrintf( "devAvmeMRD:bo__init(): Invalid bit number %d specified for %s\n", (unsigned int)sbn, pbo->name );
            sts         = MRD__STS_ERROR;
            pbo->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Allocate memory for MRD record instance */
         pinst = calloc( 1, MRD__S_INST );
         if( pinst == NULL )
         {
            epicsPrintf( "devAvmeMRD:bo__init(): Failure to allocate instance memory for %s\n", pbo->name );
            sts         = MRD__STS_ERROR;
            pbo->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Initialize MRD record instance structure */
         pinst->pmrd    = &mrd__info;
         pinst->pdata   = &mrd__info.base->data[pbo->out.value.vmeio.signal];
         pinst->dmask   = 1 << sbn;

         /* Update BO record structure */
         pbo->dpvt      = pinst;
         pbo->mask      = pinst->dmask;

         /* Initialize record raw input value */
         sts = mrd__read( pinst->pdata, pinst->dmask, &readback );
         if( sts == MRD__STS_OK )
         {
            pbo->rbv = readback;
         }
         else
         {
            sts      = MRD__STS_OKNOVAL;
         }

         /* Increment record instance count */
         ++mrd__info.instCount;
         break;
      }

      default:
      {
         sts         = MRD__STS_ERROR;
         pbo->pact   = MRD__K_ACTIVE;

         epicsPrintf( "devAvmeMRD:bo__init(): Invalid OUT type specified for %s\n", pbo->name );
         break;
      }

   } /* end-switch: ( pbo->inp.type ) */

   /* Return completion status */
   return( sts );

} /* end-method: bo__init() */


/*
 * bo__write()
 *
 * Description:
 *    This method performs the write for a BO record.
 *
 * Input Parameters:
 *    pbo   - Address of BO record.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_OKNOVAL
 *
 * Developer notes:
 *
 */
static long bo__write( pbo )
struct boRecord *pbo;
{
long        sts;
rMRD__INST* pinst;


   /* Initialize local variants */
   pinst = (rMRD__INST*)pbo->dpvt;

   /* Write register value */
   sts = mrd__write( pinst->pdata, pinst->dmask, pbo->rval );
   if( sts == MRD__STS_OK )
   {
   unsigned long  readback;

      /* Read register value */
      sts = mrd__read( pinst->pdata, pinst->dmask, &readback );
      if( sts == MRD__STS_OK )
      {
         /* Assign readback to record */
         pbo->rbv = readback;

         /* Return completion status */
         return( MRD__STS_OK );
      }

   }

   /* Set an alarm for the record */
   recGblSetSevr( pbo, WRITE_ALARM, INVALID_ALARM);

   /* Return completion status */
   return( MRD__STS_OKNOVAL );

} /* end-method: bo__write() */


/*
 * longin__init()
 *
 * Description:
 *    This method performs the initialization for a LONGIN record.
 *
 * Input Parameters:
 *    pli   - Address of LOGIN record.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_OKNOVAL
 *
 * Developer notes:
 *
 */
static long longin__init( pli )
struct longinRecord* pli;
{
long sts = MRD__STS_OK;


   /* Evaluate input type */
   switch( pli->inp.type )
   {
      case VME_IO:
      {
      unsigned long  start;
      unsigned long  width;
      rMRD__INST*    pinst;

         /* Evaluate base address */
         if( mrd__info.base == NULL )
         {
            epicsPrintf( "devAvmeMRD:longin__init(): Base address not specified for %s\n", pli->name );
            sts         = MRD__STS_ERROR;
            pli->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate card number */
         if( pli->inp.value.vmeio.card != mrd__info.card )
         {
            epicsPrintf( "devAvmeMRD:longin__init(): Invalid card number %d for %s\n", (unsigned int)pli->inp.value.vmeio.card, pli->name );
            sts         = MRD__STS_ERROR;
            pli->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate register number */
         if( pli->inp.value.vmeio.signal >= MRD__K_MAXREGS )
         {
            epicsPrintf( "devAvmeMRD:longin__init(): Invalid register number %d specified for %s\n", pli->inp.value.vmeio.signal, pli->name );
            sts         = MRD__STS_ERROR;
            pli->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Acquire data start bit number and data width */
         sts = sscanf( pli->inp.value.vmeio.parm, "%d,%d", (unsigned int*)&start, (unsigned int*)&width );

         /* Evaluate completion status */
         if( sts != 2 )
         {
            epicsPrintf( "devAvmeMRD:longin__init(): Completion status failure %d from sscanf() for %s\n", (unsigned int)sts, pli->name );
            sts         = MRD__STS_ERROR;
            pli->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate data start bit number and width */
         if( (start >= MRD__K_MAXBITS) || (width <= 0) || ((start + width) > MRD__K_MAXBITS) )
         {
            epicsPrintf( "devAvmeMRD:longin__init(): Invalid bit number %d specified for %s\n", (unsigned int)start, pli->name );
            sts         = MRD__STS_ERROR;
            pli->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Allocate memory for MRD record instance */
         pinst = calloc( 1, MRD__S_INST );
         if( pinst == NULL )
         {
            epicsPrintf( "devAvmeMRD:longin__init(): Failure to allocate instance memory for %s\n", pli->name );
            sts         = MRD__STS_ERROR;
            pli->pact   = MRD__K_ACTIVE;

            break;
         }

         /* Initialize MRD record instance structure */
         pinst->pmrd    = &mrd__info;
         pinst->pdata   = &mrd__info.base->data[pli->inp.value.vmeio.signal];
         pinst->dsbn    = start;
         pinst->dmask   = ((1 << width) - 1) << start;

         /* Update LONGIN record structure */
         pli->dpvt      = pinst;

         /* Increment record instance count */
         ++mrd__info.instCount;
         break;
      }

      default:
      {
         sts         = MRD__STS_ERROR;
         pli->pact   = MRD__K_ACTIVE;

         epicsPrintf( "devAvmeMRD:longin__init(): Invalid INP type specified for %s\n", pli->name );
         break;
      }

   } /* end-switch: ( pli->inp.type ) */

   /* Return completion status */
   return( sts );

} /* end-method: longin__init() */


/*
 * longin_read()
 *
 * Description:
 *    This method performs the read for a LONGIN record.
 *
 * Input Parameters:
 *    pli   - Address of LONGIN record.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_OKNOVAL
 *
 * Developer notes:
 *
 */
static long longin__read( pli )
struct longinRecord* pli;
{
long           sts;
unsigned long  readback;
rMRD__INST*    pinst;


   /* Initialize local variants */
   pinst = (rMRD__INST*)pli->dpvt;

   /* Read register value */
   sts = mrd__read( pinst->pdata, pinst->dmask, &readback );
   if( sts == MRD__STS_OK )
   {
      pli->val = readback >> pinst->dsbn;
      pli->udf = 0;
   }
   else
   {
      sts      = MRD__STS_OKNOVAL;

      /* Set an alarm for the record */
      recGblSetSevr( pli, READ_ALARM, INVALID_ALARM);
   }

   /* Return completion status */
   return( sts );

} /* end-method: longin__read() */


/*
 * mbbi__init()
 *
 * Description:
 *    This method performs the initialization for a MBBI record.
 *
 * Input Parameters:
 *    pmbbi - Address of MBBI record.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_ERROR
 *
 * Developer notes:
 *    1) The mask (data width) is determined from the NOBT field.
 *
 */
static long mbbi__init( pmbbi )
struct mbbiRecord* pmbbi;
{
long sts = MRD__STS_OK;


   /* Evaluate input type */
   switch( pmbbi->inp.type )
   {
      case VME_IO:
      {
      unsigned long  sbn;
      unsigned long  readback;
      rMRD__INST*    pinst;

         /* Evaluate base address */
         if( mrd__info.base == NULL )
         {
            epicsPrintf( "devAvmeMRD:mbbi__init(): Base address not specified for %s\n", pmbbi->name );
            sts         = MRD__STS_ERROR;
            pmbbi->pact = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate card number */
         if( pmbbi->inp.value.vmeio.card != mrd__info.card )
         {
            epicsPrintf( "devAvmeMRD:mbbi__init(): Invalid card number %d for %s\n", (unsigned int)pmbbi->inp.value.vmeio.card, pmbbi->name );
            sts         = MRD__STS_ERROR;
            pmbbi->pact = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate register number */
         if( pmbbi->inp.value.vmeio.signal >= MRD__K_MAXREGS )
         {
            epicsPrintf( "devAvmeMRD:mbbi__init(): Invalid register number %d specified for %s\n", pmbbi->inp.value.vmeio.signal, pmbbi->name );
            sts         = MRD__STS_ERROR;
            pmbbi->pact = MRD__K_ACTIVE;

            break;
         }

         /* Acquire data start bit number */
         sts = sscanf( pmbbi->inp.value.vmeio.parm, "%d", (unsigned int*)&sbn );

         /* Evaluate completion status */
         if( sts != 1 )
         {
            epicsPrintf( "devAvmeMRD:mbbi__init(): Completion status failure %d from sscanf() for %s\n", (unsigned int)sts, pmbbi->name );
            sts         = MRD__STS_ERROR;
            pmbbi->pact = MRD__K_ACTIVE;

            break;
         }

         /* Evaluate starting bit number */
         if( sbn >= MRD__K_MAXBITS )
         {
            epicsPrintf( "devAvmeMRD:mbbi__init(): Invalid bit number %d specified for %s\n", (unsigned int)sbn, pmbbi->name );
            sts         = MRD__STS_ERROR;
            pmbbi->pact = MRD__K_ACTIVE;

            break;
         }

         /* Allocate memory for MRD record instance */
         pinst = calloc( 1, MRD__S_INST );
         if( pinst == NULL )
         {
            epicsPrintf( "devAvmeMRD:mbbi__init(): Failure to allocate instance memory for %s\n", pmbbi->name );
            sts         = MRD__STS_ERROR;
            pmbbi->pact = MRD__K_ACTIVE;

            break;
         }

         /* Initialize MRD record instance structure */
         pinst->pmrd    = &mrd__info;
         pinst->pdata   = &mrd__info.base->data[pmbbi->inp.value.vmeio.signal];
         pinst->dmask   = pmbbi->mask << sbn;

         /* Update MBBI record structure */
         pmbbi->dpvt    = pinst;
         pmbbi->shft    = sbn;
         pmbbi->mask    = pinst->dmask;

         /* Initialize record raw input value */
         sts = mrd__read( pinst->pdata, pinst->dmask, &readback );
         if( sts == MRD__STS_OK )
         {
            pmbbi->rval = readback;
         }
         else
         {
            sts         = MRD__STS_OKNOVAL;
         }

         /* Increment record instance count */
         ++mrd__info.instCount;
         break;
      }

      default:
      {
         sts         = MRD__STS_ERROR;
         pmbbi->pact = MRD__K_ACTIVE;

         epicsPrintf( "devAvmeMRD:mbbi__init(): Invalid INP type specified for %s\n", pmbbi->name );
         break;
      }

   } /* end-switch: ( pmbbi->inp.type ) */

   /* Return completion status */
   return( sts );

} /* end-method: mbbi__init() */


/*
 * mbbi__read()
 *
 * Description:
 *    This method performs the read for a MBBI record.
 *
 * Input Parameters:
 *    pmbbi    - Address of MBBI record.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_OKNOVAL
 *
 * Developer notes:
 *
 */
static long mbbi__read( pmbbi )
struct mbbiRecord* pmbbi;
{
long           sts;
unsigned long  readback;
rMRD__INST*    pinst;


   /* Initialize local variants */
   pinst = (rMRD__INST*)pmbbi->dpvt;

   /* Read register value */
   sts = mrd__read( pinst->pdata, pinst->dmask, &readback );
   if( sts == MRD__STS_OK )
   {
      pmbbi->rval = readback;
   }
   else
   {
      sts         = MRD__STS_OKNOVAL;

      /* Set an alarm for the record */
      recGblSetSevr( pmbbi, READ_ALARM, INVALID_ALARM);
   }

   /* Return completion status */
   return( sts );

} /* end-method: mbbi__read() */


/****************************************************************************
 * Define public interface methods
 ****************************************************************************/


/*
 * devAvmeMRDReport()
 *
 * Description:
 *    This method calls the underlying report method mrd__report(). Please see
 *    its description.
 *
 * Input Parameters:
 *    None.
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   mrd__report()
 *
 * Developer notes:
 *
 */
int devAvmeMRDReport( )
{
   return( mrd__report( 0 ) );
}

/*
 * devAvmeMRDConfig()
 *
 * Description:
 *    This method is called from the startup script to initialize
 *    the MRD-100. It should be called prior to any usage of the
 *    module.
 *
 * Input Parameters:
 *    base     - Base A32/D32 VMEbus address.
 *    vector   - Interrupt vector (0..255).
 *    level    - Interrupt request level (1..7)
 *
 * Output Parameters:
 *    None.
 *
 * Returns:
 *    Completion status:   MRD__STS_OK
 *                         MRD__STS_ERROR
 *
 * ssDeveloper notes:
 *
 */
int devAvmeMRDConfig( base, vector, level )
unsigned long base;
unsigned long vector;
unsigned long level;
{
STATUS               sts;
unsigned long        value;
static unsigned char init = FALSE;


   /* Evaluate card initialization */
   if( init == FALSE )
   {
      init = TRUE;
   }
   else
   {
      epicsPrintf( "\ndevAvmeMRDConfig(): MRD-100 already initialized\n" );

      return( MRD__STS_OK );
   }

   /* Evaluate passed base address */
   if( base >= MRD__K_MAXADDR )
   {
      epicsPrintf( "devAvmeMRDConfig(): Invalid base address 0x%8.8X\n", (unsigned int)base );

      return( MRD__STS_ERROR );
   }

   /* Evaluate passed interrupt vector */
   if( (vector == 0) || (vector > MRD__K_IVEC) )
   {
      epicsPrintf( "devAvmeMRDConfig(): Invalid vector 0x%8.8X\n", (unsigned int)vector );

      return( MRD__STS_ERROR );
   }

   /* Evaluate passed interrupt level */
   if( (level < 1) || (level > 7) )
   {
      epicsPrintf( "devAvmeMRDConfig(): Invalid Interrupt level %d\n", (unsigned int)level );

      return( MRD__STS_ERROR );
   }

   /* Map base address into local space */
   sts = sysBusToLocalAdrs( VME_AM_EXT_USR_DATA, (char*)base, (char**)&mrd__info.base );
   if( sts )
   {
      memset( &mrd__info, 0, MRD__S_INFO );
      epicsPrintf( "devAvmeMRDConfig(): Failure to map address 0x%8.8X\n", (unsigned int)base );

      return( MRD__STS_ERROR );
   }

   /* Probe (read) base address */
   sts = vxMemProbe( (char*)mrd__info.base, VX_READ, sizeof(value), (char*)&value );
   if( sts )
   {
      memset( &mrd__info, 0, MRD__S_INFO );
      epicsPrintf( "devAvmeMRDConfig(): Failure to probe address 0x%8.8X\n", (unsigned int)base );

      return( MRD__STS_ERROR );
   }

   /* Initialize MRD INFO structure */
   mrd__info.card = 0;
   mrd__info.lock = epicsMutexMustCreate();

   /* Initialize EPICS IO scan */
   scanIoInit( &mrd__info.ioscanpvt );

   /* Connect with VxWorks interrupt mechanism */
   sts = intConnect( INUM_TO_IVEC(vector), mrd__isr, (int)&mrd__info );
   if( sts )
   {
      memset( &mrd__info, 0, MRD__S_INFO );
      epicsPrintf( "devAvmeMRDConfig(): Failure to connect with interrupt\n" );

      return( MRD__STS_ERROR );
   }

   /* Initialize MRD */
   mrd__info.base->rREGS.IVEC  = vector;
   mrd__info.base->rREGS.IDC   = MRD__K_IDC;
   mrd__info.base->rREGS.ICR   = MRD__K_ICR;

   /* Enable VME interrupt (IRQ) level */
   sts = sysIntEnable( level );
   if( sts )
   {
      epicsPrintf( "devAvmeMRDConfig(): Failure to enable IRQ%1.1d\n", (unsigned int)level );

      return( MRD__STS_ERROR );
   }

   /* Add to the list of routines called when VxWorks is rebooted */
   sts = rebootHookAdd( (FUNCPTR)mrd__reboot );
   if( sts )
   {
      epicsPrintf( "devAvmeMRDConfig(): Failure to add reboot hook\n" );

      return( MRD__STS_ERROR );
   }

   /* Return completion status */
   return( MRD__STS_OK );

} /* end-method: devAvmeMRDConfig() */

