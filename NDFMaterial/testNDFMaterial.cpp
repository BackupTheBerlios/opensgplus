// OpenSG Tutorial Example: testNDFMaterial
//
// Demonstrates the use of the NDFMaterial Chunk
// 
// An NDF material let's you paint the shape of a highlight.
// See bigstar.ppm for an example.
//
// For a full explanation of NDF Shading, please look at:
// http://www.opensg.org/OpenSGPLUS/symposium/Papers2002/Kautz_NDF.pdf


// Headers
#include <GL/glut.h>
#include <OSGConfig.h>
#include <OSGSimpleGeometry.h>
#include <OSGGLUT.h>
#include <OSGGLUTWindow.h>
#include <OSGSimpleSceneManager.h>
#include <OSGAction.h>
#include <OSGSFSysTypes.h>
#include <OSGSceneFileHandler.h>
#include <OSGBaseFunctions.h>

#include <OSGNode.h>
#include <OSGGroup.h>
#include <OSGTransform.h>
#include <OSGPointLight.h>

#include "OSGNDFMaterial.h"

#include <unistd.h>
#include <iostream.h>


// Activate the OpenSG namespace
OSG_USING_NAMESPACE


// ------------------- global vars ----------------------
//
// The SimpleSceneManager to manage simple applications
SimpleSceneManager *mgr;
// The scene
NodePtr scene;
// The NDF used
ImagePtr ndf;
// Point light source
PointLightPtr pl;
// Material pointers
NDFMaterialPtr mat;

// ------------------- switches ----------------------
//
bool moveLight = false;
bool shadowing = true;
bool fresnel = true;
bool correctCosine = true;
bool showOnlyDiffuse = false;
UInt8 UseFPorRC = 0;
float transparency = 0.5;
Color3f fresnelColor( 1.0, 1.0, 1.0 );
Color3f diffuseColor( 0.0, 1.0, 1.0 );
Color3f specularColor( 1.0, 1.0, 1.0 );
Color3f ambientColor( 0.0, 0.0, 0.0 );
Real32 Ka = 0.2;
Real32 Kd = 0.8;
Real32 Ks = 1.0;
Color3f LightSpecularColor(1.0, 1.0, 1.0);
Real32 lightRadius = 10.0;
Pnt3f LightPos(0,0,0.5);

// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{

    char *ndfFilename, *diffuseFilename;
    ndfFilename = "bigstar.jpg";           // Default 'specular' texture name
    diffuseFilename = "marble.jpg";   // Default 'diffuse' texture name

    Color4f tmp;

    cout << "\n=================================================" << endl;
    cout << "*************************************************" << endl;
    cout << "*   OpenSG - NDF Shading Demo                   *" << endl;
    cout << "=================================================" << endl;
    cout << "Keys: 'q' exit" << endl;
    cout << "      'f' toggle Fresnel" << endl;
    cout << "      's' toggle self-shadowing" << endl;
    cout << "      'c' toggle correct cosine" << endl;
    cout << "      'd' toggle diffuse term only" << endl;
    cout << "      'l' toggle light" << endl;
    cout << "      '}' move light source forward" << endl;
    cout << "      '{' move light source backward" << endl;
    cout << "     'Up' move light source up    (NumLock is ON)" << endl;
    cout << "   'Down' move light source down  (NumLock is ON)" << endl;
    cout << "   'Left' move light source left  (NumLock is ON)" << endl;
    cout << "  'Right' move light source right (NumLock is ON)" << endl;
    cout << "\nUSAGE: ./main file.scene diffuseImage ndfImage S F C D R DiffuseCol FresnelCol SpecularCol AmbientCol" << endl;
    cout << "where: S - take into account Self Shadowing (0/1)" << endl;
    cout << "       F - take into account Fresnel Term   (0/1)" << endl;
    cout << "       C - take into account Correct Cosine (0/1)" << endl;
    cout << "       D - show only diffuse material (0/1)" << endl;
    cout << "       R - use fragment program or register combiners (0/1)" << endl;
    cout << "       DiffuseCol  - diffuse color  (rgb)" << endl;
    cout << "       FresnelCol  - Fresnel color  (rgb)" << endl;
    cout << "       SpecularCol - specular color (rgb)" << endl;
    cout << "       AmbientCol  - ambient color  (rgb)\n" << endl;

    if( argc >= 3 ) diffuseFilename = argv[2];
    if( argc >= 4 ) ndfFilename = argv[3];
    if( argc >= 5 ) shadowing = (bool)atoi(argv[4]);
    if( argc >= 6 ) fresnel   = (bool)atoi(argv[5]);
    if( argc >= 7 ) correctCosine = (bool)atoi(argv[6]);
    if( argc >= 8 ) showOnlyDiffuse = (bool)atoi(argv[7]);
    if( argc >= 9 ) UseFPorRC = atoi(argv[8]);
    if( argc >= 12 )
      diffuseColor = Color3f( atof(argv[9]), atof(argv[10]), atof(argv[11]) );
    if( argc >= 15 )
      fresnelColor = Color3f( atof(argv[12]), atof(argv[13]), atof(argv[14]) );
    if( argc >= 18 )
      specularColor = Color3f( atof(argv[15]), atof(argv[16]), atof(argv[17]) );
    if( argc >= 21 )
      ambientColor = Color3f( atof(argv[18]), atof(argv[19]), atof(argv[20]) );

    // OSG init
    osgInit(argc,argv);

    // disable display lists...
    FieldContainerPtr pProto = Geometry::getClassType().getPrototype();

    GeometryPtr pGeoProto = GeometryPtr::dcast(pProto);

    if(pGeoProto != NullFC)
    {
        pGeoProto->setDlistCache(false);
    }

    // GLUT init
    int winid = setupGLUT(&argc, argv);

    // the connection between GLUT and OpenSG
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->setSize( 800, 800 );
    gwin->init();

    // create material
    //--------------------------------------------------------------------
    // read the image for "specular" texture
    ImagePtr imageNDF, imageDiffuse;
    imageNDF = Image::create();
    imageNDF->read( ndfFilename );

    // read the image for "diffuse" texture
    imageDiffuse = Image::create();
    // If diffuse texture is not needed You can just use this
    // white texture, which doesn't make any influence for
    // the color of the result picture. (Result picture will
    // take its color from Diffuse color and from Specular Term)
    /*UChar8 imgdata1[] =
        {  255,255,255,  255,255,255,  255,255,255, 255,255,255 };
    imageDiffuse->set(Image::OSG_RGB_PF, 2, 2, 1, 1, 1, 0, imgdata1 );
    */
    imageDiffuse->read( diffuseFilename );

    mat = NDFMaterial::create();

    // Set all NDFmaterial attributes
    beginEditCP(mat);
    {
     mat->setAmbient(ambientColor);
     mat->setDiffuse(diffuseColor);
     mat->setSpecular(specularColor);
     mat->setKa(Ka);
     mat->setKd(Kd);
     mat->setKs(Ks);
     mat->setDiffuseImage( imageDiffuse );
     mat->setNDFImage( imageNDF );
     mat->setFresnelTerm( fresnel );
     mat->setSelfShadowingTerm( shadowing );
     mat->setFresnel( fresnelColor );
     mat->setCorrectCosine( correctCosine );
     mat->setLightPos( LightPos );
     mat->setLightSpecular( LightSpecularColor );
     mat->setOnlyDiffuse( showOnlyDiffuse );
     mat->setUseFPorRC( UseFPorRC );
    }
    endEditCP(mat);
    //--------------------------------------------------------------------

    // load the scene
    //--------------------------------------------------------------------
    if(argc < 2 || !strcmp( "torus", argv[1] ) )
    {
      //cerr << "No file given!" << endl;
      //cerr << "Supported file formats:" << endl;

      std::list<const char*> suffixes;
      SceneFileHandler::the().getSuffixList(suffixes);

      for(std::list<const char*>::iterator it  = suffixes.begin();
	  it != suffixes.end();
	  ++it)
      {
	  //cerr << (*it) << endl;
      }

      // create root node
      scene = Node::create();

      // create torus
      GeometryPtr geo = makeTorusGeo(.8, 1.8, 32, 32);
      beginEditCP( geo, Geometry::MaterialFieldMask | Geometry::DlistCacheFieldMask );
      {
        geo->setMaterial( mat );
	geo->setDlistCache( false );
      }
      endEditCP( geo, Geometry::MaterialFieldMask | Geometry::DlistCacheFieldMask );

      NodePtr torus = Node::create();
      beginEditCP( torus, Node::CoreFieldMask );
        torus->setCore( geo );
      endEditCP( torus, Node::CoreFieldMask );
      //cerr << "geo created" << endl;

      // create light
      //---------------------------------------------------
      NodePtr tnode = Node::create();
      TransformPtr tr = Transform::create();

      beginEditCP(tr);
      {
	tnode->setCore(tr);
        //tnode->addChild(lightGeo); // geometry for light, if needed
      }
      endEditCP(tr);

      NodePtr plight = Node::create();
      pl = PointLight::create();
      beginEditCP(plight);
        plight->setCore( pl );
      endEditCP(plight);

      // set props of light source
      beginEditCP(pl);
      {
	pl->setAmbient( 1, 1, 1, 1 );
	pl->setDiffuse( 1, 1, 1, 1 );
	pl->setSpecular( 1, 1, 1, 1 );
        pl->setPosition(0,0,-2);
	pl->setBeacon( tnode );
      }
      endEditCP(pl);
      //cerr << "light created" << endl;
      //---------------------------------------------------
      tmp = pl->getSpecular();
      LightSpecularColor[0] = tmp[0];
      LightSpecularColor[1] = tmp[1];
      LightSpecularColor[2] = tmp[2];

      // Because of problem with light parameters (vertex program inside the
      // material cannot 'see' light source parametrs for some reasons :-( )
      // we need to push light position as local parametr to the vertex program
      beginEditCP(mat);
      {
        mat->setLightPos(pl->getPosition());
        mat->setLightSpecular( LightSpecularColor );
      }
      endEditCP(mat);

      // add light and torus to scene
      GroupPtr gr = Group::create();
      beginEditCP(scene);
      {
	scene->setCore( gr );
	scene->addChild( plight );
	scene->addChild( torus );
      }
      endEditCP(scene);
      //cerr << "scene finished" << endl;
    }
    else
    {
      // create root node
      scene = Node::create();

      NodePtr tnode = Node::create();
      TransformPtr tr = Transform::create();

      beginEditCP(tr);
      {
	tnode->setCore(tr);
        //tnode->addChild(lightGeo); // geometry for light, if needed
      }
      // create point light
      NodePtr plight = Node::create();
      pl = PointLight::create();
      beginEditCP(plight);
        plight->setCore( pl );
      endEditCP(plight);

      // set props of light source
      beginEditCP(pl);
      {
	pl->setAmbient( 1, 1, 1, 1 );
	pl->setDiffuse( 1, 1, 1, 1 );
	pl->setSpecular( 1, 1, 1, 1 );
        //pl->setPosition(0,0,0.5);
        pl->setPosition(0,1,5);
	pl->setBeacon( tnode );
      }
      endEditCP(pl);
      //cerr << "light created" << endl;

      tmp = pl->getSpecular();
      LightSpecularColor[0] = tmp[0];
      LightSpecularColor[1] = tmp[1];
      LightSpecularColor[2] = tmp[2];

      beginEditCP(mat);
      {
        mat->setLightPos(pl->getPosition());
        mat->setLightSpecular( LightSpecularColor );
      }
      endEditCP(mat);


      // add group and first child: plight
      GroupPtr gr = Group::create();
      beginEditCP(scene);
        scene->setCore( gr );
	scene->addChild( plight );
      endEditCP(scene);

      // load scene (1st time)
      NodePtr sceneLoad;
      sceneLoad = SceneFileHandler::the().read(argv[1], 0);

      GeometryPtr geo = GeometryPtr::dcast(sceneLoad->getCore());
      beginEditCP( geo, Geometry::MaterialFieldMask | Geometry::DlistCacheFieldMask );
      {
	 geo->setMaterial( mat );
	geo->setDlistCache( false );
      }
      endEditCP( geo, Geometry::MaterialFieldMask | Geometry::DlistCacheFieldMask );
      // add it to scene
      beginEditCP(scene);
        scene->addChild( sceneLoad );
      endEditCP(scene);

      //cerr << "geometry loaded" << endl;
      //cerr << "scene finished" << endl;

    }

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (scene);

    // show the whole scene
    mgr->showAll();

    // no headlight
    mgr->turnHeadlightOff();

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
  // render scene
  mgr->redraw();
}

void idle(void) {
  if( moveLight ) {
    // computing new position of satelit
    static Real32 angle = 0.0;

    angle += 10.0;

    Vec3f t( lightRadius*sin(angle / 1000),
	     1,
	     lightRadius*cos(angle / 1000));

    //cout << "Light: " << t << endl;

    beginEditCP(pl);
    {
      pl->setPosition(t);
    }
    endEditCP(pl);

    beginEditCP(mat);
    {
      mat->setLightPos(pl->getPosition());
    }
    endEditCP(mat);

    glutPostRedisplay();
    usleep( 5 ); // keep load low
  } else{
    usleep( 5 ); // keep load low
  }
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
    Pnt3f lpos;
    Vec3f t;
    cout << "\n------------------------------------\n";

    switch(k)
    {
    case 27:
    case 'q':
      exit(1);

    case 'l':
      moveLight ^= 1;
       break;

    case 's':
      shadowing ^= 1;
      beginEditCP(mat);
      mat->setSelfShadowingTerm( shadowing );
      endEditCP(mat);
      break;

    case 'f':
      fresnel ^= 1;
      beginEditCP(mat);
      mat->setFresnelTerm( fresnel );
      endEditCP(mat);
      break;

    case 'c':
      correctCosine ^= 1;
      beginEditCP(mat);
      mat->setCorrectCosine( correctCosine );
      endEditCP(mat);
      break;

    case 'd':
      showOnlyDiffuse ^= 1;
      beginEditCP(mat);
      mat->setOnlyDiffuse( showOnlyDiffuse );
      endEditCP(mat);
      break;

    case 54:  // (Right). Moving light to the right
      lpos = pl->getPosition();
      // Change position of light source
      t[0] = lpos[0] + 0.2;
      t[1] = lpos[1];
      t[2] = lpos[2];
      beginEditCP(pl);
         pl->setPosition(t);
      endEditCP(pl);
      cout << "Light position: " << pl->getPosition() << "\n\n";
      // Push the shanges to the NDFmaterial (to the vertex program)
      beginEditCP(mat);
         mat->setLightPos(pl->getPosition());
      endEditCP(mat);
      break;

    case 52:  // (Left). Moving light to the left
      lpos = pl->getPosition();
      // Change position of light source
      t[0] = lpos[0] - 0.2;
      t[1] = lpos[1];
      t[2] = lpos[2];
      beginEditCP(pl);
         pl->setPosition(t);
      endEditCP(pl);
      cout << "Light position: " << pl->getPosition() << "\n\n";
      // Push the shanges to the NDFmaterial (to the vertex program)
      beginEditCP(mat);
         mat->setLightPos(pl->getPosition());
      endEditCP(mat);
      break;

    case 56:  // (Up). Moving light to the up
      lpos = pl->getPosition();
      // Change position of light source
      t[0] = lpos[0];
      t[1] = lpos[1] + 0.2;
      t[2] = lpos[2];
      beginEditCP(pl);
         pl->setPosition(t);
      endEditCP(pl);
      cout << "Light position: " << pl->getPosition() << "\n\n";
      // Push the shanges to the NDFmaterial (to the vertex program)
      beginEditCP(mat);
         mat->setLightPos(pl->getPosition());
      endEditCP(mat);
      break;

    case 50:  // (Down. Moving light to the left
      lpos = pl->getPosition();
      // Change position of light source
      t[0] = lpos[0];
      t[1] = lpos[1] - 0.2;
      t[2] = lpos[2];
      beginEditCP(pl);
         pl->setPosition(t);
      endEditCP(pl);
      cout << "Light position: " << pl->getPosition() << "\n\n";
      // Push the shanges to the NDFmaterial (to the vertex program)
      beginEditCP(mat);
         mat->setLightPos(pl->getPosition());
      endEditCP(mat);
      break;

    case 93:  // }  Moving light forward
      lpos = pl->getPosition();
      // Change position of light source
      t[0] = lpos[0];
      t[1] = lpos[1];
      t[2] = lpos[2] + 0.2;
      beginEditCP(pl);
         pl->setPosition(t);
      endEditCP(pl);
      cout << "Light position: " << pl->getPosition() << "\n\n";
      // Push the shanges to the NDFmaterial (to the vertex program)
      beginEditCP(mat);
         mat->setLightPos(pl->getPosition());
      endEditCP(mat);
      break;

    case 91:  // {  Moving light back
      lpos = pl->getPosition();
      // Change position of light source
      t[0] = lpos[0];
      t[1] = lpos[1];
      t[2] = lpos[2] - 0.2;
      beginEditCP(pl);
         pl->setPosition(t);
      endEditCP(pl);
      cout << "Light position: " << pl->getPosition() << "\n\n";
      // Push the shanges to the NDFmaterial (to the vertex program)
      beginEditCP(mat);
         mat->setLightPos(pl->getPosition());
      endEditCP(mat);
      break;
    }

    if(shadowing) cout << "SelfShadowing   ON  ('s')\n";
    else cout << "SelfShadowing   OFF  ('s')\n";
    if(fresnel) cout << "FresnelTerm     ON  ('f')\n";
    else cout << "FresnelTerm     OFF  ('f')\n";
    if(correctCosine) cout << "CorrectCosine   ON  ('c')\n";
    else cout << "CorrectCosine   OFF  ('c')\n";
    if(showOnlyDiffuse) cout << "ShowOnlyDiffuse ON  ('d')\n";
    else cout << "ShowOnlyDiffuse OFF  ('d')\n";

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
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    
    return winid;
}




