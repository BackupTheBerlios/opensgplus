#include <OSGConfig.h>

#ifdef OSG_STREAM_IN_STD_NAMESPACE
#include <iostream>
#else
#include <iostream.h>
#endif
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
#include <OSGRenderAction.h>
#include <OSGSceneFileHandler.h>
#include <OSGDirectionalLight.h>
#include <OSGSimpleGeometry.h>

#include "OSGViewport.h"
#include "OSGCamera.h"
#include "OSGTileCameraDecorator.h"
#include "OSGWindow.h"
#include "OSGGLUTWindow.h"
#include "OSGCamera.h"
#include "OSGRemoteAspect.h"
#include "OSGClusterException.h"
#include "OSGStreamSocket.h"
#include "OSGSelection.h"
#include "OSGStreamSockConnection.h"
#include "OSGMulticastConnection.h"
#include "OSGVRMLWriteAction.h"
#include "OSGClusterWindowAtt.h"
#include "OSGViewBufferHandler.h"

using namespace OSG;

Connection         *connection;
UInt32              serverNr=999999;
RenderAction	   *ract;
WindowPtr           osgWin;
RemoteAspect        aspect;
int                 winid;
ViewBufferHandler   bufferHandler;
ClusterWindowAttPtr windowAtt;

Bool WindowDestroyedFunction(FieldContainerPtr& fcp,
                             RemoteAspect * /*aspect*/)
{
    GLUTWindowPtr window=GLUTWindowPtr::dcast(fcp);
    ClusterWindowAttPtr att;

    // try to find the cluster attachment
    att=ClusterWindowAttPtr::dcast(
        window->findAttachment(ClusterWindowAtt::getClassType()
                               .getGroupId())
        );
    if(att == OSG::NullFC)
        return true;
    // do i have to handle this window ?
    if(att->getServerId() != serverNr)
        return true;
    if(osgWin==NullFC)
        return true;
    osgWin=NullFC;
    glutHideWindow();
    return true;
}

Bool WindowChangedFunction(FieldContainerPtr& fcp,
                           RemoteAspect * /*aspect*/)
{
    GLUTWindowPtr window=GLUTWindowPtr::dcast(fcp);
    ClusterWindowAttPtr att;

    // try to find the cluster attachment
    att=ClusterWindowAttPtr::dcast(
        window->findAttachment(ClusterWindowAtt::getClassType()
                               .getGroupId())
        );
    if(att == OSG::NullFC)
        return true;
    // do i have to handle this window ?
    if(att->getServerId() != serverNr)
        return true;
    if(osgWin == NullFC)
    {
        glEnable( GL_LIGHTING );
        glEnable( GL_LIGHT0 );
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_NORMALIZE );
        window->setWinID(winid);
        osgWin = window;
        windowAtt = att;
    }
    if(windowAtt->getComposite())
    {
        cout << osgWin->getWidth() << " " << osgWin->getHeight() << endl;
        osgWin->setSize(osgWin->getWidth(),osgWin->getHeight());
        glutReshapeWindow(osgWin->getWidth(),osgWin->getHeight());
    }
    glutShowWindow();
    return true;
}

void display()
{
    try
    {
        // receive syncronisation
        aspect.receiveSync(*connection);
        if(osgWin!=NullFC)
        {
            osgWin->frameInit();	            // frame-cleanup
            osgWin->renderAllViewports( ract );	// draw the viewports     
        }
        if(osgWin!=NullFC)
        {
            if(windowAtt->getComposite())
            {
                TileCameraDecoratorPtr tile;
                for(UInt32 i=0;i<osgWin->getPort().size();i++)
                {
                    tile=TileCameraDecoratorPtr::dcast(
                        osgWin->getPort()[i]->getCamera());
                    if(tile!=NullFC)
                    {
                        bufferHandler.send(
                            *connection,
                            ViewBufferHandler::RGB,
                            tile->getLeft()   * tile->getFullWidth(),
                            tile->getBottom() * tile->getFullHeight());
/*
                        cout << endl;
                        composition.printStatistics();
                        cout << endl;
*/
                    }
                }
//                osgWin->swap(); 
            }
            else
            {
                connection->wait();
                osgWin->swap(); 
                osgWin->frameExit();	            // frame-cleanup
            }
        }
    }
    catch(exception &e)
    {
        cout << e.what() << endl;
        exit(1);
    }
    catch(...)
    {
        cout << "Unknown exception" << endl;
        exit(1);
    }
}



Bool NodeC(FieldContainerPtr& fcp,
           RemoteAspect * /*aspect*/)
{
    NodePtr node=NodePtr::dcast(fcp);
    node->getSFVolume()->getValue().dump();
    return true;
}

int main( int argc, char **argv )
{
    char  *address=NULL;
    int    connType=0;
    int    arg;

 	// OSG init
    osgInit(argc, argv);
    // GLUT init
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	winid = glutCreateWindow("OpenSG Cluster Client");
    glutHideWindow();
    glutFullScreen();
	glutDisplayFunc(display);       
	glutIdleFunc(display);       
    ract = RenderAction::create();
    // init clustering
    for(arg=1;arg<argc;arg++)
    {
        if(strcmp(argv[arg],"-m")==0)
        {
            connType=1;
            continue;
        }
        if(argv[arg][0]=='-' && argv[arg][1]=='s')
        {
            serverNr=atoi(&argv[arg][2]);
            continue;
        }
        address=argv[arg];
    }
    if(serverNr==999999)
    {
        printf("Option -s missing. Unknown server number. E.g. -s0\n"); 
        exit(1);
    }
    switch(connType)
    {
        case 0:
            if(address==NULL)
                address="7878";
            connection=new StreamSockConnection();
            printf("Using stream connection. Waiting on port %s\n",address);
            break;
        case 1:
            if(address==NULL)
                address="7878";
            connection=new MulticastConnection();
            printf("Using multicast connection. Waiting on port %s\n",address);
            break;
    }
    try
    {
        connection->accept(address);
        aspect.registerChanged(Node::getClassType(),
                               osgFunctionFunctor2(&NodeC));

        aspect.registerChanged(GLUTWindow::getClassType(),
                               osgFunctionFunctor2(&WindowChangedFunction));
        aspect.registerDestroyed(GLUTWindow::getClassType(),
                                 osgFunctionFunctor2(&WindowDestroyedFunction));
    }
    catch(exception &e)
    {
        cout << e.what() << endl;
    }
    catch(...)
    {
        cout << "Unknown exception" << endl;
    }
    glutMainLoop();
    return 0;
}
