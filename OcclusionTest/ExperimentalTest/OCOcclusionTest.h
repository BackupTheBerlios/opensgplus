/*---------------------------------------------------------------------------*\
 *                                                                           *
 *  OSGViewer                                                                *
 *  -----------------------------------------------------------------------  *
 *                                                                           *
 *  copyright by Dirk Staneker 2001                                          *
 *  Wilhelm Schickard Institut fuer Informatik                               *
 *  Grafisch Interaktive Systeme                                             *
 *  mailto:staneker@gris.uni-tuebingen.de                                    *
 *  http://www.gris.uni-tuebingen.de/~dstaneke/                              *
 *                                                                           *
\*---------------------------------------------------------------------------*/
 
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This application is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU Library General Public License as published *
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

#ifndef _OCOCCLUSIONTEST_H

#define _OCOCCLUSIONTEST_H 1

#include<OSGDrawAction.h>
#include<OSGRenderAction.h>
#include<OSGNode.h>
#include<OSGViewport.h>
#include<OSGState.h>
#include<OSGMaterial.h>

#include"OSGSoftZBTest.h"
#include"OSGExactOccupancyMap.h"
#include"OSGShadowTest.h"
#include"OSGOcclusionPrediction.h"

#include<OSGOcclusionTest.h>

OSG_BEGIN_NAMESPACE

template<typename octype>
class OCOcclusionTest{
	private:
	SoftZBTest<> _softtest;
	ExactOccupancyMap<UInt32> _exactom;
	ShadowTest _shadowtest;
	OcclusionPrediction _prediction;
	
	octype _octest;
	
	bool _withsofttest;
	bool _withexactom;
	bool _withshadowtest;
	bool _withpredictiontest;

	unsigned int _eom_vis;
	unsigned int _eom_hid;
	unsigned int _soft_vis;
	unsigned int _soft_hid;
	unsigned int _shadow_vis;
	unsigned int _shadow_hid;
	unsigned int _prediction_vis;
	unsigned int _prediction_hid;
	unsigned int _prediction_unk;
	unsigned int _wrong_vis;
	unsigned int _wrong_hid;
	unsigned int _oc_hid;
	unsigned int _oc_vis;

	int _maxpix;
	
	bool testNodeExactOM(const OCTestNode*);
	bool testNodeSoftZB(const OCTestNode* tgn);
	bool testNodeShadow(const OCTestNode* tgn);
	Int16 testNodePrediction(const OCTestNode* tgn);

	public:
	enum { resExactOM=1, resSoftZB=2, resShadowTest=3, resOCTest=4, resPrediction=5 };

	OCOcclusionTest();
	~OCOcclusionTest();

	inline void init(Viewport* vp, const int& mp);
	inline void init(DrawAction* action, const int& mp);
	// inline void init(RenderAction* action, const int& mp);
	inline void frameExit(void);
	inline Real32* getBuffer(void);
	inline Int16 testNode(const OCTestNode* tgn);
	
	inline void setSoftTest(bool v);
	inline void setShadowTest(bool v);
	inline void setPredictionTest(bool v);
	inline void setExactOM(bool v);
	inline void printStatistics(void);
	inline void setStatVis(bool v);
};

OSG_END_NAMESPACE

#include"OCOcclusionTest.inl"

#endif	// _OCCLUSIONTEST_H
