/*---------------------------------------------------------------------------*\
 *                     OpenSG PLUS Occlusion Test                            *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2002 by the WSI/GRIS Uni Tuebingen              *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: staneker@gris.uni-tuebingen.de                                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _OSGOCCLUSIONPREDICTION_H_
#define _OSGOCCLUSIONPREDICTION_H_

#ifdef __sgi
#pragma once
#endif

#include <vector>
#include <map>

#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGSystemDef.h>
#include <OpenSG/OSGDrawAction.h>
#include <OpenSG/OSGDynamicVolume.h>
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGNodePtr.h>
#include <OpenSG/OSGCamera.h>

#include <OSGOcclusionTypes.h>

#include <OpenSG/OSGOcclusionTestBase.h>

OSG_BEGIN_NAMESPACE

//! OcclusionPrediction
//! \ingroup RenderingBackendLib


class OSG_SYSTEMLIB_DLLMAPPING OcclusionPrediction:public OcclusionTestBase
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Statistic                                  */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    OcclusionPrediction(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~OcclusionPrediction(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */

    void setup(const Int32&, Viewport*, const UInt32& x=0);

    void frameInit(void);
    void frameExit(void);

    void init(void);
    void perform(const Int32&, const OCTestNode*);
    Int32 result(const Int32&);
    void exit(void);
   
    void update(const OCTestNode*, const Int32);
    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */


    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:
    
    UInt32 _maxtests;
    Int16* _results;
    CameraPtr _cam;

    	UInt16 _frame;
	UInt16 _fdiff;
	
	struct VisibilityTrace{
		UInt32 _visibility;
		UInt16 _frame;
	};
    
	typedef std::map <Node*, VisibilityTrace*> TraceMap;
//	std::vector<VisibilityTrace*> _visibilityTraces;
	TraceMap _visibilityTraces;	// FIXME
    
    /*! \brief prohibit default function (move to 'public' if needed) */
    OcclusionPrediction(const OcclusionPrediction &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const OcclusionPrediction &source);
};

OSG_END_NAMESPACE

#include "OpenSG/OSGOcclusionPrediction.inl"

#endif /* _OSGOCCLUSIONPREDICTION_H_ */
