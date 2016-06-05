#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

float angle = 0, raio = 0.8;
int enableMenu = 0;
int dist = 25;
int proj = 0;
int width = 640;
int height = 640;
float viewangle = 40.0f;
float distanciaOrigem = 30.0;
float rotationX = 38.0, rotationY = 22.0;
int   last_x, last_y;

void init (void);
void reshape( int w, int h);
void display(void);
void desenhaEixos();
void projec();
void orthog();
void mouse(int button, int state, int x, int y);
void motion(int x, int y );
void keyboard(unsigned char key, int x, int y);

int main(int argc, char** argv){
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB);
   glutInitWindowSize (width, height);
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Projeções em OpenGL");
   init ();
   glutMouseFunc( mouse );
   glutKeyboardFunc(keyboard);
   glutMotionFunc( motion );
   glutReshapeFunc( reshape );
   glutDisplayFunc(display);
   glutMainLoop();

   return 0;
}

void init (void){
   // selecionar cor de fundo (preto)
   glClearColor (0.0, 0.0, 0.0, 0.0);

   glEnable(GL_LIGHT0);                   // habilita luz 0
   glEnable(GL_COLOR_MATERIAL);           // Utiliza cor do objeto como material
   glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	GLfloat light0_position[] = {2.0f, 2.0f, 5.0f, 0.0f};
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

   glEnable(GL_LIGHTING);                 // Habilita luz
   glEnable(GL_DEPTH_TEST);               // Habilita Z-buffer
   glEnable(GL_CULL_FACE);                // Habilita Backface-Culling
}

void reshape( int w, int h){
   width = w;
   height = h;
   glViewport(0,0,w,h);

   glutPostRedisplay();
}

void display(void){
   // Limpar todos os pixels
   glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   // inicializar sistema de projeção
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   if(!proj) orthog();
   else projec();

   desenhaEixos();

   glutSwapBuffers ();
   glutPostRedisplay();
}

void desenhaEixos(){
   glDisable(GL_LIGHTING);
   glColor3f (0.0, 1.0, 0.0);
   glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);
      glVertex3f (-dist, 0.0, 0.0);
      glVertex3f ( dist, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);
      glVertex3f (0.0, -dist, 0.0);
      glVertex3f (0.0,  dist, 0.0);
      glColor3f(0.0, 0.0, 1.0);
      glVertex3f (0.0, 0.0, -dist);
      glVertex3f (0.0, 0.0,  dist);
   glEnd();
   glEnable(GL_LIGHTING);
}

void projec(){
    gluPerspective(40.0f,(GLfloat)width/(GLfloat)height,0.1f,200.0f);	// Calculate The Aspect Ratio Of The Window

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   gluLookAt (0.0, 0.0, distanciaOrigem, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   // Movimenta a camera em torno do objeto
   // Luz fica fixa na posição (0, 0, 5) inicialmente
   glRotatef( rotationY, 1.0, 0.0, 0.0 );
   glRotatef( rotationX, 0.0, 1.0, 0.0 );

   glPushMatrix();
      glColor3f (0.283, 0.456, 0.518);
      glutSolidCube(5);

      glColor3f (1.0, 1.0, 1.0);
      glDisable(GL_LIGHTING);
      glutWireCube(10);
      glEnable(GL_LIGHTING);

      glTranslatef(0, 0, 5);
      glColor3f (1.0, 1.0, 1.0);
      glutSolidSphere(2, 50, 50);
   glPopMatrix();
}

void orthog(){
    float w = width;
    float h = height;
    int ortho = 10;

      // glOrtho(-orho, orho, -orho, orho, -200, 200);
      if (width <= height)
			glOrtho (-ortho, ortho, -ortho*h/w, ortho*h/w, -100.0, 100.0);
		else
			glOrtho (-ortho*w/h, ortho*w/h, -ortho, ortho, -100.0, 100.0);

       glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
       glLoadIdentity();           // Inicializa com matriz identidade
}

void mouse(int button, int state, int x, int y){
    if(proj){
       if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
       {
          last_x = x;
          last_y = y;
       }
       if (button == 3){
            distanciaOrigem -= 1;
       }
       if (button == 4){
            distanciaOrigem += 1;
       }
    }
   if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )   {
       proj= !proj;
       if(!proj) printf("Projecao Ortogonal.\n");
       else      printf("Projecao Perspectiva.\n");
   }
    glutPostRedisplay();
}

void motion(int x, int y ){
    if(proj){
       rotationY -= (float) (y - last_y);
       rotationX -= (float) (x - last_x);
       last_x = x;
       last_y = y;
    }
   glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
   switch (key){
      case 27 :
         exit(0);
      break;
   }
   glutPostRedisplay();
}
