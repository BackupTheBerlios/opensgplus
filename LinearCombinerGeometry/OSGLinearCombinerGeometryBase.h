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
 **     class LinearCombinerGeometry
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#ifndef _OSGLINEARCOMBINERGEOMETRYBASE_H_
#define _OSGLINEARCOMBINERGEOMETRYBASE_H_
#ifdef __sgi
#pragma once
#endif


#include <OSGConfig.h>
#include <OSGSystemDef.h>

#include <OSGBaseTypes.h>

#include <OSGGeometry.h> // Parent

#include <OSGReal32Fields.h> // Weights type
#include <OSGGeoPositionsFields.h> // Srcpositions type
#include <OSGDynamicVolume.h> // Srcvolumes type
#include <OSGBoolFields.h> // Recalconrender type
#include <OSGBoolFields.h> // Allgeometries3f type
#include <OSGBoolFields.h> // Positionsdirty type

#include <OSGLinearCombinerGeometryFields.h>

OSG_BEGIN_NAMESPACE

class LinearCombinerGeometry;
class BinaryDataHandler;

//! \brief LinearCombinerGeometry Base Class.

class OSG_SYSTEMLIB_DLLMAPPING LinearCombinerGeometryBase : public Geometry
{
  private:

    typedef Geometry Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    enum
    {
        WeightsFieldId         = Inherited::NextFieldId,
        SrcpositionsFieldId    = WeightsFieldId         + 1,
        SrcvolumesFieldId      = SrcpositionsFieldId    + 1,
        RecalconrenderFieldId  = SrcvolumesFieldId      + 1,
        Allgeometries3fFieldId = RecalconrenderFieldId  + 1,
        PositionsdirtyFieldId  = Allgeometries3fFieldId + 1,
        NextFieldId            = PositionsdirtyFieldId  + 1
    };

    static const osg::BitVector WeightsFieldMask;
    static const osg::BitVector SrcpositionsFieldMask;
    static const osg::BitVector SrcvolumesFieldMask;
    static const osg::BitVector RecalconrenderFieldMask;
    static const osg::BitVector Allgeometries3fFieldMask;
    static const osg::BitVector PositionsdirtyFieldMask;


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

           MFReal32            *getMFWeights        (void);
           MFGeoPositionsPtr   *getMFSrcpositions   (void);

           Real32              &getWeights        (const UInt32 index);
           MFReal32            &getWeights        (void);
     const MFReal32            &getWeights        (void) const;
           GeoPositionsPtr     &getSrcpositions   (const UInt32 index);
           MFGeoPositionsPtr   &getSrcpositions   (void);
     const MFGeoPositionsPtr   &getSrcpositions   (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Set                                 */
    /*! \{                                                                 */


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

    static  LinearCombinerGeometryPtr      create          (void); 
    static  LinearCombinerGeometryPtr      createEmpty     (void); 

    /*! \}                                                                 */

    /*---------------------------------------------------------------------*/
    /*! \name                       Copy                                   */
    /*! \{                                                                 */

    virtual FieldContainerPtr     shallowCopy     (void) const; 

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Fields                                  */
    /*! \{                                                                 */

    MFReal32            _mfWeights;
    MFGeoPositionsPtr   _mfSrcpositions;
    MFVolume            _mfSrcvolumes;
    SFBool              _sfRecalconrender;
    SFBool              _sfAllgeometries3f;
    SFBool              _sfPositionsdirty;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    LinearCombinerGeometryBase(void);
    LinearCombinerGeometryBase(const LinearCombinerGeometryBase &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~LinearCombinerGeometryBase(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Get                                 */
    /*! \{                                                                 */

           MFVolume            *getMFSrcvolumes     (void);
           SFBool              *getSFRecalconrender (void);
           SFBool              *getSFAllgeometries3f(void);
           SFBool              *getSFPositionsdirty (void);

           bool                &getRecalconrender (void);
     const bool                &getRecalconrender (void) const;
           bool                &getAllgeometries3f(void);
     const bool                &getAllgeometries3f(void) const;
           bool                &getPositionsdirty (void);
     const bool                &getPositionsdirty (void) const;
           Volume              &getSrcvolumes     (UInt32 index);
           MFVolume            &getSrcvolumes     (void);
     const MFVolume            &getSrcvolumes     (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Set                                 */
    /*! \{                                                                 */

     void setRecalconrender (const bool &value);
     void setAllgeometries3f(const bool &value);
     void setPositionsdirty (const bool &value);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

    void executeSyncImpl(      LinearCombinerGeometryBase *pOther,
                         const BitVector         &whichField);

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;

    static FieldDescription   *_desc[];
    static FieldContainerType  _type;


    // prohibit default functions (move to 'public' if you need one)
    void operator =(const LinearCombinerGeometryBase &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------


typedef LinearCombinerGeometryBase *LinearCombinerGeometryBaseP;

OSG_END_NAMESPACE

#define OSGLINEARCOMBINERGEOMETRYBASE_HEADER_CVSID "@(#)$Id: OSGLinearCombinerGeometryBase.h,v 1.1 2003/04/05 16:17:42 dirk Exp $"

#endif /* _OSGLINEARCOMBINERGEOMETRYBASE_H_ */