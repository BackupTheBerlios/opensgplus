

#ifndef _OSGWatermark_H_
#define _OSGWatermark_H_

#include <OSGConfig.h>
#include <OSGNode.h>
#include "OSGWMtriangleset.h"
#include "OSGWMflood2.h"
#include "OSGWMcommontypes.h"

OSG_USING_NAMESPACE

class OSGWatermark
{
    /*==========================  PUBLIC  =================================*/
public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */
    
	OSGWatermark(void);

    /*! \}                                                                 */
    
	/*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */
    
	virtual ~OSGWatermark(void);

    /*! \}                                                                 */
    
	/*---------------------------------------------------------------------*/
    /*! \name                   Methods                                    */
    /*! \{                                                                 */

    // embeds labelStr as watermark into pScene
    BOOL    embedWM(NodePtr pScene, char *labelStr);

    // retrieves the watermark frome pScene and returns it in labelStr
    BOOL    retrieveWM(NodePtr pScene, char *labelStr);

    /*! \}                                                                 */
    /*==========================  PRIVATE  =================================*/
private:

    int embedWMInternal(TriangleSet *ts, BOOL *embedded, char *labelStr,
                        GEOMARK_BitArray *isQuadVertex);
    int retrieveWMInternal(TriangleSet *ts, BOOL *wmFound, char *descrStr,
                           GEOMARK_BitArray *isQuadVertex);
};
#endif
