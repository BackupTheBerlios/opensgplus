#ifndef OSGDVRRENDERSLICE_H
#define  OSGDVRRENDERSLICE_H

#include <vector>
//#include <list>

#include <OSGGL.h>
#include <OSGVector.h>

OSG_BEGIN_NAMESPACE

/*
  A data structure that represents an OpenGL primitive with 
  its assocciated vertex data for a clipped slice
*/
class DVRRenderSlicePrimitive
{

public:
    
    // render this primitive
    void render() const;
  
    // the type of the primitive
    GLenum type;
  
    // the list of the primitive's vertices
    std::vector<GLdouble*> vertices;
};


/*
  A structure that holds the data for a complete clipped slice
*/
class DVRRenderSlice : public std::vector<DVRRenderSlicePrimitive*>
{
public:

    enum Orientation {UNDEFINED, XY, XZ, YZ};

public:

    DVRRenderSlice();
    ~DVRRenderSlice();

    // render all primitives of this slice
    void render() const;

    // empty the list of primitives
    void clear();

public:
    
    // track dynamically allocated data (vertexCombine) during 
    // tesselation with gluTesselator
    GLdouble *vertexCombineData;
    UInt32 numVertexCombineData;
    UInt32 maxVertexCombineData;

    // if true, render primitives directly instead of building the primitives 
    // list; usefull for shaders  without a slice render callback
    bool directRender;

    // number of additional (GLdouble) attributes per vertex
    UInt32 numPerVertexData;

    // orientation of this slice
    Orientation orientation;

};

OSG_END_NAMESPACE

#endif
