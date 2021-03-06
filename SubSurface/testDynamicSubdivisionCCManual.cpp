// Dynamic tesselation of Catmull-Clark subdivision surfaces
// (with creases)

// Headers
#include <OSGMyMesh.h>
#include <OSGDynamicSubdivisionCC.h>
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

// DynamicSubdivisionCC on MyPolyMesh=OpenMesh::PolyMesh_ArrayKernelT<MyTraits>
MyDynamicSubdivisionCCPtr subdivCore;
MyPolyMesh* omesh = NULL;

OpenMesh::EPropHandleT<Int32> isCrease;

int unidepth = 3;


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

   Int32 maxdepth=4;
   if (argc >2) maxdepth=atoi(argv[2]);

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
   omesh = new MyPolyMesh();
   // new version: property
   omesh->add_property(isCrease,"isCrease");
   if (!OpenMesh::IO::read_mesh(*omesh,argv[1])) {
      SWARNING << "Mesh " << argv[1] << " not found!" << std::endl;
   }
    
   //clear all edges    
   MyPolyMesh::EdgeIter eIt(omesh->edges_begin());
   MyPolyMesh::EdgeIter eEnd(omesh->edges_end());
   Int32 g=0;
   for (; eIt!=eEnd; ++eIt) {
      omesh->property(isCrease,eIt.handle()) = 0;
      // box.obj -> can
      //if (g==4 || g==6 || g==8 || g==10 || g==11 || g==9 || g==1 || g==3) omesh->property(isCrease,eIt.handle()) = 1;      
      //if (g==0) omesh->property(isCrease,eIt.handle()) = 1;      
      g++;
       
      // mark border of mesh as crease
      if (omesh->is_boundary(eIt.handle())) 
      omesh->property(isCrease,eIt.handle()) = 1;
   }

   // create DynamicSubdivisionCC node core
   subdivCore = MyDynamicSubdivisionCC::create();

   // parameter settings favouring curvature
   beginEditCP(subdivCore);
   // basis mesh
   subdivCore->setMesh(omesh);
   // backface culling; default is true
   subdivCore->setBackfaceCulling(false);  
   // curvature approx by normal cone technique
   // cosine angle of normal cone opening; default is 0.01
   subdivCore->setNormalConeAperture(0.01); 
   // classification threshold for front vertex, back vertex, silhouette vertex
   // cosine angle of silhouette cone opening; default is 0.01
   subdivCore->setVertexClassifier(0); // all is silhouette
   // minimum projection size; default is 5
   subdivCore->setMinProjSize(0.1);
   // maximum projection size; default is 15
   subdivCore->setMaxProjSize(100);
   // minimum subdivision depth; default is 0
   subdivCore->setMinDepth(0);
   // maximum subdivision depth; default is 4
   subdivCore->setMaxDepth(maxdepth);
   // auto update of tesselation; default is true
   subdivCore->setAutoUpdate(false);  
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
   case '+': unidepth++;
      beginEditCP(subdivCore);
      subdivCore->setMaxDepth(unidepth);
      subdivCore->setMinDepth(unidepth);
      endEditCP(subdivCore);
      break;
   case '-': unidepth--;
      beginEditCP(subdivCore);
      subdivCore->setMaxDepth(unidepth);
      subdivCore->setMinDepth(unidepth);
      endEditCP(subdivCore);
      break;
   case 'u':
      // if (!subdivCore->getAutoUpdate()) the tesselation update is done
      // on call subdivCore->prepareFrame
      subdivCore->prepareFrame(mgr->getWindow()->getPort(0));
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

