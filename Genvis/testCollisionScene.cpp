// test collision detection with BVol-hierarchies 
// (18-DOP, simplest realignment) 
// In this scene a model 'hand.wrl' is falling downto a model 'chessboard.wrl'
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
#include "OSGSceneFileHandler.h"

#include "OSGGVCache.h"
#include "OSGGVGenvisPreprocAction.h"
#include "OSGGVSingleBVolHierarchy.h"
#include "OSGGVDoubleTraverserBinary.h"
#include "OSGGVDoubleTraverserBinaryCoherent.h"
#include "OSGGVDoubleTraverserCoherent.h"
#include "OSGGVDoubleTraverserFixed.h"
#include "OSGGVAllTraverser.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVDynAlignCollision.h"
#include "OSGGVPrecompAlignCollision.h"
#include "OSGGVMinimumDistanceCalc.h"
#include "OSGGVMinimumDistanceBVol.h"
#include "OSGGVContactDistanceCalc.h"


// Activate the OpenSG namespace
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// The SimpleSceneManager to manage simple applications
static NodePtr scene;
enum Piece {
   Pawn0 = 0, Pawn7 = Pawn0+7,
   LeftRook, LeftKnight, LeftBishop, King, Queen, RightBishop, RightKnight, RightRook, 
   LastPiece
};
static NodePtr               white;
static NodePtr               whitePiece[LastPiece];
static NodePtr               black;
static NodePtr               blackPiece[LastPiece];
static NodePtr               first;
static NodePtr               second;
static ComponentTransformPtr secondTrf;
typedef SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K18Dop> > Hierarchy;
static Hierarchy             hier;
typedef DoubleTraverserBinary<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K18Dop> > Traverser;
static Traverser             traverser;
typedef PruningTraverser<OpenSGTraits> ATraverser;
static ATraverser            all;
static Vec3f                 velocity(0,-0.001f,0);
static SimpleSceneManager*   mgr;

// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

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
    // load the scene
    first = SceneFileHandler::the().read("chessboard.wrl");
    unsigned i;
    SLOG
	      << first->getChild(2)->getChild(0)->getChild(5)->getChild(1)->getNChildren()
	      << std::endl
	      << first->getChild(2)->getChild(0)->getChild(4)->getChild(1)->getNChildren()
	      << std::endl
	      << first->getChild(2)->getChild(0)->getChild(3)->getChild(1)->getNChildren()
	      << std::endl
	      << first->getChild(2)->getChild(0)->getChild(2)->getChild(1)->getNChildren()
	      << std::endl
	      << first->getChild(2)->getChild(0)->getChild(1)->getChild(1)->getNChildren()
	      << std::endl
	      << first->getChild(2)->getChild(0)->getChild(0)->getNChildren()
	      << std::endl;
    white = first->getChild(2)->getChild(0)->getChild(5)->getChild(1);
    for (i=0; i<LastPiece-1; ++i) {
       whitePiece[i] = first->getChild(2)->getChild(0)->getChild(5)->getChild(1)
	 ->getChild(i);
    }
    black = first->getChild(2)->getChild(0)->getChild(4)->getChild(1);
    for (i=0; i<LastPiece-1; ++i) {
       blackPiece[i] = first->getChild(2)->getChild(0)->getChild(4)->getChild(1)
	 ->getChild(i);
    }
    second = SceneFileHandler::the().read("hand.wrl");
    secondTrf = ComponentTransformPtr::dcast(second->getChild(0)->getCore());
    assert(secondTrf != NullFC);
    NodePtr scene = Node::create();
    beginEditCP(scene);
    scene->setCore(Group::create());
    scene->addChild(first);
    scene->addChild(second);
    endEditCP(scene);
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
    hier.setParameter(0, 50, 2);
    hier.setCoordinateSystem(OSGSingleBVolHierarchyBase::LocalCoordSystem);
    OSGCache::the().setHierarchy(&hier);
    OSGCache::the().apply(first);
    OSGCache::the().apply(second);
    SLOG << "adapters with 18DOPs created.." << std::endl;

    hier.hierarchy();
    SLOG << "18DOP-hierarchy (max depth 50, min num primitives 2) build...." << std::endl;
    // * create double traverser for collision detection
    traverser.setUseCoherency(false);
    traverser.getDataTyped().setStopFirst(true);
    all.setDoubleTraverser(&traverser);

    // GLUT main loop
    glutMainLoop();

    return 0;
}

//
// GLUT callback functions
//

// redraw the window
void display(void)
{
#if 1
    beginEditCP(secondTrf);
    secondTrf->getTranslation() += velocity;
    endEditCP(secondTrf);
#endif
    Traverser::ObjectT& col = traverser.getDataTyped();     
    bool result = all.apply(first, second);
    SLOG << col.getNumContacts() << " contacts with " 
	      << col.getNumBVolTests() << " BVolTests/"
	      << col.getNumMixedTests() << " MixedTests/"
	      << col.getNumPrimTests() << " TriTests  " 
	      << std::endl;
    if (result && col.getNumContacts() > 0) { // collision
       velocity.setValues(0,0.001f,0);
#if 0
       for (OSGBVolCollision::CollisionContainer::const_iterator it=col.getContacts().begin(); 
	    it!=col.getContacts().end();
	    ++it) {
	 if (it->getFirst() == NULL || it->getSecond() == NULL) {
	   continue;
	 }
	 TriangleIterator hitTri1 = it->getFirst()->getOriginal();
	 TriangleIterator hitTri2 = it->getSecond()->getOriginal();
	 SLOG << " " << hitTri1.getIndex() << ":" << hitTri2.getIndex() << std::endl;
       }
#endif
    }

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
    case '1':
      traverser.getDataTyped().setStopFirst(false);
      break;
    case '2':
      traverser.getDataTyped().setStopFirst(true);
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



