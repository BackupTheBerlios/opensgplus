#include "OSGGLUT.h"
#include "OSGGVBase.h"
#include "OSGGVKDop.h"
#include "OSGGVKDopGeometry.h"

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 698 47 373 171 1125)
#endif

static int   m_startX=-1, m_startY=-1;
static Real m_dist = 10.0f; 
static Real m_fov =  60.0f/180.0f*3.1415f;
static Real m_headingAngle=0.0f, m_rollAngle=0.0f, m_pitchAngle=0.0f;
static bool  m_rotating=false, m_driving=false, m_scaling=false;
static unsigned m_numFace = 0;
static const Polygon3EdgeIndexed* current = NULL;
static unsigned currentFace;

// 14-DOP
typedef K18Dop BVOL;
static const unsigned numFaces = 2*BVOL::Size;

void initOpenGL () 
{
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glLineWidth(2.0f);
    glPointSize(10.0f);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
}


// Standard GLUT callback functions
void idle (void)
{
   glutPostRedisplay();
}


void glBitmapText(Real x, Real y, Real z, const char *message)
{
   char* text = (char*)message;
   glRasterPos3f(x, y, z);
   while (*text) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
      text++;
   }
}

void glBitmapTextShadow(Real x, Real y,
                        unsigned res_x, unsigned res_y,
                        const char *message,
                        GLfloat r, GLfloat g, GLfloat b)
{
   Real resX = (Real)res_x;
   Real resY = (Real)res_y;
 
   bool lightingMode = glIsEnabled(GL_LIGHTING) != 0;
   bool depthTestMode = glIsEnabled(GL_DEPTH_TEST) != 0;

   if (lightingMode) glDisable(GL_LIGHTING);
   if (depthTestMode) glDisable(GL_DEPTH_TEST);

   Real xpixeloffset = 2.0f/resX;
   Real ypixeloffset = 2.0f/resY;
   glColor3f(r/3.0,g/3.0,b/3.0);
   glBitmapText(x+xpixeloffset, y-ypixeloffset, 0, message);
   glBitmapText(x+xpixeloffset, y, 0, message);
   glBitmapText(x, y-ypixeloffset, 0, message);

   glColor3f(r,g,b);
   glBitmapText(x, y, 0, message);

   if (lightingMode) glEnable(GL_LIGHTING);
   if (depthTestMode) glEnable(GL_DEPTH_TEST);
}

void draw (Polygon3SetIndexed& geom)
{
   static char buffer[255];

   glColor3f(0.0f, 0.0f, 0.0f);
   unsigned f = 0;
   for (Polygon3SetIndexed::Container::const_iterator itF=geom.faces.begin(); 
	itF != geom.faces.end(); 
	++itF, ++f) { 
     Vec3f pos(0.0f, 0.0f, 0.0f);
     for (Polygon3Indexed::Container::const_iterator itE=itF->edges.begin(); 
	  itE != itF->edges.end();
	  ++itE) {
        pos += Vec3f(itE->getPoint());
     }
     pos /= itF->edges.size();
     sprintf(buffer, "%u", f);
     glBitmapText(pos[0], pos[1], pos[2], buffer);
   }
}


void draw (Polygon3SetIndexed& geom, unsigned thisFace)
{
#if 0
   glColor3f(0.8f, 0.8f, 0.8f);
   Pnt3f pos = geom.getFace(thisFace)->firstEdge()->getPoint();
   static char buffer[255];
   sprintf(buffer, "%u", thisFace);
   glBitmapText(pos[0], pos[1], pos[2], buffer);
#endif
   glColor3f(0.0f, 0.0f, 1.0f);
   Pnt3f pos = current->getPoint();
   static char buffer[255];
   sprintf(buffer, "%u", current->face->index);
   glBitmapText(pos[0], pos[1], pos[2], buffer);
   pos = current->getOpposit()->getPoint();
   sprintf(buffer, "%u", current->getOpposit()->face->index);
   glBitmapText(pos[0], pos[1], pos[2], buffer);

   glColor3f(0.0f, 0.0f, 0.0f);
   unsigned f = 0;
   for (Polygon3SetIndexed::Container::const_iterator itF=geom.faces.begin(); 
	itF != geom.faces.end(); 
	++itF, ++f) { 
   glBegin(GL_POLYGON);
   for (Polygon3Indexed::Container::const_iterator itE=itF->edges.begin(); 
	itE != itF->edges.end();
	++itE) {
     glVertex3fv(itE->getPoint().getValues());
   }
   glEnd();
   }
}
void drawWireframe (Polygon3SetIndexed& geom, unsigned f)
{
   Polygon3Indexed* itF = geom.getFace(f);
   glBegin(GL_LINE_LOOP);
   for (Polygon3Indexed::Container::const_iterator itE=itF->edges.begin(); 
	itE != itF->edges.end();
	++itE) {
      glVertex3fv(itE->getPoint().getValues());
   }
   glEnd();
}
void drawWireframe (Polygon3SetIndexed&  geom)
{
   glColor3f(0.0f, 0.0f, 1.0f);
   Pnt3f pos = current->getPoint();
   static char buffer[255];
   sprintf(buffer, "%u", current->face->index);
   glBitmapText(pos[0], pos[1], pos[2], buffer);
   pos = current->getOpposit()->getPoint();
   sprintf(buffer, "%u", current->getOpposit()->face->index);
   glBitmapText(pos[0], pos[1], pos[2], buffer);

   for (Polygon3SetIndexed::Container::const_iterator itF=geom.faces.begin(); 
	itF != geom.faces.end(); 
	++itF) { 
      glBegin(GL_LINES);
      for (Polygon3Indexed::Container::const_iterator itE=itF->edges.begin(); 
	   itE != itF->edges.end();
	   ++itE) {
	if (itE.operator->() == current 
	    || itE.operator->() == current->getOpposit()) {
	   glColor3f(1.0f, 0.0f, 0.0f);
	   glVertex3fv(current->getPoint().getValues());
	   glColor3f(0.0f, 1.0f, 0.0f);
	   glVertex3fv(current->getOpposit()->getPoint().getValues());
	} else {
	   glColor3f(0.0f, 0.0f, 0.0f);
	   glVertex3fv(itE->getPoint().getValues());
	   glColor3f(0.0f, 0.0f, 0.0f);
	   glVertex3fv(itE->getOpposit()->getPoint().getValues());
	}
      }
      glEnd();
   }
}

void drawRegularPolytope (unsigned n)
{
   for (unsigned num=0; num<2*BVOL::Size; ++num) {
   VectorClass  center = BVOL::getDirection()[num];
   static char buffer[255];
   sprintf(buffer, "%u", num);
   glColor3f(0.8f, 0.8f, 0.8f);
   glBitmapText(center[0], center[1], center[2], buffer);
   Polygon3SetIndexed&  geom   = BVOL::unitDop().getGeometry().getPolygonSet();
   Polygon3Indexed*     itF    = geom.getFace(num);
   Polygon3EdgeIndexed* e      = itF->firstEdge();
   Real         radius = 0;
   do {
      radius = stdMax(radius, VectorClass(e->getPoint()-center).length());
      e = e->getNext();
   } while (e != itF->firstEdge());
   glColor3f(0.0f, 0.0f, 0.0f);
   glBegin(GL_POLYGON);
   Real step  = 2*Pi/n;
   Real angle = 0;
   VectorClass direction;
   for (unsigned i=0; i<n; ++i, angle+=step) {
      direction = center
	+ (radius*cosf(angle)) * (BVOL::BVolGeometry::getFrameX()[num])
	+ (radius*sinf(angle)) * (BVOL::BVolGeometry::getFrameY()[num]);
      //direction /= direction.length();
      glVertex3fv(direction.getValues());
   }
   glEnd();
   }
}

void testTopology (Polygon3SetIndexed& geom) 
{
   const Polygon3EdgeIndexed* start      = NULL;
   for (Polygon3SetIndexed::Container::const_iterator itF=geom.faces.begin(); 
	itF != geom.faces.end(); 
	++itF) {
      std::cout << "face " << itF->index << std::endl;
      unsigned numEdge = 0;
      for (Polygon3Indexed::Container::const_iterator itE=itF->edges.begin(); 
	   itE != itF->edges.end();
	   ++itE) {
  	 std::cout << numEdge++ << ": ";
	 start = itE.operator->();
	 do {
	    std::cout << ( start->face->index >= BVOL::Size ? "-" : "+" ) 
		      << ( start->face->index % BVOL::Size ) << " ";
	    start = start->getCW();
	 } while (start != itE.operator->());
	 std::cout << std::endl;
      }
      std::cout << std::endl;
   }
}

void initScene () 
{
    Polygon3SetIndexed& geom = BVOL::unitDop().getGeometry().getPolygonSet();
    testTopology(geom);
    current = geom.getFace(0)->firstEdge();
}

void display( void )
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glLoadIdentity();
   glTranslatef(0.0, 0.0, -m_dist);
   glRotatef(-m_headingAngle, 0.0, 0.0, 1.0);
   glRotatef(-m_pitchAngle, 1.0, 0.0, 0.0);
   glRotatef(-m_rollAngle, 0.0, 1.0, 0.0);
   //Vec3f center = -m_root->getBoundingVolume().getCenter();
   //glTranslatef(center[0], center[1], center[2]);

#if 0
   glColor3f(0.0f, 1.0f, 0.0f);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(0.5, 0.5);
   //BVOL::unitDop().draw();
   draw(BVOL::unitDop().getGeometry().getPolygonSet());
   glDisable(GL_POLYGON_OFFSET_FILL);
   glColor3f(0.0f, 0.0f, 0.0f);
#endif
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   //BVOL::unitDop().drawWireframe();
   drawWireframe(BVOL::unitDop().getGeometry().getPolygonSet());
   draw(BVOL::unitDop().getGeometry().getPolygonSet());
   //drawRegularPolytope(20);

   glutSwapBuffers();
}

void reshape( int w, int h )
{
   if (h == 0) { 
      h = 1; 
   }

   // Set the viewport to be the entire window
   glViewport(0, 0, w, h);
   // reset coordinate system
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(m_fov, (Real)w/(Real)h, 0.01, 10000);
   glMatrixMode(GL_MODELVIEW);

   glutPostRedisplay();
}

void motion(int x, int y)
{
   Real normX = 50.0f/glutGet(GLUT_WINDOW_WIDTH),
     normY = 50.0f/glutGet(GLUT_WINDOW_HEIGHT),
     normDist = 50.0f;
   if (m_rotating) {
      m_rollAngle  -= normX*Real(x-m_startX);
      m_pitchAngle -= normY*Real(y-m_startY);
   } else if (m_driving) {
      m_headingAngle = fmod(m_headingAngle+normX*(x-m_startX), 2*Pi);
   } else if (m_scaling) {
      m_dist += normDist*Real(y-m_startY);
   }
   m_startX = x;
   m_startY = y;

   glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
  m_startX = x;
  m_startY = y;
  if ( state ) {
     if (button == GLUT_LEFT_BUTTON) {
	m_rotating = false;
     } else if (button == GLUT_MIDDLE_BUTTON) {
	m_driving = false;
     } else if (button == GLUT_RIGHT_BUTTON) {
	m_scaling = false;
     }
  } else {
     if (button == GLUT_LEFT_BUTTON) {
	m_rotating = true;
     } else if (button == GLUT_MIDDLE_BUTTON) {
	m_driving = true;
     } else if (button == GLUT_RIGHT_BUTTON) {
        m_scaling = true;
     }
  }
  glutPostRedisplay();
}

void key(unsigned char key, int x, int y)
{
    Polygon3SetIndexed& geom = BVOL::unitDop().getGeometry().getPolygonSet();
    switch(key) {
    case 27:    
       exit(1);
    case '+':   
       current = current->getNext();
       break;
    case '-':   
       current = current->getPrev();
       break;
    case 'o':
       current = current->getOpposit();
       break;
    case 'c':
       current = current->getCCW();
       break;
    case 'C':
       current = current->getCW();
    case 'F':
       current = geom.getFace((current->face->index+1)%numFaces)->firstEdge();
       break;
    case 'f':
       current = geom.getFace((current->face->index-1)%numFaces)->firstEdge();
       break;
    }
    glutPostRedisplay();
}

// Initialize GLUT & OpenSG and set up the scene
int main (int argc, char **argv)
{
   // GLUT init
   glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   int winid = glutCreateWindow("testKDopGeometryRender");
   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   
   initOpenGL();
   
   initScene();

   // GLUT main loop
   glutMainLoop();
   
   return 0;
}

#ifdef OSG_WIN32_ICL
#pragma warning (error : 698 47 373 171)
#endif


