/**CFile****************************************************************

  FileName    [nmInt.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Name manager.]

  Synopsis    [Internal declarations.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id$]

***********************************************************************/
 
#ifndef __NM_INT_H__
#define __NM_INT_H__

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#include "extra.h"
#include "vec.h"
#include "nm.h"

////////////////////////////////////////////////////////////////////////
///                         PARAMETERS                               ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                         BASIC TYPES                              ///
////////////////////////////////////////////////////////////////////////

typedef struct Nm_Entry_t_ Nm_Entry_t;
struct Nm_Entry_t_
{
    int              ObjId;         // object ID
    char             Name[0];       // name of the object
};

struct Nm_Man_t_
{
    Nm_Entry_t **    pBinsI2N;      // mapping IDs into names
    Nm_Entry_t **    pBinsN2I;      // mapping names into IDs 
    int              nBins;         // the number of bins in tables
    int              nEntries;      // the number of entries
    int              nSizeFactor;   // determined how much larger the table should be
    int              nGrowthFactor; // determined how much the table grows after resizing
    Extra_MmFlex_t * pMem;          // memory manager for entries (and names)
};

////////////////////////////////////////////////////////////////////////
///                      MACRO DEFINITIONS                           ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DECLARATIONS                         ///
////////////////////////////////////////////////////////////////////////

/*=== nmTable.c ==========================================================*/
extern int              Nm_ManTableAdd( Nm_Man_t * p, Nm_Entry_t * pEntry );
extern int              Nm_ManTableDelete( Nm_Man_t * p, Nm_Entry_t * pEntry );
extern Nm_Entry_t *     Nm_ManTableLookupId( Nm_Man_t * p, int ObjId );
extern Nm_Entry_t *     Nm_ManTableLookupName( Nm_Man_t * p, char * pName, Nm_Entry_t ** ppSecond );
extern unsigned int     Cudd_PrimeNm( unsigned int p );

#ifdef __cplusplus
}
#endif

#endif

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


