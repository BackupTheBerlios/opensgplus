// test point location with BVol-hierarchies (18-DOP) 
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

#include "OSGGVCache.h"
#include "OSGGVGenvisPreprocAction.h"
#include "OSGGVBVolHierarchy.h"
#include "OSGGVBVolPointLocation.h"

// Activate the OpenSG namespace
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// The SimpleSceneManager to manage simple applications
static NodePtr scene;
static SimpleSceneManager* mgr;
static GeoPositions3fPtr   positions;
static Vec3f               velocity;
typedef FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<K18Dop> > Hierarchy;
static  Hierarchy hier;

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

    // load the scene

    if(argc < 2)
    {
        FWARNING(("No file given!\n"));
        FWARNING(("Supported file formats:\n"));
        
        std::list<const char*> suffixes;
        SceneFileHandler::the().getSuffixList(suffixes);
        
        for(std::list<const char*>::iterator it  = suffixes.begin();
                                             it != suffixes.end();
                                           ++it)
        {
            FWARNING(("%s\n", *it));
        }

        scene = makeSphere(4, 1);
    } else {
        /*
            All scene file loading is handled via the SceneFileHandler.
        */
        scene = SceneFileHandler::the().read(argv[1]);
	std::cout << "scene '" << argv[1] << "' loaded" << std::endl;
    }
    addRefCP(scene);

   // point geometry
   positions = GeoPositions3f::create();
   beginEditCP(positions);
   positions->addValue( Pnt3f(0,0,0) );
   endEditCP(positions);

   GeoColors3fPtr colors = GeoColors3f::create();
   beginEditCP(colors);
   colors->addValue( Color3f(1.0f,0.0f,0.0f) );
   endEditCP(colors);

   GeoIndicesUI32Ptr index = GeoIndicesUI32::create();     
   beginEditCP(index);
   index->addValue( 0 );
   endEditCP(index);

   GeoPLengthsUI32Ptr lens = GeoPLengthsUI32::create();    
   beginEditCP(lens);
   lens->addValue(1);
   endEditCP(lens);
   GeoPTypesUI8Ptr type = GeoPTypesUI8::create();  
   beginEditCP(type);
   type->addValue( GL_POINTS );
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

   beginEditCP(scene);
   scene->addChild(linegeo);
   endEditCP(scene);

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (scene);

    // show the whole scene
    mgr->showAll();

    // do point location
    // * fill cache
    OSGCache::the().setHierarchy(NULL);
    OSGCache::the().apply(scene);
    std::cout << "cache filled." << std::endl;

    // * build hierarchy
    hier.setParameter("LongestSideMedian", 50, 2);
    OSGCache::the().setHierarchy(&hier);
    OSGCache::the().apply(scene);
    std::cout << "adapters with 18DOPs created.." << std::endl;
    hier.hierarchy();
    std::cout << "18DOP-hierarchy (max depth 50, min num primitives 2) build...." << std::endl;
    const BoundingVolume<Real>& rootBVol = hier.getRoot()->getBoundingVolume();
    velocity.setValues(0,0.01f*(rootBVol.maxVector()[1]-rootBVol.minVector()[1]),0);
    beginEditCP(positions);
    positions->setValue(Pnt3f(0.5f*(rootBVol.minVector()[0]+rootBVol.maxVector()[0]),
			      rootBVol.minVector()[1]-10.0f*velocity[1], 
			      0.5f*(rootBVol.minVector()[2]+rootBVol.maxVector()[2])), 0); 
    endEditCP(positions);

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
    // * point location in bvol-hierarchy
    {
      SingleTraverser<OpenSGTraits,BVolPointLocationTraits<OpenSGTraits> > trav;
      beginEditCP(positions);
      positions->setValue(positions->getValue(0) + velocity, 0);
      endEditCP(positions);
      trav.getData().setPoint(positions->getValue(0));

      Hierarchy::GroupType* root = hier.getRoot();
      OSGCache::CacheData& data = OSGCache::the()[scene];
      trav.apply(data, root);
      if (trav.getData().getHitTyped() != NULL) {
	std::cout << "bvol-hierarchy: hit found (" 
		  << *trav.getData().getHitTyped()
		  << ")"
		  << std::endl;
      } else {
	std::cout << "bvol-hierarchy: no hit found" << std::endl; 
      }
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
    glPointSize(5.0f);

    return winid;
}



