// test GeometryClustered core (vertex clustering with regular grid)
//
// parameters:
//    scenefile name
//    sphere with radius 1.0, subdivision 5 if no scenefile is given 
// keys:
//    ESC - exit program
//    '+' - increase grid size
//    '-' - decrease grid size

// Headers
#include "OSGGLUT.h"
#include "OSGConfig.h"
#include "OSGGLUTWindow.h"
#include "OSGSimpleSceneManager.h"
#include "OSGAction.h"
#include "OSGSceneFileHandler.h"
#include "OSGGeometry.h"
#include "OSGSimpleGeometry.h"

#include "OSGGeometryClustered.h"

// Activate the OpenSG namespace
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// The SimpleSceneManager to manage simple applications
static NodePtr scene;
static SimpleSceneManager*   mgr;

// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{
    // OSG init
    osgInit(argc,argv);

    // replace Geometry prototype by GeometryClustered prototype
    GeometryClusteredPtr 
      protoGeometryClustered = GeometryClustered::create();
#if 0
    beginEditCP(protoGeometryClustered);
    protoGeometryClustered->setDlistCache(false);
    endEditCP(protoGeometryClustered);
#endif
    Geometry::getClassType().setPrototype(protoGeometryClustered);

    // GLUT init
    int winid = setupGLUT(&argc, argv);

    // the connection between GLUT and OpenSG
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();

    // load the scene
    if (argc < 2) {
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

        scene = 
	  //makeTorus(.5, 2, 16, 16);
	  makeSphere(5, 1.0);
	SLOG << "torus generated" << std::endl;
    } else {
        /*
            All scene file loading is handled via the SceneFileHandler.
        */
        scene = SceneFileHandler::the().read(argv[1]);
	SLOG << "scene '" << argv[1] << "' loaded" << std::endl;
    }
    addRefCP(scene);

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (scene);

    // show the whole scene
    mgr->showAll();

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
      subRefCP(scene);
      exit(1);
    case '+':
      GeometryClustered::cellSize += 1.f;
      break;
    case '-':
      GeometryClustered::cellSize -= 1.f;
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
    glDisable(GL_CULL_FACE);

    return winid;
}



