// test min distance computation with BVol-hierarchies (18-DOP) 
// In this scene a sphere is falling downto a plane
//
// parameters:
//    none
// keys:
//    ESC - exit program

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
#include "OSGGVDoubleTraverserFixed.h"
#include "OSGGVMinimumDistanceBVol.h"


// Activate the OpenSG namespace
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// The SimpleSceneManager to manage simple applications
static NodePtr scene;
static NodePtr               first;
static ComponentTransformPtr firstTrf;
static NodePtr               second;
static ComponentTransformPtr secondTrf;
static GeoPositions3fPtr positions;
static AllTraverser<OpenSGTraits> all;
typedef SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<K18Dop> > Hierarchy;
static Hierarchy hier;
typedef DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K18Dop>,6,6> Traverser;
static Traverser             traverser;
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
   first->addChild(makeBox(5.0f, 5.0f, 5.0f, 10, 10, 10));
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

   // line geometry
   positions = GeoPositions3f::create();
   beginEditCP(positions);
   positions->addValue( Pnt3f(0,0,0) );
   positions->addValue( Pnt3f(0,0,0) );
   endEditCP(positions);

   GeoColors3fPtr colors = GeoColors3f::create();
   beginEditCP(colors);
   colors->addValue( Color3f(1.0f,1.0f,1.0f) );
   colors->addValue( Color3f(1.0f,0.0f,0.0f) );
   endEditCP(colors);

   GeoIndicesUI32Ptr index = GeoIndicesUI32::create();     
   beginEditCP(index);
   index->addValue( 0 );
   index->addValue( 1 );
   endEditCP(index);

   GeoPLengthsUI32Ptr lens = GeoPLengthsUI32::create();    
   beginEditCP(lens);
   lens->addValue(2);
   endEditCP(lens);
   GeoPTypesUI8Ptr type = GeoPTypesUI8::create();  
   beginEditCP(type);
   type->addValue( GL_LINES );
   endEditCP(type);
   
   GeometryPtr linegeocore = Geometry::create();
   beginEditCP(linegeocore);
   linegeocore->setMaterial(getDefaultUnlitMaterial());
   linegeocore->setPositions(positions);
   linegeocore->setColors(colors);
   linegeocore->setIndices( index );
   linegeocore->setLengths( lens );
   linegeocore->setTypes( type );
   endEditCP( linegeocore );
    
   NodePtr linegeo = Node::create();
   beginEditCP(linegeo);
   linegeo->setCore( linegeocore );
   endEditCP( linegeo );

    scene = Node::create ();
    beginEditCP(scene);
    scene->setCore(Group::create());
    scene->addChild(first);
    scene->addChild(second);
    endEditCP(scene);
    addRefCP(scene);
    std::cout << "scene created" << std::endl;
    NodePtr scene2 = Node::create ();
    beginEditCP(scene2);
    scene2->setCore(Group::create());
    scene2->addChild(scene);
    scene2->addChild(linegeo);
    endEditCP(scene2);

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (scene2);

    // show the whole scene
    mgr->showAll();

    // restructure
    // * fill cache
    GenvisPreprocAction prep;
    prep.apply(scene);
    std::cout << "cache filled." << std::endl;
    // * build hierarchy
    hier.setCoordinateSystem(OSGSingleBVolHierarchyBase::LocalCoordSystem);
    OSGCache::the().setHierarchy(&hier);
    OSGCache::the().apply(scene);
    std::cout << "adapters with 18DOPs created.." << std::endl;

    hier.setParameter(OSGStaticInput::LongestSideMedianId, 50, 2);
    hier.hierarchy();
    std::cout << "18DOP-hierarchy (max depth 50, min num primitives 2) build...." << std::endl;
    // * create double traverser for collision detection
    // realignemnt with DynamicRealign
    traverser.setUseCoherency(false);
    traverser.getDataTyped().setLineGeometry(positions);
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
   beginEditCP(secondTrf);
   secondTrf->getTranslation() += velocity;
   endEditCP(secondTrf);
   
   Traverser::ObjectT& col = traverser.getDataTyped();     
   bool result = all.apply(first, second);
   if (col.getNumContacts() != 1) {
       SFATAL << "Number of results != 1!  Should not occur!" << std::endl;
       exit(1);
   }
   CollisionInterface<OSGTriangleBase,Distance> data(col.getContacts()[0]);
   SLOG << data.getData().distance << " distance, "
	<< col.getDistance() << " abs distance" 
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
    glutIdleFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    return winid;
}



