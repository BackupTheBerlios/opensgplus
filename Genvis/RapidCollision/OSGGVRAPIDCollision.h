#ifndef OSGGVRAPIDCOLLISION_H
#define OSGGVRAPIDCOLLISION_H

#include <vector>
#include "OSGGVBase.h"
#include "OSGGVCollisionData.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVRAPIDAdapter.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief DataBase for collision detection with RAPID. RAPID creates one datastructure
           per object used in collision detection and reports just the ids of colliding
           triangles. This class stores with a list of colliding object adapters (getContacts)
	   a list of colliding face adapters (getContactFaces). 
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING RAPIDCollision : public BVolCollisionBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolCollisionBase<BasicTraits>      Inherited;
   typedef typename BasicTraits::TransformType TransformType;
   typedef OpenSGRAPIDAdapter<BasicTraits>     ObjectAdapterType;
   typedef OpenSGRAPIDFaceAdapter<BasicTraits> FaceAdapterType;
   typedef std::vector<FaceAdapterType>        FaceContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   RAPIDCollision ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline int                       getMode () const;
   virtual inline bool              getStopFirst () const;
   virtual void                     setStopFirst (bool flag=false);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Collision results.                                           */
   /*! \{                                                                 */
   inline FaceContainer&            getContactFaces ();
   inline const FaceContainer&      getContactFaces () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   virtual inline bool              Init (); 
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   i32                m_mode;
   FaceContainer      m_faces;
};
typedef  RAPIDCollision<OpenSGTraits> OSGRAPIDCollision;

template <class BasicTraits>
inline int RAPIDCollision<BasicTraits>::getMode () const
{
  return m_mode;
}
template <class BasicTraits>
inline bool RAPIDCollision<BasicTraits>::getStopFirst () const
{
  return Inherited::getStopFirst();
}

template <class BasicTraits>
inline RAPIDCollision<BasicTraits>::FaceContainer&       
RAPIDCollision<BasicTraits>::getContactFaces ()
{
   return m_faces;
}
template <class BasicTraits>
inline const RAPIDCollision<BasicTraits>::FaceContainer& 
RAPIDCollision<BasicTraits>::getContactFaces () const
{
   return m_faces;
}

template<class BasicTraits>
inline bool RAPIDCollision<BasicTraits>::Init ()
{
   bool result = Inherited::Init();
   //getContactFaces().reserve(GV_MAX_NUM_FACES);
   getContactFaces().clear();
   return result;
}

END_GENVIS_NAMESPACE
#endif

