#include <GL/glut.h>
#include <GL/gl.h>
#include <OSGConfig.h>
#include <OSGGLUTWindow.h>
#include <OSGSimpleSceneManager.h>
#include <OSGSimpleGeometry.h>
#include <OSGGeoLoadManager.h>
#include <OSGGroup.h>
#include <OSGTransform.h>
#include <OSGSceneFileHandler.h>

// Activate the OpenSG namespace
OSG_USING_NAMESPACE

GeoLoadManager loadManager;

class MySceneManager:public SimpleSceneManager
{
public:
    MySceneManager(){}
    virtual void redraw( void ) {
        int i;
        GeoLoadManager::ResultT region;
        if (_internalRoot == NullFC)
        {
            initialize();
            showAll();
        }
        _cart->getSFMatrix()->setValue(_navigator.getMatrix());    
        updateHighlight();
        _win->activate();
        _win->frameInit();
        loadManager.balance(_win->getPort()[0],16,region);
        _win->renderAllViewports( _action );
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0,_win->getPort()[0]->getPixelWidth(),
                   0,_win->getPort()[0]->getPixelHeight());
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_COLOR_MATERIAL);
        for(i=0;i<region.size();i+=4)
        {
/*
            cout << region[i+0] << " ";
            cout << region[i+1] << " ";
            cout << region[i+2] << " ";
            cout << region[i+3] << endl;
*/
            glBegin(GL_LINE_LOOP);
            glColor3f(1, 1, 0);
            glVertex3f(region[i+0],region[i+1],0);
            glVertex3f(region[i+2],region[i+1],0);
            glVertex3f(region[i+2],region[i+3],0);
            glVertex3f(region[i+0],region[i+3],0);
            glEnd();
        }
        glDisable(GL_COLOR_MATERIAL);
        glEnable(GL_DEPTH_TEST);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        
        _win->swap();
        _win->frameExit();
    }
};

SimpleSceneManager *mgr;
NodePtr             scene;

// Standard GLUT callback functions
void display( void )
{
    mgr->redraw();
}

void reshape( int w, int h )
{
    mgr->resize( w, h );
    glutPostRedisplay();
}

void motion(int x, int y)
{
    mgr->mouseMove( x, y );
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if ( state )
        mgr->mouseButtonRelease( button, x, y );
    else
        mgr->mouseButtonPress( button, x, y );
    glutPostRedisplay();
}

void key(unsigned char key, int , int )
{
    switch(key)
    {
    case 27:    exit(1);
    case 'a':   mgr->setHighlight(scene);
                break;
    case 's':   mgr->setHighlight(NullFC);
                break;
    case 'l':   mgr->useOpenSGLogo();
                break;
    case 'f':   mgr->setNavigationMode(Navigator::FLY);
                break;
    case 't':   mgr->setNavigationMode(Navigator::TRACKBALL);
                break;
    }
    glutPostRedisplay();
}

// Initialize GLUT & OpenSG and set up the scene
int main (int argc, char **argv)
{
    int i;
    float ca=-1,cb=-1,cc=-1;
    // OSG init
    osgInit(argc,argv);

    // GLUT init
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    int winid = glutCreateWindow("OpenSG");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(key);

    // the connection between GLUT and OpenSG
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();

    // create the scene
    float x,y,z;
    scene = Node::create();
    beginEditCP(scene);
    scene->setCore(Group::create());
    endEditCP(scene);

    NodePtr file;

    for(i=1;i<argc;++i)
    {
        if(argv[i][0] >='0' &&
           argv[i][0] <='9')
        {
            if(ca==-1)
                ca=atoi(argv[i]);
            else
                if(cb==-1)
                    cb=atoi(argv[i]);
                else
                    if(cc==-1)
                        cc=atoi(argv[i]);
        }
        else
        {
            file = OSG::SceneFileHandler::the().read(argv[1]);
            beginEditCP(scene);
            scene->addChild(file);
            endEditCP(scene);
        }
    }
    if(ca==-1)
        ca=4;
    if(cb==-1)
        cb=ca;
    if(cc==-1)
        cc=cb;
    if(file == NullFC)
    {
        NodePtr node;
        TransformPtr trans;
        for(x=-ca/2 ; x<ca/2 ; x++)
            for(y=-cb/2 ; y<cb/2 ; y++)
                for(z=-cc/2 ; z<cc/2 ; z++)
                {
                    trans=Transform::create();
                    node=Node::create();
                    beginEditCP(trans);
                    beginEditCP(node);
                    node->setCore(trans);
                    trans->getMatrix().setTranslate(x,y,z);
                    node->addChild( makeBox(.6,.6,.6,5,5,5) );
                    beginEditCP(scene);
                    scene->addChild(node);
                    endEditCP(scene);
                    endEditCP(trans);
                    endEditCP(node);
                }
    }
    // create the SimpleSceneManager helper
    mgr = new MySceneManager;

    mgr->setWindow( gwin );
    mgr->setRoot( scene );

    mgr->showAll();

    loadManager.add(scene);

    // GLUT main loop
    glutMainLoop();

    return 0;
}

