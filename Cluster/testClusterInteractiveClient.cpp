#include <OSGConfig.h>

#ifdef OSG_STREAM_IN_STD_NAMESPACE
#include <iostream>
#else
#include <iostream.h>
#endif
#include <vector>
#include <GL/glut.h>

#include <OSGFieldContainerFactory.h>
#include <OSGSFSysTypes.h>
#include <OSGVector.h>
#include <OSGQuaternion.h>
#include <OSGMatrix.h>
#include <OSGMatrixUtility.h>
#include <OSGBoxVolume.h>
#include <OSGNode.h>
#include <OSGGroup.h>
#include <OSGTransform.h>
#include <OSGAttachment.h>
#include <OSGMFVecTypes.h>
#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGSceneFileHandler.h>
#include <OSGDirectionalLight.h>
#include <OSGSimpleGeometry.h>
#include <OSGVRMLTransform.h>
#include <OSGGeoFunctions.h>
#include <OSGVolumeDraw.h>
#include <OSGGLUTWindow.h>

#include "OSGVRMLFile.h"
#include "OSGGLUTWindow.h"
#include "OSGViewport.h"
#include "OSGCamera.h"
#include "OSGTileCameraDecorator.h"
#include "OSGWindow.h"
#include "OSGCamera.h"
#include "OSGPerspectiveCamera.h"
#include "OSGSolidBackground.h"
#include "OSGGradientBackground.h"
#include "OSGTrackball.h"
#include "OSGRemoteAspect.h"
#include "OSGStreamSocket.h"
#include "OSGStreamSockConnection.h"
#include "OSGMulticastConnection.h"
#include "OSGClusterWindowAtt.h"

using namespace OSG;
using namespace std;

NodePtr		          root;
NodePtr		          file;
ViewportPtr           vp;
TransformPtr          cam_trans;
VRMLTransformPtr      trans;
PerspectiveCameraPtr  cam;
VRMLFile             *pLoader = NULL;
Connection           *connection;
int                   servers;
RemoteAspect          aspect;

// **************
// GLUT variables
// **************

Trackball tball;

int mouseb = 0;
int lastx=0, lasty=0;
int winwidth=0, winheight=0;

DrawAction * ract;
WindowPtr win;
int winid;

// Program

Action::ResultE calcVNormal( CNodePtr &, Action * action )
{
	NodePtr node = action->getActNode();
	GeometryPtr g = GeometryPtr::dcast( node->getCore() );

	if ( g->getNormals() == NullFC )
	{
		calcVertexNormals( g );
	}	
	
	return Action::Continue;
}

void createSceneGraph(int argc,char **argv)
{
    int width=servers*500;
    int height=500;
    int nhserv=servers;
    int nvserv=1;
    int i;
    char *filename;
    GLUTWindowPtr window;
    ClusterWindowAttPtr pWindowAtt;
    TileCameraDecoratorPtr deco;
    NodePtr transNode;
    Color3f bkgndcolor(0,0,1);
    
	// create the graph
	// beacon for camera and light	
    NodePtr b1n = Node::create();
    GroupPtr b1 = Group::create();
	beginEditCP(b1n);
	b1n->setCore( b1 );
	endEditCP(b1n);
	// transformation
    NodePtr t1n = Node::create();
    TransformPtr t1 = Transform::create();
	beginEditCP(t1n);
	t1n->setCore( t1 );
	t1n->addChild( b1n );
	endEditCP(t1n);
	cam_trans = t1;
	// light
	NodePtr dlight = Node::create();
	DirectionalLightPtr dl = DirectionalLight::create();
	beginEditCP(dlight);
	dlight->setCore( dl );
	endEditCP(dlight);
	beginEditCP(dl);
	dl->setAmbient( .3, .3, .3, 1 );
	dl->setDiffuse( 1, 1, 1, 1 );
	dl->setDirection(0,0,1);
	dl->setBeacon( b1n);
	endEditCP(dl);
	// root
    root = Node::create();
    GroupPtr gr1 = Group::create();
	beginEditCP(root);
	root->setCore( gr1 );
	root->addChild( t1n );
	root->addChild( dlight );
	endEditCP(root);
	// Load the file
	NodePtr file = NullFC;

    for(i=1;i<argc;i++)
    {
        if(strlen(argv[i])>2 && strncmp(argv[i],"-hs",3) == 0)
        {
            nhserv=atoi(&argv[i][3]);
        }
        else if(strlen(argv[i])>2 && strncmp(argv[i],"-vs",3) == 0)
        {
            nvserv=atoi(&argv[i][3]);
        }
        else if(strlen(argv[i])>2 && strncmp(argv[i],"-w",2) == 0)
        {
            width=atoi(&argv[i][2]);
        }
        else if(strlen(argv[i])>2 && strncmp(argv[i],"-h",2) == 0)
        {
            height=atoi(&argv[i][2]);
        }
        else if(strlen(argv[i])>2 && strncmp(argv[i],"-b",2) == 0)
        {
			if ( sscanf( &argv[i][2], "%f,%f,%f", 
					&bkgndcolor[0], &bkgndcolor[1], &bkgndcolor[2] ) != 3 )
			{
				printf("Backgroundcolor not understood, ignoring.\n");
				bkgndcolor.setValuesRGB(0,0,1);
			}
        }
        else if(strlen(argv[i])>2 &&
           strncmp(argv[i],"-f",2) == 0)
        {
            filename=&argv[i][2];
            if(strcmp(filename+strlen(filename)-4,".wrl")==0)
            {
                if(pLoader==NULL)
                {
                    pLoader = new VRMLFile();
                    pLoader->scanStandardPrototypes("std.wrl", 0);
                }
                pLoader->scanFile(filename, 0);
                Action *act = Action::create();
                file = pLoader->getRoot();
                act->registerEnterFunction( Geometry::getClassType(),
                                            osgFunctionFunctor2( calcVNormal ) );
                act->apply( file );
            }
            else
                file = SceneFileHandler::the().read(filename,0);
        }
    }
	if ( file == NullFC )
	{
		cerr << "Couldn't load file, ignoring" << endl;
		file = makeTorus( .5, 2, 16, 16 );
        beginEditCP(file->getCore());
        endEditCP(file->getCore());
	}
	file->updateVolume();

	Vec3f min,max;
	file->getVolume().getBounds( min, max );
	
	cout << "Volume: from " << min << " to " << max << endl;

    transNode = Node::create();
    trans = VRMLTransform::create();
	beginEditCP(transNode);
	transNode->setCore( trans );
	transNode->addChild( file );
	endEditCP(transNode);

	beginEditCP(dlight);
	dlight->addChild( transNode );
	endEditCP(dlight);

	cerr << "Tree: " << endl;

	// Camera
	PerspectiveCameraPtr cam = PerspectiveCamera::create();
	beginEditCP(cam);
	cam->setBeacon( b1n );
	cam->setFov( 60 );
	cam->setNear( (max-min).length() / 1000.f );
	cam->setFar( (max-min).length() * 2.f );
	endEditCP(cam);

	// Background
	SolidBackgroundPtr bkgnd = SolidBackground::create();
	beginEditCP(bkgnd);
	bkgnd->setColor( bkgndcolor );
	endEditCP(bkgnd);

    // Viewport

    // one window for each server
    for(i=0;i<(int)servers;i++)
    {
        deco = TileCameraDecorator::create();
        beginEditCP(deco);
        deco->setCamera( cam );
        deco->setSize( 1.0/nhserv * (i % nhserv),
                       1.0/nvserv * int(i / nhserv),
                       1.0/nhserv * ( (i % nhserv) + 1),
                       1.0/nvserv * ( int(i / nhserv) + 1) );
        deco->setFullWidth(width);
        deco->setFullHeight(height);
        endEditCP(deco);

        vp = Viewport::create();
        beginEditCP(vp);
        vp->setCamera( deco );
        vp->setBackground( bkgnd );
        vp->setRoot( root );
        vp->setSize( 0,0, 1,1 );
        endEditCP(vp);

        // Server information
        pWindowAtt = ClusterWindowAtt::create();
        beginEditCP(pWindowAtt);
        {
            pWindowAtt->setServerId(i);
            pWindowAtt->setComposite(false);
        }
        endEditCP(pWindowAtt);

        window = GLUTWindow::create();
        beginEditCP(window);
        window->addAttachment(pWindowAtt);
        window->addPort( vp );
        window->setSize(width/nhserv,height/nvserv);
        endEditCP(window);
    }

    // move geometry in to viewfrustum
	Vec3f pos( 0, 0, max[2] + ( max[2] - min[2] ) * 2 );

	beginEditCP(cam_trans);
	cam_trans->getSFMatrix()->getValue().setTransform( pos );
	endEditCP(cam_trans);
	
	// the interaction window
	
	// Trackball
	
	tball.setMode( Trackball::OSGObject );
	tball.setStartPosition( pos, true );
	tball.setSum( true );
	tball.setTranslationMode( Trackball::OSGFree );
    tball.setTranslationScale(10000.);
	
	// Viewport

	vp = Viewport::create();
	vp->setCamera( cam );
	vp->setBackground( bkgnd );
	vp->setRoot( root );
	vp->setSize( 0,0, 1,1 );

	// Window
	
	GLUTWindowPtr gwin;

	GLint glvp[4];
	glGetIntegerv( GL_VIEWPORT, glvp );

	gwin = GLUTWindow::create();
	gwin->setWinID(winid);
	gwin->setSize( glvp[2], glvp[3] );

	win = gwin;

	win->addPort( vp );

	win->init();

}

// ***********************************
// GLUT callback functions & variables
// ***********************************

void 
display(void)
{
	Matrix m1, m2, m3;
    Quaternion q1;

    tball.getRotation().getValue(m3);

    q1.setValue(m3);

    m1.setRotate(q1);

	m2.setTranslate( tball.getPosition() );

	m1.mult( m2 );
	beginEditCP( cam_trans );
	cam_trans->getSFMatrix()->setValue( m1 );
	endEditCP( cam_trans );

	win->draw( ract );	

    try
    {
        // send syncronisation
        aspect.sendSync(*connection,Thread::getCurrentChangeList());
        Thread::getCurrentChangeList()->clearAll();
        // sync swap
        connection->signal();
	}
    catch(exception &e)
    {
        cout << e.what() << endl;
        exit(0);
    }
    catch(...)
    {
        cout << "unknown exception" << endl;
        exit(0);
    }
}

void reshape( int w, int h )
{
	cerr << "Reshape: " << w << "," << h << endl;
	win->resize( w, h );
	
	glutPostRedisplay();
}


// tball stuff

void
motion(int x, int y)
{	
	Real32 w = win->getWidth(), h = win->getHeight();
	

	Real32	a = -2. * ( lastx / w - .5 ),
				b = -2. * ( .5 - lasty / h ),
				c = -2. * ( x / w - .5 ),
				d = -2. * ( .5 - y / h );

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

void
mouse(int button, int state, int x, int y)
{
	if ( state == 0 )
	{
		switch ( button )
		{
		case GLUT_LEFT_BUTTON:	break;
		case GLUT_MIDDLE_BUTTON:tball.setAutoPosition(true);
								break;
		case GLUT_RIGHT_BUTTON:	tball.setAutoPositionNeg(true);
								break;
		}
		mouseb |= 1 << button;
	}
	else if ( state == 1 )
	{
		switch ( button )
		{
		case GLUT_LEFT_BUTTON:	break;
		case GLUT_MIDDLE_BUTTON:tball.setAutoPosition(false);
								break;
		case GLUT_RIGHT_BUTTON:	tball.setAutoPositionNeg(false);
								break;
		}		
		mouseb &= ~(1 << button);
	}
	lastx = x;
	lasty = y;
	
	glutPostRedisplay();
}


void key(unsigned char key, int x, int y)
{
    MulticastConnection *mc;
	switch ( key )
	{
        case 's':
            mc=dynamic_cast<MulticastConnection*>(connection);
            if(mc)
            {
                mc->printStatistics();
                mc->clearStatistics();
                fflush(stdout);
            }
            break;
        case 27:	// should kill the clients here
            osgExit(); 
            exit(0);
	}
	
	glutPostRedisplay();
}


Action::ResultE wireDraw( CNodePtr &, Action * action )
{
	NodePtr node = action->getActNode();
	
	node->updateVolume();

	Bool l = glIsEnabled( GL_LIGHTING );
	glDisable( GL_LIGHTING );

	glColor3f( .8,.8,.8 );

	drawVolume( node->getVolume() );

	if ( l )
		glEnable( GL_LIGHTING );
	
	return Action::Continue;
}

Action::ResultE ignore( CNodePtr &, Action * action )
{	
	return Action::Continue;
}

int main( int argc, char **argv )
{
    int connType=0;
    int arg;

 	// OSG init
    osgInit(argc, argv);

	// create the GLUT window for interaction
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	winid = glutCreateWindow("OpenSG Cluster Client");
	glutKeyboardFunc(key);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);       
	glutMouseFunc(mouse);   
	glutMotionFunc(motion); 

	ract = DrawAction::create();
	// just draw the group's volumes as wireframe, ignore geometries
	ract->registerEnterFunction( Group::getClassType(),
									osgFunctionFunctor2( wireDraw ) );
	ract->registerEnterFunction( Geometry::getClassType(),
									osgFunctionFunctor2( ignore ) );

    // clear changelist from prototypes
    OSG::Thread::getCurrentChangeList()->clearAll();

    try
    {
        for(arg=1;arg<argc;arg++)
        {
            if(strcmp(argv[arg],"-m")==0)
                connType=1;
        }
        switch(connType)
        {
            case 0:
                connection=new StreamSockConnection();
                break;
            case 1:
                connection=new MulticastConnection();
                break;
        }
        servers=0;
        for(arg=1;arg<argc;arg++)
        {
            if(argv[arg][0] == '-')
                continue;
            cout << "Connect to :" << argv[arg] << endl;
            connection->connect(argv[arg]);
            servers++;
        }
        if(servers==0)
        {
            cout << argv[0] << " [-m] [-ffile] [-wwidth] [-hheight] server1 server2 ... serverN" << endl;
            exit(0);
        }
        createSceneGraph(argc,argv);
        glutMainLoop();
	}
    catch(exception &e)
    {
        cout << e.what() << endl;
    }
    catch(...)
    {
        cout << "unknown exception" << endl;
    }
	return 0;
}