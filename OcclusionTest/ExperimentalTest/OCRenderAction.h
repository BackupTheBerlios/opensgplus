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

#ifndef _OCRENDERACTION_H_
#define _OCRENDERACTION_H_
#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <vector>
#include <queue>

#include <OSGSystemDef.h>
#include <OSGBaseTypes.h>
#include <OSGRenderAction.h>
#include <OSGMatrix.h>
#include <OSGMaterial.h>
#include <OSGMaterialDrawable.h>
#include <OSGStatElemTypes.h>

#include<OSGOcclusionStatistics.h>
#include<OSGOcclusionTestDummy.h>
#include<OSGOcclusionZBTest.h>

#include"OCOcclusionTest.h"

OSG_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
//  Forward References
//---------------------------------------------------------------------------

class Material;
class DrawTreeNode;
class Geometry;
class State;
class Light;
class LightChunk;

#if defined(OSG_OPT_DRAWTREE)
class DrawTreeNodeFactory;
#endif

//---------------------------------------------------------------------------
//   Types
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Class
//---------------------------------------------------------------------------

typedef std::pair<UInt32      ,      Matrix                 > MatrixStore;
typedef std::map <Material   *, std::vector<DrawTreeNode *> > MaterialMap;
typedef std::pair<LightChunk *,      Matrix                 > LightStore;

/*! \brief OCRenderAction class
 */

class OSG_SYSTEMLIB_DLLMAPPING OCRenderAction : public RenderAction
{
  public:

    //-----------------------------------------------------------------------
    //   constants                                                               
    //-----------------------------------------------------------------------

    static StatElemDesc<StatTimeElem> statDrawTime;
    static StatElemDesc<StatIntElem > statNMaterials;
    static StatElemDesc<StatIntElem > statNMatrices;
    static StatElemDesc<StatIntElem > statNGeometries;
    static StatElemDesc<StatIntElem > statNTransGeometries;

    //-----------------------------------------------------------------------
    //   enums                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   types                                                               
    //-----------------------------------------------------------------------
  
    //-----------------------------------------------------------------------
    //   class functions                                                     
    //-----------------------------------------------------------------------

    static OCRenderAction *create      (void                    );
    
    static void          setPrototype(OCRenderAction *pPrototype);
    static OCRenderAction *getPrototype(void                    );


    static void registerEnterDefault (const FieldContainerType &type, 
                                      const Action::Functor    &func);
    
    static void registerLeaveDefault (const FieldContainerType &type, 
                                      const Action::Functor    &func);

    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------

    virtual ~OCRenderAction(void); 

    /*------------------------- your_category -------------------------------*/

    Material   *getMaterial  (void               ) const;
    void        setMaterial  (Material *pMaterial);
    
    virtual Action::ResultE start(void       );
    virtual Action::ResultE stop (ResultE res); 

	    

    /*------------------------- your_operators ------------------------------*/

           void          push_matrix(const Matrix &matrix);
           void          pop_matrix (      void          );

    inline const Matrix &top_matrix (      void          );

    /*------------------------- assignment ----------------------------------*/

    void dropGeometry(Geometry  *pGeo);
    void dropLight   (Light     *pLight);
    void dropFunctor (Material::DrawFunctor &func, Material *mat);

    /*------------------------- comparison ----------------------------------*/

    void setSortTrans(bool bVal);
    void setZWriteTrans(bool bVal);

    /*------------------------- comparison ----------------------------------*/

    // test a single node
    bool isVisible( Node* node );
    
    // visibility levels
    int  pushVisibility(void);
    void  popVisibility(void);


	enum { OCTestNone=0, OCTestHP=1, OCTestNV=2, OCTestStencil=3, OCTestZB=4, OCTestDummy=5, OCTestTwoPass=6 };	// FIXME UGLY
	
	void setOcclusionTest(const int);
	void setShowHiddenBB(const bool v);
	void setShowVisibleBB(const bool v);
	void setSoftTest(const bool v);
	void setShadowTest(const bool v);
	void setPredictionTest(const bool v);
	void setExactOM(const bool v);
	void setMaxPix(const int& mp);
    
  protected:

	virtual ResultE apply(std::vector<NodePtr>::iterator begin, std::vector<NodePtr>::iterator end);
	virtual ResultE apply(NodePtr node);

    //-----------------------------------------------------------------------
    //   enums                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   types                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   class variables                                                     
    //-----------------------------------------------------------------------

    static OCRenderAction    *_pPrototype;

    static std::vector<Functor> *_vDefaultEnterFunctors;
    static std::vector<Functor> *_vDefaultLeaveFunctors;

    //-----------------------------------------------------------------------
    //   class functions                                                     
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   instance variables                                                  
    //-----------------------------------------------------------------------

#if defined(OSG_OPT_DRAWTREE)
    DrawTreeNodeFactory      *_pNodeFactory;
#endif

    Material                 *_pMaterial;

    UInt32                    _uiMatrixId;

    MatrixStore               _currMatrix;
    Matrix                    _camInverse;
    Matrix		      _forWorldMatrix;

    MaterialMap               _mMatMap;
    
    DrawTreeNode             *_pRoot;
    DrawTreeNode             *_pMatRoot;
    DrawTreeNode             *_pTransMatRoot;

    UInt32                    _uiActiveMatrix;
    State                    *_pActiveState;

    UInt32                    _uiNumMaterialChanges;
    UInt32                    _uiNumMatrixChanges;
    UInt32                    _uiNumGeometries;
    UInt32                    _uiNumTransGeometries;
 
    bool                      _bSortTrans;
    bool                      _bZWriteTrans;

    std::vector<LightStore>   _vLights;
    
    std::vector<FrustumVolume::PlaneSet>  _visibilityStack;

//    Time                 _tMatSlot

    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------

    // prohibit default functions (move to 'public' if you need one)

    OCRenderAction(void);
    OCRenderAction(const OCRenderAction &source);

    void operator =(const OCRenderAction &source);

    // access default functors

    virtual std::vector<Functor> *getDefaultEnterFunctors(void);
    virtual std::vector<Functor> *getDefaultLeaveFunctors(void);

            void dump(DrawTreeNode *pRoot, UInt32 uiIndent);
    virtual void draw(DrawTreeNode *pRoot);

    inline  void updateTopMatrix(void);

  private:

    	int _rendered;

	int _octesttype;
	bool _showhidbb;
	bool _showvisbb;
	int _ocres;
	int _maxpix;
	
	OCOcclusionTest<OcclusionHPTest> _occltesthp;
	OCOcclusionTest<OcclusionHPTestStats> _occltesthpstats;
	OCOcclusionTest<OcclusionNVTest> _occltestnv;
	OCOcclusionTest<OcclusionNVTestStats> _occltestnvstats;
	OCOcclusionTest<OcclusionStencilTest> _occlteststencil;
	OCOcclusionTest<OcclusionStencilTestStats> _occlteststencilstats;
	OCOcclusionTest<OcclusionTest<OcclusionZBTest<16, 16>, OcclusionStatisticsOff> > _occltestzb;
	OCOcclusionTest<OcclusionTest<OcclusionZBTest<16, 16>, OcclusionStatisticsOn> > _occltestzbstats;
	OCOcclusionTest<OcclusionTest<OcclusionTestDummy, OcclusionStatisticsOff> > _occltestdummy;
	OCOcclusionTest<OcclusionTest<OcclusionTestDummy, OcclusionStatisticsOn> > _occltestdummystats;
	
	Matrix _projmatrix;

	class PrioCore{
		public:
		NodePtr node;
		bool vfc;
    		MatrixStore currMatrix;
	};
	
    	class PrioNode{
		public:
		Real32 priority;
		PrioCore* core;

		bool operator<(const PrioNode &a) const {
		        return (priority<a.priority);
		};
	};

	int _lastprionode;
	PrioCore _prionodes[20000];
    	std::priority_queue<PrioNode> _pqueue;
    	std::vector<NodePtr> _nodeStack;

	inline void transformNode(NodePtr& node, short* bb_x, short* bb_y, Real32* bb_z, char& bb_minidx);
	void checkNode(NodePtr node, const bool& vfc);
	void myrecurse(NodePtr node);
	void calcPrimitives(NodePtr node, int& tri, int& lin, int& pnt);
	
    //-----------------------------------------------------------------------
    //   enums                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   types                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   friend classes                                                      
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   friend functions                                                    
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   class variables                                                     
    //-----------------------------------------------------------------------

    static char cvsid[];
    
    //-----------------------------------------------------------------------
    //   class functions                                                     
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   instance variables                                                  
    //-----------------------------------------------------------------------
    
    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------
};

class OCRenderFunctions{
	public:
	static Action::ResultE renderTransformEnter(CNodePtr &, Action *);
	static Action::ResultE renderTransformLeave(CNodePtr &, Action *);
	static Action::ResultE renderGroupEnter(CNodePtr &, Action *action);
	static Action::ResultE renderGroupLeave(CNodePtr &, Action *action);
	static Action::ResultE renderMaterialDrawableActionHandler(CNodePtr&, Action *action);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

OSG_END_NAMESPACE

#include "OCRenderAction.inl"

#endif /* _OCRENDERACTION_H_ */


