#include <OpenSG/OSGConfig.h>

#ifdef OSG_STREAM_IN_STD_NAMESPACE
#include <iostream>
#else
#include <iostream.h>
#endif

#include "OSGOcclusion.h"
#include "OSGStencilTest.h"
#include "OSGHPTest.h"
#include "OSGNVTest.h"

using namespace OSG;

int main( int argc, char *argv[] )
{
//    osgInit(argc, argv);
    Occlusion o;
    StencilTest s;
    HPTest h;
    NVTest n;
}
