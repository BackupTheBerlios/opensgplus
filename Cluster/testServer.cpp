#include <GL/glut.h>
#include <OSGConfig.h>
#include <iostream>
#include <OSGLog.h>
#include <OSGStreamSockConnection.h>
#include <OSGClusterServer.h>
#include <OSGGLUTWindow.h>
#include <OSGRenderAction.h>

OSG_USING_NAMESPACE

int             winid;
ClusterServer  *server;
GLUTWindowPtr   window;
RenderAction   *ract;

void display()
{
    try
    {
        server->render(ract);
    } 
    catch(exception &e)
    {
        SLOG << e.what() << endl;
        delete server;
        osgExit(); 
        exit(0);
    }
}
void reshape( int width, int height )
{
	window->resize( width, height );
}

int main(int argc,char **argv)
{
    char           *name          ="ClusterServer";
    char           *connectionType="StreamSock";

    if(argc>1)
        name          =argv[1];
    for(int i=1;i<argc;i++)
    {
        if(strlen(argv[i])>1 && argv[i][0]=='-')
        {
            switch(argv[i][1])
            {
                case 'm':
                    connectionType="Multicast";
                    break;
                case 'h':
                    cout << argv[0] 
                         << "-m"
                         << endl;
                    cout << "-m  use multicast" << endl;
                    return 0;
            }
        }
    }
    try
    {
        osgInit(argc, argv);
        glutInit(&argc, argv);
        glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
        winid = glutCreateWindow("OpenSG Cluster Client");

//        glutFullScreen();

        glutReshapeWindow(500,500);

        glutDisplayFunc(display);       
        glutIdleFunc(display);       
        glutReshapeFunc(reshape);       
        glEnable( GL_LIGHTING );
        glEnable( GL_LIGHT0 );
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_NORMALIZE );
        ract=RenderAction::create();
        window     = GLUTWindow::create();
        window->setWinID(winid);
        window->init();
        server     = new ClusterServer(window,
                                       name,
                                       connectionType);
        server->start();
        glutMainLoop();
    } 
    catch(exception &e)
    {
        SLOG << e.what() << endl;
        delete server;
        osgExit(); 
    }
    return 0;
}
