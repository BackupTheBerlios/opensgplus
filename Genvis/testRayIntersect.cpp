// test intersection of BVol-hierarchy with a ray
// test intersection of regular grid with a ray
//
// parameters:
//    scenefile name
//    box with side length 1.0, subdivisions 16 if no scenefile is given 
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
#include "OSGGVBVolRayIntersect.h"
#include "OSGGVRegularGrid.h"
#include "OSGGVRecursiveFillGrid.h"

// Activate the OpenSG namespace
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// The SimpleSceneManager to manage simple applications
static NodePtr           scene;
static GeoPositions3fPtr positions;
static SimpleSceneManager* mgr;
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

        scene = makeBox(1, 1, 1, 10, 10, 10);
	std::cout << "box generated" << std::endl;
    } else {
        /*
            All scene file loading is handled via the SceneFileHandler.
        */
        scene = SceneFileHandler::the().read(argv[1]);
	std::cout << "scene '" << argv[1] << "' loaded" << std::endl;
    }

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

    // do ray intersect
    // * fill cache
    {
      GenvisPreprocAction prep;
      prep.apply(scene);
    }
    std::cout << "cache filled." << std::endl;

    // * build hierarchy
    OSGCache::the().setHierarchy(&hier);
    OSGCache::the().apply(scene);
    std::cout << "adapters with 18DOPs created.." << std::endl;

    hier.setParameter(OSGStaticInput::LongestSideMedianId, 50, 10);
    hier.hierarchy();
    BVolGroup<K18Dop>* root = hier.getRoot();
    std::cout << "18DOP-hierarchy (max depth 50, min num primitives 10) build...." << std::endl;

    // * build regular-grid
    OSGCache::AdapterVector prim;
    OSGCache::the().collectAdapter(Hierarchy::AdapterType::getAdapterId(), prim);
    // create grid for scene extent
    // parameter: create prim.size() voxels in total
    OSGRegularGrid       grid(AABox(root->getBoundingVolume()), 
			      prim.size(), 
			      RegularGridBase::MaxVoxels);
    // insert all adapters into the grid
    OSGRecursiveFillGrid(grid).fillVoxels(prim);
    std::cout << "regular-grid build...." << std::endl;

    // * ray intersect with bvol-hierarchy
    {
      Ray          ray(Pnt3f(0,0,10), Vec3f(0,0,-1));
      Intersection in(ray);
      beginEditCP(positions);
      positions->setValue(ray.getOrigin(), 0);
      SingleEnterLeaveTraverser<OpenSGTraits,BVolRayIntersectTraits<OpenSGTraits> > trav;
      trav.getData().setIntersect(in);
      OSGCache::CacheData& data = OSGCache::the()[scene];
      trav.apply(data, root, data.getAdapterMatrix(0));
      if (trav.getData().getHit()) {
	std::cout << "bvol-hierarchy: hit found (" 
		  << "distance=" << trav.getData().getHitDist() 
		  << ", point="  << trav.getData().getHitPoint()
		  << ")"
		  << std::endl;
	positions->setValue(trav.getData().getHitPoint(), 1);
	in.clearData(NULL);
      } else {
	std::cout << "bvol-hierarchy: no hit found" << std::endl; 
	positions->setValue(ray.getOrigin()+infinity*ray.getDirection(), 1);
      }
      endEditCP(positions);
    }

    // * ray intersect with regular-grid
    {
      Ray          ray(Pnt3f(0,0,10), Vec3f(0,0,-1));
      Intersection in(ray);
      if (grid.calcIntersect(in)) {
	std::cout << "regular-grid: hit found (" 
		  << "distance=" << in.getDist() 
		  << ", point="  << ray.getOrigin()+in.getDist()*ray.getDirection()
		  << ")"
		  << std::endl;
	in.clearData(NULL);
      } else {
	std::cout << "regular-grid: no hit found" << std::endl; 
      }
    }

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
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    return winid;
}



