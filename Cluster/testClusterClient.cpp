#include <OSGConfig.h>

#ifdef OSG_STREAM_IN_STD_NAMESPACE
#include <iostream>
#else
#include <iostream.h>
#endif
#include <vector>

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
#include <OSGComponentTransform.h>
#include <OSGGeoFunctions.h>

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
ComponentTransformPtr      trans;
PerspectiveCameraPtr  cam;
Connection           *connection;
int                   servers;
RemoteAspect          aspect;

OSG::Action::ResultE calcVNormal( OSG::CNodePtr &, OSG::Action * action )
{
	OSG::NodePtr node = action->getActNode();
	OSG::GeometryPtr g = OSG::GeometryPtr::dcast( node->getCore() );

	if ( g->getNormals() == OSG::NullFC )
	{
		OSG::calcVertexNormals( g );
	}	
	
	return OSG::Action::Continue;
}

void createSceneGraph(int argc,char **argv)
{
    int width=servers*500;
    int height=500;
    int i;
    char *filename;
    GLUTWindowPtr window;
    ClusterWindowAttPtr pWindowAtt;
    TileCameraDecoratorPtr deco;
    NodePtr transNode;
    
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
        if(strlen(argv[i])>2 && strncmp(argv[i],"-w",2) == 0)
        {
            width=atoi(&argv[i][2]);
        }
        if(strlen(argv[i])>2 && strncmp(argv[i],"-h",2) == 0)
        {
            height=atoi(&argv[i][2]);
        }
        if(strlen(argv[i])>2 &&
           strncmp(argv[i],"-f",2) == 0)
        {
            filename=&argv[i][2];
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
    trans = ComponentTransform::create();
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
	cam->setNear( 5 );
	cam->setFar( 10000 );
	endEditCP(cam);

	// Background
	SolidBackgroundPtr bkgnd = SolidBackground::create();
	beginEditCP(bkgnd);
	bkgnd->setColor( Color3f( 0,0,1 ) );
	endEditCP(bkgnd);

    // Viewport

    // one window for each server
    for(i=0;i<(int)servers;i++)
    {
        deco = TileCameraDecorator::create();
        beginEditCP(deco);
        deco->setCamera( cam );
        deco->setSize( 1.0/servers * i,
                       0.0, 
                       1.0/servers * (i + 1),
                       1.0);
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
        window->setSize(width/servers,height);
        endEditCP(window);

    }

    // move geometry in to viewfrustum
	Vec3f pos( 0, 0, max[2] + ( max[2] - min[2] ) * 2 );

	beginEditCP(cam_trans);
	cam_trans->getSFMatrix()->getValue().setTransform( pos );
	endEditCP(cam_trans);
}

void renderLoop()
{
    Quaternion           rot;
    static float         rad=0;

    while(1)
    {
        beginEditCP(trans);
        trans->getSFRotation()->setValue(rot);
        endEditCP(trans);
        rot=Quaternion(Vec3f(0,1,.3),rad);
        rad+=.2;
        // send syncronisation
        aspect.sendSync(*connection,OSG::Thread::getCurrentChangeList());
        OSG::Thread::getCurrentChangeList()->clearAll();
        // synchronize swap
        connection->signal();
    }
}

int main( int argc, char **argv )
{
    int connType=0;
    int arg;

 	// OSG init
    osgInit(argc, argv);
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
            cout << argv[0] << " [-ffile] [-wwidth] [-hheight] server1 server2 ... serverN" << endl;
            exit(0);
        }
        createSceneGraph(argc,argv);
        renderLoop();
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


