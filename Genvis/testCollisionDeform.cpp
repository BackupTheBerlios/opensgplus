// test collision detection with BVol-hierarchies (18-DOP) 
// In this scene two spheres are shown, where one is rotating
// and deformed randomly. 
//
// parameters:
//    none
// keys:
//    ESC - exit program

// Headers
#include "OSGGLUT.h"
#include "OSGConfig.h"
#include "OSGSimpleGeometry.h"
#include "OSGGLUTWindow.h"
#include "OSGSimpleSceneManager.h"
#include "OSGAction.h"
#include "OSGSceneFileHandler.h"

#include "OSGGVGenvisPreprocAction.h"
#include "OSGGVSingleBVolHierarchy.h"
#include "OSGGVPruningTraverser.h"
#include "OSGGVDoubleTraverserBinary.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVDynAlignCollision.h"
#include "OSGGVPrecompAlignCollision.h"

// Activate the OpenSG namespace
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// The SimpleSceneManager to manage simple applications
static NodePtr scene;
typedef SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K18Dop> > Hierarchy;
static Hierarchy           hier;
typedef DoubleTraverserBinary<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K18Dop> > Traverser;
static Traverser traverser;
typedef PruningTraverser<OpenSGTraits> ATraverser;
static ATraverser          all;
static SimpleSceneManager* mgr;
static OSGCache            cache;

// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

class DeformSphere
{
public:
   DeformSphere (NodePtr root,
		 UInt16 depth, UInt32 radius);

   void createSphere (UInt16 depth, UInt32 radius);
   void moveSphere   (const Matrix& t);
   void deformSphere ();

   NodePtr getTransformNode () const;
   NodePtr getSphereNode () const;

private:
   NodePtr m_content;
   NodePtr m_sphere;
};
inline NodePtr DeformSphere::getTransformNode () const
{
   return m_content;
}
inline NodePtr DeformSphere::getSphereNode () const
{
   return m_sphere;
}
inline void DeformSphere::createSphere (UInt16 depth, UInt32 radius)
{
   m_content        = Node::create();
   TransformPtr trf = Transform::create();
   m_sphere  = makeSphere(depth, radius);
   beginEditCP(m_content);
   m_content->setCore(trf);
   m_content->addChild(m_sphere);
   endEditCP(m_content);
}
inline DeformSphere::DeformSphere (NodePtr root, UInt16 depth, UInt32 radius)
{
   createSphere(depth, radius);
   beginEditCP(root);
   root->addChild(m_content);
   endEditCP(root);
}
inline void DeformSphere::moveSphere   (const Matrix& t)
{
   TransformPtr trf = TransformPtr::dcast(m_content->getCore());
   beginEditCP(trf);
   trf->getMatrix().mult(t);
   endEditCP(trf);
}
inline void DeformSphere::deformSphere ()
{
   const Real32 stdDeviation = 0.1f;

   GeometryPtr geom = GeometryPtr::dcast(m_sphere->getCore());
   GeoPositionsPtr points = geom->getPositions();
   beginEditCP(points);
   for (UInt32 i=0; i<points->size(); ++i) {
      Real32 factor = 1.0f + stdDeviation * (rand()/Real32(RAND_MAX) - 0.5f);
      factor /= Vec3f(points->getValue(i)).length();
      points->setValue(factor*points->getValue(i), i);
   }
   endEditCP(points);
}
static const UInt32  numSpheres = 2;
static DeformSphere* sphere[numSpheres];

NodePtr makeSpheres () 
{
   NodePtr scene = Node::create();
   beginEditCP(scene);
   scene->setCore(OSG::Group::create());
   endEditCP(scene);
   for (UInt32 i=0; i<numSpheres; ++i) {
      sphere[i] = new DeformSphere(scene, 3, 1.0f);
   }
   return scene;
}

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{
    // OSG init
    osgInit(argc,argv);

    // change Geometry prototype to not using display lists
    GeometryPtr protoGeometry = Geometry::create();
    beginEditCP(protoGeometry);
    protoGeometry->setDlistCache(false);
    endEditCP(protoGeometry);
    Geometry::getClassType().setPrototype(protoGeometry);

    // GLUT init
    int winid = setupGLUT(&argc, argv);

    // the connection between GLUT and OpenSG
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();

    // create the scene
    scene = makeSpheres();
    addRefCP(scene);
    SLOG << "scene created" << std::endl;

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (scene);

    // show the whole scene
    mgr->showAll();

    // restructure
    // * fill cache
    GenvisPreprocAction prep;
    prep.apply(scene);
    SLOG << "cache filled." << std::endl;
    // * build hierarchy
    hier.setCoordinateSystem(OSGSingleBVolHierarchyBase::LocalCoordSystem);
    OSGCache::the().setHierarchy(&hier);
    OSGCache::the().apply(scene);
    SLOG << "adapters with 18DOPs created.." << std::endl;

    hier.setParameter(OSGStaticInput::LongestSideMedianId, 50, 2);
    hier.hierarchy();
    SLOG << "18DOP-hierarchy (max depth 50, min num primitives 2) build...." << std::endl;
    // * create double traverser for collision detection
    traverser.setUseCoherency(false);
    traverser.getDataTyped().setStopFirst(false);
    all.setDoubleTraverser(&traverser);

    // GLUT main loop
    glutMainLoop();
    
    return 0;
}

//
// GLUT callback functions
//

// idle function
void idle (void)
{
   // deformation of sphere0
   sphere[0]->deformSphere();
   // rotation of sphere0
   static Quaternion rotY(Vec3f(0,1,0), 0.1f*Pi);
   static Matrix     m;
   m.setRotate(rotY);
   sphere[0]->moveSphere(m);

   // update hierarchy (for sphere0) after deformation
   hier.updateHierarchy(sphere[0]->getTransformNode());
   // update all hierarchies after deformation
   //hier.updateHierarchy();

   glutPostRedisplay();
}

// redraw the window
void display(void)
{
   Traverser::ObjectT& col = traverser.getDataTyped();     
   bool result = all.apply(sphere[0]->getTransformNode(), sphere[1]->getTransformNode());
   //bool result = all.apply(scene, scene);
   SLOG << col.getNumContacts() << " contacts with " 
	<< col.getNumBVolTests() << " BVolTests/"
	<< col.getNumMixedTests() << " MixedTests/"
	<< col.getNumPrimTests() << " TriTests  " 
	<< std::endl;
   mgr->redraw();
}

// react to size changes
void reshape(int w, int h)
{
    mgr->resize(w, h);
    glutPostRedisplay();
}

// react to mouse button presses
void mouse(int button, int state, int x, int y)
{
    if (state)
        mgr->mouseButtonRelease(button, x, y);
    else
        mgr->mouseButtonPress(button, x, y);
        
    glutPostRedisplay();
}

// react to mouse motions with pressed buttons
void motion(int x, int y)
{
    mgr->mouseMove(x, y);
    glutPostRedisplay();
}

// react to keys
void keyboard(unsigned char k, int x, int y)
{
    switch(k)
    {
    case 27:    
      hier.destroy();
      subRefCP(scene);
      exit(0);
    }
}

// setup the GLUT library which handles the windows for us
int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    int winid = glutCreateWindow("OpenSG");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    return winid;
}



