/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
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

#ifndef _OSGSTEREOCAMERA_H_
#define _OSGSTEREOCAMERA_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>
#include <OSGClusterDef.h>
#include <OSGStereoCameraBase.h>

OSG_BEGIN_NAMESPACE

/*! \brief *put brief class description here* 
 */

class OSG_CLUSTERLIB_DLLMAPPING StereoCamera : public StereoCameraBase
{
  private:

    typedef StereoCameraBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                      Sync                                    */
    /*! \{                                                                 */

    virtual void changed(BitVector whichField, 
                         UInt32    origin    );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Output                                   */
    /*! \{                                                                 */

    virtual void dump(      UInt32     uiIndent = 0, 
                      const BitVector  bvFlags  = 0) const;

    /*! \}                                                                 */

    virtual void getProjection( Matrix& result, UInt32 width, UInt32 height);
    virtual void getViewing( Matrix& result, UInt32 width, UInt32 height); 

    /*=========================  PROTECTED  ===============================*/
  protected:

    // Variables should all be in StereoCameraBase.

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    StereoCamera(void);
    StereoCamera(const StereoCamera &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~StereoCamera(void); 

    /*! \}                                                                 */
    
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class StereoCameraBase;

    static void initMethod(void);

    // prohibit default functions (move to 'public' if you need one)

    void operator =(const StereoCamera &source);
};

typedef StereoCamera *StereoCameraP;

OSG_END_NAMESPACE

#include <OSGStereoCamera.inl>
#include <OSGStereoCameraBase.inl>

#define OSGSTEREOCAMERA_HEADER_CVSID "@(#)$Id: OSGStereoCamera.h,v 1.2 2002/06/01 11:19:59 vossg Exp $"

#endif /* _OSGSTEREOCAMERA_H_ */
