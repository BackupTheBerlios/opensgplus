// test AllTraverser for the broad phase of collision detection
// (generates ALL BVol-hierarchy pairs for two OpenSG nodes)
//
// parameters:
//    none
// keys:
//    ESC - exit program
//    '1' - search all collisions
//    '2' - stop on first collision found on a BVol-hierarchy pair

// Headers
#include "OSGGLUT.h"
#include "OSGConfig.h"
#include "OSGGLUTWindow.h"
#include "OSGSimpleSceneManager.h"
#include "OSGAction.h"
#include "OSGComponentTransform.h"
#include "OSGGeometry.h"
#include "OSGSimpleGeometry.h"

#include "OSGGVCache.h"
#include "OSGGVGenvisPreprocAction.h"
#include "OSGGVSingleBVolHierarchy.h"
#include "OSGGVDoubleTraverserBinary.h"
#include "OSGGVDoubleTraverserBinaryCoherent.h"
#include "OSGGVDoubleTraverserCoherent.h"
#include "OSGGVDoubleTraverserFixed.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVDynAlignCollision.h"
#include "OSGGVPrecompAlignCollision.h"
#include "OSGGVMinimumDistanceCalc.h"
#include "OSGGVMinimumDistanceBVol.h"
#include "OSGGVContactDistanceCalc.h"
#include "OSGGVAllTraverser.h"
#include "OSGGVPruningTraverser.h"
#include "OSGGVProfiler.h"

// Activate the OpenSG namespace
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// The SimpleSceneManager to manage simple applications
static NodePtr scene;
static NodePtr        first;
static const unsigned numFirst = 100;
static NodePtr        second;
static const unsigned numSecond= 100;
typedef SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K18Dop> > Hierarchy;
static Hierarchy                hier;
static genvis::OSGAllTraverser* all = NULL;
static SimpleSceneManager*      mgr = NULL;

// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

static NodePtr makePerturbedAll (UInt16 numSubdiv, 
				 Real32 radius,
				 Real32 stdDeviation = 0.1f)
{
   NodePtr         sphereNode = makeSphere(numSubdiv, radius);
   GeometryPtr     sphere = GeometryPtr::dcast(sphereNode->getCore());
   GeoPositionsPtr points = sphere->getPositions();
   beginEditCP(points);
   for (UInt32 i=0; i<points->size(); ++i) {
      Real32 factor = 1.0f + stdDeviation * (rand()/Real32(RAND_MAX) - 0.5f);
      points->setValue(factor*points->getValue(i), i);
   }
   endEditCP(points);
   return sphereNode;
}
static NodePtr makePerturbedUniform (UInt16 numSubdiv, 
				     Real32 radius,
				     Real32 rate = 0.1f)
{
   static Real32 factor = 1.1f;

   NodePtr         sphereNode = makeSphere(numSubdiv, radius);
   GeometryPtr     sphere = GeometryPtr::dcast(sphereNode->getCore());
   GeoPositionsPtr points = sphere->getPositions();
   beginEditCP(points);
   for (UInt32 i=0; i<points->size(); ++i) {
      Real32 random = (rand()/Real32(RAND_MAX));
      if (random <= rate) {
	 points->setValue(factor*points->getValue(i), i);
      }
   }
   endEditCP(points);
   return sphereNode;
}

void randomTransform (Matrix& m)
{
  Vec3f p(rand(), rand(), rand());
  Real32 len = p.length();
  p /= len;
  Quaternion q;
  q.setValueAsAxisRad(p, len);
  m.setRotate(q);
  Vec3f t(10.0f*rand()/Real32(RAND_MAX), 10.0f*rand()/Real32(RAND_MAX), 10.0f*rand()/Real32(RAND_MAX));
  m.setTranslate(t);
}

NodePtr makeTransformedCube (Real32 xsize,
			     Real32 ysize,
			     Real32 zsize,
			     UInt16 hor,
			     UInt16 vert,
			     UInt16 depth)
{
   NodePtr node = Node::create();
   beginEditCP(node);
   node->setCore(Transform::create());
   node->addChild(makeBox(xsize, ysize, zsize, hor, vert, depth));
   endEditCP(node);
   return node;
}

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{
    // OSG init
    osgInit(argc,argv);

    // GLUT init
    int winid = setupGLUT(&argc, argv);

    // the connection between GLUT and OpenSG
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();

    // create the scene
    first = Node::create ();
    beginEditCP(first);
    first->setCore(Group::create());
    unsigned i;
    for (i=0; i<numFirst; ++i) {
       first->addChild(makeTransformedCube(1.0f,1.0f,1.0f,1,1,1));
    }
    endEditCP(first);
    second = Node::create ();
    beginEditCP(second);
    second->setCore(Group::create());
    for (i=0; i<numSecond; ++i) {
       second->addChild(makeTransformedCube(1.0f,1.0f,1.0f,1,1,1));
    }
    endEditCP(second);

    for (i=0; i<numFirst; ++i) {
      TransformPtr trf = TransformPtr::dcast(first->getChild(i)->getCore());
      beginEditCP(trf);
      randomTransform(trf->getMatrix());
      endEditCP(trf);
    }
    for (i=0; i<numSecond; ++i) {
      TransformPtr trf = TransformPtr::dcast(second->getChild(i)->getCore());
      beginEditCP(trf);
      randomTransform(trf->getMatrix());
      endEditCP(trf);
    }

    scene = Node::create ();
    beginEditCP(scene);
    scene->setCore(Group::create());
    scene->addChild(first);
    scene->addChild(second);
    endEditCP(scene);
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
    hier.setParameter(OSGStaticInput::LongestSideMedianId, 50, 2);
    OSGCache::the().setHierarchy(&hier);
    OSGCache::the().apply(scene);
    SLOG << "adapters with 18DOPs created.." << std::endl;
    hier.hierarchy();
    SLOG << "18DOP-hierarchy (max depth 50, min num primitives 2) build...." << std::endl;
    // * create double traverser for collision detection
    OSGDoubleTraverser* traverser = new DoubleTraverserBinary<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K18Dop> >();
    traverser->setUseCoherency(false);
   ((OSGBVolCollision&)traverser->getData()).setStopFirst(false);
   all = new AllTraverser<OpenSGTraits>();
   all->setDoubleTraverser(traverser);

   // GLUT main loop
   Profiler::the().Reset();
   glutMainLoop();

   return 0;
}

//
// GLUT callback functions
//

// redraw the window
void display(void)
{
   Profiler::the().StartProfile("Frame");
   unsigned i;
   for (i=0; i<numFirst; ++i) {
     TransformPtr trf = TransformPtr::dcast(first->getChild(i)->getCore());
     beginEditCP(trf);
     trf->getMatrix()[3] += Vec4f((rand()/Real32(RAND_MAX)-0.5f),
				  (rand()/Real32(RAND_MAX)-0.5f),
				  (rand()/Real32(RAND_MAX)-0.5f), 0.0f);
     endEditCP(trf);
   }
   for (i=0; i<numSecond; ++i) {
     TransformPtr trf = TransformPtr::dcast(second->getChild(i)->getCore());
     beginEditCP(trf);
     trf->getMatrix()[3] += Vec4f((rand()/Real32(RAND_MAX)-0.5f),
				  (rand()/Real32(RAND_MAX)-0.5f),
				  (rand()/Real32(RAND_MAX)-0.5f), 0.0f);
     endEditCP(trf);
   }
   bool result = all->apply(first, second);
#if 1
   OSGBVolCollision& col = (OSGBVolCollision&)all->getData();     
   SLOG << col.getNumContacts() << " contacts with " 
	     << col.getNumBVolTests() << " BVolTests/"
	     << col.getNumMixedTests() << " MixedTests/"
	     << col.getNumPrimTests() << " TriTests  " 
	     << std::endl;
#endif
   Profiler::the().EndProfile("Frame");
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
    case 27: {    
      hier.destroy();
      subRefCP(scene);
      delete &(all->getDoubleTraverser());
      delete all;
      Profiler::the().dump(SLOG);
      exit(0);
    }
    case '1':
      ((OSGBVolCollision&)all->getData()).setStopFirst(false);
      break;
    case '2':
      ((OSGBVolCollision&)all->getData()).setStopFirst(true);
      break;
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
    glutIdleFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    return winid;
}



