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

#ifndef _OSGDYNAMICSUBDIVISIONCC_H_
#define _OSGDYNAMICSUBDIVISIONCC_H_
#ifdef __sgi
#pragma once
#endif

#include "OSGConfig.h"

#include "OSGDynamicSubdivisionCCBase.h"

OSG_BEGIN_NAMESPACE

template <class MESH>
class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionCC : public DynamicSubdivisionCCBase<MESH>
{
  private:

    typedef DynamicSubdivisionCCBase<MESH> Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                      Sync                                    */
    /*! \{                                                                 */

    virtual void changed(BitVector  whichField, 
                         UInt32     origin    );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Output                                   */
    /*! \{                                                                 */

    virtual void dump(      UInt32     uiIndent = 0, 
                      const BitVector  bvFlags  = 0) const;

	

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    // Variables should all be in DynamicSubdivisionCCBase.

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    DynamicSubdivisionCC(void);
    DynamicSubdivisionCC(const DynamicSubdivisionCC &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~DynamicSubdivisionCC(void); 

    /*! \}                                                                 */
    Action::ResultE drawEnter  (Action *action);

    Action::ResultE renderEnter(Action *action);

	/*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Volume                                  */
    /*! \{                                                                 */

    virtual void adjustVolume    (Volume &volume);

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class DynamicSubdivisionCCBase<MESH>;

    static void initMethod(void);


    // prohibit default functions (move to 'public' if you need one)

    void operator =(const DynamicSubdivisionCC &source);
};

OSG_END_NAMESPACE

#include "OSGDynamicSubdivisionCCBase.inl"
#include "OSGDynamicSubdivisionCC.inl"

#define OSGDYNAMICSUBDIVISIONCC_HEADER_CVSID "@(#)$Id: OSGDynamicSubdivisionCC.h,v 1.1 2003/07/11 14:46:51 fuenfzig Exp $"

#endif /* _OSGDYNAMICSUBDIVISIONCC_H_ */
