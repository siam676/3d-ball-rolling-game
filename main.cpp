#include <GL/freeglut.h>
#include <cmath>
#include <string>
#include <ctime>


//GLOBAL VARIABLES


float ballX = 0, ballY = 0.5f, ballZ = 0;
float velX = 0, velZ = 0;
float ballRot = 0;
float ballScale = 1.0f;

float camX = 0, camY = 10, camZ = 20;

bool lightOn = true;
bool topView = false;

bool wPressed = false;
bool aPressed = false;
bool sPressed = false;
bool dPressed = false;

float fanRotate = 0;

int score = 0;
int coinsCollected = 0;

const float MAP_SIZE = 40.0f;

// Bridge Constants
const float BRIDGE_X = 12.0f;
const float BRIDGE_Z = 0.0f;
const float BRIDGE_W = 4.0f;
const float BRIDGE_L = 15.0f;
const float BRIDGE_H = 2.5f;


//OBJECTS


const int obstacleCount = 100;
float obstaclePos[obstacleCount][2];

struct Coin {
    float x, z;
    bool active;
};

const int coinCount = 50;
Coin coins[coinCount];


//INIT OBJECTS


void initGameObjects() {
    srand(time(NULL));

    for(int i=0; i<obstacleCount; i++) {
        obstaclePos[i][0] = (rand() % (int)(MAP_SIZE * 1.9f)) - (MAP_SIZE * 0.95f);
        obstaclePos[i][1] = (rand() % (int)(MAP_SIZE * 1.9f)) - (MAP_SIZE * 0.95f);

        // Keep spawn clear
        if(abs(obstaclePos[i][0]) < 5 && abs(obstaclePos[i][1]) < 5) {
            obstaclePos[i][0] += 10;
        }
        // Keep bridge clear
        if(abs(obstaclePos[i][0] - BRIDGE_X) < 5 && abs(obstaclePos[i][1] - BRIDGE_Z) < 10) {
             obstaclePos[i][0] -= 15;
        }
    }

    for(int i=0; i<coinCount; i++) {
        coins[i].x = (rand() % (int)(MAP_SIZE * 1.8f)) - (MAP_SIZE * 0.9f);
        coins[i].z = (rand() % (int)(MAP_SIZE * 1.8f)) - (MAP_SIZE * 0.9f);
        coins[i].active = true;
    }
}


//DRAWING HELPERS


void drawText(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    for(char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void drawSky() {
    glDisable(GL_LIGHTING);

    glPushMatrix();
    glColor3f(0.45f, 0.75f, 1.0f);
    glTranslatef(0, 40, 0);
    glutSolidSphere(200, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 0.9f, 0.2f);
    glTranslatef(50, 55, -80);
    glutSolidSphere(5, 30, 30);
    glPopMatrix();

    for(int i = -3; i <= 3; i++) {
        glPushMatrix();
        glColor3f(0.2f, 0.4f, 0.2f);
        glTranslatef(i * 30, -5, -120);
        glutSolidCone(20, 30, 20, 20);
        glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(-25, 0, -60);
    glColor3f(0.6f, 0.6f, 0.6f);
    glScalef(10, 5, 10);
    glutSolidCube(1);
    glPopMatrix();

    for(int i = 0; i < 15; i++) {
        float x = -60 + i * 8;
        float z = -55;

        glPushMatrix();
        glTranslatef(x, 0, z);

        glColor3f(0.4f, 0.2f, 0.1f);
        glPushMatrix();
        glTranslatef(0, 1, 0);
        glScalef(0.5, 2, 0.5);
        glutSolidCube(1);
        glPopMatrix();

        glColor3f(0.1f, 0.6f, 0.1f);
        glTranslatef(0, 3, 0);
        glutSolidSphere(1.5, 10, 10);

        glPopMatrix();
    }

    if(lightOn)
        glEnable(GL_LIGHTING);
}

void drawGround() {
    glBegin(GL_QUADS);
    for(int x=-MAP_SIZE; x<MAP_SIZE; x++) {
        for(int z=-MAP_SIZE; z<MAP_SIZE; z++) {
            if((x+z)%2==0) glColor3f(0.85f, 0.85f, 0.85f);
            else glColor3f(0.45f, 0.45f, 0.45f);
            glNormal3f(0, 1, 0);
            glVertex3f(x, 0, z);
            glVertex3f(x+1, 0, z);
            glVertex3f(x+1, 0, z+1);
            glVertex3f(x, 0, z+1);
        }
    }
    glEnd();
}


//WALLS & ROOM FEATURES


void drawWalls() {
    float h = 5.0f;

    // FRONT WALL
    for(float x=-MAP_SIZE; x<MAP_SIZE; x+=1.0f) {
        for(float y=0; y<h; y+=1.0f) {
            bool windowGap = (x > -3 && x < 3 && y > 1 && y < 4);
            bool pictureGap = (x > -18 && x < -12 && y > 1 && y < 4);
            if(windowGap || pictureGap) continue;

            if(((int)x + (int)y) % 2 == 0) glColor3f(0.60f,0.20f,0.15f);
            else glColor3f(0.75f,0.30f,0.20f);

            glPushMatrix();
            glTranslatef(x, y+0.5f, -MAP_SIZE);
            glutSolidCube(1);
            glPopMatrix();
        }
    }

    // WINDOW
    glPushMatrix();
    glTranslatef(0, 2.5, -MAP_SIZE+0.05);
    glColor3f(0.4f, 0.7f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-3,-1.5,0); glVertex3f(3,-1.5,0); glVertex3f(3,1.5,0); glVertex3f(-3,1.5,0);
    glEnd();

    // FRAME
    glColor3f(0.35f, 0.2f, 0.05f);
    glLineWidth(6);
    glBegin(GL_LINES);
    glVertex3f(-3,-1.5,0.01); glVertex3f(3,-1.5,0.01);
    glVertex3f(3,-1.5,0.01); glVertex3f(3,1.5,0.01);
    glVertex3f(3,1.5,0.01); glVertex3f(-3,1.5,0.01);
    glVertex3f(-3,1.5,0.01); glVertex3f(-3,-1.5,0.01);
    glVertex3f(0,-1.5,0.01); glVertex3f(0,1.5,0.01);
    glVertex3f(-3,0,0.01); glVertex3f(3,0,0.01);
    glEnd();
    glPopMatrix();


    // UPDATED PICTURE FRAME


    glPushMatrix();
    glTranslatef(-15, 2.5, -MAP_SIZE + 0.05);

    // frame (thicker & nicer)
    glColor3f(0.35f, 0.20f, 0.10f);
    glBegin(GL_QUADS);
    glVertex3f(-2.2f, -1.6f, 0);
    glVertex3f( 2.2f, -1.6f, 0);
    glVertex3f( 2.2f,  1.6f, 0);
    glVertex3f(-2.2f,  1.6f, 0);
    glEnd();

    // inner border
    glColor3f(0.55f, 0.35f, 0.15f);
    glBegin(GL_QUADS);
    glVertex3f(-2.0f, -1.4f, 0.01f);
    glVertex3f( 2.0f, -1.4f, 0.01f);
    glVertex3f( 2.0f,  1.4f, 0.01f);
    glVertex3f(-2.0f,  1.4f, 0.01f);
    glEnd();

    // painting background (sky + ground look)
    glBegin(GL_QUADS);

    // sky part
    glColor3f(0.4f, 0.7f, 1.0f);
    glVertex3f(-2.0f, 0.0f, 0.02f);
    glVertex3f( 2.0f, 0.0f, 0.02f);
    glVertex3f( 2.0f,  1.4f, 0.02f);
    glVertex3f(-2.0f,  1.4f, 0.02f);

    // ground part
    glColor3f(0.2f, 0.6f, 0.3f);
    glVertex3f(-2.0f, -1.4f, 0.02f);
    glVertex3f( 2.0f, -1.4f, 0.02f);
    glVertex3f( 2.0f, 0.0f, 0.02f);
    glVertex3f(-2.0f, 0.0f, 0.02f);

    glEnd();

    // small sun in picture
    glPushMatrix();
    glTranslatef(1.2f, 0.8f, 0.03f);
    glColor3f(1.0f, 0.9f, 0.2f);
    glutSolidSphere(0.25, 20, 20);
    glPopMatrix();

    glPopMatrix();

    // BACK WALL
    for(float x=-MAP_SIZE; x<MAP_SIZE; x+=1.0f) {
        for(float y=0; y<h; y+=1.0f) {
            if(((int)x + (int)y)%2==0) glColor3f(0.60f,0.20f,0.15f);
            else glColor3f(0.75f,0.30f,0.20f);
            glPushMatrix();
            glTranslatef(x, y+0.5f, MAP_SIZE);
            glutSolidCube(1);
            glPopMatrix();
        }
    }

    // LEFT WALL
    for(float z=-MAP_SIZE; z<MAP_SIZE; z+=1.0f) {
        for(float y=0; y<h; y+=1.0f) {
            if(((int)z + (int)y)%2==0) glColor3f(0.60f,0.20f,0.15f);
            else glColor3f(0.75f,0.30f,0.20f);
            glPushMatrix();
            glTranslatef(-MAP_SIZE, y+0.5f, z);
            glutSolidCube(1);
            glPopMatrix();
        }
    }

    // RIGHT WALL
    for(float z=-MAP_SIZE; z<MAP_SIZE; z+=1.0f) {
        for(float y=0; y<h; y+=1.0f) {
            if(((int)z + (int)y)%2==0) glColor3f(0.60f,0.20f,0.15f);
            else glColor3f(0.75f,0.30f,0.20f);
            glPushMatrix();
            glTranslatef(MAP_SIZE, y+0.5f, z);
            glutSolidCube(1);
            glPopMatrix();
        }
    }
}


// BRIDGE & FAN


void drawBridge() {
    glPushMatrix();
    glTranslatef(BRIDGE_X, 0, BRIDGE_Z);
    for (float i = -BRIDGE_L / 2; i <= BRIDGE_L / 2; i += 0.8f) {
        float h = BRIDGE_H * cos((i / (BRIDGE_L / 2)) * (3.14159 / 3.5));
        glPushMatrix();
        glTranslatef(0, h, i);
        glScalef(BRIDGE_W, 0.2f, 0.7f);
        glColor3f(0.4f, 0.25f, 0.1f);
        glutSolidCube(1.0);
        glPopMatrix();

        // Railings
        glColor3f(0.3f, 0.15f, 0.05f);
        glPushMatrix();
        glTranslatef(BRIDGE_W/2 - 0.2f, h + 0.4f, i);
        glutSolidCube(0.3);
        glTranslatef(-(BRIDGE_W - 0.4f), 0, 0);
        glutSolidCube(0.3);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawFan() {
    glPushMatrix();
    glTranslatef(0, 10, 0);
    glRotatef(fanRotate, 0, 1, 0);
    for(int i=0; i<4; i++) {
        glRotatef(90, 0, 1, 0);
        glPushMatrix();
        glTranslatef(3, 0, 0);
        glScalef(6, 0.1, 0.8);
        glColor3f(0.7, 0.7, 0.7);
        glutSolidCube(1);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawCoin(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.7, z);
    glRotatef(fanRotate*2.5, 0, 1, 0);
    glColor3f(1.0, 0.84, 0.0);
    glutSolidTorus(0.08, 0.25, 10, 20);
    glPopMatrix();
}


//UPDATE & PHYSICS


void update(int v) {
    float acc = 0.025f;
    if(wPressed) velZ -= acc;
    if(sPressed) velZ += acc;
    if(aPressed) velX -= acc;
    if(dPressed) velX += acc;

    velX *= 0.96f; velZ *= 0.96f;
    ballX += velX; ballZ += velZ;
    ballRot += sqrt(velX*velX + velZ*velZ) * 60;
    fanRotate += 8.0f;

    // BRIDGE PHYSICS
    float relX = ballX - BRIDGE_X;
    float relZ = ballZ - BRIDGE_Z;

    if (abs(relX) < BRIDGE_W / 2.0f && abs(relZ) < BRIDGE_L / 2.0f) {
        float archH = BRIDGE_H * cos((relZ / (BRIDGE_L / 2.0f)) * (3.14159 / 3.5));
        ballY = (0.5f * ballScale) + archH;
    } else {
        if (ballY > 0.5f * ballScale) {
            ballY -= 0.15f;
            if (ballY < 0.5f * ballScale) ballY = 0.5f * ballScale;
        }
        // Side hit bridge
        if (abs(relZ) < BRIDGE_L / 2.0f && abs(relX) < (BRIDGE_W / 2.0f + 0.3f)) {
             if (ballY < BRIDGE_H * 0.5f) {
                 ballX -= velX * 2.0f; velX *= -0.5f;
             }
        }
    }

    // OBSTACLES
    for(int i=0; i<obstacleCount; i++) {
        float dx = ballX - obstaclePos[i][0];
        float dz = ballZ - obstaclePos[i][1];
        float hitBox = (0.5f * ballScale) + 0.55f;
        if(sqrt(dx*dx + dz*dz) < hitBox && ballY < 1.5f) {
            ballX -= velX * 1.8f; ballZ -= velZ * 1.8f;
            velX *= -0.5f; velZ *= -0.5f;
        }
    }

    // COINS
    for(int i=0; i<coinCount; i++) {
        if(coins[i].active) {
            float dx = ballX - coins[i].x;
            float dz = ballZ - coins[i].z;
            if(sqrt(dx*dx + dz*dz) < (0.5f * ballScale + 0.5f)) {
                coins[i].active = false; score += 250; coinsCollected++;
            }
        }
    }

    // BOUNDS
    float limit = MAP_SIZE - 1;
    if(abs(ballX) > limit) { ballX = (ballX > 0 ? limit : -limit); velX *= -0.7f; }
    if(abs(ballZ) > limit) { ballZ = (ballZ > 0 ? limit : -limit); velZ *= -0.7f; }

    camX += (ballX - camX) * 0.05f;
    camZ += ((ballZ + 18) - camZ) * 0.05f;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


//DISPLAY


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if(topView) gluLookAt(ballX, 50, ballZ, ballX, 0, ballZ, 0, 0, -1);
    else gluLookAt(camX, camY + (ballScale*2), camZ, ballX, ballY, ballZ, 0, 1, 0);

    if(lightOn) {
        glEnable(GL_LIGHTING);
        float p[] = {0, 30, 0, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, p);
    } else glDisable(GL_LIGHTING);

    drawSky();
    drawGround();
    drawWalls();
    drawBridge();
    drawFan();

    // Obstacles
    for(int i=0; i<obstacleCount; i++) {
        glPushMatrix();
        glTranslatef(obstaclePos[i][0], 0.5, obstaclePos[i][1]);
        glColor3f(0.7, 0.1, 0.1);
        glutSolidCube(1);
        glColor3f(1, 1, 1);
        glutWireCube(1.01);
        glPopMatrix();
    }

    // Coins
    for(int i=0; i<coinCount; i++) {
        if(coins[i].active) drawCoin(coins[i].x, coins[i].z);
    }

    // Ball
    glPushMatrix();
    glTranslatef(ballX, ballY, ballZ);
    glScalef(ballScale, ballScale, ballScale);
    glRotatef(ballRot, 1, 0, 1);
    glColor3f(0.2f, 0.5f, 1.0f);
    glutSolidSphere(0.5, 30, 30);
    glPopMatrix();

    // UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);
    drawText(20, 560, "Score: " + std::to_string(score));
    drawText(20, 530, "Coins: " + std::to_string(coinsCollected) + " / " + std::to_string(coinCount));
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
    if(key=='w') wPressed = true;
    if(key=='s') sPressed = true;
    if(key=='a') aPressed = true;
    if(key=='d') dPressed = true;
    if(key=='q') ballScale += 0.1f;
    if(key=='e') ballScale -= 0.1f;
    if(ballScale < 0.2f) ballScale = 0.2f;
    if(key=='v') topView = !topView;
    if(key=='l') lightOn = !lightOn;
    if(key==27) exit(0);
}

void keyboardUp(unsigned char key, int x, int y) {
    if(key=='w') wPressed = false;
    if(key=='s') sPressed = false;
    if(key=='a') aPressed = false;
    if(key=='d') dPressed = false;
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)w/h, 1, 500);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    initGameObjects();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("3d ball rolling game of graphics 444.1");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
