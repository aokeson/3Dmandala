/*
 *  Mandala
 *
 *  CSCI 5229
 *  Fall 2016 Semester Project
 *  Allows users to create a mandala by drawing 3 dimensional shapes and then automatically
 *  rotating those shapes about the y axis.
 *
 *  Key bindings:
 *  i               Toggle between normal and instructor demo mode
 *  m               Toggle between orthogonal, perspective, and perspective with 1st person navigation
 *  x               Toggle axes
 *  0               Reset view angle
 *  ESC             Exit
 *
 *  Orthogonal/Perspective Navigation
 *  arrows          Move left and right/change view angle
 *  +/-             Zoom in and out
 *  
 *  1st person Navigation
 *  up/down arrow   Move eye up and down
 *  w/s             Move into and out of scene
 *  a/d             Move eye left and right
 *  
 */


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include "shapes.h"
#include "button.h"

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int winw=1000;  
int winh=650;

int instructor=0;   // Instructor demo mode
int th=0;           // Azimuth of view angle
int ph=0;           // Elevation of view angle
int axes=1;         // Display axes
int mode=0;         // Change view
int fov=55;         // Field of view (for perspective)
double asp=1;       // Aspect ratio
double dim=5.0;     // Size of world

// 1st person perspective variables
float Ax=0;     // Position
float Ay=0;
float Az=10;
float Ex=0;     // Direction
float Ey=0;
float Ez=0;

double w2h;     // width to height ratio of the screen

// Light values
int ambient   =  50;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  GLUT calls this routine when the window is resized
 */
void Resize(int w, int h)
{
	w2h = (h>0) ? (double)w/h : 1;

	winw = w;
	winh = h;

	/*
	 *	Allow drawing in full region of the screen
	 */
	glViewport(0,0,w,h);
}

// Find RGB values for colors based on the color number
double color(int shapeNumber,int component)
{
    if (shapeNumber==1)
    {
        if (component==1) return 1;
        else return 0;
    }
    else if (shapeNumber==2)
    {
        if (component==1) return 1;
        else if (component==2) return 0.6471;
        else return 0;
    }
    else if (shapeNumber==3)
    {
        if (component==3) return 0;
        else return 1;
    }
    else if (shapeNumber==4)
    {
        if (component==2) return 0.502;
        else return 0;
    }
    else if (shapeNumber==5)
    {
        if (component==3) return 1;
        else return 0;
    }
    else if (shapeNumber==6)
    {
        if (component==1) return 0.5804;
        else if (component==2) return 0;
        else return 0.82745;
    }
    else
        return 1;
}


/*----------------------------------------------------------------------------------------
 *	This function will be used to draw the 3D scene
 *  The base code for this function came from Rob Bateman's button code
 */
void Draw3D()
{
	const double len=1.5;  //  Length of axes

    //  Orthogonal - set world orientation
	if (mode == 0)
	{
    	glRotatef(ph,1,0,0);
    	glRotatef(th,0,1,0);
  	}
    //  Perspective - set eye position
	else if (mode == 1)
	{
    	double Ex = -2*dim*Sin(th)*Cos(ph);
    	double Ey = +2*dim        *Sin(ph);
    	double Ez = +2*dim*Cos(th)*Cos(ph);
    	gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
	}
	//  1st person perspective
	else
	{
    	Ex = Cos(th-90) * Cos(ph);
    	Ey = Sin(ph);
    	Ez = Sin(th-90) * Cos(ph);

    	glMatrixMode(GL_MODELVIEW);
    	glLoadIdentity();
    	gluLookAt(Ax, Ay, Az, Ax + Ex, Ay + Ey, Az + Ez, 0.0,1.0,0.0);
	}

	glShadeModel(GL_SMOOTH);

	//  Translate intensity to color vectors
    float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
    float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
    float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
    //  Light positions
    float Position[]  = {xlight,ylight,zlight,1.0};
    //  Draw light position as ball (still no lighting here)
    //glColor3f(1,1,1);
    ball(Position[0],Position[1],Position[2] , 0.1);
    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,0);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    //  Enable lights
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
    glLightfv(GL_LIGHT0,GL_POSITION,Position);

	// Draw each shape symmetry number of times around y axis
    for (int i=0;i<symmetry;i++)
    {
        // Draw each shape
        for (int k=0;k<10;k++)
        {
            double hypotenuse = sqrt(shapes[k][1]*shapes[k][1]+shapes[k][3]*shapes[k][3]);
            double symmetryAngle = i*360.0/symmetry;
            double angle = atan2(shapes[k][3], shapes[k][1])*(180.0/3.1415927)+symmetryAngle;
            // Coordinates of shapes around axis
            double newX = hypotenuse*Cos(angle);
            double newY = shapes[k][2];
            double newZ = hypotenuse*Sin(angle);
    		if (shapes[k][0]==1)
        		cube(newX,newY,newZ,shapes[k][4],shapes[k][5],shapes[k][6],shapes[k][7],shapes[k][8]+symmetryAngle,shapes[k][9],color(shapes[k][10],1),color(shapes[k][10],2),color(shapes[k][10],3),shapes[k][11]);
        	else if (shapes[k][0]==2)
    			sphere(newX,newY,newZ,shapes[k][4],shapes[k][5],shapes[k][6],shapes[k][7],shapes[k][8]+symmetryAngle,shapes[k][9],color(shapes[k][10],1),color(shapes[k][10],2),color(shapes[k][10],3),shapes[k][11]);
    		else if (shapes[k][0]==3)
        		cylinder(newX,newY,newZ,shapes[k][4],shapes[k][5],shapes[k][6],shapes[k][7],shapes[k][8]+symmetryAngle,shapes[k][9],color(shapes[k][10],1),color(shapes[k][10],2),color(shapes[k][10],3),shapes[k][11]);
        	else if (shapes[k][0]==4)
        		pyramid(newX,newY,newZ,shapes[k][4],shapes[k][5],shapes[k][6],shapes[k][7],shapes[k][8]+symmetryAngle,shapes[k][9],color(shapes[k][10],1),color(shapes[k][10],2),color(shapes[k][10],3),shapes[k][11]);
    		else if (shapes[k][0]==5)
    			cone(newX,newY,newZ,shapes[k][4],shapes[k][5],shapes[k][6],shapes[k][7],shapes[k][8]+symmetryAngle,shapes[k][9],color(shapes[k][10],1),color(shapes[k][10],2),color(shapes[k][10],3),shapes[k][11]);
        }
    }
    
    glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	//  White
    glColor3f(1,1,1);
    //  Draw axes
    if (axes)
    {
		glBegin(GL_LINES);
		glVertex3d(0.0,0.0,0.0);
		glVertex3d(len,0.0,0.0);
		glVertex3d(0.0,0.0,0.0);
		glVertex3d(0.0,len,0.0);
		glVertex3d(0.0,0.0,0.0);
		glVertex3d(0.0,0.0,len);
		glEnd();
		//  Label axes
		glRasterPos3d(len,0.0,0.0);
		Print("X");
		glRasterPos3d(0.0,len,0.0);
		Print("Y");
		glRasterPos3d(0.0,0.0,len);
		Print("Z");
	}
}


/*----------------------------------------------------------------------------------------
 *	This is the main display callback function. It sets up the drawing for 
 *	The 3D scene first then calls the Draw3D() function. After that it switches to 
 *	an orthographic projection and calls ButtonDraw().
 *  The base code for this function came from Rob Bateman's button code
 */
void Draw()
{
	/*
	 *	Clear the background
	 */
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/*
	 *	Enable lighting and the z-buffer
	 */
	glEnable(GL_DEPTH_TEST);

	/*
	 *	Set perspective viewing transformation
	 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (mode==0)
		glOrtho(-w2h*dim,+w2h*dim, -dim,+dim, -dim,+dim);
	else
		gluPerspective(fov,w2h*asp,dim/4,4*dim);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*
	 *	Draw the 3D elements in the scene
	 */
	Draw3D();

	/*
	 *	Disable depth test for 2D elements
	 */
	glDisable(GL_DEPTH_TEST);

	/*
	 *	Set the orthographic viewing transformation
	 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,winw,winh,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*
	 *	Draw the 2D buttons
	 */
	ButtonDraw();

	/*
	 *	Bring the back buffer to the front and vice-versa.
	 */
	glutSwapBuffers();
}


/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
	// Movement for non 1st person perspective
    if (mode==0 || mode==1)
	{
    	//  Right arrow key - increase angle by 5 degrees
    	if (key == GLUT_KEY_RIGHT)
        	th += 5;
    	//  Left arrow key - decrease angle by 5 degrees
    	else if (key == GLUT_KEY_LEFT)
        	th -= 5;
    	//  Up arrow key - increase elevation by 5 degrees
    	else if (key == GLUT_KEY_UP)
        	ph += 5;
    	//  Down arrow key - decrease elevation by 5 degrees
    	else if (key == GLUT_KEY_DOWN)
        	ph -= 5;
    	//  Keep angles to +/-360 degrees
    	th %= 360;
    	ph %= 360;
	}
	// Movement for 1st person perspective
    else if (mode==2)
	{
		// Look up
        if (key == GLUT_KEY_UP)
		{
			ph -= 5;
        	Ex = Cos(th-90);
        	Ey = Sin(th-90);
        	Ez = Cos(ph);
		}
		// Look down
        else if (key == GLUT_KEY_DOWN)
		{
        	ph += 5;
        	Ex = Cos(th-90);
        	Ey = Sin(th-90);
        	Ez = Cos(ph);
		}
        //  Keep angles to +/-360 degrees
		th %= 360;
    	ph %= 360;
	}
	//  Tell GLUT it is necessary to redisplay the scene
	glutPostRedisplay();
}


/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}


/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
	// Movement for 1st person perspective
    if (mode == 2)
	{
    	//  Look right
    	if (ch == 'd')
    	{
        	th += 5;
        	Ex = Cos(th-90);
        	Ey = Sin(th-90);
        	Ez = 0;
    	}
    	//  Look left
    	else if (ch == 'a')
    	{
        	th -= 5;
        	Ex = Cos(th-90);
        	Ey = Sin(th-90);
        	Ez = 0;
    	}
    	//  Move forward
    	else if (ch == 'w')
    	{
        	float ax = Cos(th-90)*Cos(ph);
        	float ay = Sin(ph);
        	float az = Sin(th-90)*Cos(ph);
        	Ax = Ax + ax;
        	Ay = Ay + ay;
         	Az = Az + az;
    	}
    	//  Move backward
    	else if (ch == 's')
    	{
        	float ax = Cos(th-90)*Cos(ph);
        	float ay = Sin(ph);
        	float az = Sin(th-90)*Cos(ph);
        	Ax = Ax - ax;
        	Ay = Ay - ay;
        	Az = Az - az;
    	}
    	//  Keep angles to +/-360 degrees
    	th %= 360;
    	ph %= 360;
	}
	// Orthogonal field of view
    if (mode==0)
	{
		//  Change field of view angle
		if (ch == '-' && ch>1)
			dim += 0.1;
		else if (ch == '+' && ch<179)
			dim -= 0.1;
	}
	// Perspective field of view
    if (mode==1)
	{
		//  Change field of view angle
        if (ch == '-' && ch>1)
			fov++;
		else if (ch == '+' && ch<179)
			fov--;
	}
	//  Exit on ESC
	if (ch == 27)
		exit(0);
	//  Reset view angle
	else if (ch == '0')
		th = ph = 0;
	//  Toggle axes
	else if (ch == 'x' || ch == 'X')
		axes = 1-axes;
	//  Switch display mode
	else if (ch == 'm')
	{
    	if (mode == 2)
    		mode = 0;
    	else
        	mode++;
	}
    //  Switch instructor demo mode
    else if (ch =='i')
    {
        if (instructor == 0)
        {
            instructor = 1;
            changeToInstructor();
        }
        else if (instructor == 1)
        {
            instructor = 2;
            changeToAwesome();
        }
        else
        {
            instructor = 0;
            changeToNormal();
        }
    }

	//  Tell GLUT it is necessary to redisplay the scene
	glutPostRedisplay();
}


/*----------------------------------------------------------------------------------------
 *
 */
int main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize(winw,winh);
	glutInitWindowPosition(200,100);
	glutCreateWindow("03 - Mouse Motion");
	//  Tell GLUT to call "idle" when there is nothing else to do
   	glutIdleFunc(idle);
	glutDisplayFunc(Draw);
	glutReshapeFunc(Resize);
	//  Tell GLUT to call "special" when an arrow key is pressed
    glutSpecialFunc(special);
    //  Tell GLUT to call "key" when a key is pressed
    glutKeyboardFunc(key);
    //  Tell GLUT to call "MouseButton" when a button is pressed
	glutMouseFunc(MouseButton);
    //  Tell GLUT to call "MouseMotion" when the mouse moves
	glutMotionFunc(MouseMotion);
    //  Tell GLUT to call "MousePassiveMotion" to keep track of the mouse movements
	glutPassiveMotionFunc(MousePassiveMotion);
	Init();
    // Load all the images into shapes.h
    loadImages();

	glutMainLoop();
	return 0;
}
