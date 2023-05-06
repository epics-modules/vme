vme - Generic VME Record
========================

Mark Rivers


Contents
--------

- [Overview](#Overview)
- [Field Descriptions](#Fields)
- [Files](#Files)
- [Release Notes](#Release)
- [Example](#Example)

Overview
--------

The VME record is designed to perform generic VME I/O. The VME base address, addressing mode (A16, A24, A32), address increment, data size (D8, D16, D32), and I/O direction are all controlled by record fields which can be modified at run time. Applications for this record include accessing VME modules for which no device support exists, and performing IOC diagnostics.

The VME record is intended only for this specialized VME I/O function, and thus does not have a separate device support layer. The VME record itself performs the VME I/O. All VME I/O is done with the vxWorks function vxMemProbe so that bus errors are handled harmlessly.

The VME record supports array operations, i.e. reading or writing continguous blocks of VME addresses in a single record processing operation. The VAL field contains the data to be written or the data read. The SARR (Status Array) field contains the status of each VME I/O operation, i.e. whether the operation succeeded or generated a VME bus error.

The VME record should be used with care, since it is possible to write to any location in the VME address space. It is definitely possible to affect VME modules in unintended ways! Even read-only operations can have significant side-effects.

- - - - - -

<a name="Fields"></a>

Record Field Descriptions
-------------------------

| Name | Access | Prompt | Data type | Description |
|---|---|---|---|---|
| VAL | R/W\* | "Current value" | DBF\_LONG (array) | The data to be written to VME or the data read from VME. This is always an array of LONGS, regardless of whether DSIZ is D8, D16 or D32. The maximum array length is determined by NMAX at IOC initialization. The actual array length used is equal to NUSE. |
| SARR | R | "Status array" | DBF\_UCHAR (array) | The status of each VME I/O operation. This is an array of UCHAR. Each element is the status returned by vmMemProbe, i.e. 0 for success, 0xFF for failure. The maximum array length is determined by NMAX at IOC initialization. The actual array length used is equal to NUSE. |
| ADDR | R/W | "VME address (hex)" | DBF\_LONG | The starting address of the VME I/O operation. When NUSE is greater than 1 then the address of each succeeding VME I/O operation will be incremented by AINC. The value of ADDR is not modified during this address autoincrementing. |
| AMOD | R/W | "VME address mode" | DBF\_RECCHOICE | The VME address mode. The choices are "A16" (default), "A24" and "A32". |
| DSIZ | R/W | "VME data size" | DBF\_RECCHOICE | The VME data transfer size. The choices are "D8", "D16" (default) and "D32". Note that the AINC field is not automatically changed to 1, 2, or 4 for these addressing modes. The reason for this is that there are some VME boards which require, for example, D8 transfers but only respond to odd addresses, so DSIZ must be D8 but AINC must be 2. There are also some devices which want AINC of 0, because successive data are read from the same VME address. |
| RDWT | R/W | "Read/write" | DBF\_RECCHOICE | The data transfer direction. The choices are "Read" (VME bus to VAL field, the default) and "Write" (VAL field to VME bus). |
| NMAX | R | "Max. number of values" | DBF\_LONG | The maximum length of the VAL and SARR arrays. It cannot be modified after the IOC is initialized. Default=32. |
| NUSE | R/W | "Number of values to R/W" | DBF\_LONG | The actual number of values to be transferred when the record processes. It must be less than or equal to NMAX. Default=1. |
| AINC | R/W | "Address increment (0-4)" | DBF\_LONG | The address increment which is added to the previous VME address on each I/O transfer. It is typically (but not always) 1 for D8 transfers, 2 for D16 transfers, and 4 for D32 transfers. Default=2. |
| Private Fields |
| BPTR | N | "Buffer Pointer" | DBF\_NOACCESS | The pointer to the buffer for the VAL field. |
| SPTR | N | "Status Pointer" | DBF\_NOACCESS | The pointer to the buffer for the SARR field. |

Note: In the Access column above: 

| R | Read only |  
| R/W | Read and write are allowed | 
| R/W\* | Read and write are allowed; write triggers record processing if the record's SCAN field is set to "Passive". | 
| N | No access allowed | 

- - - - - -

<a name="Files"></a>

Files
-----

The following table briefly describes all of the files required to implement the VME record. The reader is assumed to be familiar with the [EPICS Application Source/Release Control document](http://www.aps.anl.gov/asd/controls/epics/EpicsDocumentation/
AppDevManuals/iocAppBuildSRcontrol.html) which describes how to build an EPICS application tree into which these files are to be placed, and how to run "gnumake" to build the record support. These files can all be obtained in a [compressed tar file](pub/vme_record.tar.Z). This file should be untarred in a `<top>/xxxApp/` directory.

| Files to be placed in `<top>/xxxApp/src/` | |
|---|---|
| vmeRecord.c | The source file for the record |
| vmeRecord.dbd | The database definition file for the record | 

#### Makefile.Vx

This file is not included in the distribution. However, the user must edit this file and add the following lines: 

```  
RECTYPES += vmeRecord.h 
SRCS.c   += ../vmeRecord.c 
LIBOBJS  += vmeRecord.o
```

#### xxxApp.dbd

This file is not included in the distribution.  However, the user must edit this file and add the following line:  

```
include "vmeRecord.dbd" 
```

__Files to be placed in  `<top>/xxxApp/op/adl/`__

#### VME_IO.adl 

This file builds an `medm` screen to access the VME record. The `medm` screen can be used to probe single memory locations only, since `medm` does not have a widget to display array data as text. To use it from the command line, type the following:  
```
cars> medm -x -macro REC=my_vme_record VME_IO.adl  
```
where `my_vme_record` is the name of a VME record in an IOC. 

This file can also be used as a related display from other `medm` screens by passing the argument `REC=my_vme_record`

- - - - - -

<a name="Release"></a>

Release Notes
-------------

- Version 1.0, March 1996. Initial release for R3.12 by Mark Rivers.
- Version 1.1, December 1997. Conversion to EPICS R3.13 by Tim Mooney.

- - - - - -


Example
-------

The following is an IDL program which uses the VME record to determine and print out a complete map of all VME bus A16 addresses which respond to D16 read bus cycles. It is useful for determining what VME addresses are currently in use. 

```
pro vme_mem_map, record

; This procedure prints a table of the VME A16 address space using the
; VME record.
;
; It requires as input the name of a VME record which must exist in the IOC to
; be tested.  For efficiency this record should have a reasonably large value
; of NMAX, but one which is smaller than the channel access transfer limit.
; 2048 is a good value to use.  The name of the record must be passed without 
; a trailing period or field name, e.g. 'test_vme1'.

; Determine maximum number of VME cycles which can be done in a single record
; processing operation
t = caget(record+'.NMAX', n)

; Set the value of NUSE (number to actually use) to this value
t = caput(record+'.NUSE', n)

; Set the address mode to A16
t = caput(record+'.AMOD', 'A16')

; Set the data size to D16
t = caput(record+'.DSIZ', 'D16')

; Set the address increment to 2
t = caput(record+'.AINC', 2)

; Make arrays to hold data and status return info
ntot = 2L^16/2
data = lonarr(ntot)
status = bytarr(ntot)

; Compute addresses of each point
address = 2*lindgen(ntot)

for i=0L, ntot-1, n do begin
   ; Set the base address
   t = caput(record+'.ADDR', address(i))
   ; Process the record
   t = caput(record+'.PROC', 1)
   t = caget(record+'.VAL', d)    ; This copies n values into data()
   data(i)=d
   t = caget(record+'.SARR', d) ; This copies n values into status()
   status(i) = d
endfor

; Print addresses which responded
valid = 0
for i=0L, ntot-1 do begin
   if (valid) then begin
      if (status(i) ne 0) then begin
        ; We have a transition from valid address to invalid.
        print, address(start), data(start), address(i-1), data(i-1), $
            format="(z4, '( ', z8,') --- ', z4, ' (', z8, ')')"
        valid = 0
      endif else if (i eq ntot-1) then begin
        ; We have valid addresses to the end
        print, address(start), data(start), address(i), data(i), $
            format="(z4, '( ', z8,') --- ', z4, ' (', z8, ')')"
      endif
   endif else begin
      if (status(i) eq 0) then begin
        ; We have a transition from invalid address to valid address
        start = i
        valid = 1
      endif
   endelse
endfor
end
```

- - - - - -

Suggestions and comments to: [Mark Rivers ](mailto:rivers@cars.uchicago.edu) : (rivers@cars.uchicago.edu)   
Last modified: December 12, 1996
