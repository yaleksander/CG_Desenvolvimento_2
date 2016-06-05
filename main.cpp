#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

// número máximo de pontos
#define MAXNUM 100
// precisão do cursor para selecionar pontos
#define LONGE 0.05
// densidade (thickness) da curva BSpline
#define THICKN 200

// só pra não precisar escrever std:: antes de qualquer operação básica de entrada ou saída
using namespace std;

float angle = 0, raio = 0.8;
int enableMenu = 0;
int distancia = 25;
int proj = 0;
int windowY = 640;
int windowX = 640;
float viewangle = 40.0f;
float distanciaOrigem = 30.0;
float rotationX = 38.0, rotationY = 22.0;
int   last_x, last_y;
int moving = -1;
// quantidade de pontos
int quant = 0;
// controle para mostrar/esconder pontos e linhas de construção
bool verPontos = true;
bool verLinhas = true;
// vetor com as coordenadas dos pontos
float pontos[100][3];
// vetor com as dimensões do ortho

float orthoDim[4];

void init (void);
void reshape( int w, int h);
void display(void);
void desenhaEixos();
void projec();
void orthog();
void mouse(int button, int state, int x, int y);
void motion(int x, int y );
void keyboard(unsigned char key, int x, int y);
// função auxiliar pra transformar um int em uma string
string toString(int n);
// função auxiliar pra calcular a distância euclidiana entre dois pontos
float dist(float a, float b, float c, float d);
// função auxiliar pra adicionar um ponto na tela
void createPoint(float x, float y);
// função auxiliar pra apagar um ponto da tela
void erasePoint(int p);
// função auxiliar para selecionar um ponto quando o mouse é clicado
void selectPoint(int x, int y, bool left);
// função auxiliar de BSpline que prepara o desenho
void desenhaCurva(void);
// função para desenhar a curva BSpline
void BSpline(float vet[4][2]);
// função para desenhar na tela

int main(int argc, char** argv){
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB);
   glutInitWindowSize (windowY, windowX);
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
   windowY = w;
   windowX = h;
   glViewport(0,0,w,h);
    if (w <= h){
            orthoDim[0] = -1.0;
            orthoDim[1] =  1.0;
            orthoDim[2] = -1 * (GLfloat) h / (GLfloat) w;
            orthoDim[3] =  1 * (GLfloat) h / (GLfloat) w;
        }
        else{
            orthoDim[0] = -1 * (GLfloat) w / (GLfloat) h;
            orthoDim[1] =  1 * (GLfloat) w / (GLfloat) h;
            orthoDim[2] = -1.0;
            orthoDim[3] =  1.0;
        }
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
      glVertex3f (-distancia, 0.0, 0.0);
      glVertex3f ( distancia, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);
      glVertex3f (0.0, -distancia, 0.0);
      glVertex3f (0.0,  distancia, 0.0);
      glColor3f(0.0, 0.0, 1.0);
      glVertex3f (0.0, 0.0, -distancia);
      glVertex3f (0.0, 0.0,  distancia);
   glEnd();
   glEnable(GL_LIGHTING);
}

void projec(){
    gluPerspective(40.0f,(GLfloat)windowY/(GLfloat)windowX,0.1f,200.0f);	// Calculate The Aspect Ratio Of The Window

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
    glOrtho(orthoDim[0], orthoDim[1], orthoDim[2], orthoDim[3], orthoDim[4], orthoDim[5]);

       glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
       glLoadIdentity();           // Inicializa com matriz identidade


		// tamanho de cada ponto
		glPointSize(10);

		// cor vermelha
		glColor3f(1.0, 0.0, 0.0);

		// são desenhados os pontos
		glBegin(GL_POINTS);
			for (int i = 0; i < quant; i++)
				glVertex3fv(pontos[i]);
		glEnd();

		// cor azul com verde (parece azul claro)
		glColor3f(0.0, 1.0, 1.0);

		// são escritos os nomes dos pontos (P0, P1, P2, ..., PN)
		for (int i = 0; i < quant; i++)
		{
			// transformamos o contador numa string (com a letra 'P' na frente) para ser escrito na tela
			string s = 'P' + toString(i);

			// coordenadas do ponto
			glRasterPos2f(pontos[i][0], pontos[i][1]);

			// desenha-se a string com o nome do ponto (cada char é uma iteração)
			for (int j = 0; j < s.size(); j++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[j]);
		}

		// cor verde
		glColor3f(0.0, 1.0, 0.0);
		for (int i = 0; i < quant - 1; i++)
		{
			// pegamos um ponto no vetor, o próximo ponto, criamos uma reta e atualizamos o contador
			glBegin(GL_LINES);
				glVertex3f(pontos[i][0], pontos[i][1], pontos[i][2]);
				glVertex3f(pontos[i+1][0], pontos[i+1][1], pontos[i+1][2]);
			glEnd();
		}


	// cor azul
	glColor3f(0.0, 0.0, 1.0);

	// espessura da linha azul
	glLineWidth(2.0);

	// por algum motivo misterioso, quando a função para definir os pontos de controle não está aqui, às vezes é criada uma linha horizontal
	// inicialmente nós imaginamos que era um problema com o buffer, mas após uma série de tentativas e erros, decidimos usar essa função. Funcionou
	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, quant, &pontos[0][0]);

	// função para desenhar a nossa curva BSpline
	desenhaCurva();
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
    if(!proj){
	// a coordenada y é invertida por padrão
	y = windowY - y;

	// GLUT_DOWN é pra impedir que a função seja chamada duas vezes
	// (uma vez quando o usuário aperta um botão e outra quando ele solta)
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		selectPoint(x, y, true);
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		selectPoint(x, y, false);

	// quando o botão é solto, o usuário não pode mais mover os pontos
	if (state == GLUT_UP)
		moving = -1;
}
   if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ){
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

string toString(int n){
	// 0 não entraria no while, então retornamos essa exceção logo de uma vez
	if (n == 0)
		return "0";

	string s = "";
	string t = "";

	// adicionamos o último dígito do número (%10) ao final da string em cada iteração e continuamos enquanto n for diferente de 0
	while (n)
	{
		// '0' é para passar o número pro padrão ANSI
		s += (char) ('0' + n % 10);

		// dividindo um int por 10, simplesmente descartamos seu último dígito. Ao dividir por 10 um int menor que 10, temos 0
		n /= 10;
	}

	// como a string é escrita de trás pra frente devido ao funcionamento do while acima, é preciso invertê-la antes de retorná-la
	int size = s.size();
	for (int i = 0; i < size; i++)
		t += s[size-i-1];

	return t;
}

float dist(float a, float b, float c, float d){
	return sqrt((c - a) * (c - a) + (d - b) * (d - b));
}

void createPoint(float x, float y){
	if (quant < MAXNUM)
	{
		pontos[quant][0] = x;
		pontos[quant][1] = y;
		pontos[quant][2] = 0;

		// assim que o ponto é criado, já podemos associar a variável de movimento 'moving' a ele
		moving = quant++;
	}
}

void erasePoint(int p){
	while (++p < quant)
	{
		pontos[p-1][0] = pontos[p][0];
		pontos[p-1][1] = pontos[p][1];
		pontos[p-1][2] = pontos[p][2];
	}
	quant--;
}

void selectPoint(int x, int y, bool left){
	// conversão explicada anteriormente
	float a = -(((float) x * (orthoDim[1] - orthoDim[0]) / (float) windowX) / orthoDim[0] + 1);
	float b = -(((float) y * (orthoDim[3] - orthoDim[2]) / (float) windowY) / orthoDim[2] + 1);

	// 'menor' é inicializado como a maior distância possível de forma que qualquer ponto dentro do ortho vai ter que tomar o seu lugar
	float menor = dist(orthoDim[0], orthoDim[2], orthoDim[1], orthoDim[3]);

	// 'p' indica o ponto selecionado. -1 quer dizer que nenhum ponto foi selecionado no final
	int p = -1;

	for (int i = 0; i < quant; i++)
	{
		float d = dist(pontos[i][0], pontos[i][1], a, b);

		// escolhe o ponto mais próximo do cursor do mouse caso ele não esteja muito longe
		// a constante 'LONGE' determina a distância máxima do cursor pro clique funcionar
		if (d < menor && d < LONGE)
		{
			menor = d;
			p = i;
		}
	}

	// se a variável booleana 'left' for falsa, o botão direito foi clicado, o que significa que o ponto deve ser apagado
	if (p >= 0 && !left && verPontos)
		erasePoint(p);

	// se 'left' for verdadeira, o botão esquerdo foi clicado, o que significa que o usuário pode mover um ponto
	if (p >= 0 && left && verPontos)
		moving = p;

	// caso 'p' seja -1, nenhum ponto foi selecionado pois todos estão muito longe. É criado um ponto novo então
	if (p < 0 && left)
		createPoint(a, b);

	// após criar, apagar ou mover um ponto, a tela é atualizada
	glutPostRedisplay();
}

void desenhaCurva(void){
	// são necessários pelo menos 4 pontos para desenhar a curva
	if(quant > 3)
	{
		float vet[4][2];

		// é feita uma iteração de 4 em 4 (0, 1, 2, 3) -> (1, 2, 3, 4) -> (2, 3, 4, 5) -> ...
		for(int i = 0; i < quant - 3; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				vet[j][0] = pontos[i+j][0];
				vet[j][1] = pontos[i+j][1];
			}
			// desenhamos o trecho da curva correspondente a esta iteração
			BSpline(vet);
		}
	}
}

void BSpline(float vet[4][2]){
	float pontoAux[2];
	float intervalo;
	float mtrIntrv[4];

	// matriz de transformação usada nos cálculos a seguir
	float mtrTrnsf[4][4] = {{-1,  3, -3, 1},
				{ 3, -6,  3, 0},
				{-3,  0,  3, 0},
				{ 1,  4,  1, 0}};

	// aqui a linha começa a ser desenhada efetivamente
	glBegin(GL_LINE_STRIP);
		for(int i = 0; i <= THICKN; i++)
		{
			// Cria os intervalos variando de 0 a 1
			intervalo = (float) i / THICKN;

			// Monta a Matriz com os intervalos
			mtrIntrv[0] = intervalo * intervalo * intervalo;
			mtrIntrv[1] = intervalo * intervalo;
			mtrIntrv[2] = intervalo;
			mtrIntrv[3] = 1;

			// Equação para calculo da posição do ponto para um intervalo t
			// S(t) = T * 1/6 * M * P
			for(int j = 0; j < 4; j++)
				for(int k = 0; k < 4; k++)
				{
					pontoAux[0] += mtrIntrv[k] * (1.0 / 6) * mtrTrnsf[k][j] * vet[j][0];
					pontoAux[1] += mtrIntrv[k] * (1.0 / 6) * mtrTrnsf[k][j] * vet[j][1];
				}

			// Cria ponto do intervalo t
			glVertex2f(pontoAux[0], pontoAux[1]);

			// Zera os pontos para o proximo calculo
			for(int j = 0; j < 2; j++)
				pontoAux[j] = 0;
		}
	glEnd();
}
