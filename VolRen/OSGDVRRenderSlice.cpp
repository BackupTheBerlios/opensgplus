
#include <OSGDVRRenderSlice.h>
#include <vector>
#include <stdlib.h>
#include <OSGGL.h>

OSG_BEGIN_NAMESPACE
     
void DVRRenderSlicePrimitive::render() const
{
  glBegin(type);
  
  for(std::vector<GLdouble*>::const_iterator i = vertices.begin(); i != vertices.end(); i++){
    glTexCoord3dv(&((*i)[3]));
    glVertex3dv(*i);
  }

  glEnd();
}


DVRRenderSlice::DVRRenderSlice()
{
  numVertexCombineData = 0;
  maxVertexCombineData = 0;

  vertexCombineData = NULL;
}

DVRRenderSlice::~DVRRenderSlice()
{
  if(maxVertexCombineData > 0)
    free(vertexCombineData); // allocation with malloc/realloc!!
  vertexCombineData = NULL;
}

void DVRRenderSlice::render() const
{  
  for(DVRRenderSlice::const_iterator i = begin(); i != end(); i++){
    (*i)->render();
  }
}

void DVRRenderSlice::clear()
{
  if(!directRender){
    for(DVRRenderSlice::const_iterator i = begin(); i != end(); i++){
      delete (*i);
    }
    std::vector<DVRRenderSlicePrimitive*>::clear();
  }
  
  numVertexCombineData = 0;

}

OSG_END_NAMESPACE
