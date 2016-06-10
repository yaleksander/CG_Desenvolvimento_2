#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <fstream>

#define MAXNUM 100
#define LONGE 0.05
#define MAXROT 144
#define MAXSMT 40
#define MINROT 12
#define MINSMT 5

using namespace std;

string str_aux;
float angle = 0, raio = 0.8;
int enableMenu = 0;
int distancia = 25;
int proj = 0;
int originalX;
int originalY;
int windowX = 640;
int windowY = 640;
float viewangle = 40.0f;
float distanciaOrigem = 30.0;
float rotationX = 38.0, rotationY = 22.0;
int last_x, last_y;
int moving = -1;
int quant = 0;
int smooth = 10;
int numRotacoes = 36;
float pontos[100][3];
float orthoDim[4] = {-1.0, 1.0, -1.0, 1.0};
float luzY = 0.0;
bool full = false;
bool wire = false;

void init(void);
void reshape(int w, int h);
void display(void);
void desenhaEixos();
void projec();
void orthog();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void createPoint(float x, float y);
void erasePoint(int p);
void selectPoint(int x, int y, bool left);
void desenhaCurva(void);
void BSpline(float vet[4][2]);
float dist(float a, float b, float c, float d);
string toString(int n);

GLfloat object_ambient[]   = {0.40, 0.90, 0.70, 1.0};
GLfloat object_difusa[]    = {0.40, 0.90, 0.70, 1.0};
GLfloat object_especular[] = {0.60, 0.90, 0.90, 1.0};
GLfloat object_emissao[]   = {0.00, 0.00, 0.00, 1.0};
GLfloat object_brilho[]    = {160.0};

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(windowY, windowX);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Projeções em OpenGL");
	init();
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

void init(void)
{
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	GLfloat light0_position[] = { 0.5f, 0.0f, 0.0f, 0.0f};
	GLfloat light1_position[] = {-0.5f, 0.0f, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	GLfloat cor[] = {0.5f, 0.5f, 0.5f, 1.0};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, cor);
	glLightfv(GL_LIGHT1, GL_SPECULAR, cor);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	cout << endl;
	cout << "  Mouse e movimento:" << endl;
	cout << "  [Desenho (2D)] - Use o botão esquerdo para criar novos pontos ou arrastar" << endl;
	cout << "  pontos existentes. O botão direito apaga pontos existentes." << endl;
	cout << "  [Visualização (3D)] - Use o botão direito para rotacionar o sólido" << endl;
	cout << endl;
	cout << "  Teclas e funções:" << endl;
	cout << "  [0] - Fecha a curva ligando os pontos finais aos iniciais" << endl;
	cout << "  [1] - Move a luz 01 para baixo no eixo y" << endl;
	cout << "  [2] - Move a luz 01 para cima no eixo y" << endl;
	cout << "  [4] - Salva a configuração atual da curva" << endl;
	cout << "  [5] - Carrega a configuração salva da curva" << endl;
	cout << "  [7] - Alterna entre os modos de desenho e visualização" << endl;
	cout << "  [8] - Liga/desliga o modo tela cheia" << endl;
	cout << "  [9] - Liga/desliga o wireframe" << endl;
	cout << "  [+] - Aumenta o número de seções transversais" << endl;
	cout << "  [-] - Diminui o número de seções transversais" << endl;
	cout << "  [*] - Aumenta a precisão (número de sublinhas) da bspline" << endl;
	cout << "  [/] - Diminui a precisão (número de sublinhas) da bspline" << endl;
	cout << endl;
}

void reshape(int w, int h)
{
	windowX = w;
	windowY = h;
	int menor = w < h ? w : h;
	glViewport((w - menor) / 2, (h - menor) / 2, menor, menor);
	glutPostRedisplay();
}

void display(void)
{
	glDisable(GL_SCISSOR_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (!proj)
		orthog();
	else
		projec();
	desenhaEixos();
	glutSwapBuffers();
	glutPostRedisplay();
}

void desenhaEixos()
{
	glDisable(GL_LIGHTING);
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		if (proj)
		{
			glVertex3f(-distancia, 0.0, 0.0);
			glVertex3f( distancia, 0.0, 0.0);
		}
		else
		{
			glVertex3f(-1.0, 0.0, 0.0);
			glVertex3f( 1.0, 0.0, 0.0);
		}
		glColor3f(0.0, 1.0, 0.0);
		if (proj)
		{
			glVertex3f(0.0, -distancia, 0.0);
			glVertex3f(0.0,  distancia, 0.0);
		}
		else
		{
			glVertex3f(0.0, -1.0, 0.0);
			glVertex3f(0.0,  1.0, 0.0);
		}
		glColor3f(0.0, 0.0, 1.0);
		if (proj)
		{
			glVertex3f(0.0, 0.0, -distancia);
			glVertex3f(0.0, 0.0,  distancia);
		}
		else
		{
			glVertex3f(0.0, 0.0, -1.0);
			glVertex3f(0.0, 0.0,  1.0);
		}
	glEnd();
	glEnable(GL_LIGHTING);
}

void projec()
{
	int menor = windowX < windowY ? windowX : windowY;
	glEnable(GL_SCISSOR_TEST);
	glScissor((windowX - menor) / 2, (windowY - menor) / 2, menor, menor);
	glClearColor(0.2, 0.0, 0.2, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gluPerspective(10.0f, 1.0f, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, distanciaOrigem, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glRotatef(rotationY, 1.0, 0.0, 0.0);
	glRotatef(rotationX, 0.0, 1.0, 0.0);
	GLfloat light0_position[] = { 0.5f, 0.0f, 0.0f, 0.0f};
	GLfloat light1_position[] = {-0.5f, luzY, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	desenhaCurva();
}

void orthog()
{
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	int menor = windowX < windowY ? windowX : windowY;
	glEnable(GL_SCISSOR_TEST);
	glScissor((windowX - menor) / 2, (windowY - menor) / 2, menor, menor);
	glClearColor(0.2, 0.0, 0.2, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(10);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
		for (int i = 0; i < quant; i++)
			glVertex3fv(pontos[i]);
	glEnd();
	glColor3f(0.0, 1.0, 1.0);
	for (int i = 0; i < quant; i++)
	{
		string s = 'P' + toString(i);
		glRasterPos2f(pontos[i][0], pontos[i][1]);
		for (int j = 0; j < s.size(); j++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[j]);
	}
	glColor3f(1.0, 1.0, 0.0);
	for (int i = 0; i < quant - 1; i++)
	{
		glBegin(GL_LINES);
			glVertex3f(pontos[i][0], pontos[i][1], pontos[i][2]);
			glVertex3f(pontos[i+1][0], pontos[i+1][1], pontos[i+1][2]);
		glEnd();
	}
	glColor3f(0.0, 0.0, 1.0);
	glLineWidth(2.0);
	desenhaCurva();
	glEnable(GL_LIGHTING);
}

void mouse(int button, int state, int x, int y)
{
	if (proj)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			last_x = x;
			last_y = y;
		}
		if (button == 3)
			distanciaOrigem -= 1;
		if (button == 4)
			distanciaOrigem += 1;
	}
	if (!proj)
	{
		y = windowY - y;
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
			selectPoint(x, y, true);
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
			selectPoint(x, y, false);
		if (state == GLUT_UP)
			moving = -1;
	}
	glutPostRedisplay();
}

void motion(int x, int y)
{
	if (proj)
	{
		rotationY -= (float) (y - last_y);
		rotationX -= (float) (x - last_x);
		last_x = x;
		last_y = y;
	}
	if (!proj)
	{
		if (moving == -1)
			return;
		y = windowY - y;
		int m = windowX < windowY ? windowX : windowY;
		float a = (x - (windowX - m) / 2.0) * 2.0 / m - 1.0;
		float b = (y - (windowY - m) / 2.0) * 2.0 / m - 1.0;
		if (abs(a) < 1.0 && abs(b) < 1.0)
		{
			pontos[moving][0] = a;
			pontos[moving][1] = b;
			glutPostRedisplay();
		}
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	ofstream arquivo;
	ifstream arquivo2("Curva_salva.txt");
	switch (key)
	{
		case '1':
			if (luzY > -2.0)
				luzY -= 0.1;
			break;
		case '2':
			if (luzY < 2.0)
				luzY += 0.1;
			break;
		case '-':
			if (numRotacoes > MINROT)
				numRotacoes -= MINROT;
			break;
		case '+':
			if (numRotacoes < MAXROT)
				numRotacoes += MINROT;
			break;
		case '/':
			if (smooth > MINSMT)
				smooth -= MINSMT;
			break;
		case '*':
			if (smooth < MAXSMT)
				smooth += MINSMT;
			break;
		case '7':
			proj = !proj;
			break;
		case '8':
			wire = !wire;
			break;
		case '9':
			if (full)
			{
				glutReshapeWindow(originalX, originalY);
				windowX = originalX;
				windowY = originalY;
			}
			else
			{
				if (!glutGet(GLUT_SCREEN_WIDTH) || !glutGet(GLUT_SCREEN_HEIGHT))
					cout << "Não é possível entrar no modo tela cheia" << endl;
				else
				{
					originalX = windowX;
					originalY = windowY;
					windowX = glutGet(GLUT_SCREEN_WIDTH);
					windowY = glutGet(GLUT_SCREEN_HEIGHT);
					glutFullScreen();
				}
			}
			full = !full;
			break;
		case '4':
			arquivo.open("Curva_salva.txt");
			arquivo << quant;
			arquivo << endl;
			for (int i = 0; i < quant; i++)
				for (int k = 0; k < 3; k++)
				{
					arquivo << pontos[i][k];
					arquivo << "\n";
				}
			arquivo.close();
			break;
		case '5':
			getline(arquivo2, str_aux);
			quant = atoi(str_aux.c_str());
			for (int i = 0; i < quant; i++)
				for (int k = 0; k < 3; k++)
				{
					getline(arquivo2, str_aux);
					pontos[i][k] = atof(str_aux.c_str());
				}
			break;
		case '0':
			if (quant > 3)
			{
				for (int i = 0; i < 3; i++)
					createPoint(pontos[i][0], pontos[i][1]);
				moving = -1;
			}
			break;
		case 27:
			exit(0);
	}
	glutPostRedisplay();
}

string toString(int n)
{
	if (n == 0)
		return "0";
	string s = "";
	string t = "";
	while (n)
	{
		s += (char) ('0' + n % 10);
		n /= 10;
	}
	int size = s.size();
	for (int i = 0; i < size; i++)
		t += s[size-i-1];
	return t;
}

float dist(float a, float b, float c, float d)
{
	return sqrt((c - a) * (c - a) + (d - b) * (d - b));
}

void createPoint(float x, float y)
{
	if (abs(x) < 1.0 && abs(y) < 1.0 && quant < MAXNUM)
	{
		pontos[quant][0] = x;
		pontos[quant][1] = y;
		pontos[quant][2] = 0;
		moving = quant++;
	}
}

void erasePoint(int p)
{
	while (++p < quant)
	{
		pontos[p-1][0] = pontos[p][0];
		pontos[p-1][1] = pontos[p][1];
		pontos[p-1][2] = pontos[p][2];
	}
	quant--;
}

void selectPoint(int x, int y, bool left)
{
	int m = windowX < windowY ? windowX : windowY;
	float a = (x - (windowX - m) / 2.0) * 2.0 / m - 1.0;
	float b = (y - (windowY - m) / 2.0) * 2.0 / m - 1.0;
	float menor = dist(orthoDim[0], orthoDim[2], orthoDim[1], orthoDim[3]);
	int p = -1;
	for (int i = 0; i < quant; i++)
	{
		float d = dist(pontos[i][0], pontos[i][1], a, b);
		if (d < menor && d < LONGE)
		{
			menor = d;
			p = i;
		}
	}
	if (p >= 0 && !left)
		erasePoint(p);
	if (p >= 0 && left)
		moving = p;
	if (p < 0 && left)
		createPoint(a, b);
	glutPostRedisplay();
}

void setMaterial(GLfloat *ambiente, GLfloat *difusa, GLfloat *especular, GLfloat *brilho, GLfloat *emissao)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambiente);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, difusa);
	glMaterialfv(GL_FRONT, GL_SPECULAR, especular);
	glMaterialfv(GL_FRONT, GL_SHININESS, brilho);
	glMaterialfv(GL_FRONT, GL_EMISSION, emissao);
}

void desenhaCurva(void)
{
	if (quant > 3)
	{
		float vet[4][2];
		for (int i = 0; i < quant - 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				vet[j][0] = pontos[i+j][0];
				vet[j][1] = pontos[i+j][1];
			}
			BSpline(vet);
		}
	}
}

void BSpline(float vet[4][2])
{
	float pontoAux[2];
	float intervalo;
	float a = 0;
	float p[2][smooth+1][3];
	float mtrIntrv[4];
	float mtrTrnsf[4][4] = {{-1,  3, -3, 1}, 
				{ 3, -6,  3, 0}, 
				{-3,  0,  3, 0}, 
				{ 1,  4,  1, 0}};
	glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= smooth; i++)
		{
			for (int j = 0; j < 2; j++)
				pontoAux[j] = 0;
			intervalo = (float) i / smooth;
			mtrIntrv[0] = intervalo * intervalo * intervalo;
			mtrIntrv[1] = intervalo * intervalo;
			mtrIntrv[2] = intervalo;
			mtrIntrv[3] = 1;
			for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++)
				{
					pontoAux[0] += mtrIntrv[k] * (1.0 / 6) * mtrTrnsf[k][j] * vet[j][0];
					pontoAux[1] += mtrIntrv[k] * (1.0 / 6) * mtrTrnsf[k][j] * vet[j][1];
					p[0][i][0] = pontoAux[0] * cos(a * M_PI / 180);
					p[0][i][1] = pontoAux[1];
					p[0][i][2] = sin(a * M_PI / 180);
				}
			if (!proj)
				glVertex2f(pontoAux[0], pontoAux[1]);
		}
	glEnd();
	if (!proj)
		return;
	for (int t = 0; t < numRotacoes; t++)
	{
		a += 360.0 / numRotacoes;
		for (int i = 0; i <= smooth; i++)
		{
			for (int j = 0; j < 2; j++)
				pontoAux[j] = 0;
			intervalo = (float) i / smooth;
			mtrIntrv[0] = intervalo * intervalo * intervalo;
			mtrIntrv[1] = intervalo * intervalo;
			mtrIntrv[2] = intervalo;
			mtrIntrv[3] = 1;
			for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++)
				{
					pontoAux[0] += mtrIntrv[k] * (1.0 / 6) * mtrTrnsf[k][j] * vet[j][0];
					pontoAux[1] += mtrIntrv[k] * (1.0 / 6) * mtrTrnsf[k][j] * vet[j][1];
					p[1][i][0] = pontoAux[0] * cos(a * M_PI / 180);
					p[1][i][1] = pontoAux[1];
					p[1][i][2] = pontoAux[0] * sin(a * M_PI / 180);
				}
		}
		wire ? glBegin(GL_LINE_STRIP) : glBegin(GL_TRIANGLE_STRIP);
			for (int i = 1; i <= smooth; i++)
			{
				float v1[3], v2[3], normal[3];
				for (int j = 0; j < 3; j++)
				{
					v1[j] = p[1][i][j] - p[1][i-1][j];
					v2[j] = p[0][i-1][j] - p[1][i-1][j];
				}
				for (int j = 0; j < 3; j++)
					normal[j] = (v1[(j+1)%3] * v2[(j+2)%3]) - (v1[(j+2)%3] * v2[(j+1)%3]);
				setMaterial(object_ambient, object_difusa, object_especular, object_brilho, object_emissao);
				glNormal3fv(normal);
				glVertex3fv(p[1][i-1]);
				glVertex3fv(p[1][i]);
				glVertex3fv(p[0][i-1]);
				glVertex3fv(p[0][i]);
			}
		glEnd();
		for (int i = 0; i <= smooth; i++)
			for (int j = 0; j < 3; j++)
				p[0][i][j] = p[1][i][j];
	}
}
