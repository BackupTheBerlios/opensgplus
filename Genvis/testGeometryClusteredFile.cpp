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
#include "OSGComponentTransform.h"
#include "OSGGeometry.h"
#include "OSGSimpleGeometry.h"
#include "OSGSFSysTypes.h"
#include "OSGSolidBackground.h"

#include "OSGGeometryClustered.h"

// Activate the OpenSG namespace
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// The SimpleSceneManager to manage simple applications
static NodePtr scene;
static NodePtr sceneOrg;
static SimpleSceneManager*   mgr;

#define GV_CLUSTERED_ADAPTIVE
#ifdef  GV_CLUSTERED_ADAPTIVE
static Real32 numCells = 5;
#else
static Real32 numCells = 32;
#endif
static UInt32 numGeom  = 0;
struct AssignGridParameter
{
   AssignGridParameter (const Real32& value) 
     : m_value(value) 
   {}
   Action::ResultE enter (NodePtr& n) {
     GeometryClusteredPtr core = GeometryClusteredPtr::dcast(n->getCore());
     if (core != NullFC) {
        beginEditCP(core, GeometryClustered::NumCellsFieldMask);
        core->setNumCells(m_value);
	endEditCP  (core, GeometryClustered::NumCellsFieldMask);
     }
     return Action::Continue;
   }

private:
   Real32 m_value;
};
struct ChangeGeometry2GeometryClustered
{
   ChangeGeometry2GeometryClustered () 
   {}
   Action::ResultE enter (NodePtr& n) {
     GeometryPtr core = GeometryPtr::dcast(n->getCore());
     if (core != NullFC) {
        ++numGeom;
	SLOG << "numGeom=" << numGeom << std::endl;
        GeometryClusteredPtr geom = GeometryClustered::create();
        beginEditCP(geom);
        geom->setMaterial(core->getMaterial());
        geom->setIndices(core->getIndices());
        geom->setPositions(core->getPositions());
        geom->setNormals(core->getNormals());
        geom->setTexCoords(core->getTexCoords());
        geom->setColors(core->getColors());
        geom->setSecondaryColors(core->getSecondaryColors());
        geom->setTypes(core->getTypes());
        geom->setLengths(core->getLengths());
        geom->getIndexMapping() = core->getIndexMapping();
	geom->setNumCells(5);
	endEditCP(geom);
        beginEditCP(n);
	n->setCore(geom);
        endEditCP(n);
     }
     return Action::Continue;
   }
};


// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{
    // OSG init
    osgInit(argc,argv);

#if 0
    // replace Geometry prototype by GeometryClustered prototype
    GeometryClusteredPtr 
      protoGeometryClustered = GeometryClustered::create();
    Geometry::getClassType().setPrototype(protoGeometryClustered);
#endif

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
	  makeTorus(.5, 2, 16, 16);
	//makeSphere(5, 1.0);
	SLOG << "torus generated" << std::endl;
    } else {
        /*
            All scene file loading is handled via the SceneFileHandler.
        */
	SLOG << "scene '" << argv[1] << "' loaded" << std::endl;
	GeometryClusteredPtr geom;
	scene = makeCoredNode<GeometryClustered>(&geom);
        beginEditCP(geom, GeometryClustered::MaterialFieldMask
		    | GeometryClustered::ModelFilenameFieldMask
		    | GeometryClustered::NumCellsFieldMask
		    | GeometryClustered::NormalScaleFieldMask);
        geom->setMaterial(getDefaultMaterial());
	geom->setModelFilename(argv[1]);
	geom->setNumCells(numCells);
	geom->setNormalScale(-1.0f);
	endEditCP(geom, GeometryClustered::MaterialFieldMask
		    | GeometryClustered::ModelFilenameFieldMask
		    | GeometryClustered::NumCellsFieldMask
		    | GeometryClustered::NormalScaleFieldMask);
    }
    addRefCP(scene);

    // clone original scene
    NodePtr groupNode = scene;

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (groupNode);
    mgr->setHighlight(scene);
    mgr->setStatistics(true);

    // show the whole scene
    mgr->showAll();
    SolidBackgroundPtr bgnd = SolidBackground::create();
    beginEditCP(bgnd);
    bgnd->setColor(Color3f(1,1,1));
    endEditCP(bgnd);
    mgr->getWindow()->getPort(0)->setBackground(bgnd);

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
   switch(k) {
   case 'w': // wireframe
     glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
     break;
   case 's': // solid
     glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
     break;
   case 27:    
     subRefCP(scene);
     osgExit();
     exit(1);
   case '+':
     numCells += 1;
     // set grid parameters
     {
       AssignGridParameter actor(numCells);
       traverse(scene, 
		osgTypedMethodFunctor1ObjPtrCPtrRef<
		Action::ResultE,
		AssignGridParameter,
		NodePtr        >(&actor, &AssignGridParameter::enter));    
     }
     SLOG << numCells << std::endl;
     break;
   case '-':
     numCells -= 1;
     // set grid parameters
     {
       AssignGridParameter actor(numCells);
       traverse(scene, 
		osgTypedMethodFunctor1ObjPtrCPtrRef<
		Action::ResultE,
		AssignGridParameter,
		NodePtr        >(&actor, &AssignGridParameter::enter));    
     }
     SLOG << numCells << std::endl;
     break;
   }
   glutPostRedisplay();
}

// setup the GLUT library which handles the windows for us
int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    int winid = glutCreateWindow("OpenSG");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    //glutIdleFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glDisable(GL_CULL_FACE);
    //glEnable(GL_CULL_FACE);

    return winid;
}



