#ifndef _ALGINTERFACE_H_
#define _ALGINTERFACE_H_

#define REPORT_MSG_PRINT		0
#define REPORT_MSG_ADVANCE_BAR	1
#define REPORT_MSG_RESET_BAR	2
#define REPORT_MSG_APP_FINISHED	3

#include "OSGWMutil.h" // for BOOL

typedef struct {
	int (*cb) (int,char*,int); 
	BOOL *pOperationAborted;  
} AlgExchangeData;

#endif
