// Headers
#include <GL/glut.h>
#include <OSGConfig.h>
#include <OSGSimpleGeometry.h>
#include <OSGGLUTWindow.h>
#include <OSGSimpleSceneManager.h>
#include <OSGAction.h>
#include <OSGSFSysTypes.h>
#include <OSGSceneFileHandler.h>
#include <OSGBaseFunctions.h>
#include <OSGSimpleTexturedMaterial.h>

#include <OSGNode.h>
#include <OSGGroup.h>
#include <OSGTransform.h>
#include <OSGPointLight.h>

#include <OSGLafortuneMaterial.h>

#include <unistd.h>
#include <iostream.h>


// Activate the OpenSG namespace
OSG_USING_NAMESPACE

// The SimpleSceneManager to manage simple applications
SimpleSceneManager *mgr;
// The scene
NodePtr scene;
// Point light source
PointLightPtr pl;
// Material pointers
LafortuneMaterialPtr mat;
// Transformation pointer
TransformPtr tr;
// Camera pointer
CameraPtr camera;

// Settings for Lafortune Material
//-------------------------------------------------------
// Note, that the picture can be dark, if diffuse color is dark. Result for
// Register combiners can be darker than for Fragment program, because
// Register combiners deal with color which are always clamped to (1.0,1.0,1.0)

//Color3f diffuseColor(0.397194,0.175152, 0.0699518 );
//Color3f diffuseColor(0.697194,0.475152, 0.3699518 );
Color3f diffuseColor(0.997194,0.775152, 0.6699518 );
Color3f specularColor( 1.0, 1.0, 1.0 );
Color3f ambientColor( 1.0, 1.0, 1.0 );
Real32 rho_d = 1.0;
float     Ks = 1.0;
float     Ka = 0.05;
float transparency = 0.0;
Color3f Cx( -0.704955, -0.825985,  -0.711507 );
Color3f Cz(0.881194, 0.683593, 0.697774 );
Color3f N(10.7439, 10.7439, 10.7439);

// Switches
//-------------------------------------------------------
bool moveLight = false;
int selector = 0;
int UseFPorRC = 0;
bool use_weight = false;


// Auxiliary variables
//-------------------------------------------------------
Vec3f min, max;            // Scene's bounding box parameters
Vec3f mino, maxo;          // Object's bounding box parameters
static Real32 angle = 0.0;
Real32 lightRadius = 10;
Pnt3f LightPos(0,0,0.5);

// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{

    char *TextureFilename;
    TextureFilename = "cloth56.jpg";           // Default 'specular' texture name
    Color4f tmp;

    cout << "\n===========================================================" << endl;
    cout << "***********************************************************" << endl;
    cout << "*      OpenSG - material with Lafortune Reflectance       *" << endl;
    cout << "===========================================================" << endl;
    cout << "\nKeys: 'q' exit" << endl;
    cout << "        'l' rotate/stop_rotate light" << endl;
    cout << "\nUSAGE: ./main file.scene rho_d Ks uw select FPorRC diffuseImage DiffuseCol Cx Cz N" << endl;
    cout << "    where: rho_d  - coefficient for diffuse color              (float)" << endl;
    cout << "           Ks     - coefficient for highlight                  (float)" << endl;
    cout << "           uw     - use the texture or not                     (0/1)" << endl;
    cout << "           select - choose variant of Lafortune model          (0/1)" << endl;
    cout << "             (0 - color = tex*diffuse + highlight, 1 - color = tex*(diffuse + highlight))" << endl;
    cout << "           FPorRC - use Fragment Program or Register Combiners (0/1)" << endl;
    cout << "           DiffuseImage - the name of the diffuse texture" << endl;
    cout << "           DiffuseCol   - diffuse color                        (rgb)" << endl;
    cout << "           Cx, Cz  - Lafortune coefficients                    (rgb)" << endl;
    cout << "           N       - shininess (the power or the highlight)    (Vec3f)\n" << endl;
    cout << "           Ka      - coefficient for ambient color             (float)" << endl;
    cout << "           AmbientCol   - ambient color                        (rgb)" << endl;

    if( argc >= 3 ) rho_d = atof(argv[2]);
    if( argc >= 4 ) Ks = atof(argv[3]);
    if( argc >= 5 ) use_weight = (bool)atoi(argv[4]);
    if( argc >= 6 ) selector = atoi(argv[5]);
    if( argc >= 7 ) UseFPorRC = atoi(argv[6]);
    if( argc >= 8 ) TextureFilename = argv[7];
    if( argc >= 11 )
    {
      diffuseColor[0] = atof(argv[8]);
      diffuseColor[1] = atof(argv[9]);
      diffuseColor[2] = atof(argv[10]);
    }
    if( argc >= 14 )
    {
      Cx[0] = atof(argv[11]);
      Cx[1] = atof(argv[12]);
      Cx[2] = atof(argv[13]);
    }
    if( argc >= 17 )
    {
      Cz[0] = atof(argv[14]);
      Cz[1] = atof(argv[15]);
      Cz[2] = atof(argv[16]);
    }
    if( argc >= 20 )
    {
      N[0] = atof(argv[17]);
      N[1] = atof(argv[18]);
      N[2] = atof(argv[19]);
    }
    if( argc >= 21 ) Ka = atof(argv[20]);
    if( argc >= 24 )
    {
      ambientColor[0] = atof(argv[21]);
      ambientColor[1] = atof(argv[22]);
      ambientColor[2] = atof(argv[23]);
    }

    // OSG init
    osgInit(argc,argv);

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
    ImagePtr TexImageP;
    TexImageP = Image::create();
    if(argc >= 7)
    {
      TexImageP->read( TextureFilename );
    }
    else
    {
      // If diffuse texture is not needed You can just use this
      // white texture, which doesn't make any influence for
      // the color of the result picture. (Result picture will
      // take its color from Diffuse color and from Specular Term)
      UChar8 imgdata1[] =
      {  255,255,255,  255,255,255,  255,255,255, 255,255,255 };

      TexImageP->set(Image::OSG_RGB_PF, 2, 2, 1, 1, 1, 0, imgdata1 );
    }

    mat = LafortuneMaterial::create();

    // Set all LafortuneMaterial attributes
    beginEditCP(mat);
    {
      mat->setAmbient(ambientColor);
      mat->setDiffuse(diffuseColor);
      mat->setRho_d(rho_d);
      mat->setTransparency(transparency);
      mat->setTextureImage( TexImageP );
      mat->setLightPos( LightPos );
      mat->setCx(Cx);
      mat->setCz(Cz);
      mat->setN(N);
      mat->setSelector(selector);
      mat->setUseweight(use_weight);
      mat->setUseFPorRC(UseFPorRC);
      mat->setKs(Ks);
      mat->setKa(Ka);
    }
    endEditCP(mat);


    //--------------------------------------------------------------------

    // load the scene
    //--------------------------------------------------------------------
    if(argc < 2 || !strcmp( "torus", argv[1] ) )
    {
      // Case when no input files are given
      cerr << "\nNo scene file given! Torus is default scene...\n" << endl;
      cerr << "Supported file formats:" << endl;
      cerr << "-----------------------" << endl;
      std::list<const char*> suffixes;
      SceneFileHandler::the().getSuffixList(suffixes);

      for(std::list<const char*>::iterator it  = suffixes.begin();
	  it != suffixes.end();
	  ++it)
      {
	  cerr << "\t" << (*it) << endl;
      }
      cerr << endl;
      // create root node
      scene = Node::create();

      // create torus
      //--------------------------------------------------------
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

      // Get object's bounding box
      torus->updateVolume();
      torus->getVolume().getBounds( mino, maxo );
      //cerr << "geo created" << endl;
      //--------------------------------------------------------

      // create light
      //--------------------------------------------------------
      // Light geo if needed:
      /*GeometryPtr geol = makeSphereGeo(4, 0.2);

      NodePtr lit = Node::create();
      beginEditCP( lit, Node::CoreFieldMask );
        lit->setCore( geol );
      endEditCP( lit, Node::CoreFieldMask );*/

      NodePtr tnode = Node::create();
      tr = Transform::create();

      beginEditCP(tr);
      {
	tnode->setCore(tr);
        //tnode->addChild(lit); // geometry for light, if needed
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
        pl->setPosition(0,0,0);
	pl->setBeacon( tnode );
      }
      endEditCP(pl);
      //cerr << "light created" << endl;
      //---------------------------------------------------
      // Because of problem with light parameters (vertex program inside the
      // material cannot 'see' light source parametrs for some reasons :-( )
      // we need to push light position as local parametr to the vertex program
      beginEditCP(mat);
      {
         mat->setLightPos(pl->getPosition());
         mat->setCompTexCoord(true);
	 mat->setHalfBBoxObj(((maxo-mino).length()/2));
      }
      endEditCP(mat);

      // add light and torus to scene
      GroupPtr gr = Group::create();
      beginEditCP(scene);
      {
	scene->setCore( gr );
	scene->addChild( plight );
	scene->addChild( torus );
	scene->addChild( tnode );
      }
      endEditCP(scene);
      //cerr << "scene finished" << endl;
    }
    else
    { // Case when scene file is given

      // create root node
      scene = Node::create();

      NodePtr tnode = Node::create();
      tr = Transform::create();

      beginEditCP(tr);
      {
	tnode->setCore(tr);
      }
      // create point light
      //---------------------------------------------------
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
	pl->setPosition(0,0,0);
	pl->setBeacon( tnode );
      }
      endEditCP(pl);
      //cerr << "light created" << endl;
      //---------------------------------------------------
      // add group and first child: plight
      GroupPtr gr = Group::create();
      beginEditCP(scene);
        scene->setCore( gr );
	scene->addChild( plight );
      endEditCP(scene);

      // load scene
      NodePtr sceneLoad;
      sceneLoad = SceneFileHandler::the().read(argv[1], 0);

      GeometryPtr geo = GeometryPtr::dcast(sceneLoad->getCore());

      beginEditCP( geo, Geometry::MaterialFieldMask | Geometry::DlistCacheFieldMask );
      {
	 geo->setMaterial( mat );
         // disable display lists...
	 geo->setDlistCache( false );
      }
      endEditCP( geo, Geometry::MaterialFieldMask | Geometry::DlistCacheFieldMask );

      // Get
      sceneLoad->updateVolume();
      sceneLoad->getVolume().getBounds( mino, maxo );

      beginEditCP(mat);
        mat->setCompTexCoord(true);
	mat->setHalfBBoxObj(((maxo-mino).length()/2));
      endEditCP(mat);

      // add it to scene
      beginEditCP(scene);
        scene->addChild( sceneLoad );
      endEditCP(scene);

      //cerr << "geometry loaded" << endl;
      //cerr << "scene finished" << endl;
    }

    // Get bounding box of the scene
    scene->updateVolume();
    scene->getVolume().getBounds( min, max );

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (scene);

    // show the whole scene
    mgr->showAll();

    // no headlight
    mgr->turnHeadlightOff();

    // Get camera
    camera = gwin->getPort()[0]->getCamera();

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

void idle(void)
{
   Vec3f lit_pos, new_lit_pos;
   lightRadius = 5.0*(max - min).length();

   if( moveLight )
   {
      angle += 8.0;
      if(angle >= 2000.0*acos(-1.0)) angle = 0.0;

      lit_pos[0] = lightRadius*sin(angle/1000);
      lit_pos[1] = (max - min).length()/2;
      lit_pos[2] = lightRadius*cos(angle/1000);
   }
   else
   {
      lit_pos[0] = min[0]/2 + max[0]/2;       lit_pos[1] = min[1]/2 + max[1]/2;
      lit_pos[2] = (max[2] + (max - min).length()*5.0);
      // Note that in almost all OFF-files normals for some reasons are
      // turned inside the object - that is why you can see black screen
      // in the case OFF-scene was loaded. The light source is just on the
      // other side of the object, thus, inverting lit_pos[2] gives
      // illuminated picture.
   }

   Matrix n, lit, litt;
   lit.setTranslate(lit_pos);
   tr->getSFMatrix()->setValue( lit ); // Light transformation

   NodePtr beacon = camera->getBeacon();
   beacon->getToWorld(n);

   n.invert();
   litt = lit;
   litt.multLeft(n);
   litt.mult(lit_pos, new_lit_pos);

   // Push 'real' transformation of the light source to the LafortuneMaterial (take
   // into account transformations of the whole scene)
   beginEditCP(mat);
     mat->setLightPos(new_lit_pos);
   endEditCP(mat);

   glutPostRedisplay();
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
    Real32 step = 1.0;
    Color3f value;

    switch(k)
    {
       case 27:
       case 'q':
         exit(1);

       case 'l':
         moveLight ^= 1;
         break;

       case 'i':  // Increase the shininess power
         // Push the changes to the LafortuneMaterial (to the vertex program)
         value = mat->getN();
         value[0] += step;  value[1] += step;  value[2] += step;
         if(value[0] > 50 || value[1] > 50 || value[2] > 50)
         value = Color3f(50,50,50);

         beginEditCP(mat);
            mat->setN(value);
         endEditCP(mat);
         cout << "New values of Lambertian shininess: " << mat->getN() << "\n";
         break;

       case 'd':  // Decrease the shininess power
         // Push the changes to the LafortuneMaterial (to the vertex program)
         value = mat->getN();
         value[0] -= step;  value[1] -= step;  value[2] -= step;
         if(value[0] < 0 || value[1] < 0 || value[2] < 0)
         value = Color3f(0,0,0);

         beginEditCP(mat);
            mat->setN(value);
         endEditCP(mat);
         cout << "New values of Lambertian shininess: " << mat->getN() << "\n";
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
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    return winid;
}




