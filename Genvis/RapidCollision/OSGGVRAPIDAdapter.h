#ifndef OSGGVRAPIDADAPTER_H
#define OSGGVRAPIDADAPTER_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVAdapterFactory.h"
class RAPID_model;

BEGIN_GENVIS_NAMESPACE

/** \brief FaceAdapter for collision detection with RAPID. This class is only
    necessary to implement the common interface for reporting collision pairs.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING OpenSGRAPIDFaceAdapter 
: public OpenSGTriangleBase<BasicTraits>, public Adapter
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGTriangleBase<BasicTraits> Inherited;
   typedef OpenSGObjectBase<BasicTraits>   InternalObjectType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   OpenSGRAPIDFaceAdapter ();
   OpenSGRAPIDFaceAdapter (InternalObjectType* adapter);
   OpenSGRAPIDFaceAdapter (InternalObjectType* adapter,
			   int                id);
   ~OpenSGRAPIDFaceAdapter ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};
typedef  OpenSGRAPIDFaceAdapter<OpenSGTraits> OSGRAPIDFaceAdapter;


/** \brief Adapter for collision detection with RAPID. This class stores a reference 
    to the RAPID datastructure.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING OpenSGRAPIDAdapter 
: public OpenSGObjectBase<BasicTraits>, public Adapter
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef OpenSGObjectBase<BasicTraits>       Inherited;
   typedef OpenSGRAPIDAdapter<BasicTraits>     SelfType;
   typedef FactoryHeap<SelfType>               FactoryType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   OpenSGRAPIDAdapter ();
   OpenSGRAPIDAdapter (const GeomObjectType& obj);
   OpenSGRAPIDAdapter (const TransformType&  m2w,
		       const GeomObjectType& obj);
   ~OpenSGRAPIDAdapter ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   void  init (const GeomObjectType& obj);
   void  init (const TransformType&  m2w,
	       const GeomObjectType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Class methods.                                               */
   /*! \{                                                                 */
   inline RAPID_model*   getModel     () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Identifier of adapter type.                                  */
   /*! \{                                                                 */
   static unsigned       getAdapterId ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   RAPID_model* m_model;
};
typedef OpenSGRAPIDAdapter<OpenSGTraits> OSGRAPIDAdapter;

template <class BasicTraits>
inline RAPID_model*   OpenSGRAPIDAdapter<BasicTraits>::getModel    () const
{
   return m_model;
}

END_GENVIS_NAMESPACE
#endif






