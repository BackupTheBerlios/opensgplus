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

#ifndef _OSGGENVISCACHE_H_
#define _OSGGENVISCACHE_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>
#include <OSGNode.h>
#include <OSGMatrix.h>

#include <OSGGenvisCacheBase.h>
#include <OSGGVBase.h>
#include <OSGGVBVolAdapterBase.h>

OSG_BEGIN_NAMESPACE

class OSG_GENVISLIB_DLLMAPPING GenvisCache : public GenvisCacheBase
{
  private:

    typedef GenvisCacheBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:
    typedef MFNodePtr::StorageType          ChildContainer;
    typedef std::vector<BVolAdapterBaseP>   AdapterVector;
    typedef MFBVolAdapterBaseP::StorageType AdapterContainer;

    /*---------------------------------------------------------------------*/
    /*! \name Data of referenced scenegraph Node.                          */
    /*! \{                                                                 */
    Matrix          getToWorldMatrix () const;
    NodePtr         getOriginal() const;
    NodePtr         getNode   () const;
    NodePtr         getParent () const;
    int             getNumChildren () const;
    ChildContainer&       getChildren ();
    const ChildContainer& getChildren () const;
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name Adapter matrices.                                            */
    /*! \{                                                                 */
    const Matrix&   getAdapterMatrix (unsigned id) const;
    void            setAdapterMatrix (unsigned id, const Matrix& m);
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name Adapter vector.                                              */
    /*! \{                                                                 */   
    void                     destroyAdapter (unsigned id, unsigned len = 0);
    void                     clearAdapter   (unsigned id, unsigned len = 0);
    AdapterContainer& getAdapter     (unsigned id);
    BVolAdapterBaseP& getAdapter     (unsigned id, unsigned num);
    const AdapterContainer& getAdapter (unsigned id) const;
    const BVolAdapterBaseP& getAdapter (unsigned id, unsigned num) const;
    void              setAdapter     (unsigned id, BVolAdapterBaseP a);
    void              setAdapter     (unsigned id, const AdapterVector&  a);
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name Collision cache.                                             */
    /*! \{                                                                 */

    AdapterContainer&       getColCache ();
    BVolAdapterBaseP&       getColCache (unsigned num);
    const AdapterContainer& getColCache () const;
    const BVolAdapterBaseP& getColCache (unsigned num) const;

    /*! \}                                                                 */
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

    // Variables should all be in GenvisCacheBase.

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    GenvisCache(void);
    GenvisCache(const GenvisCache &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~GenvisCache(void); 

    /*! \}                                                                 */
    
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class GenvisCacheBase;

    static void initMethod(void);

    // prohibit default functions (move to 'public' if you need one)

    void operator =(const GenvisCache &source);
};

typedef GenvisCache *GenvisCacheP;

OSG_END_NAMESPACE

#include <OSGGenvisCacheBase.inl>
#include <OSGGenvisCache.inl>

#define OSGGENVISCACHE_HEADER_CVSID "@(#)$Id: OSGGenvisCache.h,v 1.1 2003/09/11 16:20:29 fuenfzig Exp $"

#endif /* _OSGGENVISCACHE_H_ */
