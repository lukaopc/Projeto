///Bomberman v1.0 - Lucas e Felipe.


/**
Implementa a movimentação simples de uma camera em primeira pessoa para um personagem que anda
sobre um plano.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <gl/glut.h>
#include <memory>
#include <vector>

//openal (sound lib)
#include <al/alut.h>

//bitmap class to load bitmaps for textures
#include "bitmap.h"

//handle for the al.obj model
#include "ModelAl.h"

#include "ModelBomberman.h"

#include "Map.h"


#include "Bomb.h"

//handle generic obj models
#include "3DObject.h"

#include "Timer.h"

#include "Item.h"

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")

#define PI 3.14159265

// sound stuff
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

void mainInit();
void initSound();
void initTexture();
void initModel();
void initLight();
void createGLUI();
void mainRender();
void mainCreateMenu();
void onMouseButton(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onGLUIEvent(int id);
void onWindowReshape(int x, int y);
void mainIdle();
int main(int argc, char **argv);
void setWindow();
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void updateState();
void renderFloor();
void updateCam();
void renderMinimap();
void cutSceneMinimap(float x, float y, float w, float h);

void drawTextOnScene(char *string);
void glEnable2D();
void glDisable2D();
void renderMapAndTimer();
void initModelBomb();


/**
Screen dimensions
*/
int windowWidth = 600;
int windowHeight = 650;

/**
Screen position
*/
int windowXPos = 300;
int windowYPos = 50;

int mainWindowId = 0;

double xOffset = -1.9;
double yOffset = -1.3;
int mouseLastX = 0;
int mouseLastY = 0;
bool loadedModels = false;
float roty = 0.0f;
float rotx = 90.0f;

bool rightPressed = false;
bool leftPressed = false;
bool upPressed = false;
bool downPressed = false;

bool spacePressed = false;

bool changeCameraView = false;
int viewCameraNumber=3;

float speedX = 0.0f;
float speedY = 0.0f;
float speedZ = 0.0f;

float posX = 0.0f;
float posY = 0.0f;
float posZ = 2.0f;
float rotation = 0.0f;
GLfloat light_2_spot_direction[]={0.0,1.0,-1.0};

/*
variavel auxiliar pra dar variação na altura do ponto de vista ao andar.
*/
float headPosAux = 0.0f;

float maxSpeed = 0.25f;

float planeSize = 8.0f;

// more sound stuff (position, speed and orientation of the listener)
ALfloat listenerPos[]= {0.0,0.0,4.0};
ALfloat listenerVel[]= {0.0,0.0,0.0};
ALfloat listenerOri[]= {0.0,0.0,1.0,
	0.0,1.0,0.0
};

// now the position and speed of the sound source
ALfloat source0Pos[]= { -2.0, 0.0, 0.0};
ALfloat source0Vel[]= { 0.0, 0.0, 0.0};

// buffers for openal stuff
ALuint  buffer[NUM_BUFFERS];
ALuint  source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];
ALsizei size,freq;
ALenum  format;
ALvoid  *data;



// parte de código extraído de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff
int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte	    *bitsOfModelMap;
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */
GLenum      type;            /* Texture type */
GLuint      texture[4];         /* Texture object */



bool crouched = false;
bool running = false;
bool jumping = false;
float jumpSpeed = 0.06;
float gravity = 0.004;
float heightLimit = 0.2;
float posYOffset = 1.0;

float backgrundColor[4] = {1.0f,1.0f,1.0f,1.0f};

Map map = Map();

//Bomb bomb;
CTimer goTimer[5];
bool activeBombs[5] = {false, false, false, false, false};
Coordinate bombCoordinate[5];
CModelAl bombTest;
bool bombCreated = false;
clock_t beginTimeBomb;
int numberBombsCreated=0;
CModelAl fireTest[4]; //fogos

Coordinate itemCoordinate;
Bomb bombPlanted[5];

void setWindow()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(posX+ 0.5*sin(roty*PI/180),posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)),posZ-0.5*cos(roty*PI/180),
		posX + 2*sin(roty*PI/180),posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -2*cos(roty*PI/180),
		0.0,1.0,0.0);
}

/**
Atualiza a posição e orientação da camera
*/
void updateCam()
{
	if(viewCameraNumber==3) { //se for 3, troca pra 1
		//viewCameraNumber=1;
		gluLookAt(posX+ 0.5*sin(roty*PI/180),posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)),posZ-0.5*cos(roty*PI/180),
			posX + 2*sin(roty*PI/180),posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -2*cos(roty*PI/180),
			0.0,1.0,0.0);
	}else if(viewCameraNumber==1) {
		//viewCameraNumber=2;
		setViewport(0, windowWidth, 0, windowHeight);
		gluLookAt(0.0, map.width()+10, 0.001, //eye
			0.0, 1.0, 0.0, //center
			0.0, 1.0, 0.0); //up
	}else if(viewCameraNumber==2){
		//viewCameraNumber=3;
		gluLookAt(posX- 4.5*sin(roty*PI/180), posY + 3 + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)), posZ+4.5*cos(roty*PI/180),
			posX + 2*sin(roty*PI/180),posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -2*cos(roty*PI/180),
			0.0,1.0,0.0);
	}

	// atualiza a posição do listener e da origen do som, são as mesmas da camera, já que os passos vem de onde o personagem está
	listenerPos[0] = posX;
	listenerPos[1] = posY;
	listenerPos[2] = posZ;
	source0Pos[0] = posX;
	source0Pos[1] = posY;
	source0Pos[2] = posZ;
}

void initLight()
{
	glEnable(GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHT1 );

	GLfloat light_ambient[] = { backgrundColor[0], backgrundColor[1], backgrundColor[2], backgrundColor[3] };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position1[] = {0.0, 0.0, 0.0, 1.0 };


	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
}

void setViewport(GLint left, GLint right, GLint bottom, GLint top)
{
	glViewport(left, bottom, right - left, top - bottom);
}

void initTimer(){
	for(i=0; i<5; i++) {
		if (!goTimer[i].Init()) {
			//Cleanup();
			cout << "ERROR: failed to initialize the clock. Program terminated." << endl;
			exit(0);
		}
	}
}

/**
Initialize
*/
void mainInit()
{
	std::srand(std::time(0));

	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);

	// habilita remocao de faces ocultas
	glFrontFace (GL_CCW);
	glEnable(GL_CULL_FACE);

	// habilita o z-buffer
	glEnable(GL_DEPTH_TEST);

	// inicializa textura do chao
	initTexture();

	// carrega o mapa
	map.load();

	// carrega as texturas do mapa
	map.player()->loadTexture(&texture[1]);

	// load the boxes
	std::vector<std::shared_ptr<Machine>> enemies = map.enemies();
	std::vector<std::shared_ptr<Machine>>::iterator enemiesIt;
	for (enemiesIt = enemies.begin() ; enemiesIt != enemies.end(); ++enemiesIt) {
		(*enemiesIt)->loadTexture(&texture[1]);
	}

	// load the boxes
	std::vector<std::shared_ptr<DestructibleWall>> destructibleWall = map.destructibleWalls();
	std::vector<std::shared_ptr<DestructibleWall>>::iterator boxesIt;
	for (boxesIt = destructibleWall.begin() ; boxesIt != destructibleWall.end(); ++boxesIt) {
		(*boxesIt)->loadTexture(&texture[2]);
	}

	// configura o tamanho do plano conforme o mapa
	planeSize = map.width();

	posX = map.player()->position().x;
	posY = map.player()->position().y;
	posZ = map.player()->position().z;

	initSound();

	initModel();
	initModelBomb();

	initLight();

	initTimer();

	printf("w - andar \n");
	printf("s - ir pra tras \n");
	printf("a - gira 90 graus sentido anti-horario \n");
	printf("d - gira 90 graus sentido horario \n");
	printf("espaco - solta bomba \n");

}

void initModel()
{
	bombTest = CModelAl();
	bombTest.Init();

	for(int i=0; i < 4; i++){
		fireTest[i] = CModelAl();
		fireTest[i].Init();
	}
}

void initModelBomb(){
	for(int i=0; i < 5; i++){
		bombPlanted[i] = Bomb();
		bombPlanted[i].init();
		bombPlanted[i].loadTexture(&texture[3]);
	}
}


/**
Initialize openal and check for errors
*/
void initSound()
{

	printf("Initializing OpenAl \n");

	// Init openAL
	alutInit(0, NULL);

	alGetError(); // clear any error messages

	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);

	if(alGetError() != AL_NO_ERROR)
	{
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else
	{
		printf("init() - No errors yet.\n");
	}

	alutLoadWAVFile("..\\resources\\audio\\Footsteps.wav",&format,&data,&size,&freq,false);
	alBufferData(buffer[0],format,data,size,freq);

	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if(alGetError() != AL_NO_ERROR)
	{
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else
	{
		printf("init - no errors after alGenSources\n");
	}

	listenerPos[0] = posX;
	listenerPos[1] = posY;
	listenerPos[2] = posZ;

	source0Pos[0] = posX;
	source0Pos[1] = posY;
	source0Pos[2] = posZ;

	alListenerfv(AL_POSITION,listenerPos);
	alListenerfv(AL_VELOCITY,listenerVel);
	alListenerfv(AL_ORIENTATION,listenerOri);

	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 1.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER,buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);

	printf("Sound ok! \n\n");
}

/**
Initialize the texture
*/
void initTexture(void)
{

	printf ("\nLoading texture..\n");
	// Load a texture object (256x256 true color)
	bits = LoadDIBitmap("..\\resources\\images\\grass.bmp", &info);
	if (bits == (GLubyte *)0)
	{
		printf ("Error loading texture!\n\n");
		return;
	}


	// Figure out the type of texture
	if (info->bmiHeader.biHeight == 1)
		type = GL_TEXTURE_1D;
	else
		type = GL_TEXTURE_2D;

	// Create and bind a texture object
	glGenTextures(2, &texture[0]);
	glBindTexture(type, texture[0]);

	// Create an RGBA image
	rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);

	i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
	for( rgbaptr = rgba, ptr = bits;  i > 0; i--, rgbaptr += 4, ptr += 3)
	{
		rgbaptr[0] = ptr[2];     // windows BMP = BGR
		rgbaptr[1] = ptr[1];
		rgbaptr[2] = ptr[0];
		rgbaptr[3] = (ptr[0] + ptr[1] + ptr[2]) / 3;
	}

	// Set texture parameters
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(type, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );

	printf("Textura %d\n", texture);
	printf("Textures ok.\n\n", texture);
}


void renderFloor()
{
	glPushMatrix();
	glBindTexture(type, texture[0]);

	// set things up to render the floor with the texture
	glShadeModel(GL_SMOOTH);
	glEnable(type);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTranslatef(-(float)planeSize/2.0f, 0.0f, -(float)planeSize/2.0f);

	float textureScaleX = 1.0;
	float textureScaleY = 1.0;
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	int xQuads = 40;
	int zQuads = 40;
	for (int i = 0; i < xQuads; i++)
	{
		for (int j = 0; j < zQuads; j++)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f);   // coords for the texture
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(i * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);

			glTexCoord2f(0.0f, 0.0f);  // coords for the texture
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);

			glTexCoord2f(0.0f, 1.0f);  // coords for the texture
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);

			glTexCoord2f(1.0f, 1.0f);  // coords for the texture
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(i * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);

			glEnd();
		}
	}

	glDisable(type);
	glPopMatrix();
}

void renderScene()
{
	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//minimap rendering	
	renderMinimap();

	setViewport(0, windowWidth, 0, 480);
	updateCam();
	map.collectItems();
	renderFloor();
	map.draw();
	renderMapAndTimer();

	if(map.enemies().size()==0){
		glEnable2D();
			glColor3f(0.2, 0.8, 0);
			drawTextOnScene("PARABENS!! VOCE VENCEU!! :)))");
		glDisable2D();
	}

	if( map.player() && map.collidesWithEnemy(*(map.player()))) {
		//aqui algum inimigo encostou no jogador
		glEnable2D();
			glColor3f(0.2, 0.8, 0);
			drawTextOnScene("Inimigo te matou!! :( ");
		glDisable2D();
	}
	if( !map.player() ) {
		//aqui o player ja morreu
		glEnable2D();
			glColor3f(0.2, 0.8, 0);
			drawTextOnScene("Aff, voce se matou!! :S ");
		glDisable2D();
	}

}

void renderMapAndTimer() {

	// bombs timer
	int nSeconds[5];
	
	for(int i = 0; i < 5; i++) {
		nSeconds[i] = (int)goTimer[i].GetTimeSeconds();
	}

	for(int i = 0; i < 5; i++) {
		if(activeBombs[i]) {
			goTimer[i].Update();
			if(nSeconds[i] < 3.0 ){
				bombPlanted[i].setPosition(bombCoordinate[i]);
				bombPlanted[i].draw();
			} else{
				// number of collisions: collisions in the range + central block
				int numberOfCollisions = map.player()->bombsRange() * 4 + 1;
				Coordinate *collisions = (Coordinate *) malloc(numberOfCollisions*sizeof(Coordinate));

				// generate all the collisions of this bomb
				int collisionsCount = 0;
				int bombRange = map.player()->bombsRange();
				for(int d = -bombRange; d <= bombRange; d++) {
					collisions[collisionsCount].x = bombCoordinate[i].x + d * Block::EDGE_SIZE;
					collisions[collisionsCount].y = bombCoordinate[i].y;
					collisions[collisionsCount].z = bombCoordinate[i].z;

					collisionsCount += 1;

					if(d != 0) {
						// d = 0: central block. Was already added above...
						collisions[collisionsCount].x = bombCoordinate[i].x;
						collisions[collisionsCount].y = bombCoordinate[i].y;
						collisions[collisionsCount].z = bombCoordinate[i].z + d * Block::EDGE_SIZE;

						collisionsCount += 1;
					}
				}

				for(int j = 0; j < numberOfCollisions; j++) {
					Block collisionBlock = Block(collisions[j], bombPlanted[i].collisionSideSize());
					if(map.collidesWithSomething(collisionBlock)){
						map.killWhatCollidesWith(collisionBlock);
					}
				}
				activeBombs[i] = false;
				numberBombsCreated -= 1;
			}
		}
	}
}

void renderMinimap(){
	if(viewCameraNumber==3 || viewCameraNumber==2){
		/* Perspective view */
		//minimap
		glPushMatrix();
		glViewport(windowWidth/2-100, windowHeight-windowHeight/4, windowWidth/3.5, windowHeight/4);
		gluLookAt(0.0, map.width()+10, 0.001, //eye
			0.0, 1.0, 0.0, //center
			0.0, 1.0, 0.0); //up
		renderFloor();
		map.draw();
		renderMapAndTimer();
		glPopMatrix();

	}	
}

void cutSceneMinimap(float x, float y, float w, float h){
	glScissor(x,y,w,h);
    glViewport(x,y,w,h);
}

void drawTextOnScene(char *string)
{
	//glPushMatrix();

	// Posição onde o texto será colocado
	glRasterPos2f(-7.4, 5);

	// Exibe caracter a caracter
	while(*string)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *string++);
	}

	//glPopMatrix();
}

void glEnable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix;
	glLoadIdentity;
	gluOrtho2D(0, windowWidth, 0, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix;
	glLoadIdentity;

	glDisable(GL_DEPTH_TEST);
}

void glDisable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix;
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix;

	glEnable(GL_DEPTH_TEST);
}

void updateState()
{
	if (upPressed || downPressed)
	{
		// efeito de "sobe e desce" ao andar
		headPosAux += 8.5f;
		if (headPosAux > 180.0f)
		{
			headPosAux = 0.0f;
		}

		if (upPressed)
		{
			map.player()->moveForward();
		}
		else
		{
			map.player()->moveBackward();
		}

		if(map.collidesWithWall(*map.player()) || map.collidesWithBox(*map.player())
			|| map.collidesWithEnemy(*map.player())) {
				// if the movement generated a collision, back the movement
				if (upPressed)
				{
					map.player()->moveBackward();
				}
				else
				{
					map.player()->moveForward();
				}
		}

		posX = map.player()->position().x;
		posZ = map.player()->position().z;
	}
	else
	{
		// parou de andar, para com o efeito de "sobe e desce"
		headPosAux = fmod(headPosAux, 90) - 1 * headPosAux / 90;
		headPosAux -= 4.0f;
		if (headPosAux < 0.0f)
		{
			headPosAux = 0.0f;
		}
	}

	posY += speedY;
	posYOffset += 0.01;
	if (posYOffset > 0.2)
	{
		posYOffset = 0.2;
	}


	if(changeCameraView)
	{
		changeCameraView=false;
		if(viewCameraNumber==3) {
			viewCameraNumber=1;
		}else if(viewCameraNumber==1){
			viewCameraNumber=2;
		}else if(viewCameraNumber==2){
			viewCameraNumber=3;
		}
	}

	if(leftPressed)
	{
		map.player()->rotateModel(-90.0f);
		roty -= 90;
		leftPressed=false;
	}

	if(rightPressed)
	{
		map.player()->rotateModel(90.0f);
		roty += 90;
		rightPressed=false;
	}

	if(spacePressed){
		if(map.player()) {
			if(numberBombsCreated < map.player()->bombs()){
				int freePosition = 0;
				for(int i=0; i<5; i++) {
					if(!activeBombs[i]) {
						freePosition = i;
						break;
					}
				}
				bombCreated = true;
				bombCoordinate[freePosition] = map.player()->position();
				goTimer[freePosition].Init();
				activeBombs[freePosition] = true;
				numberBombsCreated += 1;
			}
		}
		spacePressed=false;
	}

	map.executeMachine();
	
	// check collisions with bombs
	std::vector<std::shared_ptr<Machine>> enemies = map.enemies();
	std::vector<std::shared_ptr<Machine>>::iterator enemiesIt;
	for (enemiesIt = enemies.begin() ; enemiesIt != enemies.end(); ++enemiesIt) {
		for(int i=0; i < 5; i++) {
			if(activeBombs[i] && (*enemiesIt)->collidesWith(bombPlanted[i])) {
				(*enemiesIt)->moveBackward();
			}
		}
	}
}

/**
Render scene
*/
void mainRender()
{
	updateState();
	renderScene();
	glFlush();
	glutPostRedisplay();
	Sleep(30);
}

/**
Handles events from the mouse right button menu
*/
void mainHandleMouseRightButtonMenuEvent(int option)
{
	switch (option)
	{
	case 1 :
		exit(0);
		break;
	default:
		break;
	}
}

/**
Create mouse button menu
*/
void mainCreateMenu()
{
	glutCreateMenu(mainHandleMouseRightButtonMenuEvent);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


/**
Key press event handler
*/
void onKeyDown(unsigned char key, int x, int y)
{

	//printf("%d \n", key);
	switch (key)
	{
	case 32: //space
		spacePressed = true;
		break;
	case 119: //w
		if (!upPressed)
		{
			alSourcePlay(source[0]);
		}
		upPressed = true;
		break;
	case 115: //s
		downPressed = true;
		break;
	case 97: //a
		leftPressed = true;
		break;
	case 100: //d
		rightPressed = true;
		break;
	case 99: //c
		crouched = true;
		break;
	case 114: //r
		running = true;
		break;
	case 118: //v
		changeCameraView = true;
		break;
	default:
		break;
	}

	//glutPostRedisplay();
}

/**
Key release event handler
*/
void onKeyUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 32: //space
		spacePressed = false;
		break;
	case 119: //w
		if (upPressed)
		{
			alSourceStop(source[0]);
		}
		upPressed = false;
		break;
	case 115: //s
		downPressed = false;
		break;
	case 97: //a
		leftPressed = false;
		break;
	case 100: //d
		rightPressed = false;
		break;
	case 99: //c
		crouched = false;
		break;
	case 114: //r
		running = false;
		break;
	case 27:
		exit(0);
		break;
	case 118: //v
		changeCameraView = false;
		break;
	default:
		break;
	}

	//glutPostRedisplay();
}

void SpecialInput(int key, int x, int y) {
	switch(key) {
	case GLUT_KEY_UP:
		if (upPressed)
		{
			alSourcePlay(source[0]);
		}
		upPressed = true;
		break;	
	case GLUT_KEY_DOWN:
		downPressed = true;
		break;
	case GLUT_KEY_LEFT:
		leftPressed = true;
		break;
	case GLUT_KEY_RIGHT:
		rightPressed = true;
		break;
	}
}

void SpecialInputUp(int key, int x, int y) {
	switch(key) {
	case GLUT_KEY_UP:
		if (upPressed)
		{
			alSourceStop(source[0]);
		}
		upPressed = false;
		break;	
	case GLUT_KEY_DOWN:
		downPressed = false;
		break;
	case GLUT_KEY_LEFT:
		leftPressed = false;
		break;
	case GLUT_KEY_RIGHT:
		rightPressed = false;
		break;
	}
}

void onWindowReshape(int x, int y)
{
	windowWidth = x;
	windowHeight = y;
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);
}

/**
Glut idle funtion
*/
void mainIdle()
{
	/**
	Set the active window before send an glutPostRedisplay call
	so it wont be accidently sent to the glui window
	*/
	glutSetWindow(mainWindowId);
	glutPostRedisplay();
	/*	goTimer.Update();

	if (!gnSeconds && !gnHour && !gnMinutes && !gnMilli)
	{
	gnSeconds = (int)goTimer.GetTimeSeconds();
	gnHour = (int)gnSeconds / 3600;
	gnMinutes = (int)((gnSeconds % 3600) / 60);
	gnMilli = (int)((goTimer.GetTimeSeconds() - (double)gnSeconds) * 1000.0);
	gnSeconds = gnSeconds % 60;
	}
	*/


}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(windowXPos,windowYPos);

	/**
	Store main window id so that glui can send it redisplay events
	*/
	mainWindowId = glutCreateWindow("Bomberman - Lucas e Felipe");

	glutDisplayFunc(mainRender);

	glutReshapeFunc(onWindowReshape);


	/**
	Register keyboard events handlers
	*/
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialFunc(SpecialInput);
	glutSpecialUpFunc(SpecialInputUp); 

	mainInit();

	glutMainLoop();

	return 0;
}
