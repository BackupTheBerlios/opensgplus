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

#ifndef _OSGDYNAMICSUBDIVISIONLP_H_
#define _OSGDYNAMICSUBDIVISIONLP_H_
#ifdef __sgi
#pragma once
#endif

#include "OSGConfig.h"
#include "OSGDynamicSubdivisionLPBase.h"
#include "OSGViewport.h"
#include "OSGAction.h"

OSG_BEGIN_NAMESPACE

template <class MESH>
class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionLP : public DynamicSubdivisionLPBase<MESH>
{
  private:

    typedef DynamicSubdivisionLPBase<MESH>   Inherited;

    /*==========================  PUBLIC  =================================*/
  public:
    typedef typename Inherited::OpenMesh              OpenMesh;
    typedef typename Inherited::OpenMeshP             OpenMeshP;
    typedef typename Inherited::SFOpenMeshP           SFOpenMeshP;
    typedef typename Inherited::OpenMeshTesselator    OpenMeshTesselator;
    typedef typename Inherited::OpenMeshTesselatorP   OpenMeshTesselatorP;
    typedef typename Inherited::SFOpenMeshTesselatorP SFOpenMeshTesselatorP;

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
    /*---------------------------------------------------------------------*/
    /*! \name                     Functions                                */
    /*! \{                                                                 */

    void  prepareFrame (const ViewportPtr& port);

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    // Variables should all be in DynamicSubdivisionLPBase.

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    DynamicSubdivisionLP(void);
    DynamicSubdivisionLP(const DynamicSubdivisionLP &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~DynamicSubdivisionLP(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                  Functor Functions                           */
    /*! \{                                                                 */

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
    friend class DynamicSubdivisionLPBase<MESH>;

    static void initMethod(void);


    // prohibit default functions (move to 'public' if you need one)

    void operator =(const DynamicSubdivisionLP &source);
};

OSG_END_NAMESPACE

#include "OSGDynamicSubdivisionLPBase.inl"
#include "OSGDynamicSubdivisionLP.inl"

#define OSGDYNAMICSUBDIVISIONLP_HEADER_CVSID "@(#)$Id: OSGDynamicSubdivisionLP.h,v 1.2 2004/03/24 12:47:40 fuenfzig Exp $"

#endif /* _OSGDYNAMICSUBDIVISIONLP_H_ */
