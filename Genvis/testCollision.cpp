// test collision detection with BVol-hierarchies 
// (18-DOP, simplest realignment) 
// In this scene a sphere is falling downto a plane
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
static NodePtr               first;
static ComponentTransformPtr firstTrf;
static NodePtr               second;
static ComponentTransformPtr secondTrf;
static genvis::OSGAllTraverser*    all = NULL;
static genvis::OSGDoubleTraverser* traverser = NULL;
static Vec3f                 velocity(0,0,-0.01f);
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
    first = Node::create ();
    beginEditCP(first);
    first->setCore(ComponentTransform::create());
    first->addChild(makePlane(20.0f, 20.0f, 10, 10));
    endEditCP(first);
    secondTrf = ComponentTransform::create();
    beginEditCP(secondTrf);
    secondTrf->getTranslation().setValues(0, 0, 10.0f);
    endEditCP(secondTrf);
    second = Node::create ();
    beginEditCP(second);
    second->setCore(secondTrf);
    second->addChild(makeSphere(3, 0.5f));
    endEditCP(second);

    scene = Node::create ();
    beginEditCP(scene);
    scene->setCore(Group::create());
    scene->addChild(first);
    scene->addChild(second);
    endEditCP(scene);
    addRefCP(scene);
    std::cout << "scene created" << std::endl;

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
    std::cout << "cache filled." << std::endl;
    // * build hierarchy
    SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K18Dop> > hier;
    hier.setCoordinateSystem(OSGSingleBVolHierarchyBase::LocalCoordSystem);
    OSGCache::the().setHierarchy(&hier);
    OSGCache::the().apply(scene);
    std::cout << "adapters with 18DOPs created.." << std::endl;

    hier.setParameter(OSGStaticInput::LongestSideMedianId, 50, 2);
    hier.hierarchy();
    std::cout << "18DOP-hierarchy (max depth 50, min num primitives 2) build...." << std::endl;
    // * create double traverser for collision detection
    // realignemnt with DynamicRealign
    traverser = new DoubleTraverserBinary<OpenSGTraits,DynamicAlignCollisionTraits<OpenSGTraits,K18Dop> >();
    traverser->setUseCoherency(false);
   ((OSGBVolCollision&)traverser->getData()).setStopFirst(false);
   all = new PruningTraverser<OpenSGTraits>();
   all->setDoubleTraverser(traverser);

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
    beginEditCP(secondTrf);
    secondTrf->getTranslation() += velocity;
    endEditCP(secondTrf);

    OSGBVolCollision& col = (OSGBVolCollision&)all->getData();     
    bool result = all->apply(first, second);
    std::cout << col.getNumContacts() << " contacts with " 
	      << col.getNumBVolTests() << " BVolTests/"
	      << col.getNumMixedTests() << " MixedTests/"
	      << col.getNumPrimTests() << " TriTests  " 
	      << std::endl;
    if (result && col.getNumContacts() > 0) { // collision
       velocity *= -1;
       beginEditCP(secondTrf);
       secondTrf->getTranslation() += velocity;
       endEditCP(secondTrf);
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
      subRefCP(scene);
      delete traverser;
      exit(1);
    case '1':
      ((OSGBVolCollision&)traverser->getData()).setStopFirst(false);
      break;
    case '2':
      ((OSGBVolCollision&)traverser->getData()).setStopFirst(true);
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



