/*
 *  I got this code from https://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/opengl_programming.html#6
 *  It adds 2D buttons to the screen
 *  My only edits to this file were adding instances of buttons I need
 *  and writing the functions to handle those button clicks.
 *  This file is also where my global variables for the shapes, light, and axes of symmetry are stored.
 */

/*----------------------------------------------------------------------------------------
 *	
 *	project:	05_Overlay1
 *	author:		Rob Bateman
 *	note:		
 *				Quite often you may want to draw an overlay of 2D graphics over your 3D
 *				Data. This source file gives an example of how to do it. 
 *
 *				Essentually all you have to do is to set a 3D viewing projection, draw
 *				your 3D items, switch to orthographic display and then draw your 2D items
 *				over the top.  
 *
 *				In order to make the code a bit more readable, two functions have been 
 *				provided, Draw2D() & Draw3D(). These are called from within the Draw() 
 *				function. 
 */

/*----------------------------------------------------------------------------------------
 *	Includes
 */


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

float xlight=0;		// coordinates of the light
float ylight=0;
float zlight=0;

int symmetry=4;		// Number of axes of symmetry

double shapes[10][12]={{0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0},
					   {0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0},
					   {0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0},
					   {0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0},
					   {0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0},
					   {0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0},
					   {0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0},
					   {0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0},
					   {0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0},
					   {0, 0,0,0, 0.25,0.25,0.25, 0,0,0, 0, 0}};					   

/*----------------------------------------------------------------------------------------
 *	Global Variables
 */

/*
 *	A structure to represent the mouse information
 */
struct Mouse 
{
	int x;		/*	the x coordinate of the mouse cursor	*/
	int y;		/*	the y coordinate of the mouse cursor	*/
	int lmb;	/*	is the left button pressed?		*/
	int mmb;	/*	is the middle button pressed?	*/
	int rmb;	/*	is the right button pressed?	*/

	/*
	 *	These two variables are a bit odd. Basically I have added these to help replicate
	 *	the way that most user interface systems work. When a button press occurs, if no
	 *	other button is held down then the co-ordinates of where that click occured are stored.
	 *	If other buttons are pressed when another button is pressed it will not update these
	 *	values. 
	 *
	 *	This allows us to "Set the Focus" to a specific portion of the screen. For example,
	 *	in maya, clicking the Alt+LMB in a view allows you to move the mouse about and alter
	 *	just that view. Essentually that viewport takes control of the mouse, therefore it is
	 *	useful to know where the first click occured.... 
	 */
	int xpress; /*	stores the x-coord of when the first button press occurred	*/
	int ypress; /*	stores the y-coord of when the first button press occurred	*/
};

/*
 *	rename the structure from "struct Mouse" to just "Mouse"
 */
typedef struct Mouse Mouse;

/*
 *	Create a global mouse structure to hold the mouse information.
 */
Mouse TheMouse = {0,0,0,0,0};


// Set constants to display an example scene
void changeToInstructor()
{
    symmetry = 7;
    for (int count=0;count<10;count++){
    	shapes[count][0]=count%6;
		shapes[count][1]=(count/1.9)*pow(-1,count);
		shapes[count][2]=((count%2)*10-count)/2.3*pow(-1,count)*pow(-1,2*count)+2;
		shapes[count][3]=count/3.23;
		shapes[count][4]=0.15*(1+(count%2));
		shapes[count][5]=0.1*(1+(count%4));
		shapes[count][6]=0.05*(1+(count%6));
		shapes[count][7]=15*(count%3);
		shapes[count][8]=15*(count%4);
		shapes[count][9]=15*(count%5);
		shapes[count][10]=(10-count)%7;
		shapes[count][11]=count%8;
    }
}

// Set constants back to initial values
void changeToNormal()
{
	symmetry = 4;
	for (int count=0;count<10;count++){
		shapes[count][0]=0;
		shapes[count][1]=0;
		shapes[count][2]=0;
		shapes[count][3]=0;
		shapes[count][4]=0.25;
		shapes[count][5]=0.25;
		shapes[count][6]=0.25;
		shapes[count][7]=0;
		shapes[count][8]=0;
		shapes[count][9]=0;
		shapes[count][10]=1;
		shapes[count][11]=1;
		shapes[count][12]=1;
		shapes[count][13]=0;
	}
}

// Set constants to awesome demo
void changeToAwesome()
{
	symmetry = 75;
	shapes[0][0]=1;
	shapes[0][1]=1;
	shapes[0][2]=1;
	shapes[0][3]=0;
	shapes[0][4]=0.25;
	shapes[0][5]=0.25;
	shapes[0][6]=0.25;
	shapes[0][7]=0;
	shapes[0][8]=0;
	shapes[0][9]=0;
	shapes[0][10]=1;
	shapes[0][11]=1;
	shapes[0][12]=1;
	shapes[0][13]=0;
	for (int count=1;count<10;count++){
		shapes[count][0]=0;
		shapes[count][1]=0;
		shapes[count][2]=0;
		shapes[count][3]=0;
		shapes[count][4]=0.25;
		shapes[count][5]=0.25;
		shapes[count][6]=0.25;
		shapes[count][7]=0;
		shapes[count][8]=0;
		shapes[count][9]=0;
		shapes[count][10]=1;
		shapes[count][11]=1;
		shapes[count][12]=1;
		shapes[count][13]=0;
	}
}


/*----------------------------------------------------------------------------------------
 *	Button Stuff
 */

/*
 *	We will define a function pointer type. ButtonCallback is a pointer to a function that
 *	looks a bit like this :
 *
 *	void func() { 
 *	}
 */
typedef void (*ButtonCallback)();

/*
 *	This is a simple structure that holds a button.
 */
struct Button 
{
	int   x;							/* top left x coord of the button */
	int   y;							/* top left y coord of the button */
	int   w;							/* the width of the button */
	int   h;							/* the height of the button */
	int	  state;						/* the state, 1 if pressed, 0 otherwise */
	int	  highlighted;					/* is the mouse cursor over the control? */
	char* label;						/* the text label of the button */
	ButtonCallback callbackFunction;	/* A pointer to a function to call if the button is pressed */

	int id;								/* A unique ID to represent this Button */

	struct Button* next;				/* a pointer to the next node in the linked list */
};
typedef struct Button Button;

/*
 *	The start of a linked list of buttons
 */
Button* pButtonList = NULL;

int GlobalRef=0;

/*
 *		
 */
int CreateButton(char *label,ButtonCallback cb,int x,int y,int w,int h)
{
	Button* p = (Button*)malloc( sizeof(Button) );
	assert(p);
	memset(p,0,sizeof(Button));
	p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
	p->callbackFunction = cb;
	p->label = (char*)malloc( strlen(label)+1 );
	if(p->label)
		sprintf(p->label,label);

	p->next = pButtonList;
	pButtonList = p;

	return p->id = ++GlobalRef;
}

int DeleteButtonByName(char *label)
{
	Button* previous=NULL,*curr=pButtonList;
	while (curr!=NULL) {
		if (strcmp(label,curr->label)==0) {
			if(previous)
				previous->next = curr->next;
			else
				pButtonList = curr->next;

			if (curr->label) {

				free(curr->label);
			}
			free(curr);
			return 1;
		}
		previous = curr;
		curr = curr->next;
	}
	return 0;
}

int DeleteButtonById(int id)
{
	Button	*previous = NULL       ,
			*curr     = pButtonList;

	while (curr!=NULL) 
	{
		if ( id == curr->id )
		{
			if(previous)
				previous->next = curr->next;
			else
				pButtonList = curr->next;

			if (curr->label) {

				free(curr->label);
			}
			free(curr);
			return 1;
		}
		previous = curr;
		curr = curr->next;
	}
	return 0;
}


/*----------------------------------------------------------------------------------------
 *	\brief	This function draws a text string to the screen using glut bitmap fonts.
 *	\param	font	-	the font to use. it can be one of the following : 
 *
 *					GLUT_BITMAP_9_BY_15		
 *					GLUT_BITMAP_8_BY_13			
 *					GLUT_BITMAP_TIMES_ROMAN_10	
 *					GLUT_BITMAP_TIMES_ROMAN_24	
 *					GLUT_BITMAP_HELVETICA_10	
 *					GLUT_BITMAP_HELVETICA_12	
 *					GLUT_BITMAP_HELVETICA_18	
 *
 *	\param	text	-	the text string to output
 *	\param	x		-	the x co-ordinate
 *	\param	y		-	the y co-ordinate
 */
void Font(void *font,char *text,int x,int y)
{
	glRasterPos2i(x, y);

	while( *text != '\0' )
	{
		glutBitmapCharacter( font, *text );
		++text;
	}
}


/*----------------------------------------------------------------------------------------
 *	\brief	This function is used to see if a mouse click or event is within a button 
 *			client area.
 *	\param	b	-	a pointer to the button to test
 *	\param	x	-	the x coord to test
 *	\param	y	-	the y-coord to test
 */
int ButtonClickTest(Button* b,int x,int y) 
{
	if( b) 
	{
		/*
		 *	If clicked within button area, then return true
		 */
	    if( x > b->x      && 
			x < b->x+b->w &&
			y > b->y      && 
			y < b->y+b->h ) {
				return 1;
		}
	}

	/*
	 *	otherwise false.
	 */
	return 0;
}

/*----------------------------------------------------------------------------------------
 *	\brief	This function draws the specified button.
 *	\param	b	-	a pointer to the button to check.
 *	\param	x	-	the x location of the mouse cursor.
 *	\param	y	-	the y location of the mouse cursor.
 */
void ButtonRelease(int x,int y)
{
	Button* b = pButtonList;
	while(b) 
	{
		/*
		 *	If the mouse button was pressed within the button area
		 *	as well as being released on the button.....
		 */
		if( ButtonClickTest(b,TheMouse.xpress,TheMouse.ypress) && 
			ButtonClickTest(b,x,y) )
		{
			/*
			 *	Then if a callback function has been set, call it.
			 */
			if (b->callbackFunction) {
				b->callbackFunction();
			}
		}

		/*
		 *	Set state back to zero.
		 */
		b->state = 0;

		b=b->next;
	}
}

/*----------------------------------------------------------------------------------------
 *	\brief	This function draws the specified button.
 *	\param	b	-	a pointer to the button to check.
 *	\param	x	-	the x location of the mouse cursor.
 *	\param	y	-	the y location of the mouse cursor.
 */
void ButtonPress(int x,int y)
{
	Button* b = pButtonList;
	while(b) 
	{
		/*
		 *	if the mouse click was within the buttons client area, 
		 *	set the state to true.
		 */
		if( ButtonClickTest(b,x,y) )
		{
			b->state = 1;
		}
		b=b->next;
	}
}


/*----------------------------------------------------------------------------------------
 *	\brief	This function draws the specified button.
 *	\param	b	-	a pointer to the button to check.
 *	\param	x	-	the x location of the mouse cursor.
 *	\param	y	-	the y location of the mouse cursor.
 */
void ButtonPassive(int x,int y)
{
	int needRedraw = 0;
	Button* b = pButtonList;
	while(b) 
	{
		/*
		 *	if the mouse moved over the control
		 */
		if( ButtonClickTest(b,x,y) )
		{
			/*
			 *	If the cursor has just arrived over the control, set the highlighted flag
			 *	and force a redraw. The screen will not be redrawn again until the mouse
			 *	is no longer over this control
			 */
			if( b->highlighted == 0 ) {
				b->highlighted = 1;
				needRedraw=1;
			}
		}
		else

		/*
		 *	If the cursor is no longer over the control, then if the control
		 *	is highlighted (ie, the mouse has JUST moved off the control) then
		 *	we set the highlighting back to false, and force a redraw. 
		 */
		if( b->highlighted == 1 )
		{
			b->highlighted = 0;
			needRedraw=1;
		}
		b=b->next;
	}
	if (needRedraw) {
		glutPostRedisplay();
	}
}

/*----------------------------------------------------------------------------------------
 *	\brief	This function draws the specified button.
 */
void ButtonDraw()
{
	int fontx;
	int fonty;

	Button* b = pButtonList;
	while(b) 
	{
		/*
		 *	We will indicate that the mouse cursor is over the button by changing its
		 *	colour.
		 */
		if (b->highlighted) 
			glColor3f(0.7f,0.7f,0.8f);
		else 
			glColor3f(0.6f,0.6f,0.6f);

		/*
		 *	draw background for the button.
		 */
		glBegin(GL_QUADS);
			glVertex2i( b->x     , b->y      );
			glVertex2i( b->x     , b->y+b->h );
			glVertex2i( b->x+b->w, b->y+b->h );
			glVertex2i( b->x+b->w, b->y      );
		glEnd();

		/*
		 *	Draw an outline around the button with width 3
		 */
		glLineWidth(3);

		/*
		 *	The colours for the outline are reversed when the button. 
		 */
		if (b->state) 
			glColor3f(0.4f,0.4f,0.4f);
		else 
			glColor3f(0.8f,0.8f,0.8f);

		glBegin(GL_LINE_STRIP);
			glVertex2i( b->x+b->w, b->y      );
			glVertex2i( b->x     , b->y      );
			glVertex2i( b->x     , b->y+b->h );
		glEnd();

		if (b->state) 
			glColor3f(0.8f,0.8f,0.8f);
		else 
			glColor3f(0.4f,0.4f,0.4f);

		glBegin(GL_LINE_STRIP);
			glVertex2i( b->x     , b->y+b->h );
			glVertex2i( b->x+b->w, b->y+b->h );
			glVertex2i( b->x+b->w, b->y      );
		glEnd();

		glLineWidth(1);


		/*
		 *	Calculate the x and y coords for the text string in order to center it.
		 */
		const unsigned char * hello = b->label;
		fontx = b->x + (b->w - glutBitmapLength(GLUT_BITMAP_HELVETICA_10,hello)) / 2 ;
		fonty = b->y + (b->h+10)/2;

		/*
		 *	if the button is pressed, make it look as though the string has been pushed
		 *	down. It's just a visual thing to help with the overall look....
		 */
		if (b->state) {
			fontx+=2;
			fonty+=2;
		}

		/*
		 *	If the cursor is currently over the button we offset the text string and draw a shadow
		 */
		if(b->highlighted)
		{
			glColor3f(0,0,0);
			Font(GLUT_BITMAP_HELVETICA_10,b->label,fontx,fonty);
			fontx--;
			fonty--;
		}

		glColor3f(1,1,1);
		Font(GLUT_BITMAP_HELVETICA_10,b->label,fontx,fonty);

		b = b->next;
	}
}


/*----------------------------------------------------------------------------------------
 *	These are the callback functions. Notice that it's type is the same
 *	an the ButtonCallback type. We can assign a pointer to this function which
 *	we can store and later call.
 */

static void symmetryUp()
{
	symmetry++;
}

static void symmetryDown()
{
	if (symmetry!=1)
		symmetry-=1;
}

static void lightLeft()
{
	xlight-=0.5;
}
static void lightRight()
{
	xlight+=0.5;
}
static void lightBack()
{
	zlight-=0.5;
}
static void lightForward()
{
	zlight+=0.5;
}
static void lightDown()
{
	ylight-=0.5;
}
static void lightUp()
{
	ylight+=0.5;
}

static void changeShape0(){
	if (shapes[0][0]==5)
		shapes[0][0]=0;
	else
		shapes[0][0]++;
}static void up0(){
	shapes[0][2]+=0.5;
}static void down0(){
	shapes[0][2]-=0.5;
}static void left0(){
	shapes[0][1]-=0.5;
}static void right0(){
	shapes[0][1]+=0.5;
}static void forward0(){
	shapes[0][3]+=0.5;
}static void back0(){
	shapes[0][3]-=0.5;
}static void rotateX0(){
	shapes[0][7]+=15;
}static void rotateY0(){
	shapes[0][8]+=15;
}static void rotateZ0(){
	shapes[0][9]+=15;
}static void scaleXUp0(){
	shapes[0][4]+=0.25;
}static void scaleXDown0(){
	shapes[0][4]-=0.25;
}static void scaleYUp0(){
	shapes[0][5]+=0.25;
}static void scaleYDown0(){
	shapes[0][5]-=0.25;
}static void scaleZUp0(){
	shapes[0][6]+=0.25;
}static void scaleZDown0(){
	shapes[0][6]-=0.25;
}static void changeColor0(){
	if (shapes[0][10]==6)
		shapes[0][10]=0;
	else
		shapes[0][10]++;
}static void changeTexture0(){
	if (shapes[0][11]==7)
		shapes[0][11]=0;
	else
		shapes[0][11]++;
}

static void changeShape1(){
	if (shapes[1][0]==5)
		shapes[1][0]=0;
	else
		shapes[1][0]++;
}static void up1(){
	shapes[1][2]+=0.5;
}static void down1(){
	shapes[1][2]-=0.5;
}static void left1(){
	shapes[1][1]-=0.5;
}static void right1(){
	shapes[1][1]+=0.5;
}static void forward1(){
	shapes[1][3]+=0.5;
}static void back1(){
	shapes[1][3]-=0.5;
}static void rotateX1(){
	shapes[1][7]+=15;
}static void rotateY1(){
	shapes[1][8]+=15;
}static void rotateZ1(){
	shapes[1][9]+=15;
}static void scaleXUp1(){
	shapes[1][4]+=0.25;
}static void scaleXDown1(){
	shapes[1][4]-=0.25;
}static void scaleYUp1(){
	shapes[1][5]+=0.25;
}static void scaleYDown1(){
	shapes[1][5]-=0.25;
}static void scaleZUp1(){
	shapes[1][6]+=0.25;
}static void scaleZDown1(){
	shapes[1][6]-=0.25;
}static void changeColor1(){
	if (shapes[1][10]==6)
		shapes[1][10]=0;
	else
		shapes[1][10]++;
}static void changeTexture1(){
	if (shapes[1][11]==7)
		shapes[1][11]=0;
	else
		shapes[1][11]++;
}

static void changeShape2(){
	if (shapes[2][0]==5)
		shapes[2][0]=0;
	else
		shapes[2][0]++;
}static void up2(){
	shapes[2][2]+=0.5;
}static void down2(){
	shapes[2][2]-=0.5;
}static void left2(){
	shapes[2][1]-=0.5;
}static void right2(){
	shapes[2][1]+=0.5;
}static void forward2(){
	shapes[2][3]+=0.5;
}static void back2(){
	shapes[2][3]-=0.5;
}static void rotateX2(){
	shapes[2][7]+=15;
}static void rotateY2(){
	shapes[2][8]+=15;
}static void rotateZ2(){
	shapes[2][9]+=15;
}static void scaleXUp2(){
	shapes[2][4]+=0.25;
}static void scaleXDown2(){
	shapes[2][4]-=0.25;
}static void scaleYUp2(){
	shapes[2][5]+=0.25;
}static void scaleYDown2(){
	shapes[2][5]-=0.25;
}static void scaleZUp2(){
	shapes[2][6]+=0.25;
}static void scaleZDown2(){
	shapes[2][6]-=0.25;
}static void changeColor2(){
	if (shapes[2][10]==6)
		shapes[2][10]=0;
	else
		shapes[2][10]++;
}static void changeTexture2(){
	if (shapes[2][11]==7)
		shapes[2][11]=0;
	else
		shapes[2][11]++;
}

static void changeShape3(){
	if (shapes[3][0]==5)
		shapes[3][0]=0;
	else
		shapes[3][0]++;
}static void up3(){
	shapes[3][2]+=0.5;
}static void down3(){
	shapes[3][2]-=0.5;
}static void left3(){
	shapes[3][1]-=0.5;
}static void right3(){
	shapes[3][1]+=0.5;
}static void forward3(){
	shapes[3][3]+=0.5;
}static void back3(){
	shapes[3][3]-=0.5;
}static void rotateX3(){
	shapes[3][7]+=15;
}static void rotateY3(){
	shapes[3][8]+=15;
}static void rotateZ3(){
	shapes[3][9]+=15;
}static void scaleXUp3(){
	shapes[3][4]+=0.25;
}static void scaleXDown3(){
	shapes[3][4]-=0.25;
}static void scaleYUp3(){
	shapes[3][5]+=0.25;
}static void scaleYDown3(){
	shapes[3][5]-=0.25;
}static void scaleZUp3(){
	shapes[3][6]+=0.25;
}static void scaleZDown3(){
	shapes[3][6]-=0.25;
}static void changeColor3(){
	if (shapes[3][10]==6)
		shapes[3][10]=0;
	else
		shapes[3][10]++;
}static void changeTexture3(){
	if (shapes[3][11]==7)
		shapes[3][11]=0;
	else
		shapes[3][11]++;
}

static void changeShape4(){
	if (shapes[4][0]==5)
		shapes[4][0]=0;
	else
		shapes[4][0]++;
}static void up4(){
	shapes[4][2]+=0.5;
}static void down4(){
	shapes[4][2]-=0.5;
}static void left4(){
	shapes[4][1]-=0.5;
}static void right4(){
	shapes[4][1]+=0.5;
}static void forward4(){
	shapes[4][3]+=0.5;
}static void back4(){
	shapes[4][3]-=0.5;
}static void rotateX4(){
	shapes[4][7]+=15;
}static void rotateY4(){
	shapes[4][8]+=15;
}static void rotateZ4(){
	shapes[4][9]+=15;
}static void scaleXUp4(){
	shapes[4][4]+=0.25;
}static void scaleXDown4(){
	shapes[4][4]-=0.25;
}static void scaleYUp4(){
	shapes[4][5]+=0.25;
}static void scaleYDown4(){
	shapes[4][5]-=0.25;
}static void scaleZUp4(){
	shapes[4][6]+=0.25;
}static void scaleZDown4(){
	shapes[4][6]-=0.25;
}static void changeColor4(){
	if (shapes[4][10]==6)
		shapes[4][10]=0;
	else
		shapes[4][10]++;
}static void changeTexture4(){
	if (shapes[4][11]==7)
		shapes[4][11]=0;
	else
		shapes[4][11]++;
}

static void changeShape5(){
	if (shapes[5][0]==5)
		shapes[5][0]=0;
	else
		shapes[5][0]++;
}static void up5(){
	shapes[5][2]+=0.5;
}static void down5(){
	shapes[5][2]-=0.5;
}static void left5(){
	shapes[5][1]-=0.5;
}static void right5(){
	shapes[5][1]+=0.5;
}static void forward5(){
	shapes[5][3]+=0.5;
}static void back5(){
	shapes[5][3]-=0.5;
}static void rotateX5(){
	shapes[5][7]+=15;
}static void rotateY5(){
	shapes[5][8]+=15;
}static void rotateZ5(){
	shapes[5][9]+=15;
}static void scaleXUp5(){
	shapes[5][4]+=0.25;
}static void scaleXDown5(){
	shapes[5][4]-=0.25;
}static void scaleYUp5(){
	shapes[5][5]+=0.25;
}static void scaleYDown5(){
	shapes[5][5]-=0.25;
}static void scaleZUp5(){
	shapes[5][6]+=0.25;
}static void scaleZDown5(){
	shapes[5][6]-=0.25;
}static void changeColor5(){
	if (shapes[5][10]==6)
		shapes[5][10]=0;
	else
		shapes[5][10]++;
}static void changeTexture5(){
	if (shapes[5][11]==7)
		shapes[5][11]=0;
	else
		shapes[5][11]++;
}

static void changeShape6(){
	if (shapes[6][0]==5)
		shapes[6][0]=0;
	else
		shapes[6][0]++;
}static void up6(){
	shapes[6][2]+=0.5;
}static void down6(){
	shapes[6][2]-=0.5;
}static void left6(){
	shapes[6][1]-=0.5;
}static void right6(){
	shapes[6][1]+=0.5;
}static void forward6(){
	shapes[6][3]+=0.5;
}static void back6(){
	shapes[6][3]-=0.5;
}static void rotateX6(){
	shapes[6][7]+=15;
}static void rotateY6(){
	shapes[6][8]+=15;
}static void rotateZ6(){
	shapes[6][9]+=15;
}static void scaleXUp6(){
	shapes[6][4]+=0.25;
}static void scaleXDown6(){
	shapes[6][4]-=0.25;
}static void scaleYUp6(){
	shapes[6][5]+=0.25;
}static void scaleYDown6(){
	shapes[6][5]-=0.25;
}static void scaleZUp6(){
	shapes[6][6]+=0.25;
}static void scaleZDown6(){
	shapes[6][6]-=0.25;
}static void changeColor6(){
	if (shapes[6][10]==6)
		shapes[6][10]=0;
	else
		shapes[6][10]++;
}static void changeTexture6(){
	if (shapes[6][11]==7)
		shapes[6][11]=0;
	else
		shapes[6][11]++;
}

static void changeShape7(){
	if (shapes[7][0]==5)
		shapes[7][0]=0;
	else
		shapes[7][0]++;
}static void up7(){
	shapes[7][2]+=0.5;
}static void down7(){
	shapes[7][2]-=0.5;
}static void left7(){
	shapes[7][1]-=0.5;
}static void right7(){
	shapes[7][1]+=0.5;
}static void forward7(){
	shapes[7][3]+=0.5;
}static void back7(){
	shapes[7][3]-=0.5;
}static void rotateX7(){
	shapes[7][7]+=15;
}static void rotateY7(){
	shapes[7][8]+=15;
}static void rotateZ7(){
	shapes[7][9]+=15;
}static void scaleXUp7(){
	shapes[7][4]+=0.25;
}static void scaleXDown7(){
	shapes[7][4]-=0.25;
}static void scaleYUp7(){
	shapes[7][5]+=0.25;
}static void scaleYDown7(){
	shapes[7][5]-=0.25;
}static void scaleZUp7(){
	shapes[7][6]+=0.25;
}static void scaleZDown7(){
	shapes[7][6]-=0.25;
}static void changeColor7(){
	if (shapes[7][10]==6)
		shapes[7][10]=0;
	else
		shapes[7][10]++;
}static void changeTexture7(){
	if (shapes[7][11]==7)
		shapes[7][11]=0;
	else
		shapes[7][11]++;
}

static void changeShape8(){
	if (shapes[8][0]==5)
		shapes[8][0]=0;
	else
		shapes[8][0]++;
}static void up8(){
	shapes[8][2]+=0.5;
}static void down8(){
	shapes[8][2]-=0.5;
}static void left8(){
	shapes[8][1]-=0.5;
}static void right8(){
	shapes[8][1]+=0.5;
}static void forward8(){
	shapes[8][3]+=0.5;
}static void back8(){
	shapes[8][3]-=0.5;
}static void rotateX8(){
	shapes[8][7]+=15;
}static void rotateY8(){
	shapes[8][8]+=15;
}static void rotateZ8(){
	shapes[8][9]+=15;
}static void scaleXUp8(){
	shapes[8][4]+=0.25;
}static void scaleXDown8(){
	shapes[8][4]-=0.25;
}static void scaleYUp8(){
	shapes[8][5]+=0.25;
}static void scaleYDown8(){
	shapes[8][5]-=0.25;
}static void scaleZUp8(){
	shapes[8][6]+=0.25;
}static void scaleZDown8(){
	shapes[8][6]-=0.25;
}static void changeColor8(){
	if (shapes[8][10]==6)
		shapes[8][10]=0;
	else
		shapes[8][10]++;
}static void changeTexture8(){
	if (shapes[8][11]==7)
		shapes[8][11]=0;
	else
		shapes[8][11]++;
}

static void changeShape9(){
	if (shapes[9][0]==5)
		shapes[9][0]=0;
	else
		shapes[9][0]++;
}static void up9(){
	shapes[9][2]+=0.5;
}static void down9(){
	shapes[9][2]-=0.5;
}static void left9(){
	shapes[9][1]-=0.5;
}static void right9(){
	shapes[9][1]+=0.5;
}static void forward9(){
	shapes[9][3]+=0.5;
}static void back9(){
	shapes[9][3]-=0.5;
}static void rotateX9(){
	shapes[9][7]+=15;
}static void rotateY9(){
	shapes[9][8]+=15;
}static void rotateZ9(){
	shapes[9][9]+=15;
}static void scaleXUp9(){
	shapes[9][4]+=0.25;
}static void scaleXDown9(){
	shapes[9][4]-=0.25;
}static void scaleYUp9(){
	shapes[9][5]+=0.25;
}static void scaleYDown9(){
	shapes[9][5]-=0.25;
}static void scaleZUp9(){
	shapes[9][6]+=0.25;
}static void scaleZDown9(){
	shapes[9][6]-=0.25;
}static void changeColor9(){
	if (shapes[9][10]==6)
		shapes[9][10]=0;
	else
		shapes[9][10]++;
}static void changeTexture9(){
	if (shapes[9][11]==7)
		shapes[9][11]=0;
	else
		shapes[9][11]++;
}static void symmetryUp60(){
	symmetry+=60;
}static void symmetryDown60(){
	symmetry-=60;
}


/*----------------------------------------------------------------------------------------
 *	\brief	This function is called to initialise opengl.
 */

void Init()
{
	// Create and place the menu buttons
	glEnable(GL_LIGHT0);
	CreateButton("Change Shape",changeShape0,5,5,75,15);
	CreateButton("Change Color",changeColor0,5,20,75,15);
	CreateButton("Change Texture",changeTexture0,5,35,75,15);
	CreateButton("<",left0,85,20,15,15);
	CreateButton("^",back0,100,5,15,15);
	CreateButton("v",forward0,100,35,15,15);
	CreateButton(">",right0,115,20,15,15);
	CreateButton("^",up0,135,10,15,15);
	CreateButton("v",down0,135,30,15,15);
	CreateButton("Rotate X",rotateX0,155,5,45,15);
	CreateButton("Rotate Y",rotateY0,155,20,45,15);
	CreateButton("Rotate Z",rotateZ0,155,35,45,15);
	CreateButton("Scale X ^",scaleXUp0,205,5,45,15);
	CreateButton("Scale Y ^",scaleYUp0,205,20,45,15);
	CreateButton("Scale Z ^",scaleZUp0,205,35,45,15);
	CreateButton("v",scaleXDown0,250,5,15,15);
	CreateButton("v",scaleYDown0,250,20,15,15);
	CreateButton("v",scaleZDown0,250,35,15,15);

	CreateButton("Change Shape",changeShape1,5,60,75,15);
	CreateButton("Change Color",changeColor1,5,75,75,15);
	CreateButton("Change Texture",changeTexture1,5,90,75,15);
	CreateButton("<",left1,85,75,15,15);
	CreateButton("^",back1,100,60,15,15);
	CreateButton("v",forward1,100,90,15,15);
	CreateButton(">",right1,115,75,15,15);
	CreateButton("^",up1,135,65,15,15);
	CreateButton("v",down1,135,85,15,15);
	CreateButton("Rotate X",rotateX1,155,60,45,15);
	CreateButton("Rotate Y",rotateY1,155,75,45,15);
	CreateButton("Rotate Z",rotateZ1,155,90,45,15);
	CreateButton("Scale X ^",scaleXUp1,205,60,45,15);
	CreateButton("Scale Y ^",scaleYUp1,205,75,45,15);
	CreateButton("Scale Z ^",scaleZUp1,205,90,45,15);
	CreateButton("v",scaleXDown1,250,60,15,15);
	CreateButton("v",scaleYDown1,250,75,15,15);
	CreateButton("v",scaleZDown1,250,90,15,15);

	CreateButton("Change Shape",changeShape2,5,115,75,15);
	CreateButton("Change Color",changeColor2,5,130,75,15);
	CreateButton("Change Texture",changeTexture2,5,145,75,15);
	CreateButton("<",left2,85,130,15,15);
	CreateButton("^",back2,100,115,15,15);
	CreateButton("v",forward2,100,145,15,15);
	CreateButton(">",right2,115,130,15,15);
	CreateButton("^",up2,135,120,15,15);
	CreateButton("v",down2,135,140,15,15);
	CreateButton("Rotate X",rotateX2,155,115,45,15);
	CreateButton("Rotate Y",rotateY2,155,130,45,15);
	CreateButton("Rotate Z",rotateZ2,155,145,45,15);
	CreateButton("Scale X ^",scaleXUp2,205,115,45,15);
	CreateButton("Scale Y ^",scaleYUp2,205,130,45,15);
	CreateButton("Scale Z ^",scaleZUp2,205,145,45,15);
	CreateButton("v",scaleXDown2,250,115,15,15);
	CreateButton("v",scaleYDown2,250,130,15,15);
	CreateButton("v",scaleZDown2,250,145,15,15);

	CreateButton("Change Shape",changeShape3,5,115+55,75,15);
	CreateButton("Change Color",changeColor3,5,130+55,75,15);
	CreateButton("Change Texture",changeTexture3,5,145+55,75,15);
	CreateButton("<",left3,85,130+55,15,15);
	CreateButton("^",back3,100,115+55,15,15);
	CreateButton("v",forward3,100,145+55,15,15);
	CreateButton(">",right3,115,130+55,15,15);
	CreateButton("^",up3,135,120+55,15,15);
	CreateButton("v",down3,135,140+55,15,15);
	CreateButton("Rotate X",rotateX3,155,115+55,45,15);
	CreateButton("Rotate Y",rotateY3,155,130+55,45,15);
	CreateButton("Rotate Z",rotateZ3,155,145+55,45,15);
	CreateButton("Scale X ^",scaleXUp3,205,115+55,45,15);
	CreateButton("Scale Y ^",scaleYUp3,205,130+55,45,15);
	CreateButton("Scale Z ^",scaleZUp3,205,145+55,45,15);
	CreateButton("v",scaleXDown3,250,115+55,15,15);
	CreateButton("v",scaleYDown3,250,130+55,15,15);
	CreateButton("v",scaleZDown3,250,145+55,15,15);

	CreateButton("Change Shape",changeShape4,5,115+110,75,15);
	CreateButton("Change Color",changeColor4,5,130+110,75,15);
	CreateButton("Change Texture",changeTexture4,5,145+110,75,15);
	CreateButton("<",left4,85,130+110,15,15);
	CreateButton("^",back4,100,115+110,15,15);
	CreateButton("v",forward4,100,145+110,15,15);
	CreateButton(">",right4,115,130+110,15,15);
	CreateButton("^",up4,135,120+110,15,15);
	CreateButton("v",down4,135,140+110,15,15);
	CreateButton("Rotate X",rotateX4,155,115+110,45,15);
	CreateButton("Rotate Y",rotateY4,155,130+110,45,15);
	CreateButton("Rotate Z",rotateZ4,155,145+110,45,15);
	CreateButton("Scale X ^",scaleXUp4,205,115+110,45,15);
	CreateButton("Scale Y ^",scaleYUp4,205,130+110,45,15);
	CreateButton("Scale Z ^",scaleZUp4,205,145+110,45,15);
	CreateButton("v",scaleXDown4,250,115+110,15,15);
	CreateButton("v",scaleYDown4,250,130+110,15,15);
	CreateButton("v",scaleZDown4,250,145+110,15,15);

	CreateButton("Change Shape",changeShape5,5,115+165,75,15);
	CreateButton("Change Color",changeColor5,5,130+165,75,15);
	CreateButton("Change Texture",changeTexture5,5,145+165,75,15);
	CreateButton("<",left5,85,130+165,15,15);
	CreateButton("^",back5,100,115+165,15,15);
	CreateButton("v",forward5,100,145+165,15,15);
	CreateButton(">",right5,115,130+165,15,15);
	CreateButton("^",up5,135,120+165,15,15);
	CreateButton("v",down5,135,140+165,15,15);
	CreateButton("Rotate X",rotateX5,155,115+165,45,15);
	CreateButton("Rotate Y",rotateY5,155,130+165,45,15);
	CreateButton("Rotate Z",rotateZ5,155,145+165,45,15);
	CreateButton("Scale X ^",scaleXUp5,205,115+165,45,15);
	CreateButton("Scale Y ^",scaleYUp5,205,130+165,45,15);
	CreateButton("Scale Z ^",scaleZUp5,205,145+165,45,15);
	CreateButton("v",scaleXDown5,250,115+165,15,15);
	CreateButton("v",scaleYDown5,250,130+165,15,15);
	CreateButton("v",scaleZDown5,250,145+165,15,15);

	CreateButton("Change Shape",changeShape6,5,115+220,75,15);
	CreateButton("Change Color",changeColor6,5,130+220,75,15);
	CreateButton("Change Texture",changeTexture6,5,145+220,75,15);
	CreateButton("<",left6,85,130+220,15,15);
	CreateButton("^",back6,100,115+220,15,15);
	CreateButton("v",forward6,100,145+220,15,15);
	CreateButton(">",right6,115,130+220,15,15);
	CreateButton("^",up6,135,120+220,15,15);
	CreateButton("v",down6,135,140+220,15,15);
	CreateButton("Rotate X",rotateX6,155,115+220,45,15);
	CreateButton("Rotate Y",rotateY6,155,130+220,45,15);
	CreateButton("Rotate Z",rotateZ6,155,145+220,45,15);
	CreateButton("Scale X ^",scaleXUp6,205,115+220,45,15);
	CreateButton("Scale Y ^",scaleYUp6,205,130+220,45,15);
	CreateButton("Scale Z ^",scaleZUp6,205,145+220,45,15);
	CreateButton("v",scaleXDown6,250,115+220,15,15);
	CreateButton("v",scaleYDown6,250,130+220,15,15);
	CreateButton("v",scaleZDown6,250,145+220,15,15);

	CreateButton("Change Shape",changeShape7,5,115+275,75,15);
	CreateButton("Change Color",changeColor7,5,130+275,75,15);
	CreateButton("Change Texture",changeTexture7,5,145+275,75,15);
	CreateButton("<",left7,85,130+275,15,15);
	CreateButton("^",back7,100,115+275,15,15);
	CreateButton("v",forward7,100,145+275,15,15);
	CreateButton(">",right7,115,130+275,15,15);
	CreateButton("^",up7,135,120+275,15,15);
	CreateButton("v",down7,135,140+275,15,15);
	CreateButton("Rotate X",rotateX7,155,115+275,45,15);
	CreateButton("Rotate Y",rotateY7,155,130+275,45,15);
	CreateButton("Rotate Z",rotateZ7,155,145+275,45,15);
	CreateButton("Scale X ^",scaleXUp7,205,115+275,45,15);
	CreateButton("Scale Y ^",scaleYUp7,205,130+275,45,15);
	CreateButton("Scale Z ^",scaleZUp7,205,145+275,45,15);
	CreateButton("v",scaleXDown7,250,115+275,15,15);
	CreateButton("v",scaleYDown7,250,130+275,15,15);
	CreateButton("v",scaleZDown7,250,145+275,15,15);

	CreateButton("Change Shape",changeShape8,5,115+330,75,15);
	CreateButton("Change Color",changeColor8,5,130+330,75,15);
	CreateButton("Change Texture",changeTexture8,5,145+330,75,15);
	CreateButton("<",left8,85,130+330,15,15);
	CreateButton("^",back8,100,115+330,15,15);
	CreateButton("v",forward8,100,145+330,15,15);
	CreateButton(">",right8,115,130+330,15,15);
	CreateButton("^",up8,135,120+330,15,15);
	CreateButton("v",down8,135,140+330,15,15);
	CreateButton("Rotate X",rotateX8,155,115+330,45,15);
	CreateButton("Rotate Y",rotateY8,155,130+330,45,15);
	CreateButton("Rotate Z",rotateZ8,155,145+330,45,15);
	CreateButton("Scale X ^",scaleXUp8,205,115+330,45,15);
	CreateButton("Scale Y ^",scaleYUp8,205,130+330,45,15);
	CreateButton("Scale Z ^",scaleZUp8,205,145+330,45,15);
	CreateButton("v",scaleXDown8,250,115+330,15,15);
	CreateButton("v",scaleYDown8,250,130+330,15,15);
	CreateButton("v",scaleZDown8,250,145+330,15,15);

	CreateButton("Change Shape",changeShape9,5,115+385,75,15);
	CreateButton("Change Color",changeColor9,5,130+385,75,15);
	CreateButton("Change Texture",changeTexture9,5,145+385,75,15);
	CreateButton("<",left9,85,130+385,15,15);
	CreateButton("^",back9,100,115+385,15,15);
	CreateButton("v",forward9,100,145+385,15,15);
	CreateButton(">",right9,115,130+385,15,15);
	CreateButton("^",up9,135,120+385,15,15);
	CreateButton("v",down9,135,140+385,15,15);
	CreateButton("Rotate X",rotateX9,155,115+385,45,15);
	CreateButton("Rotate Y",rotateY9,155,130+385,45,15);
	CreateButton("Rotate Z",rotateZ9,155,145+385,45,15);
	CreateButton("Scale X ^",scaleXUp9,205,115+385,45,15);
	CreateButton("Scale Y ^",scaleYUp9,205,130+385,45,15);
	CreateButton("Scale Z ^",scaleZUp9,205,145+385,45,15);
	CreateButton("v",scaleXDown9,250,115+385,15,15);
	CreateButton("v",scaleYDown9,250,130+385,15,15);
	CreateButton("v",scaleZDown9,250,145+385,15,15);

	CreateButton("Light <",lightLeft,5,570,35,15);
	CreateButton("Light ^",lightBack,40,555,35,15);
	CreateButton("Light v",lightForward,40,585,35,15);
	CreateButton("Light >",lightRight,75,570,35,15);
	CreateButton("Light ^",lightUp,115,560,35,15);
	CreateButton("Light v",lightDown,115,580,35,15);
	CreateButton("Axes of Symmetry ^",symmetryUp,160,560,105,15);
	CreateButton("Axes of Symmetry v",symmetryDown,160,580,105,15);
	CreateButton("Axes of Symmetry +60",symmetryUp60,270,560,105,15);
	CreateButton("Axes of Symmetry -60",symmetryDown60,270,580,105,15);


}


/*----------------------------------------------------------------------------------------
 *	\brief	This function is called whenever a mouse button is pressed or released
 *	\param	button	-	GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, or GLUT_MIDDLE_BUTTON
 *	\param	state	-	GLUT_UP or GLUT_DOWN depending on whether the mouse was released
 *						or pressed respectivly. 
 *	\param	x		-	the x-coord of the mouse cursor.
 *	\param	y		-	the y-coord of the mouse cursor.
 */
void MouseButton(int button,int state,int x, int y)
{
	/*
	 *	update the mouse position
	 */
	TheMouse.x = x;
	TheMouse.y = y;

	/*
	 *	has the button been pressed or released?
	 */
	if (state == GLUT_DOWN) 
	{
		/*
		 *	This holds the location of the first mouse click
		 */
		//if ( !(TheMouse.lmb || TheMouse.mmb || TheMouse.rmb) ) {
			TheMouse.xpress = x;
			TheMouse.ypress = y;
		//}

		/*
		 *	Which button was pressed?
		 */
		switch(button) 
		{
		case GLUT_LEFT_BUTTON:
			TheMouse.lmb = 1;
			ButtonPress(x,y);
		case GLUT_MIDDLE_BUTTON:
			TheMouse.mmb = 1;
			break;
		case GLUT_RIGHT_BUTTON:
			TheMouse.rmb = 1;
			break;
		}
	}
	else 
	{
		/*
		 *	Which button was released?
		 */
		switch(button) 
		{
		case GLUT_LEFT_BUTTON:
			TheMouse.lmb = 0;
			ButtonRelease(x,y);
			break;
		case GLUT_MIDDLE_BUTTON:
			TheMouse.mmb = 0;
			break;
		case GLUT_RIGHT_BUTTON:
			TheMouse.rmb = 0;
			break;
		}
	}

	/*
	 *	Force a redraw of the screen. If we later want interactions with the mouse
	 *	and the 3D scene, we will need to redraw the changes.
	 */
	glutPostRedisplay();
}


/*----------------------------------------------------------------------------------------
 *	\brief	This function is called whenever the mouse cursor is moved AND A BUTTON IS HELD.
 *	\param	x	-	the new x-coord of the mouse cursor.
 *	\param	y	-	the new y-coord of the mouse cursor.
 */
void MouseMotion(int x, int y)
{
	/*
	 *	Calculate how much the mouse actually moved
	 */
	//int dx = x - TheMouse.x;
	//int dy = y - TheMouse.y;

	/*
	 *	update the mouse position
	 */
	TheMouse.x = x;
	TheMouse.y = y;


	/*
	 *	Check MyButton to see if we should highlight it cos the mouse is over it
	 */
	ButtonPassive(x,y);

	/*
	 *	Force a redraw of the screen
	 */
	glutPostRedisplay();
}

/*----------------------------------------------------------------------------------------
 *	\brief	This function is called whenever the mouse cursor is moved AND NO BUTTONS ARE HELD.
 *	\param	x	-	the new x-coord of the mouse cursor.
 *	\param	y	-	the new y-coord of the mouse cursor.
 */
void MousePassiveMotion(int x, int y)
{
	/*
	 *	Calculate how much the mouse actually moved
	 */
	//int dx = x - TheMouse.x;
	//int dy = y - TheMouse.y;

	/*
	 *	update the mouse position
	 */
	TheMouse.x = x;
	TheMouse.y = y;

	/*
	 *	Check MyButton to see if we should highlight it cos the mouse is over it
	 */
	ButtonPassive(x,y);

	/*
	 *	Note that I'm not using a glutPostRedisplay() call here. The passive motion function 
	 *	is called at a very high frequency. We really don't want much processing to occur here.
	 *	Redrawing the screen every time the mouse moves is a bit excessive. Later on we 
	 *	will look at a way to solve this problem and force a redraw only when needed. 
	 */
}