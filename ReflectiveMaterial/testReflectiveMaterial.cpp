/***************************************************************************\
 *   Initial coding by Polina Kondratieva                                  *
 *                                                                         *
 *   contact:    PolinaKondratieva@gmx.de     (supervisor Jan Kautz)       *
\***************************************************************************/


#include <OSGGLUT.h>

#include <OSGConfig.h>
#include <OSGSimpleGeometry.h>
#include <OSGGLUTWindow.h>

#include <OSGTime.h>
#include <OSGImage.h>
#include <OSGTextureChunk.h>

#include <OSGSceneFileHandler.h>
#include <OSGTransform.h>
#include <OSGRenderAction.h>
#include <OSGTrackball.h>
#include <OSGSolidBackground.h>
#include <OSGSimpleTexturedMaterial.h>
#include <OSGPointLight.h>
#include "OSGReflectiveMaterial.h"

OSG_USING_NAMESPACE

RenderAction *renact;

ReflectiveMaterialPtr cubemat;
TransformPtr cam_trans, camgf_tr;
TransformPtr light_tr;
TransformPtr facetr[6];
TransformPtr sphere2tr;
TransformPtr envtr;
TransformPtr objtr;

PerspectiveCameraPtr camgf;
PerspectiveCameraPtr cam;
WindowPtr win;
Trackball tball;
ViewportPtr vp2;

NodePtr envnode, cam_beacon, delight;
NodePtr light_transn;
NodePtr b1n, t1n;
PointLightPtr ple;

Color3f Fresnel = Color3f(0.05,0.05,0.05);


int mouseb = 0;
int lastx=0, lasty=0;
float object_size = 1.0f;
float scene_scale = 7;
Vec3f min,max;
Vec3f litPos(0,0,1);
Vec3f eyePos(0,0,1);
Real32 transparency = 0.0;


// Draw the environment scene
//==========================================================================
void draw_env_scene(ImagePtr *imfg, ImagePtr sphimg)
{
    SimpleTexturedMaterialPtr matfg[6];
    float cube_size;
    cube_size = scene_scale * object_size;

    envtr = Transform::create();
    beginEditCP(envnode);
       envnode->setCore(envtr);
    endEditCP(envnode);

    // Make faces of the Cube (Environment)
    //---------------------------------------------------------
    GeometryPtr geofg[6];

    for(int i = 0; i < 6; i++)
    {
       matfg[i] = SimpleTexturedMaterial::create();
       beginEditCP(matfg[i]);
          matfg[i]->setColorMaterial(GL_NONE);
	  matfg[i]->setLit(true);
          matfg[i]->setImage        (imfg[i]);
          matfg[i]->setMinFilter    (GL_LINEAR_MIPMAP_LINEAR);
          matfg[i]->setMagFilter    (GL_NEAREST);
          matfg[i]->setEnvMode      (GL_DECAL); // do not use MODULATE -> it gives no contribution to CubeMap!
          matfg[i]->setEnvMap       (false);
       endEditCP  (matfg[i]);

       geofg[i] = makePlaneGeo(cube_size, cube_size, 16, 16);
       beginEditCP( geofg[i], Geometry::MaterialFieldMask);
          geofg[i]->setMaterial( matfg[i] );
       beginEditCP( geofg[i], Geometry::MaterialFieldMask);

       NodePtr face = Node::create();
       beginEditCP( face, Node::CoreFieldMask );
          face->setCore( geofg[i] );
       endEditCP( face, Node::CoreFieldMask );

       facetr[i] = Transform::create();
       NodePtr trface = Node::create();
       beginEditCP(trface);
          trface->setCore(facetr[i]);
          trface->addChild(face);
       endEditCP(trface);

       beginEditCP(envnode);
          envnode->addChild(trface);
       endEditCP(envnode);
    }

    // Add an additional sphere to Environment
    //---------------------------------------------------------
    sphere2tr = Transform::create();
    NodePtr trsphere2 = Node::create();

    SimpleTexturedMaterialPtr sphmat;
    sphmat = SimpleTexturedMaterial::create();

    beginEditCP(sphmat);
       sphmat->setColorMaterial(GL_NONE);
       sphmat->setLit(true);
       sphmat->setImage        (sphimg);
       sphmat->setMinFilter    (GL_LINEAR_MIPMAP_LINEAR);
       sphmat->setMagFilter    (GL_NEAREST);
       sphmat->setEnvMode      (GL_DECAL);// do not use MODULATE -> it gives no contribution to CubeMap!
       sphmat->setEnvMap       (false);
    endEditCP  (sphmat);

    NodePtr sphere2 = makeSphere(3, cube_size/10);
    GeometryPtr geosph = GeometryPtr::dcast(sphere2->getCore());

    beginEditCP( geosph, Geometry::MaterialFieldMask);
       geosph->setMaterial( sphmat );
    beginEditCP( geosph, Geometry::MaterialFieldMask);

    beginEditCP(trsphere2);
      trsphere2->setCore(sphere2tr);
      trsphere2->addChild(sphere2);
    endEditCP(trsphere2);

    beginEditCP(envnode);
      envnode->addChild(trsphere2);
    endEditCP(envnode);

    // Geometry for light source if needed...
    //-------------------------------------------------
    /*NodePtr geo_node = makeLatLongSphere(16, 16, cube_size/100);
    GeometryPtr geol = GeometryPtr::dcast(geo_node->getCore());

    SimpleMaterialPtr sm = SimpleMaterial::create();

    beginEditCP(sm, SimpleMaterial::DiffuseFieldMask |
                    SimpleMaterial::LitFieldMask);
    {
       sm->setLit(false);
       sm->setDiffuse(Color3f(1.0,0.0,1.0));
    }
    endEditCP  (sm, SimpleMaterial::DiffuseFieldMask |
                    SimpleMaterial::LitFieldMask);

    beginEditCP(geol, Geometry::MaterialFieldMask);
    {
       geol->setMaterial(sm);
    }
    endEditCP  (geol, Geometry::MaterialFieldMask);*/

    // Add light to the scene
    //-------------------------------------------------
    light_transn = Node::create();
    light_tr = Transform::create();

    beginEditCP(light_tr);
       light_transn->setCore(light_tr);
       //light_transn->addChild(geo_node); // geometry for light, if needed
    endEditCP(light_tr);

    beginEditCP(envnode); //trnode);
       envnode->addChild(light_transn);
    endEditCP(envnode); //trnode);

    delight = Node::create();
    ple = PointLight::create();

    beginEditCP(delight);
       delight->setCore( ple );
    endEditCP(delight);

    beginEditCP(ple);
       ple->setAmbient( .3, .3, .3, 1 );
       ple->setDiffuse( 1, 1, 1, 1 );
       ple->setPosition(0,0,0);
       ple->setBeacon( light_transn );
    endEditCP(ple);
}
//==========================================================================


// Redraw the window
//==========================================================================
void display(void)
{
    float cube_size = scene_scale * object_size * 0.5; // half of the cube side

    Real32 t = glutGet(GLUT_ELAPSED_TIME );

    // Transformations for the whole scene (realized with help of trackball)
    //----------------------------------------------------------------------
    Matrix m1, m2, m3;
    Quaternion q1;
    tball.getRotation().getValue(m3);
    q1.setValue(m3);
    m1.setRotate(q1);
    m2.setTranslate( tball.getPosition() );
    // Translation x Rotation => first - rotate, then translate
    m1.multLeft( m2 );
    //----------------------------------------------------------------------

    Matrix m50, m6, mobj_tr, mlit;
    Matrix m70, m71, m72, m73, m74, m75, m10, mobj, menv, msphere2;
    Matrix mface[6];

    m6.setTranslate(Vec3f(0,0,cube_size*0.3));   // main camera transformation

    // Faces transformations (to build the box)
    //----------------------------------------------------------------------
    m70.setTranslate(Vec3f( 0,  0,-cube_size));  // back face
    m71.setTranslate(Vec3f( cube_size, 0, 0 ));  // right face
    m72.setTranslate(Vec3f(-cube_size, 0, 0 ));  // left face
    m73.setTranslate(Vec3f( 0, cube_size, 0 ));  // up face
    m74.setTranslate(Vec3f( 0,-cube_size, 0 ));  // bottom face
    m75.setTranslate(Vec3f( 0,  0, cube_size));  // front face

    mface[0] = Matrix::identity();
    mface[0].multLeft(m70);
    mface[1].setTransform(Vec3f(0,0,0), Quaternion( Vec3f(0,1,0), deg2rad(90)));
    mface[1].multLeft(m71);
    mface[2].setTransform(Vec3f(0,0,0), Quaternion( Vec3f(0,1,0), deg2rad(-90)));
    mface[2].multLeft(m72);
    mface[3].setTransform(Vec3f(0,0,0), Quaternion( Vec3f(1,0,0), deg2rad(90)));
    mface[3].multLeft(m73);
    mface[4].setTransform(Vec3f(0,0,0), Quaternion( Vec3f(1,0,0), deg2rad(-90)));
    mface[4].multLeft(m74);
    mface[5] = Matrix::identity();
    mface[5].multLeft(m75);

    for(int i = 0; i < 6; i++)
    {
       // Set transformations for the cube faces
       facetr[i]->getSFMatrix()->setValue( mface[i] );
    }
    //----------------------------------------------------------------------

    // Translation of the whole scene
    menv.setTranslate(Vec3f(0,0,0));
    menv.multLeft(m1);

    // Object's transformation
    //----------------------------------------------------------------------
    //mobj.setTransform(Vec3f(0,0,0), Quaternion( Vec3f(0,1,0), t / 300.f));
    //mobj.setRotate(Quaternion( Vec3f(0,1,0), t / 300.f));
    mobj = Matrix::identity();
    Vec3f put_center = max + min;
      // At first move the object into the center of the scene
      // then rotate around the its' axes if needed.
    mobj_tr.setTranslate(Vec3f(-put_center[0]/2, -put_center[1]/2, -put_center[2]/2));
    mobj.mult(mobj_tr);
    mobj_tr = Matrix::identity();
    mobj_tr.setTranslate(Vec3f(0,0,0));
    //mobj_tr.setTranslate(Vec3f(object_size*1.2*cos(t/1200.f), object_size*0.5*cos(t/400.f), object_size*0.1*sin(t / 1200.f)));
    mobj.multLeft(mobj_tr);
    mobj.multLeft(m1); // without this multiplication, the whole scene rotates,
    //                 // but the reflective object is 'sticked' to the screen!
    //----------------------------------------------------------------------

    // Transformation of the "Environment Mapping camera"
    //----------------------------------------------------------------------
    m50 = mobj_tr;
    //----------------------------------------------------------------------

    // Light transformations
    //----------------------------------------------------------------------
    //mlit.setTransform(Vec3f(cube_size*0.8*cos(t/1000.f), cube_size*0.08*cos(t/200.f), 0.8*sin(t / 1000.f)));
    Matrix mlitt;
    mlit.setTranslate(Vec3f(cube_size*0.8,cube_size*0.8,cube_size*0.8));
    Pnt3f lightpos = ple->getPosition();
    Vec3f ttt;
    mlitt = mlit;
    mlitt.mult(lightpos, ttt);  // ttt -> Light source position for CubeMap
    //----------------------------------------------------------------------

    // Second sphere (environment sphere) transformations
    //----------------------------------------------------------------------
    msphere2.setTranslate(Vec3f(-cube_size*0.8,cube_size*0.8,-cube_size*0.8));

    //----------------------------------------------------------------------

    cam_trans->getSFMatrix()->setValue( m6);   // Main camera transformation
    camgf_tr->getSFMatrix()->setValue( m50);   // EnvMap camera transformation

    envtr->getSFMatrix()->setValue( menv);     // Scene transformation
    objtr->getSFMatrix()->setValue( mobj );    // Object transformation
    sphere2tr->getSFMatrix()->setValue( msphere2 ); // Sphere2 transformation
    light_tr->getSFMatrix()->setValue( mlit ); // Light transformation


    // Calculation of the viewing position
    //----------------------------------------------------------------------
    Matrix view;
    cam->getViewing(view, (*vp2).getPixelWidth(), (*vp2).getPixelHeight());

    // Set parameters to the Vertex program
    //*********************************************************************
    // For some reasons Vertex Program considers all parameters which
    // are transmited from the main program inside the coordinate system of
    // the object (to which this VP is attached). That is why if the object
    // is moving along some trajectory and the light source is just
    // set in the (0,0,0) position, VP see this light source always inside
    // the object! So the light source by the opinion of the VP is follow
    // the object what is not true.
    //
    // To avoid this we decided to find the light source and viewer positions
    // in the coordinate system of the object and then pass these values
    // to the VP. For this reason here are used ttt[i]-mobj[3][i] and
    // view[3][i]+mobj[3][i] (reverse coordinate system for viewer) instead
    // of simple ttt[i] and view[3][i]
    //----------------------------------------------------------------------
    litPos[0] = ttt[0]-mobj[3][0];
    litPos[1] = ttt[1]-mobj[3][1];
    litPos[2] = ttt[2]-mobj[3][2];

    eyePos[0] = -(view[3][0]+mobj[3][0]);
    eyePos[1] = -(view[3][1]+mobj[3][1]);
    eyePos[2] = -(view[3][2]+mobj[3][2]);

    // Push the LightSource position and Viewer position to the CubeMapMaterial
    beginEditCP(cubemat);
       cubemat->setLightPos(litPos);
       cubemat->setViewPos(eyePos);
    endEditCP(cubemat);

    win->render(renact);
}
//==========================================================================

// React to size changes
//==========================================================================
void reshape(int w, int h)
{
    win->resize( w, h );
    Real32 tw = osgnextpower2(w), th = osgnextpower2(h);

    glutPostRedisplay();
}
//==========================================================================

// React to mouse button presses
//==========================================================================
void mouse(int button, int state, int x, int y)
{
    if ( state == 0 )
    {
       switch ( button )
       {
          case GLUT_LEFT_BUTTON:  break;
          case GLUT_MIDDLE_BUTTON:tball.setAutoPosition(true);
                                  break;
          case GLUT_RIGHT_BUTTON: tball.setAutoPositionNeg(true);
                                  break;
       }
       mouseb |= 1 << button;
    }
    else if ( state == 1 )
    {
       switch ( button )
       {
          case GLUT_LEFT_BUTTON:  break;
          case GLUT_MIDDLE_BUTTON:tball.setAutoPosition(false);
                                  break;
          case GLUT_RIGHT_BUTTON: tball.setAutoPositionNeg(false);
                                  break;
       }
       mouseb &= ~(1 << button);
    }
    lastx = x;
    lasty = y;

    glutPostRedisplay();
}
//==========================================================================

// React to mouse motions with pressed buttons
//==========================================================================
void motion(int x, int y)
{
    Real32 w = win->getWidth(), h = win->getHeight();

    Real32  a = -2. * ( lastx / w - .5 ),  b = -2. * ( .5 - lasty / h ),
            c = -2. * ( x / w - .5 ),      d = -2. * ( .5 - y / h );

    if ( mouseb & ( 1 << GLUT_LEFT_BUTTON ) )
    {
        tball.updateRotation( a, b, c, d );
    }
    else if ( mouseb & ( 1 << GLUT_MIDDLE_BUTTON ) )
    {
        tball.updatePosition( a, b, c, d );
    }
    else if ( mouseb & ( 1 << GLUT_RIGHT_BUTTON ) )
    {
        tball.updatePositionNeg( a, b, c, d );
    }
    lastx = x;
    lasty = y;
    glutPostRedisplay();
}
//==========================================================================

// React to keys
//==========================================================================
void keyboard(unsigned char k, int, int)
{
    switch(k)
    {
       case 27: osgExit();  exit(1);

       // Increase Fresnel parameter
       case '+':
         if( Fresnel[0] < 1.0 ) Fresnel[0] += 0.01;
         if( Fresnel[1] < 1.0 ) Fresnel[1] += 0.01;
         if( Fresnel[2] < 1.0 ) Fresnel[2] += 0.01;

         std::cout << "Fresnel: " << Fresnel << "\n";

         beginEditCP(cubemat);
            cubemat->setFresnel(Fresnel);
         endEditCP(cubemat);
       break;

       // Decrease Fresnel parameter
       case '-':
         if( Fresnel[0] > 0.0 ) Fresnel[0] -= 0.01;
         if( Fresnel[1] > 0.0 ) Fresnel[1] -= 0.01;
         if( Fresnel[2] > 0.0 ) Fresnel[2] -= 0.01;

         std::cout << "Fresnel: " << Fresnel << "\n";

         beginEditCP(cubemat);
            cubemat->setFresnel(Fresnel);
         endEditCP(cubemat);
       break;
    }
}
//==========================================================================

// Main...
//==========================================================================
int main(int argc, char **argv)
{
    osgInit(argc,argv);

    // GLUT init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    int id=glutCreateWindow("OpenSG");

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    GLint glvp[4];
    glGetIntegerv( GL_VIEWPORT, glvp );

    GLUTWindowPtr gwin=GLUTWindow::create();
    gwin->setId(id);
    gwin->setSize( glvp[2], glvp[3] );

    win = gwin;

    // Draw the reflective object
    //--------------------------------------------------------------------------
    NodePtr sceneLoad;
    if( argc < 2)
    {
       std::cout << "\n====================================================================\n";
       std::cout << " Usage:\n";
       std::cout << "   testReflectiveMaterial <back> <right> <left> <top> <bottom>\n";
       std::cout << "                     <front> <object.off,obj> <texture_sphere>\n";
       std::cout << "                     <texture_object>\n";
       std::cout << "\n";
       std::cout << "   instead of <object.off,obj> you can use sphere, torus, plane\n\n";
       std::cout << " Keys:\n";
       std::cout << "   '+' - increase Fresnel term by 0.01 (for each term of RGB)\n";
       std::cout << "   '-' - decrease Fresnel term by 0.01 (for each term of RGB)\n";
       std::cout << "   Esc - exit\n\n";
       std::cout << " Load images for the Environment Cube in the following consequence:\n";
       std::cout << "   c_negz.jpg c_posx.jpg c_negx.jpg c_posy.jpg c_negy.jpg c_posz.jpg\n";
       std::cout << "=====================================================================\n\n";

    }

    if( (argc > 7 && strcmp("torus", argv[7])) && (strcmp("plane", argv[7]) &&strcmp("sphere", argv[7])))
    {
	sceneLoad = SceneFileHandler::the().read(argv[7], 0);
    }
    else
    {
        if(argc > 7)
	{
	   if(!strcmp("torus", argv[7]))
	     sceneLoad = makeTorus(.9, 2, 32, 32);
           else if(!strcmp("plane", argv[7]))
	           sceneLoad = makePlane(10.0, 10.0, 16, 16);
                else
	           sceneLoad = makeSphere(3, 6);
	}
	else sceneLoad = makeSphere(3, 6);
    }

    GeometryPtr geo = GeometryPtr::dcast(sceneLoad->getCore());

    NodePtr cube = Node::create();
    beginEditCP(cube);
      cube->setCore(geo);
    endEditCP(cube);

    // Get the size of the reflective object (the size of the whole
    // scene is defined according to this size).
    //--------------------------------------------------------------------------
    cube->updateVolume();
    cube->getVolume().getBounds( min, max );
    object_size = (max - min).length();
    float halfBBoxObj = object_size/2;

    // Some preliminaries for images
    //----------------------------------------------------------------------
    ImagePtr imfg[6];
    UChar8 data[] = {  255, 0xff, 0xff,  0xff, 0xff, 0xff,
                       0xff, 0xff, 0xff,  0xff, 0xff, 0xff };

    for(int i = 0; i < 6; i++)
    {
       // Load images for the Environment Cube in the following
       // consequence:
       //   negZ.jpg posX.jpg negX.jpg posY.jpg negY.jpg posZ.jpg
       imfg[i] = Image::create();
       if(argc > (i+1))
           imfg[i]->read(argv[i+1]);
       else
       {
          beginEditCP(imfg[i]);
            imfg[i]->set( Image::OSG_RGB_PF, 2, 2, 1, 1, 1, 0, data );
          endEditCP(imfg[i]);
       }
    }
    ImagePtr sphimg;
    sphimg = Image::create();

    if(argc > 8) sphimg->read(argv[8]);
    else
    {
       beginEditCP(sphimg);
         sphimg->set( Image::OSG_RGB_PF, 2, 2, 1, 1, 1, 0, data );
       endEditCP(sphimg);
    }
    //----------------------------------------------------------------------

    cam_beacon = Node::create();         // Main camera
    camgf = PerspectiveCamera::create(); // Camera for CubeMapping
    camgf_tr = Transform::create();      // Transform for CubeMapping Camera
    envnode = Node::create();            // Environment scene

    // Draw the Environment scene (Cube with the additional sphere)
    draw_env_scene(imfg, sphimg);

    // Set camera for mapping the Environment (the scene) into the
    // CubeMap texture
    //--------------------------------------------------------------------------
    beginEditCP(cam_beacon);
       cam_beacon->setCore( camgf_tr );
    endEditCP(cam_beacon);

    camgf->setBeacon( cam_beacon ); //88
    camgf->setFov( deg2rad( 90 ) );
    camgf->setNear( 0.1 );
    camgf->setFar( 10000 );
    camgf->setAspect(1);

    // Set parameters for the refflective material
    //----------------------------------------------------------------------
    cubemat = ReflectiveMaterial::create();

    ImagePtr img_dif = Image::create();
    if(argc > 9) img_dif->read(argv[9]);
    else
    {
       beginEditCP(img_dif);
          img_dif->set( Image::OSG_RGB_PF, 2, 2, 1, 1, 1, 0, data );
       endEditCP(img_dif);
    }

    beginEditCP(cubemat);
       //cubemat->setDiffuse(Color3f(1.0,0.3,0.3));
       cubemat->setDiffuse(Color3f(1.0,1.0,1.0));
       cubemat->setImageDiffuse(img_dif);
       cubemat->setFresnel(Fresnel);
       cubemat->setImageNegZ(imfg[0]);
       cubemat->setImagePosX(imfg[1]);
       cubemat->setImageNegX(imfg[2]);
       cubemat->setImagePosY(imfg[3]);
       cubemat->setImageNegY(imfg[4]);
       cubemat->setImagePosZ(imfg[5]);
       cubemat->setTransparency(transparency);
       cubemat->setLightPos(litPos);
       cubemat->setViewPos(eyePos);
       cubemat->setCompTexCoord(true);
       cubemat->setHalfBBoxObj(halfBBoxObj);
    endEditCP(cubemat);

    // This is very important!!! It grabs the scene into the texture
    beginEditCP(cubemat);
      cubemat->MakeCubeMap(camgf, win, envnode);
    endEditCP(cubemat);

    beginEditCP( geo, Geometry::MaterialFieldMask | Geometry::DlistCacheFieldMask );
       geo->setMaterial( cubemat );
       geo->setDlistCache( false );
    endEditCP( geo, Geometry::MaterialFieldMask | Geometry::DlistCacheFieldMask );

    // Make beacon for the main camera
    //--------------------------------------------------------------------------
    b1n = Node::create();
    GroupPtr b1 = Group::create();
    beginEditCP(b1n);
       b1n->setCore( b1 );
    endEditCP(b1n);

    t1n = Node::create();
    cam_trans = Transform::create();
    beginEditCP(t1n);
       t1n->setCore( cam_trans );
       t1n->addChild( b1n );
    endEditCP(t1n);

    objtr = Transform::create();
    NodePtr trobj = Node::create();
    beginEditCP(trobj);
       trobj->setCore(objtr);
       trobj->addChild(cube);
    endEditCP(trobj);

    // Build the main node of the scene and attach all the children to it
    //--------------------------------------------------------------------------
    NodePtr trenv = Node::create();
    GroupPtr trenv_core = Group::create();

    beginEditCP(trenv);
       trenv->setCore(trenv_core);
       trenv->addChild(envnode);
       trenv->addChild(trobj);
       trenv->addChild(t1n);
       trenv->addChild(delight);
       trenv->addChild(cam_beacon);
    endEditCP(trenv);


    // Settings for the main camera and viewport
    //--------------------------------------------------------------------------
    cam = PerspectiveCamera::create();
    cam->setBeacon( b1n );
    cam->setFov( deg2rad( 60.f)); //90 ) );
    cam->setNear( 0.1 );
    cam->setFar( 10000 );

    SolidBackgroundPtr bkgnd0 = SolidBackground::create();

    vp2 = Viewport::create();
    beginEditCP(vp2);
       vp2->setCamera(cam);
       vp2->setRoot(trenv);
       vp2->setBackground(bkgnd0);
       vp2->setLeft(0);
       vp2->setRight(1);
       vp2->setBottom(0);
       vp2->setTop(1);
    endEditCP(vp2);

    beginEditCP(win);
      win->addPort(vp2);
    endEditCP(win);

    win->init();

    renact = RenderAction::create();

    // Settings for trackball
    //--------------------------------------------------------------------------
    //Vec3f postr( 0, 0, max[2] + ( max[2] - min[2] ) * 1.1 );
    Vec3f postr( 0, 0, 0); //max[2] + ( max[2] - min[2] ) * 0.8 );

    //tball.setMode( Trackball::OSGCamera);
    tball.setMode( Trackball::OSGObject );
    tball.setStartPosition( postr, true );
    tball.setSum( true );
    tball.setTranslationMode( Trackball::OSGFree );

    // GLUT main loop
    glutMainLoop();

    return 0;
}
