/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *           Copyright (C) 2000,2001,2002 by the OpenSG Forum                *
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

#ifndef _CLUSTERNODE_H_
#define _CLUSTERNODE_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGClusterDef.h>
#include <OSGBaseTypes.h>
#include <OSGBinaryDataHandler.h>

OSG_BEGIN_NAMESPACE

class OSG_CLUSTERLIB_DLLMAPPING RenderNode 
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    RenderNode(void);
    RenderNode(const RenderNode &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~RenderNode(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Get                                     */
    /*! \{                                                                 */

    Real32 getVisibleFaceCost   (void) const;
    Real32 getInvisibleFaceCost (void) const;
    Real32 getDrawPixelCost     (void) const;
    Real32 getReadPixelCost     (void) const;
    Real32 getWritePixelCost    (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Set                                     */
    /*! \{                                                                 */

    void setVisibleFaceCost    (Real32 value);
    void setInvisibleFaceCost  (Real32 value);
    void setDrawPixelCost      (Real32 value);
    void setReadPixelCost      (Real32 value);
    void setWritePixelCost     (Real32 value);
    void setGroup              (const RenderNode *begin,const RenderNode *end);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   performance analysis                       */
    /*! \{                                                                 */

    void   determinePerformance( void );
    Real32 estimatePerformance ( Real32 invisibleFaces,
                                 Real32 visibleFaces,
                                 Real32 pixel ) const;
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                load/store                                    */
    /*! \{                                                                 */

    void copyToBin  (BinaryDataHandler &handle);
    void copyFromBin(BinaryDataHandler &handle);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Assignment                                */
    /*! \{                                                                 */

    RenderNode & operator =(const RenderNode &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    dump                                      */
    /*! \{                                                                 */
    
    void dump(void) const;
    
    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Fields                                  */
    /*! \{                                                                 */

    Real32                _visibleFaceCost;
    Real32                _invisibleFaceCost;
    Real32                _drawPixelCost;
    Real32                _readPixelCost;
    Real32                _writePixelCost;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   helper                                     */
    /*! \{                                                                 */

    double runFaceBench(UInt32 dlist,UInt32 size,Real32 visible);

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

};

OSG_END_NAMESPACE

#define OSG_CLUSTERNODE_HEADER_CVSID "@(#)$Id:$"

#include "OSGRenderNode.inl"

#endif /* _CLUSTERNODE_H_ */




