#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _LOOKUPA_H_
#define _LOOKUPA_H_

/*
------------------------------------------------------------------------------
By Bob Jenkins, September 1996
lookupa.h, a hash function for table lookup, same function as lookup.c.
You may use this code in any way you wish.  It has no warranty.
Source is http://ourworld.compuserve.com/homepages/bob_jenkins/lookupa.h
------------------------------------------------------------------------------
*/
#include "OSGWMstandard.h"
#define CHECKSTATE  8
#define hashsize(n) ((ub4) 1 << (n))
#define hashmask(n) (hashsize(n) - 1)

ub4     lookup(ub1 *k, ub4 length, ub4 level);
void    checksum(ub1 *k, ub4 length, ub4 *state);
#endif
#endif
