/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000-2002 by the OpenSG Forum                   *
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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <queue>

#include "OSGConfig.h"

#include <OSGLog.h>
#include <OSGFieldContainer.h>
#include <OSGFieldContainerPtr.h>
#include <OSGNode.h>
#include <OSGNodeCore.h>
#include <OSGAction.h>
#include <OCRenderAction.h>
#include <OSGWindow.h>
#include <OSGCamera.h>
#include <OSGViewport.h>
#include <OSGBackground.h>
#include <OSGTransform.h>
#include <OSGGroup.h>
#include <OSGGeometry.h>
#include <OSGTypedFunctors.h>
#include <OSGVolumeDraw.h>
#include <OSGGeoFunctions.h>

#include <OSGBaseFunctions.h>
#if defined(OSG_OPT_DRAWTREE)
# include <OSGDrawTreeNodeFactory.h>
#else
# include <OSGDrawTreeNode.h>
#endif

#include <OSGMaterial.h>

#include <OSGGeometry.h>
#include <OSGLog.h>

#include <OSGLight.h>

#include <OSGGL.h>
#include <OSGVolumeDraw.h>
#include <OSGOcclusionTypes.h>

OSG_USING_NAMESPACE

float colr, colg, colb;

int vistriangles, vislines, vispoints;
int hidtriangles, hidlines, hidpoints;

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::OCRenderAction
    \ingroup GrpSystemAction

    The render action class.

*/

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char OCRenderAction::cvsid[] = "@(#)$Id: $";


/*! \brief the prototype which is copied to create new actions
 */

OCRenderAction *OCRenderAction::_pPrototype = NULL;

/*! \brief Default functors for instantiation
 */

std::vector<Action::Functor> *OCRenderAction::_vDefaultEnterFunctors = NULL;
std::vector<Action::Functor> *OCRenderAction::_vDefaultLeaveFunctors = NULL;


StatElemDesc<StatTimeElem> OCRenderAction::statDrawTime("OCdrawTime", "time for draw tree traversal");
StatElemDesc<StatIntElem > OCRenderAction::statNMaterials("OCNMaterials", "number of material changes");
StatElemDesc<StatIntElem > OCRenderAction::statNMatrices("OCNMatrices",  "number of matrix changes");
StatElemDesc<StatIntElem > OCRenderAction::statNGeometries("OCNGeometries", "number of Geometry nodes");
StatElemDesc<StatIntElem > OCRenderAction::statNTransGeometries("OCNTransGeometries", "number of transformed Geometry nodes");

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/



/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*! \brief Default registration. static, so it can be called during static init
 */

void OCRenderAction::registerEnterDefault(const FieldContainerType &type, 
                                        const Action::Functor    &func)
{
    if(_vDefaultEnterFunctors == NULL)
    {
        _vDefaultEnterFunctors = new std::vector<Action::Functor>;
    }

    while(type.getId() >= _vDefaultEnterFunctors->size())
    {
        _vDefaultEnterFunctors->push_back( 
            osgTypedFunctionFunctor2CPtrRef<
                ResultE, 
                CNodePtr,
                Action *                   >(&Action::_defaultEnterFunction));
    }
    
    (*_vDefaultEnterFunctors)[type.getId()] = func;
}

void OCRenderAction::registerLeaveDefault(const FieldContainerType &type, 
                                        const Action::Functor    &func)
{
    if(_vDefaultLeaveFunctors == NULL)
    {
        _vDefaultLeaveFunctors = new std::vector<Action::Functor>;
    }

    while(type.getId() >= _vDefaultLeaveFunctors->size())
    {
        _vDefaultLeaveFunctors->push_back( 
            osgTypedFunctionFunctor2CPtrRef<
                ResultE, 
                CNodePtr,
                Action *                   >(&Action::_defaultLeaveFunction));
    }
    
    (*_vDefaultLeaveFunctors)[type.getId()] = func;
}


/*! \brief  prototype access
 *  after setting the prototype all new DrawActions are clones of it
 */

void OCRenderAction::setPrototype(OCRenderAction *pPrototype)
{
    _pPrototype = pPrototype;
}

OCRenderAction *OCRenderAction::getPrototype(void)
{
    return _pPrototype;
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

OCRenderAction::OCRenderAction(void) :
     RenderAction           (),
#if defined(OSG_OPT_DRAWTREE)
    _pNodeFactory        (NULL),
#endif
    _pMaterial           (NULL),

    _uiMatrixId          (0),
    _currMatrix          (),

    _mMatMap             (),

    _pRoot               (NULL),
    _pMatRoot            (NULL),
    _pTransMatRoot       (NULL),

    _uiActiveMatrix      (0),
    _pActiveState        (NULL),

    _uiNumMaterialChanges(0),
    _uiNumMatrixChanges  (0),
    _uiNumGeometries     (0),
    _uiNumTransGeometries(0),

    _bSortTrans          (true),
    _bZWriteTrans        (false),

    _vLights(),
    _visibilityStack(),
	_octesttype(0),
	_showhidbb(false),
	_showvisbb(false),
	_maxpix(0)
{
/*
	registerEnterFunction(Geometry::getClassType(), osgTypedFunctionFunctor2CPtrRef<Action::ResultE, CNodePtr, Action*>(OCNodeAction::ovGeometryFunctions));
	registerEnterFunction(DirectionalLight::getClassType(), osgTypedFunctionFunctor2CPtrRef<Action::ResultE, CNodePtr, Action*>(OCNodeAction::ovLightFunctions));
	registerEnterFunction(Group::getClassType(), osgTypedFunctionFunctor2CPtrRef<Action::ResultE, CNodePtr, Action*>(OCNodeAction::ovGroupFunctions));
*/

	registerEnterFunction(Transform::getClassType(), osgTypedFunctionFunctor2CPtrRef<Action::ResultE, CNodePtr, Action*> (OCRenderFunctions::renderTransformEnter));
	registerLeaveFunction(Transform::getClassType(), osgTypedFunctionFunctor2CPtrRef<Action::ResultE, CNodePtr, Action*>(OCRenderFunctions::renderTransformLeave));
	registerEnterFunction(Group::getClassType(), osgTypedFunctionFunctor2CPtrRef<Action::ResultE, CNodePtr, Action*> (OCRenderFunctions::renderGroupEnter));
	registerLeaveFunction(Group::getClassType(), osgTypedFunctionFunctor2CPtrRef<Action::ResultE, CNodePtr, Action*>(OCRenderFunctions::renderGroupLeave));
	registerEnterFunction(Geometry::getClassType(), osgTypedFunctionFunctor2CPtrRef<Action::ResultE, CNodePtr, Action *>(OCRenderFunctions::renderMaterialDrawableActionHandler));

    if(_vDefaultEnterFunctors != NULL)
        _enterFunctors = *_vDefaultEnterFunctors;

    if(_vDefaultLeaveFunctors != NULL)
        _leaveFunctors = *_vDefaultLeaveFunctors;

    	NodePtr dummy1;
    	_nodeStack.reserve(10000);

#if defined(OSG_OPT_DRAWTREE)
    _pNodeFactory = new DrawTreeNodeFactory;
#endif
}


/** \brief Copy-Constructor
 */

OCRenderAction::OCRenderAction(const OCRenderAction &source) :
     RenderAction(source),
#if defined(OSG_OPT_DRAWTREE)
    _pNodeFactory        (NULL),
#endif
    _pMaterial           (source._pMaterial),

    _uiMatrixId          (source._uiMatrixId),
    _currMatrix          (source._currMatrix),

    _mMatMap             (source._mMatMap),

    _pRoot               (source._pRoot),
    _pMatRoot            (source._pMatRoot),
    _pTransMatRoot       (source._pTransMatRoot),

    _uiActiveMatrix      (source._uiActiveMatrix),
    _pActiveState        (source._pActiveState),

    _uiNumMaterialChanges(source._uiNumMaterialChanges),
    _uiNumMatrixChanges  (source._uiNumMatrixChanges),
    _uiNumGeometries     (source._uiNumGeometries),
    _uiNumTransGeometries(source._uiNumTransGeometries),

    _bSortTrans          (source._bSortTrans),
    _bZWriteTrans        (source._bZWriteTrans),
    _vLights             (source._vLights),
    _visibilityStack     (source._visibilityStack)
{
#if defined(OSG_OPT_DRAWTREE)
    _pNodeFactory = new DrawTreeNodeFactory;
#endif
}

/** \brief create a new DrawAction by cloning the prototype
 */

OCRenderAction * OCRenderAction::create(void)
{
    OCRenderAction *returnValue;
    
    if(_pPrototype)
    {
        returnValue = new OCRenderAction(*_pPrototype);
    }
    else
    {
        returnValue = new OCRenderAction();
    }

    return returnValue;
}


/** \brief Destructor
 */

OCRenderAction::~OCRenderAction(void)
{
#if defined(OSG_OPT_DRAWTREE)
    delete _pNodeFactory;
#endif    
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

// rendering state handling

void OCRenderAction::setMaterial(Material *pMaterial)
{
    _pMaterial = pMaterial;
}

void OCRenderAction::dropGeometry(Geometry *pGeo)
{
	std::cerr << "oerks\n";
}

void OCRenderAction::dropFunctor(Material::DrawFunctor &func, Material *mat)
{
    Material *pMat;
    State    *pState;
    
    if(_pMaterial != NULL)
    {
        pMat = _pMaterial;
    }
    else if(mat != NULL)
    {
        pMat = mat;
    }
    else
    {
        return;
    }

    MaterialMap::iterator it        = _mMatMap.find(pMat);

#if 0
    pMat->rebuildState();
#endif

    pState = pMat->getState().getCPtr();
    
#if defined(OSG_OPT_DRAWTREE)
        DrawTreeNode *pNewElem = _pNodeFactory->create();
#else
        DrawTreeNode *pNewElem = new DrawTreeNode;
#endif 
       
       if(it == _mMatMap.end())
       {
#if defined(OSG_OPT_DRAWTREE)
            DrawTreeNode *pNewMatElem = _pNodeFactory->create();
#else
            DrawTreeNode *pNewMatElem = new DrawTreeNode;
#endif            
            _mMatMap[pMat].push_back(pNewMatElem);
            
            pNewElem->setNode       (getActNode());
            
            pNewElem->setFunctor    (func);
            pNewElem->setMatrixStore(_currMatrix);
            
            pNewMatElem->addChild(pNewElem);
            pNewMatElem->setState(pState);
            
           _pMatRoot->addChild(pNewMatElem);
        }
        else
        {
            std::vector<DrawTreeNode *>::iterator treesIt = it->second.begin();
            std::vector<DrawTreeNode *>::iterator treesEnd= it->second.end();

            pNewElem->setFunctor    (func);
            pNewElem->setMatrixStore(_currMatrix);
            
            for(;treesIt != treesEnd; ++treesIt)
            {
               (*treesIt)->addChild(pNewElem);
            }
        }

    draw(_pMatRoot->getFirstChild());

    _mMatMap.clear();
#if defined(OSG_OPT_DRAWTREE)
    _pNodeFactory->freeAll();
#else
    subRefP(_pRoot);
    subRefP(_pMatRoot);
#endif

	    DrawTreeNode::_iCreateCount = 0;
	    DrawTreeNode::_iDeleteCount = 0;

#if defined(OSG_OPT_DRAWTREE)
    _pRoot = _pNodeFactory->create();
#else
    _pRoot         = new DrawTreeNode;
    addRefP(_pRoot);
#endif


#if defined(OSG_OPT_DRAWTREE)
    _pMatRoot = _pNodeFactory->create();
#else
    _pMatRoot      = new DrawTreeNode;

//    _pRoot->addChild(_pMatRoot);
    addRefP(_pMatRoot);
#endif

}

void OCRenderAction::dropLight(Light *pLight)
{
    if(pLight != NULL)
    {
        LightStore oStore;

        pLight->makeChunk();

        oStore.first  =  pLight->getChunk().getCPtr();
        oStore.second = _currMatrix.second;

        Matrix fromworld,tobeacon;
        
//        getActNode()->getToWorld(fromworld);

        fromworld = top_matrix();
        fromworld.invert();

        NodePtr beacon = pLight->getBeacon();

        if(beacon == NullFC)
        {
            SINFO << "draw: no beacon set!" << std::endl;
        }
        else
        {
            beacon->getToWorld(tobeacon);
    
            tobeacon.mult(fromworld);
            
            oStore.second.mult(tobeacon);
        }

        _vLights.push_back(oStore);
    }
}

bool OCRenderAction::isVisible( Node* node )
{
    if ( getFrustumCulling() == false )
        return true;
        
    getStatistics()->getElem(statCullTestedNodes)->inc();
    
    DynamicVolume vol;

//    node->getWorldVolume( vol );

    node->updateVolume();
    vol = node->getVolume();

    vol.transform(top_matrix());

    if ( _frustum.intersect( vol ) )
    {
// fprintf(stderr,"%p: node 0x%p vis\n", Thread::getCurrent(), node);
        return true;
    }
    
    getStatistics()->getElem(statCulledNodes)->inc();

// fprintf(stderr,"%p: node 0x%p invis\n", Thread::getCurrent(), node);
// _frustum.dump();
    return false;
}

//! enter a new level for visibility Return false if noed is not visible, true
//  if it is.

int OCRenderAction::pushVisibility(void)
{
    if(getFrustumCulling() == false)
        return 1;
    
    FrustumVolume::PlaneSet inplanes = FrustumVolume::P_NONE;
    /*
    FrustumVolume::PlaneSet inplanes = _visibilityStack.back();

    if(inplanes == FrustumVolume::P_ALL)
    {
        _visibilityStack.push_back(inplanes);
        return 1;
    }
    */
    int result=1;
    
    NodePtr node = getActNode();
    
    DynamicVolume vol = node->getVolume(true);
    FrustumVolume frustum = _frustum;

    updateTopMatrix();
    vol.transform(top_matrix());
    // vol.transform(_currMatrix.second);
    getStatistics()->getElem(statCullTestedNodes)->inc();
    
    if ( !intersect( frustum, vol, inplanes ) )
    {
        result = -1;
        getStatistics()->getElem(statCulledNodes)->inc();
    }
    else
    {
        if(inplanes == FrustumVolume::P_ALL)
        {
		result=1;
        }
        else
        {
		result=0;
        }
    }

   //  _visibilityStack.push_back(inplanes);
   //
    return result;
}

void OCRenderAction::popVisibility(void)
{
    if(getFrustumCulling() == false)
        return;

    _visibilityStack.pop_back();
}


/*-------------------------- your_category---------------------------------*/

void OCRenderAction::dump(DrawTreeNode *pRoot, UInt32 uiIndent)
{
    if(pRoot == NULL)
    {
        return;
    }

    indentLog(uiIndent, PWARNING);

    PWARNING << "Node : "   << pRoot                         << " | " 
             << "Geo : "    << pRoot->getGeometry()          << " | "
             << "State : "  << pRoot->getState()             << " | "
             << "Node : "   << pRoot->getNode()              << " | "
             << "Matrix : " << pRoot->getMatrixStore().first << std::endl;

    indentLog(uiIndent, PWARNING);
    PWARNING << "{" << std::endl;

    if(pRoot->getFirstChild() != NULL)
    {
        dump(pRoot->getFirstChild(), uiIndent + 4);
    }

    indentLog(uiIndent, PWARNING);
    PWARNING << "}" << std::endl;

    if(pRoot->getBrother() != NULL)
    {
        dump(pRoot->getBrother(), uiIndent);
    }
}

//#define PRINT_MAT

void OCRenderAction::draw(DrawTreeNode *pRoot)
{
    if(pRoot == NULL)
    {
        return;
    }

    UInt32 uiNextMatrix = pRoot->getMatrixStore().first;

    if(uiNextMatrix != 0 && uiNextMatrix != _uiActiveMatrix)
    {
        glLoadMatrixf(pRoot->getMatrixStore().second.getValues());

        _uiActiveMatrix = uiNextMatrix;

        _uiNumMatrixChanges++;

        _currMatrix.second = pRoot->getMatrixStore().second;
        updateTopMatrix();
    }

    State *pNewState = pRoot->getState();

    if(pNewState != NULL)
    {
        if(_pActiveState != NULL)
        {
            if(pNewState != _pActiveState)
            {
                pNewState->changeFrom(this, _pActiveState);

                _pActiveState = pNewState;
                
                _uiNumMaterialChanges++;
            }
        }
        else
        {
            _pActiveState = pRoot->getState();
             
            _pActiveState->activate(this);

            _uiNumMaterialChanges++;
        }
    }

    setActNode(pRoot->getNode());
    
    if(pRoot->getGeometry() != NULL)
    {
        pRoot->getGeometry()->drawPrimitives(this);

        _uiNumGeometries++;
    }
    else if(pRoot->hasFunctor())
    {
    	_rendered++;
        pRoot->getFunctor().call(this);

        _uiNumGeometries++;
    }

    if(pRoot->getFirstChild() != NULL)
    {
        draw(pRoot->getFirstChild());
    }

    if(pRoot->getBrother() != NULL)
    {
        draw(pRoot->getBrother());
    }

}

void OCRenderAction::setSortTrans(bool bVal)
{
    _bSortTrans = bVal;
}

void OCRenderAction::setZWriteTrans(bool bVal)
{
    _bZWriteTrans = bVal;
}

// initialisation

Action::ResultE OCRenderAction::start(void)
{
    RenderActionBase::start();

    _rendered=0;
    
	vistriangles=0;
	vislines=0;
	vispoints=0;
	hidtriangles=0;
	hidlines=0;
	hidpoints=0;

	if(_octesttype==OCTestHP)
		_occltesthp.init(getViewport(), _maxpix);
	else if(_octesttype==OCTestNV)
		_occltestnv.init(getViewport(), _maxpix);
	else if(_octesttype==OCTestStencil)
		_occlteststencil.init(getViewport(), _maxpix);
	else if(_octesttype==OCTestZB)
		_occltestzb.init(getViewport(), _maxpix);
    

    colr=1.0;
    colb=1.0;
    colg=1.0;
    
    if(_window != NULL)
    {
        _window->resizeGL();
    }

    _uiMatrixId = 1;

    _currMatrix.first = 1;
    _currMatrix.second.setIdentity();

    _visibilityStack.clear();
    _visibilityStack.push_back(FrustumVolume::P_NONE);
    
    bool full;
    
    if(_viewport != NULL)
    {
        GLint pl  = _viewport->getPixelLeft();
        GLint pr  = _viewport->getPixelRight();
        GLint pb  = _viewport->getPixelBottom();
        GLint pt  = _viewport->getPixelTop();
        GLint pw  = pr - pl + 1;
        GLint ph  = pt - pb + 1;
        
        full = _viewport->isFullWindow();

        glViewport(pl, pb, pw, ph);

        if (full == false)
        {
            glScissor (pl, pb, pw, ph);
            glEnable(GL_SCISSOR_TEST);
        }

        if(_camera != NULL)
        {
            _camera->setupProjection(this, *_viewport);

            // set the viewing
            
            _camera->getViewing(_currMatrix.second, 
                                _viewport->getPixelWidth (),
                                _viewport->getPixelHeight());

            _camInverse.invertFrom(_currMatrix.second);
            
            glMatrixMode(GL_MODELVIEW);
        }

        if(_background != NULL)
        {
            _background->clear(this, _viewport);
        }
    }
    
    updateTopMatrix();
	
    float fm[16];
    glGetFloatv(GL_PROJECTION_MATRIX,fm);
    _projmatrix.setValue(fm);

    _mMatMap.clear();

#if defined(OSG_OPT_DRAWTREE)
    _pNodeFactory->freeAll();
#else
    subRefP(_pRoot);
    subRefP(_pMatRoot);
#endif

/*
    if(_pRoot != NULL)
    {
        fprintf(stderr, "CDN %d DDN %d ODN %d ",
                DrawTreeNode::_iCreateCount,
                DrawTreeNode::_iDeleteCount,
                DrawTreeNode::_iCreateCount - DrawTreeNode::_iDeleteCount);
    }
*/

    DrawTreeNode::_iCreateCount = 0;
    DrawTreeNode::_iDeleteCount = 0;

#if defined(OSG_OPT_DRAWTREE)
    _pRoot = _pNodeFactory->create();
#else
    _pRoot         = new DrawTreeNode;
    addRefP(_pRoot);
#endif

#if defined(OSG_OPT_DRAWTREE)
    _pMatRoot = _pNodeFactory->create();
#else
    _pMatRoot      = new DrawTreeNode;

//    _pRoot->addChild(_pMatRoot);
    addRefP(_pMatRoot);
#endif

    _pActiveState   = NULL;

    _uiActiveMatrix = 0;

    _uiNumMaterialChanges = 0;
    _uiNumMatrixChanges   = 0;
    _uiNumGeometries      = 0;
    _uiNumTransGeometries = 0;

    _vLights.clear();

    if(_viewport != NULL && full == false)
    {
        glDisable(GL_SCISSOR_TEST);
    }

    return Action::Continue;
}

Action::ResultE OCRenderAction::stop(ResultE res)
{
    if(!_ownStat)
       getStatistics()->getElem(statDrawTime)->start();
    
    UInt32 i;

//    dump(_pRoot, 0);
//    dump(_pMatRoot, 0);
//    dump(_pTransMatRoot, 0);

#if defined(OSG_OPT_DRAWTREE)
    //    _pNodeFactory->printStat();    
#endif

    for(i = 0; i < _vLights.size(); i++)
    {
        glLoadMatrixf(_vLights[i].second.getValues());
        _vLights[i].first->activate(this, i);
    }

    draw(_pMatRoot->getFirstChild());

    if(_pActiveState != NULL)
    {
        _pActiveState->deactivate(this);
    }

    for(i = 0; i < _vLights.size(); i++)
    {
        _vLights[i].first->deactivate(this, i);
    }

    if(!_ownStat)
    {
        glFinish();
        getStatistics()->getElem(statDrawTime)->stop();

        getStatistics()->getElem(statNMaterials      )->set(
            _uiNumMaterialChanges);
        getStatistics()->getElem(statNMatrices       )->set(
            _uiNumMatrixChanges);
        getStatistics()->getElem(statNGeometries     )->set(
            _uiNumGeometries);
    }
  /*  
    std::cerr << "r: " << _rendered << "\n";
   */
    RenderActionBase::stop(res);
    
	if(_octesttype==OCTestHP){
		_occltesthp.frameExit();
		_occltesthp.printStatistics();
	}else if(_octesttype==OCTestNV)
		_occltestnv.frameExit();
	else if(_octesttype==OCTestStencil)
		_occlteststencil.frameExit();
	else if(_octesttype==OCTestZB)
		_occltestzb.frameExit();

/*	
	std::cerr << "visible: " << vistriangles << " ";
	std::cerr << "hidden: " << hidtriangles << " ";
	std::cerr << "sum: " << hidtriangles+vistriangles << std::endl;
 */  
	
    return res;
}

void OCRenderAction::push_matrix(const Matrix &matrix)
{
    _currMatrix.second.mult(matrix);
    updateTopMatrix();
}

void OCRenderAction::pop_matrix(void)
{
}

Action::ResultE OCRenderAction::apply(std::vector<NodePtr>::iterator begin,
                              std::vector<NodePtr>::iterator end)
{
    
    Action::ResultE res = Continue;

    // call the start function and its' returns
    if ( ( res = start() ) != Continue )
        return res;

    // call the given nodes

    for ( ; begin != end; begin ++ )
    {
        if ( *begin == NullFC )
        {
            SWARNING << "apply: encountered NullNode!" << std::endl;
            return Quit;
        }
        else
        {
            myrecurse( *begin );
        }
    }

    // call the stop function and its' returns
    res = stop( res );

    return res;
}

Action::ResultE OCRenderAction::apply(NodePtr node)
{
    _lastprionode=0;
    
    if(node == NullFC)
    {
        SWARNING << "apply: node is Null!" << std::endl;
        return Quit;
    }

    Action::ResultE res = Continue;

    // call the start function and its' returns
    if ( ( res = start() ) != Continue )
        return res;

    // call the given nodes

    myrecurse(node);

    // call the stop function and its' returns
    res = stop( res );

    return res;
}

void OCRenderAction::myrecurse(NodePtr node){
	// _nodeStack.clear();
	bool vfc=true;
	checkNode(node, vfc);

	while(!_pqueue.empty()){
		    NodePtr node=_pqueue.top().core->node;
		    _currMatrix=_pqueue.top().core->currMatrix;
		    vfc=_pqueue.top().core->vfc;
		    _pqueue.pop();
	            checkNode(node, vfc);
	}
   /* 
	while(!_nodeStack.empty()){
		NodePtr node=_nodeStack.back();
		callLeave(node);
		_nodeStack.pop_back();
	}
    */
}

void OCRenderAction::checkNode(NodePtr node, const bool& vfc)
{
	int vfcresult=1;
	do{
	    if(node==NullFC) return ;
	    if(node->getActive() == false) return ;

	    if(node->getCore() == NullFC){
	        SWARNING << "recurse: core is Null,  don't know what to do!"
        	         << std::endl;
	        return ;
	    }

	    setActNode(node);
	   
	    if(vfc){
		vfcresult=pushVisibility();
		if(vfcresult==-1)
		    return;
	    }
	    
//	    if(_rendered>=0){
		    OCTestNode ocnode;
	    	    glLoadMatrixf(_currMatrix.second.getValues());
		    ocnode._node=node;
		    transformNode(node, ocnode._x, ocnode._y, ocnode._z, ocnode._index);
	    
		    _ocres=1;
		    if(_octesttype==OCTestNone)
			_ocres=1;
		    else if(_octesttype==OCTestHP)
			_ocres=_occltesthp.testNode(&ocnode);
		    else if(_octesttype==OCTestNV)
			_ocres=_occltestnv.testNode(&ocnode);
		    else if(_octesttype==OCTestStencil)
			_ocres=_occlteststencil.testNode(&ocnode);
		    else if(_octesttype==OCTestZB)
			_ocres=_occltestzb.testNode(&ocnode);
		    else if(_octesttype==OCTestDummy)
			_ocres=_occltestdummy.testNode(&ocnode);
	
		    if(_ocres<=0){
			    if(_showhidbb){
				    glDisable(GL_DEPTH_TEST);
				    glDisable(GL_LIGHTING);
				    if(_ocres==-2){
					glColor3f(1.0,1.0, colb);
				    }else if(_ocres==-3){
					glColor3f(1.0,colg, colb);
				    }else{
					glColor3f(colr, colg, 1.0);
				    }
				    colg-=0.01;
				    colr-=0.01;
				    colb-=0.01;
				    DynamicVolume vol;
				    node->updateVolume();
				    vol=node->getVolume();
				    drawVolume(vol);
		    		    glEnable(GL_LIGHTING);
				    glEnable(GL_DEPTH_TEST);
			    }
			    calcPrimitives(node, hidtriangles, hidlines, hidpoints);
			    return ;
       	     	     }else if(_showvisbb){
			    glDisable(GL_LIGHTING);
			    if(_ocres==1){
 				// glColor3f(colr, 0.0, 0.0);
 				glColor3f(1.0, 1.0, 1.0);
			    }else{
				glColor3f(0.0, 0.0, 1.0);
			    }
			    glColor3f(colr, 1.0, colb);
			    colg-=0.01;
			    colr-=0.01;
			    colb-=0.01;
			    DynamicVolume vol;
			    node->updateVolume();
			    vol=node->getVolume();
			    drawVolume(vol);
	    		    glEnable(GL_LIGHTING);
		   }
//	    }
	    
	    if(callEnter(node)!=Continue){
	            return;
    	    }
	    // _nodeStack.push_back(node);
	    
    	    if(node->getNChildren()==1){
		    node=node->getChild(0);
	    }else{
	    	    break;
 	    }
    	}while(1);

	if(node->getCore()->getTypeId()==GeometryBase::getClassTypeId()){
		calcPrimitives(node, vistriangles, vislines, vispoints);
	}
	
	std::vector<NodePtr>::iterator nodeit;
	for (nodeit = node->getMFChildren()->begin(); nodeit != node->getMFChildren()->end(); nodeit++ )
	{
		const Pnt3f boxmin=(*nodeit)->getVolume().getMin();
		const Pnt3f boxmax=(*nodeit)->getVolume().getMax();
		
		float result=0.0;
		for(int x=0; x<3; x++){
			const float m=_currMatrix.second[x][2];
			result+=(m*boxmin[x]>m*boxmax[x]?m*boxmin[x]:m*boxmax[x]);
		}
		result+=_currMatrix.second[3][2];
		
		_prionodes[_lastprionode].currMatrix=_currMatrix;
		_prionodes[_lastprionode].node=*nodeit;
		if(vfcresult==1)
			_prionodes[_lastprionode].vfc=false;
		else
			_prionodes[_lastprionode].vfc=true;

		PrioNode prnode;
	 	prnode.priority=result;
		prnode.core=&(_prionodes[_lastprionode]);
		_pqueue.push(prnode);
		_lastprionode++;
	}
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

/*-------------------------- comparison -----------------------------------*/


/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/


std::vector<OCRenderAction::Functor> *OCRenderAction::getDefaultEnterFunctors(void)
{
    return _vDefaultEnterFunctors;
}

std::vector<OCRenderAction::Functor> *OCRenderAction::getDefaultLeaveFunctors(void)
{
    return _vDefaultLeaveFunctors;
}

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



// **************************************

Action::ResultE OCRenderFunctions::renderTransformEnter(CNodePtr &, Action *action)
{
    OCRenderAction *pAction = dynamic_cast<OCRenderAction *>(action);
//    pAction->pushVisibility();
    NodePtr node = action->getActNode();
    TransformPtr p=TransformPtr::dcast(node->getCore());
    pAction->push_matrix(p->getMatrix());
    return Action::Continue;
}

Action::ResultE OCRenderFunctions::renderTransformLeave(CNodePtr &node, Action *action)
{
//   OCRenderAction *pAction = dynamic_cast<OCRenderAction *>(action);
//   pAction->popVisibility();
//    pAction->pop_matrix();
    return Action::Continue;
}

Action::ResultE OCRenderFunctions::renderGroupEnter(CNodePtr &, Action *action)
{
//    OCRenderAction *ra = dynamic_cast<OCRenderAction *>(action);
//    ra->pushVisibility();
    return Action::Continue;
}

Action::ResultE OCRenderFunctions::renderGroupLeave(CNodePtr &, Action *action)
{
 //   OCRenderAction *ra = dynamic_cast<OCRenderAction *>(action);
 //   ra->popVisibility();
    return Action::Continue;
}

Action::ResultE OCRenderFunctions::renderMaterialDrawableActionHandler(CNodePtr&, Action *action)
{
    OCRenderAction *a = dynamic_cast<OCRenderAction *>(action);

    Material::DrawFunctor func;
    NodePtr node = action->getActNode();

    GeometryPtr p=GeometryPtr::dcast(node->getCore());
    func = osgTypedMethodFunctor1ObjPtr(&(*p), &Geometry::drawPrimitives);

    Material* m = a->getMaterial();

    if(m == NULL)
    {
        if(p->getMaterial() != NullFC)
        {
            m = p->getMaterial().getCPtr();
        }
        else
        {
            fprintf(stderr, "MaterialDrawable::render: no Material!?!\n");
            return Action::Continue;
        }
    }

    a->dropFunctor(func, m);
    
    return Action::Continue;
}

void OCRenderAction::setOcclusionTest(const int t){ _octesttype=t; };
void OCRenderAction::setShowHiddenBB(const bool v){ _showhidbb=v; };
void OCRenderAction::setShowVisibleBB(const bool v){ _showvisbb=v; };

void OCRenderAction::calcPrimitives(NodePtr node, int& tri, int& lin, int& pnt){
	std::vector<NodePtr>::iterator nodeit;
	for (nodeit = node->getMFChildren()->begin(); nodeit != node->getMFChildren()->end(); nodeit++ ){
		calcPrimitives(*nodeit, tri, lin, pnt);
	}
	if(node->getCore()->getTypeId()==GeometryBase::getClassTypeId()){
	    	GeometryPtr geo=GeometryPtr::dcast(node->getCore());
		unsigned int t=0,l=0,p=0;
		calcPrimitiveCount(geo, t, l, p);
		tri+=t;
		lin+=l;
		pnt+=p;
	}
}
