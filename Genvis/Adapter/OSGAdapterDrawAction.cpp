//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"

#include <OSGLog.h>
#include <OSGFieldContainer.h>
#include <OSGFieldContainerPtr.h>
#include <OSGNode.h>
#include <OSGNodeCore.h>
#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGTransform.h>
#include <OSGViewport.h>
#include <OSGCamera.h>
#include <OSGGL.h>
#include <OSGGeometry.h>
#include <OSGVolumeDraw.h>
#include <OSGGVBVolAdapterBase.h>
#include "OSGGVStaticInput.h"
#include "OSGGVInput.h"
#include "OSGAdapterDrawAction.h"

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::AdapterDrawAction
    \ingroup ActionLib
*/

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char AdapterDrawAction::cvsid[] = "@(#)$Id: OSGAdapterDrawAction.cpp,v 1.3 2004/12/20 15:56:34 fuenfzig Exp $";

AdapterDrawAction* AdapterDrawAction::_prototype = NULL;

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/



/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

#if 0
vector<Action::Functor>* AdapterDrawAction::_defaultEnterFunctors = NULL;
vector<Action::Functor>* AdapterDrawAction::_defaultLeaveFunctors = NULL;

void AdapterDrawAction::registerEnterDefault (const FieldContainerType &type, 
						  const Action::Functor &func )
{
    if ( ! _defaultEnterFunctors )
        _defaultEnterFunctors = new vector<Action::Functor>;

    while(type.getId() >= _defaultEnterFunctors->size())
    {
        _defaultEnterFunctors->push_back( 
                osgFunctionFunctor2(&Action::_defaultEnterFunction));
    }
    
    (*_defaultEnterFunctors)[ type.getId() ] = func;
}

void AdapterDrawAction::registerLeaveDefault (const FieldContainerType &type, 
						  const Action::Functor &func )
{
    if ( ! _defaultLeaveFunctors )
        _defaultLeaveFunctors = new vector<Action::Functor>;

    while(type.getId() >= _defaultLeaveFunctors->size())
    {
        _defaultLeaveFunctors->push_back( 
                osgFunctionFunctor2(&Action::_defaultLeaveFunction));
    }
    
    (*_defaultLeaveFunctors)[ type.getId() ] = func;
}
#endif

void AdapterDrawAction::setPrototype( AdapterDrawAction * proto )
{
    _prototype = proto;
}

AdapterDrawAction* AdapterDrawAction::getPrototype( void )
{
    return _prototype;
}

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/



/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

/** \brief Constructor
 */

AdapterDrawAction::AdapterDrawAction(void) 
  : Inherited ()
{
#if 0
    if ( _defaultEnterFunctors )
        _enterFunctors = *_defaultEnterFunctors;

    if ( _defaultLeaveFunctors )
        _leaveFunctors = *_defaultLeaveFunctors;
#else
    if ( Inherited::getDefaultEnterFunctors() != NULL) {
        _enterFunctors = *Inherited::getDefaultEnterFunctors();
    }
    if ( Inherited::getDefaultLeaveFunctors() != NULL) {
        _leaveFunctors = *Inherited::getDefaultLeaveFunctors();
    }
#endif
    setLocalMode();
}


/** \brief Copy-Constructor
 */

AdapterDrawAction::AdapterDrawAction( const AdapterDrawAction & source ) 
  : Inherited (source)
{
   setLocalMode(source.getLocalMode());
}

/** \brief create a new action
 */

AdapterDrawAction* AdapterDrawAction::create( void )
{
    AdapterDrawAction* act = NULL;
    if ( _prototype ) {
        act = new AdapterDrawAction( *_prototype );
    } else {
        act = new AdapterDrawAction();
    }
    return act;
}


/** \brief Destructor
 */

AdapterDrawAction::~AdapterDrawAction(void)
{
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

inline OSGCache::CacheData& getCacheData (OSGCache::CacheData& data, BVolAdapterBase* group)
{
   if (group->getParent() == NULL) { // no parent
      return data;
   }
   // triangle adapter
   OpenSGTriangleBase<OpenSGTraits>*  tri = 
     dynamic_cast<OpenSGTriangleBase<OpenSGTraits>*>(group->getParent());
   if (tri != NULL) {
      return tri->getObjectAdapter();
   }
   // object adapter
   OpenSGObjectBase<OpenSGTraits>* obj = 
     dynamic_cast<OpenSGObjectBase<OpenSGTraits>*>(group->getParent());
   if (obj != NULL) {
      return obj->getObjectAdapter();
   }
   // no adapter
   return data;
}

UInt32 AdapterDrawAction::selectVisiblesLocal (void)
{
   if (!getFrustumCulling()) {
        return getNNodes();
   }

   GLint l = false;
   bool  d = getVolumeDrawing();
   if ( d ) {
      l = glIsEnabled(GL_LIGHTING);
      glDisable(GL_LIGHTING);
   }

   useNodeList();
   
   UInt32 count = 0;
   for ( UInt32 i = 0; i < getNNodes(); ++i ) {
        if ( isVisible( getNode(i).getCPtr() ) ) {
            addNode( getNode(i) );
            ++count;
        }

        if (d) {
	   OSGCache::CacheData& data = OSGCache::the()[getNode(i)];

	   OSGCache::CacheData::AdapterContainer& all = 
	     data.getAdapter(BVolAdapterBase::getAdapterId());
	   // for BVolHierarchyBase there are more than one leaf nodes, then render
	   // for SingleHierarchyBase there are more than one inner nodes, then skip
	   if (all.size() > 1 && ((BVolAdapterBase*)all[0])->isInner()) { 
	      continue; 
	   }

	   glColor3f(0,1,0);
	   OSGCache::CacheData::AdapterContainer::const_iterator iter=all.begin();   
	   for (; iter != all.end(); ++iter) {
	      ((BVolAdapterBase*)*iter)->getBoundingVolume().drawWireframe();
	   }
        }
   }
   if (l) { 
      glEnable(GL_LIGHTING);
   }
   
   return count;
}
UInt32 AdapterDrawAction::selectVisiblesGlobal (void)
{
   if (!getFrustumCulling()) {
        return getNNodes();
   }

   GLint l = 0;
   bool  d = getVolumeDrawing();
   if ( d ) {
      l = glIsEnabled(GL_LIGHTING);
      glDisable(GL_LIGHTING);
   }

   useNodeList();
   
   UInt32 count = 0;
   for ( UInt32 i = 0; i < getNNodes(); ++i ) {
        if ( isVisible( getNode(i).getCPtr() ) ) {
            addNode( getNode(i) );
            ++count;
        }

        if (d) {
	   OSGCache::CacheData& data = OSGCache::the()[getNode(i)];

	   OSGCache::CacheData::AdapterContainer& all = 
	     data.getAdapter(BVolAdapterBase::getAdapterId());
	   // for BVolHierarchyBase there are more than one leaf nodes, then render
	   // for SingleHierarchyBase there are more than one inner nodes, then skip
	   if (all.size() > 1 && ((BVolAdapterBase*)all[0])->isInner()) { 
	      continue; 
	   }

	   glColor3f(0,1,0);
	   glPushMatrix();
	   glLoadIdentity();
	   getCamera()->setup(this, *getViewport());
	   //const Matrix& matrixI = data.getToWorldMatrix();
	   //glMultMatrixf(matrixI.getValues());
	   OSGCache::CacheData::AdapterContainer::const_iterator iter=all.begin();   
	   for (; iter != all.end(); ++iter) {
	      ((BVolAdapterBase*)*iter)->getBoundingVolume().drawWireframe();
	   }
	   glPopMatrix();
        }
   }
   if (l) { 
      glEnable(GL_LIGHTING);
   }
   
   return count;
}


UInt32 AdapterDrawAction::selectVisibles (void)
{
   return (this->*f_selectVisibles)();
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

/*

AdapterDrawAction& AdapterDrawAction::operator = (const AdapterDrawAction &source)
{
    if (this == &source)
        return *this;

    // copy parts inherited from parent
    *(static_cast<Inherited *>(this)) = source;

    // free mem alloced by members of 'this'

    // alloc new mem for members

    // copy 
}

*/

/*-------------------------- comparison -----------------------------------*/

/** \brief assignment
 */

bool AdapterDrawAction::operator < (const AdapterDrawAction& other) const
{
    return this < &other;
}

/** \brief equal
 */

bool AdapterDrawAction::operator == (const AdapterDrawAction& OSG_CHECK_ARG(other)) const
{
    return false;
}

/** \brief unequal
 */

bool AdapterDrawAction::operator != (const AdapterDrawAction &other) const
{
    return ! (*this == other);
}


/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/



///---------------------------------------------------------------------------
///  FUNCTION: 
///---------------------------------------------------------------------------
//:  Example for the head comment of a function
///---------------------------------------------------------------------------
///
//p: Paramaters: 
//p: 
///
//g: GlobalVars:
//g: 
///
//r: Return:
//r: 
///
//c: Caution:
//c: 
///
//a: Assumptions:
//a: 
///
//d: Description:
//d: 
///
//s: SeeAlso:
//s: 
///---------------------------------------------------------------------------

