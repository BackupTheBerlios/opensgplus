#ifndef OSGGVSTATICINPUT_H
#define OSGGVSTATICINPUT_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base class for creating secondary structures with scenegraph entities. 
    Adaptation of scenegraph entities is done with adapter classes, for example
    \see OpenSGObject2BVol
    \see OpenSGFace2BVol
    \see OpenSGPrimitive2BVol 
    The adapter objects are created by a cache traversal, at which functors have
    been registered.
    Memory management of this adapter objects is done by destroy and clear.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING StaticInput
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::TransformType  TransformType;
   typedef typename BasicTraits::GeomObjectType GeomObjectType;
   typedef typename BasicTraits::GeomFaceType   GeomFaceType;
   typedef typename BasicTraits::Cache          Cache;
   typedef typename BasicTraits::CacheData      CacheData;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline StaticInput ();
   virtual inline ~StaticInput();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Cache.                                                       */
   /*! \{                                                                 */
   /*! Register functors at the cache. The adapter objects are created by
       this functors, which are node type-specific. */
   virtual void registerFunctors () = 0;
   /*! Process called after each cache application. */
   virtual inline void process (const GeomObjectType& node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Memory management.                                           */
   /*! \{                                                                 */
   /*! Delete the secondary structures and the adapter objects.
       Certainly also the references in the cache are cleared.            */
   virtual inline void destroy ();
   /*! Delete the secondary structure only. The adapter objects keep alive
       and referenced by the cache. */
   virtual inline void clear   ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};
typedef StaticInput<OpenSGTraits> OSGStaticInput;


template<class BasicTraits> 
inline StaticInput<BasicTraits>::StaticInput ()
{
}

template <class BasicTraits>
inline StaticInput<BasicTraits>::~StaticInput()
{
}

template <class BasicTraits>
inline void   StaticInput<BasicTraits>::clear ()
{
}
template <class BasicTraits>
inline void   StaticInput<BasicTraits>::destroy ()
{
}

template <class BasicTraits>
inline void StaticInput<BasicTraits>::process (const GeomObjectType&)
{
}

END_GENVIS_NAMESPACE
#endif

