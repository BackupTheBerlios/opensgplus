// test scene spatialization with BVol-hierarchy on faces
// up to level 4
//
// parameters:
//    scenefile name
//    torus with radii 0.5/2, subdivisions 16 if no scenefile is given 
// keys:
//    ESC - exit program
//    '0' - show restructured scene, computed lastly
//    '1' - show original scene
//    '2' - show face restructured scene
//    '3' - show object restructured scene
//    '4' - show result of objectSpatialize (4-ary hierarchy)
//    '5' - show result of faceSpatialize (4-ary hierarchy)

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
#include "OSGGVSpatialize.h"
#include "OSGGVFaceSpatialize.h"
#include "OSGGVSpatializeFunctions.h"

// Activate the OpenSG namespace
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// The SimpleSceneManager to manage simple applications
static NodePtr scene;
static NodePtr scene2;    
static SimpleSceneManager* mgr  = NULL;
static OSGBVolHierarchy*   hier = NULL;

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

        scene = makeTorus(.5, 2, 16, 16);
	std::cout << "torus generated" << std::endl;
    } else {
        /*
            All scene file loading is handled via the SceneFileHandler.
        */
        scene = SceneFileHandler::the().read(argv[1]);
	std::cout << "scene '" << argv[1] << "' loaded" << std::endl;
    }
    addRefCP(scene);

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (scene);
    mgr->getAction()->setVolumeDrawing(true);

    // show the whole scene
    mgr->showAll();

    // restructure
    // * fill cache
    OSGCache::the().setHierarchy(NULL);
    OSGCache::the().apply(scene);
    std::cout << "cache filled." << std::endl;

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
      if (hier != NULL) {
	 hier->destroy();
	 delete hier;
      }
      subRefCP(scene);
      subRefCP(scene2);
      exit(0);
    case '0':
      mgr->setRoot(scene2);
      std::cout << "restructured scene, computed lastly" << std::endl;
      glutPostRedisplay();
      break;
    case '1':
      mgr->setRoot(scene);
      std::cout << "original scene" << std::endl;
      glutPostRedisplay();
      break;
    case '2': {
      // * build face hierarchy
      if (hier != NULL) { 
	 hier->destroy();
	 delete hier;
      }
      hier = new FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<K18Dop> >();
      hier->setParameter("LongestSideMedian", 50, 10);
      OSGCache::the().setHierarchy(hier);
      OSGCache::the().apply(scene);
      std::cout << "adapters with 18DOPs created.." << std::endl;
      BVolGroupBase* root = hier->getRoot();
      std::cout << "18DOP-hierarchy (max depth 50, min num primitives 10) build...." << std::endl;
      // * restructure with face hierarchy up to level 4
      SingleEnterLeaveTraverser<OpenSGTraits,FaceSpatializeTraits<OpenSGTraits> > trav;
      trav.getData().setDepth(4);
      OSGCache::CacheData& data = OSGCache::the()[scene];
      trav.apply(data, root, data.getAdapterMatrix(0));
      setRefdCP(scene2, trav.getData().getRoot());
      std::cout << "scene restructured....." << std::endl;

      mgr->setRoot(scene2);
      std::cout << "restructured scene" << std::endl;
      glutPostRedisplay();
      break;
    }
    case '3': {
      // * build object hierarchy
      if (hier != NULL) { 
	 hier->destroy();
	 delete hier;
      }
      hier = new ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<K18Dop> >();
      OSGCache::the().setHierarchy(hier);
      OSGCache::the().apply(scene);
      std::cout << "adapters with 18DOPs created.." << std::endl;
      hier->setParameter("LongestSideMedian", 50, 2);
      hier->hierarchy();
      BVolGroupBase* root = hier->getRoot();
      std::cout << "18DOP-hierarchy (max depth 50, min num primitives 10) build...." << std::endl;
      // * restructure with object hierarchy up to level 4
      SingleEnterLeaveTraverser<OpenSGTraits,SpatializeTraits<OpenSGTraits> > trav;
      trav.getData().setDepth(4);
      OSGCache::CacheData& data = OSGCache::the()[scene];
      trav.apply(data, root, data.getAdapterMatrix(0));
      setRefdCP(scene2, trav.getData().getRoot());
      std::cout << "scene restructured....." << std::endl;

      mgr->setRoot(scene2);
      std::cout << "restructured scene" << std::endl;
      glutPostRedisplay();
      break;
    }
    case '4': {
      // utility function objectSpatialize
      if (hier != NULL) { 
	 hier->destroy();
	 delete hier;
	 hier = NULL;
      }
      setRefdCP(scene2, objectSpatialize<K6Dop>(scene, 50, 2, "LongestSideMean", 1));
      std::cout << "scene restructured....." << std::endl;

      mgr->setRoot(scene2);
      std::cout << "restructured scene" << std::endl;
      glutPostRedisplay();
      break;
    }
    case '5': {
      // utility function faceSpatialize
      if (hier != NULL) { 
	 hier->destroy();
	 delete hier;
	 hier = NULL;
      }
      setRefdCP(scene2, faceSpatialize<K6Dop>(scene, 50, 10, "LongestSideMean", 1));
      std::cout << "scene restructured....." << std::endl;

      mgr->setRoot(scene2);
      std::cout << "restructured scene" << std::endl;
      glutPostRedisplay();
      break;
    }
    case '6': {
      // utility function faceSpatialize
      if (hier != NULL) { 
	 hier->destroy();
	 delete hier;
	 hier = NULL;
      }
      setRefdCP(scene2, spatialize<K6Dop>(scene, 50, 10, "LongestSideMean", 1));
      std::cout << "scene restructured....." << std::endl;

      mgr->setRoot(scene2);
      std::cout << "restructured scene" << std::endl;
      glutPostRedisplay();
      break;
    }
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
    glutKeyboardFunc(keyboard);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    return winid;
}



