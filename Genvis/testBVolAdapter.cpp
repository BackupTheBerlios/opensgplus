#include "OSGGVBVolAdapter.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVTriangleAdapterAligned.h"
USING_GENVIS_NAMESPACE

int main (int, char**) 
{
  std::cout << "Sizes" 
	    << std::endl
	    << std::endl
	    << "BoundingVolume                       " << sizeof(BoundingVolume<Real>)
	    << std::endl
	    << "<" << std::endl
	    << "K6Dop                                " << sizeof(K6Dop)
	    << std::endl
	    << "K14Dop                               " << sizeof(K14Dop)
	    << std::endl
	    << std::endl
	    << "OpenSGObjectBase                     " << sizeof(OpenSGObjectBase<OpenSGTraits>)
	    << std::endl
	    << "(" << sizeof(OpenSGTraits::GeomObjectType) << "/" << sizeof(OpenSGTraits::TransformType) << ")"
	    << std::endl
	    << "<" << std::endl
	    << "OpenSGObjectAdapter,  6-Dop          " << sizeof(OpenSGObject2BVol<OpenSGTraits,K6Dop>)
	    << std::endl
	    << "OpenSGObjectAdapter, 14-Dop          " << sizeof(OpenSGObject2BVol<OpenSGTraits,K14Dop>)
	    << std::endl
	    << std::endl
	    << "OpenSGFaceBase                       " << sizeof(OpenSGFaceBase<OpenSGTraits>)
	    << std::endl
	    << "(" << sizeof(OpenSGTraits::GeomFaceType) << "/" << sizeof(OpenSGTraits::TransformType) << ")"
	    << std::endl
	    << "<" << std::endl
	    << "OpenSGFaceAdapter,  6-Dop            " << sizeof(OpenSGFace2BVol<OpenSGTraits,K6Dop>)
	    << std::endl
	    << "OpenSGFaceAdapter, 14-Dop            " << sizeof(OpenSGFace2BVol<OpenSGTraits,K14Dop>)
	    << std::endl
	    << std::endl
	    << "OpenSGTriangleBase                   " << sizeof(OpenSGTriangleBase<OpenSGTraits>)
	    << std::endl
	    << "(" 
	    << sizeof(OSG::Int32)+sizeof(OpenSGTraits::CacheData) 
	    << "/" << sizeof(OpenSGTraits::TransformType)
#ifdef GV_STORED_COORDS 
	    << "/" << sizeof(OSG::GeoPositionsPtr)
	    << "/" << sizeof(OSG::GeoNormalsPtr) 
#endif
	    << ")"
	    << std::endl
	    << "<" << std::endl
	    << "OpenSGTriangleAdapter,  6-Dop        " << sizeof(OpenSGTriangle2BVol<OpenSGTraits,K6Dop>)
	    << std::endl
	    << "OpenSGTriangleAdapter, 14-Dop        " << sizeof(OpenSGTriangle2BVol<OpenSGTraits,K14Dop>)
	    << std::endl
	    << "<" << std::endl
	    << "OpenSGTriangleAdapterAligned,  6-Dop " << sizeof(OpenSGTriangleAligned<OpenSGTraits,K6Dop>)
	    << std::endl
	    << "OpenSGTriangleAdapterAligned, 14-Dop " << sizeof(OpenSGTriangleAligned<OpenSGTraits,K14Dop>)
	    << std::endl;
   return 0;
}
