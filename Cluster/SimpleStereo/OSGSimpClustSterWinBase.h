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
 **     class SimpClustSterWin
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#ifndef _OSGSIMPCLUSTSTERWINBASE_H_
#define _OSGSIMPCLUSTSTERWINBASE_H_
#ifdef __sgi
#pragma once
#endif


#include <OSGConfig.h>
#include <OSGClusterDef.h>

#include <OSGBaseTypes.h>
#include <OSGFieldDescription.h>
#include <OSGFieldContainer.h>

#include <OSGClusterWindow.h> // Parent

#include <OSGReal32Fields.h> // Fov type
#include <OSGReal32Fields.h> // Eyedistance type
#include <OSGReal32Fields.h> // Zeroparallax type

#include <OSGSimpClustSterWinFields.h>

OSG_BEGIN_NAMESPACE

class SimpClustSterWin;
class BinaryDataHandler;

//! \brief SimpClustSterWin Base Class.

class OSG_CLUSTERLIB_DLLMAPPING SimpClustSterWinBase : public ClusterWindow
{
  private:

    typedef ClusterWindow Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    enum
    {
        FovFieldId          = Inherited::NextFieldId,
        EyedistanceFieldId  = FovFieldId          + 1,
        ZeroparallaxFieldId = EyedistanceFieldId  + 1,
        NextFieldId         = ZeroparallaxFieldId + 1
    };

    static const osg::BitVector FovFieldMask;
    static const osg::BitVector EyedistanceFieldMask;
    static const osg::BitVector ZeroparallaxFieldMask;

    /*---------------------------------------------------------------------*/
    /*! \name                    Class Get                                 */
    /*! \{                                                                 */

    static        FieldContainerType &getClassType    (void); 
    static        UInt32              getClassTypeId  (void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Get                                    */
    /*! \{                                                                 */

    virtual       FieldContainerType &getType  (void); 
    virtual const FieldContainerType &getType  (void) const; 

    virtual       UInt32              getContainerSize(void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Get                                 */
    /*! \{                                                                 */

    inline       SFReal32            *getSFFov            (void);
    inline       SFReal32            *getSFEyedistance    (void);
    inline       SFReal32            *getSFZeroparallax   (void);

    inline       Real32              &getFov            (void);
    inline const Real32              &getFov            (void) const;
    inline       Real32              &getEyedistance    (void);
    inline const Real32              &getEyedistance    (void) const;
    inline       Real32              &getZeroparallax   (void);
    inline const Real32              &getZeroparallax   (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Set                                 */
    /*! \{                                                                 */

    inline void setFov            ( const Real32 &value );
    inline void setEyedistance    ( const Real32 &value );
    inline void setZeroparallax   ( const Real32 &value );

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

    static  SimpClustSterWinPtr      create          (void); 
    static  SimpClustSterWinPtr      createEmpty     (void); 

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

    SFReal32            _sfFov;
    SFReal32            _sfEyedistance;
    SFReal32            _sfZeroparallax;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    SimpClustSterWinBase(void);
    SimpClustSterWinBase(const SimpClustSterWinBase &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~SimpClustSterWinBase(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

    void executeSyncImpl(      SimpClustSterWinBase *pOther,
                         const BitVector         &whichField);

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;

    static FieldDescription   *_desc[];
    static FieldContainerType  _type;


    // prohibit default functions (move to 'public' if you need one)
    void operator =(const SimpClustSterWinBase &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------


typedef SimpClustSterWinBase *SimpClustSterWinBaseP;

OSG_END_NAMESPACE

#define OSGSIMPCLUSTSTERWINBASE_HEADER_CVSID "@(#)$Id: OSGSimpClustSterWinBase.h,v 1.1 2001/12/21 15:10:29 marcus Exp $"

#endif /* _OSGSIMPCLUSTSTERWINBASE_H_ */
