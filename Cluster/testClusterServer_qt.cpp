#include <OSGConfig.h>

#ifdef OSG_STREAM_IN_STD_NAMESPACE
#include <iostream>
#else
#include <iostream.h>
#endif

#ifdef __sgi
#pragma set woff 1375
#endif

#include <qapplication.h>

#ifdef __sgi
#pragma set woff 1375
#endif

// Single-threaded QT example

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

#include "OSGViewport.h"
#include "OSGCamera.h"
#include "OSGTileCameraDecorator.h"
#include "OSGWindow.h"
#include "OSGQTWindow.h"
#include "OSGCamera.h"
#include "OSGRemoteAspect.h"
#include "OSGClusterException.h"
#include "OSGStreamSocket.h"
#include "OSGSelection.h"
#include "OSGStreamSockConnection.h"
#include "OSGVRMLWriteAction.h"

OSG_BEGIN_NAMESPACE

class MyOSGQGLWidget : public OSGQGLWidget
{
    Q_OBJECT
public:
    MyOSGQGLWidget( QWidget *parent=0, const char *name=0 );
    QTWindowPtr osgWin;
    static Bool WindowCreatedFunction(FieldContainerPtr& fcp,
                                      RemoteAspect * aspect);
    static Bool WindowDestroyedFunction(FieldContainerPtr& fcp,
                                        RemoteAspect * aspect);
    static Bool WindowChangedFunction(FieldContainerPtr& fcp,
                                      RemoteAspect * aspect);
    virtual void initializeGL (void);
    virtual void paintGL (void);
    virtual void resizeGL (int w, int h);
    virtual void keyPressEvent ( QKeyEvent* );
};

OSG_END_NAMESPACE

using namespace OSG;

UInt32           windowsCreated=0;
UInt32           serverNr=0;
MyOSGQGLWidget  *glWidget=NULL;
DrawAction	    *ract;
QApplication    *app;

MyOSGQGLWidget::MyOSGQGLWidget ( QWidget *parent, const char *name ) :
    OSGQGLWidget( parent, name ),
    osgWin()
{
}

void MyOSGQGLWidget::initializeGL ( void )
{
    if(osgWin==NullFC)
        return;
	osgWin->init(); 	// create the context
	osgWin->activate(); // and activate it
	
	// some manual init, will be moved into StateChunks later
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_DEPTH_TEST );
    glEnable( GL_NORMALIZE );
}

void MyOSGQGLWidget::paintGL ( void )
{
    if(osgWin==NullFC)
        return;
	osgWin->frameInit();	            // frame-cleanup
	osgWin->drawAllViewports( ract );	// draw the viewports     
    osgWin->swap(); 
	osgWin->frameExit();	            // frame-cleanup
}

void MyOSGQGLWidget::resizeGL ( int w, int h )
{   
    if(osgWin==NullFC)
        return;
   if ( ! osgWin->isResizePending() )
   {
        osgWin->resize( w, h );
    	osgWin->resizeGL();             // handle resize events
   }
}

void MyOSGQGLWidget::keyPressEvent ( QKeyEvent *ke )
{
	if ( ke->key() == Key_Escape )
	{
		qApp->quit();
	}
}

Bool WindowCreatedFunction(FieldContainerPtr& fcp,
                           RemoteAspect * aspect)
{
    cout << "qt window create" << endl;
    if(windowsCreated==serverNr)
    {
        ract = DrawAction::create();
        glWidget = new MyOSGQGLWidget();
        app->setMainWidget( glWidget );
        cout << "assign to widget" << endl;
        glWidget->osgWin = QTWindowPtr::dcast( fcp );
        glWidget->osgWin->setGlWidget( glWidget );
        glWidget->initializeGL( );
        glWidget->setFixedSize(500,500);
        glWidget->show();
    }
    windowsCreated++;
    return true;
}

Bool WindowDestroyedFunction(FieldContainerPtr& fcp,
                             RemoteAspect * aspect)
{
    cout << "qt window destroyed" << endl;
    glWidget->osgWin = QTWindowPtr();
    windowsCreated--;
    glWidget->hide();
    app->processEvents();
    app->setMainWidget( NULL );
    delete glWidget;
    glWidget=NULL;
    return true;
}

Bool WindowChangedFunction(FieldContainerPtr& fcp,
                           RemoteAspect * aspect)
{
    QTWindowPtr window=QTWindowPtr::dcast(fcp);

    cout << "qt window changed" << endl;
    glWidget->setFixedSize(window->getSFWidth()->getValue(),
                           window->getSFHeight()->getValue());
    return true;
}

OSG::Action::ResultE printGeometry( OSG::CNodePtr &, OSG::Action * action )
{
	OSG::NodePtr node = action->getActNode();
	OSG::GeometryPtr g = OSG::GeometryPtr::dcast( node->getCore() );

    cout << "!!!!!!!!!!!!!!!!!!" << endl;
    g->dump();
    cout << "!!!!!!!!!!!!!!!!!!" << endl;

	return OSG::Action::Continue;
}


void dumpTree()
{
    Action *act = Action::create();
    act->registerEnterFunction( OSG::Geometry::getClassType(),
                                OSG::osgFunctionFunctor2( printGeometry ) );
    NodePtr root=glWidget->osgWin->getPort()[0]->getRoot();
    act->apply( root );
    root->dump();

    VRMLWriteAction *pWriter = VRMLWriteAction::create();
    pWriter->open("test.wrl");
    pWriter->write(root);
    pWriter->close();
}

void mainLoop(int port)
{
    StreamSocket         sock,client;
    StreamSockConnection connection;
    RemoteAspect         aspect;
    Selection            select;
    int xx=0;

    // remote aspect sync
    cout << "Waiting for clients on port " << port << endl;
    sock.open();
    sock.setReusePort(true);
    sock.bind(AnyAddress(port));
    sock.listen();
    client=sock.accept();
    cout << "Client connected" << endl;
    client.read(&serverNr,sizeof(serverNr));
    cout << "Run as server " << serverNr << endl;
    
    connection.addSocket(client);
    
    aspect.registerCreated(QTWindow::getClassType(),
                           osgFunctionFunctor2(&WindowCreatedFunction));
    aspect.registerChanged(QTWindow::getClassType(),
                           osgFunctionFunctor2(&WindowChangedFunction));
    aspect.registerDestroyed(QTWindow::getClassType(),
                             osgFunctionFunctor2(&WindowDestroyedFunction));
    
    try
    {
        while(1)
        {
            app->processEvents();
            select.setRead(client);
            if(select.select(0.05)>0)
            {
                UInt8 trigger;
                // receive syncronisation
                aspect.receiveSync(connection);
                // sunc with render client
                client.write(&trigger,sizeof(UInt8));
                client.read (&trigger,sizeof(UInt8));
                if(!xx++)
                {
                    cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << endl;
                    dumpTree();
                    cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << endl;
                }
            }
            if(glWidget)
                glWidget->paintGL();
        }
    }
    catch(ConnectionClosed &e)
    {
        sock.close();
        client.close();
        throw e;
    }
}

int main( int argc, char **argv )
{
    char                *host;
    int                  port=7878;

 	// OSG init
    osgInit(argc, argv);

    if(argc>1)
    {
        port=atoi(argv[1]);
    }
    // QT init
    QApplication::setColorSpec( QApplication::CustomColor );
    app = new QApplication( argc, argv );
    if ( !QGLFormat::hasOpenGL() )
    {
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }
    //    glWidget->show();
        
    bool noError;
    do {
        try
        {
            mainLoop(port);
        }
        catch(ConnectionClosed &e)
        {
            cout << e << endl;
            noError=true;
        }
        catch(exception &e)
        {
            cout << e.what() << endl;
            noError=false;
        }
    } while(noError);
    return 0;
}

#include "testClusterServer_qt_moc.cpp"
