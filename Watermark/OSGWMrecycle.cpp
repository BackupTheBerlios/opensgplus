#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)

/*
--------------------------------------------------------------------
By Bob Jenkins, September 1996.  recycle.c
You may use this code in any way you wish, and it is free.  No warranty.

This manages memory for commonly-allocated structures.
It allocates RESTART to REMAX items at a time.
Timings have shown that, if malloc is used for every new structure,
  malloc will consume about 90% of the time in a program.  This
  module cuts down the number of mallocs by an order of magnitude.
This also decreases memory fragmentation, and freeing structures
  only requires freeing the root.
--------------------------------------------------------------------
*/
#include "OSGWMstandard.h"
#include <stdlib.h>
#include <string.h>
#include "OSGWMrecycle.h"

/* */

reroot *remkroot(size_t size)
{
    reroot  *r = (reroot *) malloc(sizeof(reroot));
    r->list = (recycle *) 0;
    r->trash = (recycle *) 0;
    r->size = align(size);
    r->logsize = RESTART;
    r->numleft = 0;
    return r;
}

/* */
void refree(struct reroot *r)
{
    recycle *temp;
    if(temp = r->list)
    {
        while(r->list)
        {
            temp = r->list->next;

            //GEOMARK_FREE((char *)r->list);
            GEOMARK_FREE(r->list);
            r->list = temp;
        }
    }

    //GEOMARK_FREE((char *)r);
    GEOMARK_FREE(r);
    return;
}

/* to be called from the macro renew only */
char *renewx(struct reroot *r)
{
    recycle *temp;
    if(r->trash)
    {   /* pull a node off the trash heap */
        temp = r->trash;
        r->trash = temp->next;
        (void) memset((void *) temp, 0, r->size);
    }
    else
    {   /* allocate a new block of nodes */
        r->numleft = r->size * ((ub4) 1 << r->logsize);
        if(r->numleft < REMAX)
            ++r->logsize;
        temp = (recycle *) malloc(sizeof(recycle) + r->numleft);
        temp->next = r->list;
        r->list = temp;
        r->numleft -= r->size;
        temp = (recycle *) ((char *) (r->list + 1) + r->numleft);
    }

    return (char *) temp;
}
#endif
