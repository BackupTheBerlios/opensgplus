/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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

#ifndef _OSGGEOMETRYCLUSTERED_H_
#define _OSGGEOMETRYCLUSTERED_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>
#include <OSGGeometryClusteredBase.h>

OSG_BEGIN_NAMESPACE

/*! \brief GeometryClustered class.
*/
class OSG_GENVISLIB_DLLMAPPING GeometryClustered : public GeometryClusteredBase
{
  private:

    typedef GeometryClusteredBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    SetUnion&               getPoolEntry (UInt32 i);
    const SetUnion&         getPoolEntry (UInt32 i) const;
    SetUnion&               getPoolEntry (const Pnt3f& eye, 
					  Real32 minDist, Real32 maxDist, 
					  UInt32 i);
    const SetUnion&         getPoolEntry (const Pnt3f& eye, 
					  Real32 minDist, Real32 maxDist, 
					  UInt32 i) const;

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

    virtual Action::ResultE drawPrimitives (DrawActionBase * action );
    void                    fillGrid       (GeoPositionsPtr pos);

    /*! \}                                                                 */ 

    /*=========================  PROTECTED  ===============================*/
  protected:
    // Variables should all be in GeometryClusteredBase.

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    GeometryClustered(void);
    GeometryClustered(const GeometryClustered &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~GeometryClustered(void); 

    /*! \}                                                                 */
    
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class GeometryClusteredBase;

    static void initMethod(void);

    // prohibit default functions (move to 'public' if you need one)

    void operator =(const GeometryClustered &source);
};

typedef GeometryClustered *GeometryClusteredP;

OSG_END_NAMESPACE

#include <OSGGeometryClusteredBase.inl>
#include <OSGGeometryClustered.inl>

#define OSGGEOMETRYCLUSTERED_HEADER_CVSID "@(#)$Id: OSGGeometryClustered.h,v 1.4 2004/03/12 13:37:26 fuenfzig Exp $"

#endif /* _OSGGEOMETRYCLUSTERED_H_ */
