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

#include<GL/gl.h>

#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<OSGGeometry.h>

using namespace osg;

template<typename octype>
inline
OCOcclusionTest<octype>::OCOcclusionTest():_softtest(),_octest(),
	                _withsofttest(0),_withexactom(0),_withshadowtest(0), _withpredictiontest(0){
};

template<typename octype>
inline
OCOcclusionTest<octype>::~OCOcclusionTest(){
};

template<typename octype>
inline
// void OCOcclusionTest<octype>::init(RenderAction* action, const int& mp){
void OCOcclusionTest<octype>::init(DrawAction* action, const int& mp){
	init(action->getViewport(), mp);
};

template<typename octype>
inline
void OCOcclusionTest<octype>::init(Viewport* port, const int& mp){
	_eom_vis=0;
	_eom_hid=0;
	_soft_vis=0;
	_soft_hid=0;
	_shadow_vis=0;
	_shadow_hid=0;
	_prediction_vis=0;
	_prediction_hid=0;
	_prediction_unk=0;
	_oc_hid=0;
	_oc_vis=0;
	_wrong_hid=0;
	_wrong_vis=0;
	_maxpix=mp;

	_octest.frameInit();

	_softtest.setup(1, port, mp);
	_exactom.setup(1, port);
	_prediction.setup(1, port);
//	_exactom.setup(1, 320, 320);
	Pnt3f p;
	_shadowtest.setup(1, port, p);

	_octest.setup(1, port, mp);
};

template<typename octype>
inline
void OCOcclusionTest<octype>::frameExit(void){
	_softtest.frameExit();
	_exactom.frameExit();
	_shadowtest.frameExit();
	_prediction.frameExit();
	
	_octest.frameExit();
};

template<typename octype>
inline float* OCOcclusionTest<octype>::getBuffer(void){
	return(NULL);
};

template<typename octype>
inline void OCOcclusionTest<octype>::setSoftTest(bool v){
	_withsofttest=v;
};

template<typename octype>
inline void OCOcclusionTest<octype>::setPredictionTest(bool v){
	_withpredictiontest=v;
};

template<typename octype>
inline void OCOcclusionTest<octype>::setShadowTest(bool v){
	_withshadowtest=v;
};

template<typename octype>
inline void OCOcclusionTest<octype>::setExactOM(bool v){
	_withexactom=v;
};

template<typename octype>
inline void OCOcclusionTest<octype>::printStatistics(void){
	std::cerr << " _eom_vis: " << _eom_vis
	     << " _eom_hid: " << _eom_hid
	     << " _soft_vis: " << _soft_vis
	     << " _soft_hid: " << _soft_hid
	     << " _shad_vis: " << _shadow_vis
	     << " _shad_hid: " << _shadow_hid
	     << " _pred_vis: " << _prediction_vis
	     << " _pred_unk: " << _prediction_unk
	     << " _pred_hid: " << _prediction_hid
	     << " _wrong_vis: " << _wrong_vis
	     << " _wrong_hid: " << _wrong_hid
	     << " _oc_vis: " << _oc_vis
	     << " _oc_hid: " << _oc_hid
	     << std::endl;
};

template<typename octype>
inline void OCOcclusionTest<octype>::setStatVis(bool b){
	_octest.setVisualize(b);
}

template<typename octype>
inline
bool OCOcclusionTest<octype>::testNodeExactOM(const OCTestNode* tgn){
	if(_withexactom){
		_exactom.init();
		_exactom.perform(0,tgn->_x,tgn->_y,tgn->_z,tgn->_index);
		_exactom.exit();
		if(_exactom.result(0)){
			_eom_vis++;
			if(tgn->_node->getCore()->getTypeId()==GeometryBase::getClassTypeId()){
				_exactom.insertBox(tgn->_x,tgn->_y,tgn->_z,tgn->_index);
			}
			return(true);
		}else
			_eom_hid++;
	}
	return(false);
};

template<typename octype>
inline
bool OCOcclusionTest<octype>::testNodeSoftZB(const OCTestNode* tgn){
	if(_withsofttest){
		_softtest.init();
		_softtest.perform(0,tgn->_x,tgn->_y,tgn->_z,tgn->_index);
		_softtest.exit();
		if(_softtest.result(0)){
			_soft_vis++;
			return(true);
		}else{
			_soft_hid++;
			return(false);
		}
	}
	return(true);
};

template<typename octype>
inline
Int16 OCOcclusionTest<octype>::testNodePrediction(const OCTestNode* tgn){
	if(_withpredictiontest){
		_prediction.init();
		_prediction.perform(0,tgn);
		_prediction.exit();
		Int16 res=_prediction.result(0);
		if(res>0){
			_prediction_vis++;
			return(1);
		}else if(res<0){
			_prediction_hid++;
			return(-1);
		}else{
			_prediction_unk++;
			return(0);
		}
	}
	return(0);
};

template<typename octype>
inline
bool OCOcclusionTest<octype>::testNodeShadow(const OCTestNode* tgn){
	if(_withshadowtest){
		_shadowtest.init();
		_shadowtest.perform(0,tgn->_x,tgn->_y,tgn->_z,tgn->_index);
		_shadowtest.exit();
		if(_shadowtest.result(0)){
			_shadow_vis++;
			return(true);
		}else{
			_shadow_hid++;
			return(false);
		}
	}
	return(1);
};

// template<const bool transformed>

template<typename octype>
inline Int16 OCOcclusionTest<octype>::testNode(const OCTestNode* tgn){
	Int16 result;
	if(testNodeExactOM(tgn)){
		result=resExactOM;
		if(_withpredictiontest){
			_prediction.update(tgn, result);
		}
	}else{
		Int16 pres=testNodePrediction(tgn);
		if(pres>0){
			result=resPrediction;
		}else if(pres<0){
			result=-resPrediction;
		}else{
			if(!testNodeSoftZB(tgn))
				result=resSoftZB;
			else{
				if(!testNodeShadow(tgn)) 
					result=-resShadowTest;
				else{
					_octest.init();
					_octest.perform(0,tgn);
					_octest.exit();
					result=(_octest.result(0)>_maxpix?resOCTest:-resOCTest);
					if(result<0){
						_oc_hid++;
						if(_withsofttest) _softtest.insertBox(tgn->_x,tgn->_y,tgn->_z,tgn->_index);
						if(_withshadowtest) _shadowtest.insertBox(tgn->_x,tgn->_y,tgn->_z,tgn->_index);
					}else{
						_oc_vis++;
					}
				}
			}
			if(_withpredictiontest){
				_prediction.update(tgn, result);
			}
		}
	}
	return(result);
};

