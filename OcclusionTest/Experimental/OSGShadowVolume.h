/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *                 Copyright (C) 2000 by the OpenSG Forum                    *
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


#ifndef SHADOWVOLUME_CLASS_DECLARATION
#define SHADOWVOLUME_CLASS_DECLARATION

#include "OSGConfig.h"
#include "OSGBase.h"
#include "OSGVolume.h"
#include "OSGPlane.h"
#include "OSGMatrix.h"

OSG_BEGIN_NAMESPACE

#ifdef WIN32 // Workaround for a bug in Visual C++ 6.0
class ShadowVolume;

OSG_BASE_DLLMAPPING
bool operator ==(const ShadowVolume &b1, const ShadowVolume &b2);

OSG_BASE_DLLMAPPING
inline
bool operator !=(const ShadowVolume &b1, const ShadowVolume &b2);

ostream& operator<< (ostream & os, const ShadowVolume &obj);
#endif

/** 
@brief 3D shadow frustum defined by n planes.
@author dstaneker

*/

class OSG_BASE_DLLMAPPING ShadowVolume : public Volume
{
    /*==========================  PUBLIC  =================================*/
  public:
    
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */
      
    inline ShadowVolume (                                              ); 
    inline ShadowVolume ( const UInt16&                                    );
    inline ShadowVolume ( const ShadowVolume &obj                         );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */
    
    inline ~ShadowVolume(); 
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Get                                       */
    /*! \{                                                                 */
    
    inline  const   Plane & getNear         (void           ) const;
    inline  const   Plane * getPlanes       (void           ); 
    inline  const   Plane * getPlanes       (void           ) const; 
    virtual         void    getCenter       (Pnt3f &center  ) const;
    virtual         Real32  getScalarVolume (void           ) const;
    virtual         void    getBounds       ( Pnt3f &minPnt,
                                              Pnt3f &maxPnt ) const;
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Get                                       */
    /*! \{                                                                 */

    void setPlane(const Plane&, const UInt16);
                                    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Extending                                 */
    /*! \{                                                                 */

    virtual     void extendBy   (const Pnt3f            &pt         );
    inline          void extendBy   (const Volume           &volume     );   
    inline          void extendBy   (const ShadowVolume    &bb         );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Intersection                              */
    /*! \{                                                                 */
  
            bool intersect      (const Pnt3f            &point      ) const;
            bool intersect          (const Line             &line       ) const;
            bool intersect          (const Line             &line,
                                       Real32           &minDist,
                                       Real32           &maxDist    ) const;
    inline      bool intersect          (const Volume           &volume         ) const;
    inline      bool intersect      (const ShadowVolume    &bb             ) const;
    virtual bool isOnSurface    (const Pnt3f            &point      ) const;


    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Transformation                            */
    /*! \{                                                                 */
  
    virtual void transform      (const Matrix           &m          );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Assignment                                 */
    /*! \{                                                                 */
    
    const ShadowVolume &operator =(const ShadowVolume &b1);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Output                                  */
    /*! \{                                                                 */


    virtual void dump(      UInt32    uiIndent = 0, 
                      const BitVector bvFlags  = 0) const;
    
    /*! \}                                                                 */

    /*==========================  PRIVATE  ================================*/
  private:

    UInt16 _numPlanes;
    Plane* _planes;
};

typedef ShadowVolume* ShadowVolumePtr;

OSG_END_NAMESPACE

#include "OSGShadowVolume.inl"

#endif /* SHADOWVOLUME_CLASS_DECLARATION */
