// Dynamic tesselation of Catmull-Clark subdivision surfaces
// (with creases)

// Headers
#include <OSGMyMesh.h>
#include <OSGDynamicSubdivisionLP.h>
#include <OpenMesh/Core/Utils/Property.hh>

#include <OSGGLUT.h>
#include <OSGConfig.h>
#include <OSGSimpleGeometry.h>
#include <OSGMaterialGroup.h>
#include <OSGGLUTWindow.h>
#include <OSGSimpleMaterial.h>
#include <OSGSimpleSceneManager.h>
#include <OSGAction.h>

// Activate the OpenSG namespace
OSG_USING_NAMESPACE

// The SimpleSceneManager to manage simple applications
SimpleSceneManager* mgr = NULL;

// DynamicSubdivisionLP on MyPolyMesh=OpenMesh::PolyMesh_ArrayKernelT<MyTraits>
MyDynamicSubdivisionLPPtr subdivCore;
MyTriMesh* omesh = NULL;

OpenMesh::EPropHandleT<Int32> isCrease;

// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{
   // check command line options
   if (argc < 2) {
      std::cerr << "Usage:  " << argv[0] << " mesh-filename (obj or off) [maxdepth]" << std::endl;
      exit(1);
   }
   Int32 maxdepth = 4;
   if (argc > 2) maxdepth = atoi(argv[2]);   

   // OSG init
   osgInit(argc,argv);
   OSG::Window::ignoreExtensions("GL_EXT_compiled_vertex_array");

   // GLUT init
   int winid = setupGLUT(&argc, argv);

   // the connection between GLUT and OpenSG
   GLUTWindowPtr gwin= GLUTWindow::create();
   gwin->setId(winid);
   gwin->init();

   // create OpenMesh object
   omesh = new MyTriMesh();
   // new version: property
   omesh->add_property(isCrease,"isCrease");

   if (!OpenMesh::IO::read_mesh(*omesh,argv[1])) {
      SWARNING << "Mesh " << argv[1] << " not found!" << std::endl;
   }
    
   //clear all edges    
   MyTriMesh::EdgeIter eIt(omesh->edges_begin());
   MyTriMesh::EdgeIter eEnd(omesh->edges_end());
   Int32 g=0;
   for (; eIt!=eEnd; ++eIt) {
      omesh->property(isCrease,eIt.handle()) = 0;        // mark edges as smooth
      // for crease-testing:
      //if (g==0) omesh->property(isCrease,eIt.handle()) = 1;
      g++;

       
      // mark borders of mesh as creases
      if (omesh->is_boundary(eIt.handle())) 
      omesh->property(isCrease,eIt.handle()) = 1;
   }

   // create DynamicSubdivisionCC node core
   subdivCore = MyDynamicSubdivisionLP::create();

  
   beginEditCP(subdivCore);
   subdivCore->setMaxDepth(maxdepth);  
   subdivCore->setMesh(omesh);  
   subdivCore->setBackfaceCulling(true);  
   //subdivCore->setAutoUpdate(true); // default 
   endEditCP(subdivCore);
    
   NodePtr scene = Node::create();
   beginEditCP(scene);
   scene->setCore(subdivCore);
   endEditCP(scene);

   MaterialGroupPtr matGroupCore = MaterialGroup::create();
   beginEditCP(matGroupCore);
   matGroupCore->setMaterial(getDefaultMaterial());
   endEditCP(matGroupCore);


   NodePtr matGroupNode = Node::create();
   beginEditCP(matGroupNode);
   matGroupNode->setCore(matGroupCore);
   matGroupNode->addChild(scene);
   endEditCP(matGroupNode);

   // create the SimpleSceneManager helper
   mgr = new SimpleSceneManager;
    
   // tell the manager what to manage
   mgr->turnHeadlightOn();
   mgr->setWindow(gwin );
   mgr->setRoot  (matGroupNode);
   PerspectiveCameraPtr pcam = 
     PerspectiveCameraPtr::dcast(mgr->getWindow()->getPort(0)->getCamera());
   pcam->setFov(deg2rad(45));   
   // show the whole scene
   mgr->showAll();
   mgr->getNavigator()->updateCameraTransformation();
   mgr->getAction()->setFrustumCulling(false);

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
     osgExit();
     exit(1);    
   }
   glutPostRedisplay();
}

// setup the GLUT library which handles the windows for us
int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    int x,y;
    int widthscreen, heightscreen;
    int widthwindow, heightwindow;
    GLenum mode;
    /* get width and height of the screen in pixels*/
    widthscreen  = glutGet( GLUT_SCREEN_WIDTH  );
    heightscreen = glutGet( GLUT_SCREEN_HEIGHT );
    x            = widthscreen/4; 
    y            = heightscreen/4;
    widthwindow  = widthscreen/2;
    heightwindow = heightscreen/2;
    /* position x,y are in pixels relative to the upper-left corner of the screen */
    glutInitWindowPosition(x, y);
    /* widthwindow, heightwindow are in pixels */
    glutInitWindowSize(widthwindow, heightwindow);
    /* mode is a bitwise-OR of bitmask that specifies the type of window to create: 
       GLUT_RGBA    Request RGBA window
       GLUT_DOUBLE  Request double-buffered window
       GLUT_DEPTH   Request accompanying depth buffer */
    mode=GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH;
    glutInitDisplayMode(mode);

    int winid = glutCreateWindow("OpenSG");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    return winid;
}
