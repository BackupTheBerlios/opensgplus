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

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                  This file is automatically generated.                  **
 **                                                                         **
 **          Any changes made to this file WILL be lost when it is          **
 **           regenerated, which can become necessary at any time.          **
 **                                                                         **
 **     Do not change this file, changes should be done in the derived      **
 **     class DynamicSubdivisionCC
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#ifndef _OSGDYNAMICSUBDIVISIONCCBASE_H_
#define _OSGDYNAMICSUBDIVISIONCCBASE_H_
#ifdef __sgi
#pragma once
#endif


#include "OSGConfig.h"
#include "OSGSubSurfaceDef.h"

#include "OSGBaseTypes.h"

#include "OSGGroup.h" // Parent

#include "OSGUInt16Fields.h" // MinDepth type
#include "OSGUInt16Fields.h" // MaxDepth type
#include "OSGBoolFields.h" // BackfaceCulling type
#include "OSGOpenMeshPFields.h" // Mesh type
#include "OSGOpenMeshTesselatorPFields.h" // Tesselator type

#include "OSGDynamicSubdivisionCCFields.h"

OSG_BEGIN_NAMESPACE

template <class MESH> class DynamicSubdivisionCC;
class BinaryDataHandler;

//! \brief DynamicSubdivisionCC Base Class.

template <class MESH>
class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionCCBase : public Group
{
  private:
    typedef Group Inherited;

  public:
    typedef MESH                        OpenMesh;
    typedef OpenMesh*                   OpenMeshP;
    typedef SField<OpenMeshP>           SFOpenMeshP;
    typedef	WSDmain<Vec4f,OpenMesh,1>   OpenMeshTesselator;
    typedef	OpenMeshTesselator*         OpenMeshTesselatorP;
    typedef	SField<OpenMeshTesselatorP> SFOpenMeshTesselatorP;

    /*==========================  PUBLIC  =================================*/
    enum
    {
        MinDepthFieldId        = Inherited::NextFieldId,
        MaxDepthFieldId        = MinDepthFieldId        + 1,
        BackfaceCullingFieldId = MaxDepthFieldId        + 1,
        MeshFieldId            = BackfaceCullingFieldId + 1,
        TesselatorFieldId      = MeshFieldId            + 1,
        NextFieldId            = TesselatorFieldId      + 1
    };

    static const osg::BitVector MinDepthFieldMask;
    static const osg::BitVector MaxDepthFieldMask;
    static const osg::BitVector BackfaceCullingFieldMask;
    static const osg::BitVector MeshFieldMask;
    static const osg::BitVector TesselatorFieldMask;


    /*---------------------------------------------------------------------*/
    /*! \name                    Class Get                                 */
    /*! \{                                                                 */

    static        FieldContainerType &getClassType    (void); 
    static        UInt32              getClassTypeId  (void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                FieldContainer Get                            */
    /*! \{                                                                 */

    virtual       FieldContainerType &getType  (void); 
    virtual const FieldContainerType &getType  (void) const; 

    virtual       UInt32              getContainerSize(void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Get                                 */
    /*! \{                                                                 */

           SFUInt16            *getSFMinDepth       (void);
           SFUInt16            *getSFMaxDepth       (void);
           SFBool              *getSFBackfaceCulling(void);
           SFOpenMeshP         *getSFMesh           (void);
           SFOpenMeshTesselatorP *getSFTesselator     (void);

           UInt16              &getMinDepth       (void);
     const UInt16              &getMinDepth       (void) const;
           UInt16              &getMaxDepth       (void);
     const UInt16              &getMaxDepth       (void) const;
           bool                &getBackfaceCulling(void);
     const bool                &getBackfaceCulling(void) const;
           OpenMeshP           &getMesh           (void);
     const OpenMeshP           &getMesh           (void) const;
           OpenMeshTesselatorP &getTesselator     (void);
     const OpenMeshTesselatorP &getTesselator     (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Set                                 */
    /*! \{                                                                 */

     void setMinDepth       ( const UInt16 &value );
     void setMaxDepth       ( const UInt16 &value );
     void setBackfaceCulling( const bool &value );
     void setMesh           ( const OpenMeshP &value );
     void setTesselator     ( const OpenMeshTesselatorP &value );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

    virtual void   executeSync(      FieldContainer    &other,
                               const BitVector         &whichField);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Binary Access                              */
    /*! \{                                                                 */

    virtual UInt32 getBinSize (const BitVector         &whichField);
    virtual void   copyToBin  (      BinaryDataHandler &pMem,
                               const BitVector         &whichField);
    virtual void   copyFromBin(      BinaryDataHandler &pMem,
                               const BitVector         &whichField);


    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Construction                               */
    /*! \{                                                                 */

    static  FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> >      create          (void); 
    static  FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> >      createEmpty     (void); 

    /*! \}                                                                 */

    /*---------------------------------------------------------------------*/
    /*! \name                       Copy                                   */
    /*! \{                                                                 */

    virtual FieldContainerPtr     shallowCopy     (void) const; 

    /*! \}                                                                 */
    static char               *getName(void) { 
		static char sname[1000];
		sprintf(sname, "DynamicSubdivisionCC%d", MeshIdentifier<MESH>::id);
		return sname; 
	}

    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Fields                                  */
    /*! \{                                                                 */

    SFUInt16            _sfMinDepth;
    SFUInt16            _sfMaxDepth;
    SFBool              _sfBackfaceCulling;
    SFOpenMeshP         _sfMesh;
    SFOpenMeshTesselatorP   _sfTesselator;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    DynamicSubdivisionCCBase(void);
    DynamicSubdivisionCCBase(const DynamicSubdivisionCCBase &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~DynamicSubdivisionCCBase(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

    void executeSyncImpl(      DynamicSubdivisionCCBase *pOther,
                         const BitVector         &whichField);

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;

    static OSG_SUBSURFACELIB_DLLMAPPING FieldDescription   *_desc[];
    static OSG_SUBSURFACELIB_DLLMAPPING FieldContainerType  _type;

    // prohibit default functions (move to 'public' if you need one)
    void operator =(const DynamicSubdivisionCCBase &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

OSG_END_NAMESPACE

#define OSGDYNAMICSUBDIVISIONCCBASE_HEADER_CVSID "@(#)$Id: OSGDynamicSubdivisionCCBase.h,v 1.1 2003/07/11 14:46:51 fuenfzig Exp $"

#endif /* _OSGDYNAMICSUBDIVISIONCCBASE_H_ */