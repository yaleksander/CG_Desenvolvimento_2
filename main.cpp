#include <GL/glut.h>
#include <math.h>

int width = 800, height = 600;
int last_x, last_y;
float posicaoCameraZ = 45.0;
float direcaoVisaoHorizontal = 0, posicaoCameraX = 0.0, posicaoCameraY = 0.0, alturaVisao = 0.0;

void init(void);
void display(void);
void reshape (int w, int h);
void keyboard (unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (width, height);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Desenvolvimento 2");

    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutKeyboardFunc(keyboard);
    glutMainLoop();
   return 0;
}

void init(void){
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

   glEnable(GL_LIGHT0);                  
   glEnable(GL_COLOR_MATERIAL);          
   glColorMaterial(GL_FRONT, GL_DIFFUSE);

   glEnable(GL_LIGHTING);               
   glEnable(GL_DEPTH_TEST);               
   glEnable(GL_CULL_FACE);                
}

void display(void){
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) width/(GLfloat) height, 1.0, 200.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity ();
   gluLookAt (posicaoCameraX, posicaoCameraY, posicaoCameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

      glColor3f (0.80, 0.60, 0.0);
      glutSolidCube(10);

   glutSwapBuffers();
}

void reshape (int w, int h){
   width = w;
   height = h;
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
}

void keyboard (unsigned char key, int x, int y){
   switch (key){
       case 'a':
            posicaoCameraX += 2;
            break;
       case 'd':
            posicaoCameraX -= 2;
            break;
       case 'w':
            alturaVisao += 0.02;
            break;
       case 's':
            alturaVisao -= 0.02;
            break;
      case 27:
         exit(0);
      break;
   }
   glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
   if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ){
      last_x = x;
      last_y = y;
   }
   if (button == 3){
        posicaoCameraZ+=0.3;
   }
   if (button == 4){
        posicaoCameraZ-=0.3;
   }
   glutPostRedisplay();
}

void motion(int x, int y){
   last_x = x;
   last_y = y;
}
