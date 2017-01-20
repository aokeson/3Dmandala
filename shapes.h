/*
 *  Shapes
 *
 *  Code for drawing all of the 3D shapes
 *  in the mandala.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

double rep=1;
unsigned int none;
unsigned int wood;
unsigned int argile;
unsigned int brick;
unsigned int fire;
unsigned int galaxy;
unsigned int paisley;
unsigned int water;

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))


/*
 *  Reverse n bytes
 */
static void Reverse(void* x,const int n)
{
   int k;
   char* ch = (char*)x;
   for (k=0;k<n/2;k++)
   {
      char tmp = ch[k];
      ch[k] = ch[n-1-k];
      ch[n-1-k] = tmp;
   }
}


void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}


void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}


/*
 *  Load texture from BMP file
 */
unsigned int LoadTexBMP(const char* file)
{
   unsigned int   texture;    // Texture name
   FILE*          f;          // File pointer
   unsigned short magic;      // Image magic
   unsigned int   dx,dy,size; // Image dimensions
   unsigned short nbp,bpp;    // Planes and bits per pixel
   unsigned char* image;      // Image data
   unsigned int   off;        // Image offset
   unsigned int   k;          // Counter
   int            max;        // Maximum texture dimensions

   //  Open file
   f = fopen(file,"rb");
   if (!f) Fatal("Cannot open file %s\n",file);
   //  Check image magic
   if (fread(&magic,2,1,f)!=1) Fatal("Cannot read magic from %s\n",file);
   if (magic!=0x4D42 && magic!=0x424D) Fatal("Image magic not BMP in %s\n",file);
   //  Read header
   if (fseek(f,8,SEEK_CUR) || fread(&off,4,1,f)!=1 ||
       fseek(f,4,SEEK_CUR) || fread(&dx,4,1,f)!=1 || fread(&dy,4,1,f)!=1 ||
       fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
     Fatal("Cannot read header from %s\n",file);
   //  Reverse bytes on big endian hardware (detected by backwards magic)
   if (magic==0x424D)
   {
      Reverse(&off,4);
      Reverse(&dx,4);
      Reverse(&dy,4);
      Reverse(&nbp,2);
      Reverse(&bpp,2);
      Reverse(&k,4);
   }
   //  Check image parameters
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
   if (dx<1 || dx>max) Fatal("%s image width %d out of range 1-%d\n",file,dx,max);
   if (dy<1 || dy>max) Fatal("%s image height %d out of range 1-%d\n",file,dy,max);
   if (nbp!=1)  Fatal("%s bit planes is not 1: %d\n",file,nbp);
   if (bpp!=24) Fatal("%s bits per pixel is not 24: %d\n",file,bpp);
   if (k!=0)    Fatal("%s compressed files not supported\n",file);
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k*=2);
   if (k!=dx) Fatal("%s image width not a power of two: %d\n",file,dx);
   for (k=1;k<dy;k*=2);
   if (k!=dy) Fatal("%s image height not a power of two: %d\n",file,dy);
#endif

   //  Allocate image memory
   size = 3*dx*dy;
   image = (unsigned char*) malloc(size);
   if (!image) Fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
   //  Seek to and read image
   if (fseek(f,off,SEEK_SET) || fread(image,size,1,f)!=1) Fatal("Error reading data from image %s\n",file);
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k=0;k<size;k+=3)
   {
      unsigned char temp = image[k];
      image[k]   = image[k+2];
      image[k+2] = temp;
   }

   //  Sanity check
   ErrCheck("LoadTexBMP");
   //  Generate 2D texture
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,3,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) Fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}


static void loadImages()
{
   none = LoadTexBMP("none.bmp");
   wood = LoadTexBMP("wood.bmp");
   argile = LoadTexBMP("argile.bmp");
   brick = LoadTexBMP("brick.bmp");
   fire = LoadTexBMP("fire.bmp");
   galaxy = LoadTexBMP("galaxy.bmp");
   paisley = LoadTexBMP("paisley.bmp");
   water = LoadTexBMP("water.bmp");
}


/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(int th,int ph)
{
   double x = -Sin(th)*Cos(ph);
   double y = Sin(ph);
   double z = Cos(th)*Cos(ph);
   glNormal3d(x,y,z);
   glTexCoord2d(4*(th/360.0),4*(ph/180.0)+0.5);  glVertex3d(x,y,z);
}

/*
 *  Draw a pyramid
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated thx about the x axis
 *             thy about the y axis
 *             thz about the z axis
 *     with color (r,g,b)
 *     and texture t
 */
static void pyramid(double x,double y, double z,
                    double dx,double dy,double dz,
                    double thx, double thy, double thz,
                    double r, double g, double b,
                    double t)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   glPushMatrix();

   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   if (t==0)
      glBindTexture(GL_TEXTURE_2D, none);
   else if (t==1)
      glBindTexture(GL_TEXTURE_2D, wood);
   else if (t==2)
      glBindTexture(GL_TEXTURE_2D, argile);
   else if (t==3)
      glBindTexture(GL_TEXTURE_2D, brick);
   else if (t==4)
      glBindTexture(GL_TEXTURE_2D, fire);
   else if (t==5)
      glBindTexture(GL_TEXTURE_2D, galaxy);
   else if (t==6)
      glBindTexture(GL_TEXTURE_2D, paisley);
   else
      glBindTexture(GL_TEXTURE_2D, water);

   glBegin(GL_TRIANGLES);
   glColor3f(r,g,b);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0.0,0.0); glVertex3d(-2.5, -1.86525, -1.4434);
   glTexCoord2f(rep,0.0); glVertex3d(2.5, -1.86525, -1.4434);
   glTexCoord2f(rep/2,rep); glVertex3d(0, -1.86525, 2.5566);

   glNormal3f(0, 7.217, -18.6525);
   glTexCoord2f(0.0,0.0); glVertex3d(-2.5, -1.86525, -1.4434);
   glTexCoord2f(rep,0.0); glVertex3d(2.5, -1.86525, -1.4434);
   glTexCoord2f(rep/2,rep); glVertex3d(0.0, 1.86525, 0.0);

   glNormal3f(-14.922, 6.3915, 9.32625);
   glTexCoord2f(0.0,0.0); glVertex3d(-2.5, -1.86525, -1.4434);
   glTexCoord2f(rep,0.0); glVertex3d(0, -1.86525, 2.5566);
   glTexCoord2f(rep/2,rep); glVertex3d(0.0, 1.86525, 0.0);

   glNormal3f(14.922, 6.3915, 9.32625);
   glTexCoord2f(0.0,0.0); glVertex3d(0, -1.86525, 2.5566);
   glTexCoord2f(rep,0.0); glVertex3d(2.5, -1.86525, -1.4434);
   glTexCoord2f(rep/2,rep); glVertex3d(0.0, 1.86525, 0.0);

   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated thx about the x axis
 *             thy about the y axis
 *             thz about the z axis
 *     with color (r,g,b)
 *     and texture t
 */
static void cube(double x,double y, double z,
                 double dx,double dy,double dz,
                 double thx, double thy, double thz,
                 double r, double g, double b,
                 double t)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   glPushMatrix();
   
   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   if (t==0)
      glBindTexture(GL_TEXTURE_2D, none);
   else if (t==1)
      glBindTexture(GL_TEXTURE_2D, wood);
   else if (t==2)
      glBindTexture(GL_TEXTURE_2D, argile);
   else if (t==3)
      glBindTexture(GL_TEXTURE_2D, brick);
   else if (t==4)
      glBindTexture(GL_TEXTURE_2D, fire);
   else if (t==5)
      glBindTexture(GL_TEXTURE_2D, galaxy);
   else if (t==6)
      glBindTexture(GL_TEXTURE_2D, paisley);
   else
      glBindTexture(GL_TEXTURE_2D, water);

   glBegin(GL_QUADS);
   glColor3f(r,g,b);
   glNormal3f(0, -1, 0);
   glTexCoord2f(rep,0.0);  glVertex3f(1,-1,-1);
   glTexCoord2f(0.0,0.0);  glVertex3f(-1,-1,-1);
   glTexCoord2f(0.0,rep);  glVertex3f(-1,-1,1);
   glTexCoord2f(rep,rep);  glVertex3f(1,-1,1);

   glNormal3f(0, 0, -1);
   glTexCoord2f(0.0,0.0);  glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,0.0);  glVertex3f(1,-1,-1);
   glTexCoord2f(rep,rep);  glVertex3f(1,1,-1);
   glTexCoord2f(0.0,rep);  glVertex3f(-1,1,-1);

   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0);  glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,0.0);  glVertex3f(-1,-1,1);
   glTexCoord2f(rep,rep);  glVertex3f(-1,1,1);
   glTexCoord2f(0.0,rep);  glVertex3f(-1,1,-1);

   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0,0.0);  glVertex3f(1,-1,1);
   glTexCoord2f(rep,0.0);  glVertex3f(1,-1,-1);
   glTexCoord2f(rep,rep);  glVertex3f(1,1,-1);
   glTexCoord2f(0.0,rep);  glVertex3f(1,1,1);

   glNormal3f(0, 0, 1);
   glTexCoord2f(0.0,0.0);  glVertex3f(-1,-1,1);
   glTexCoord2f(rep,0.0);  glVertex3f(1,-1,1);
   glTexCoord2f(rep,rep);  glVertex3f(1,1,1);
   glTexCoord2f(0.0,rep);  glVertex3f(-1,1,1);

   glNormal3f(0, 1, 0);
   glTexCoord2f(rep,0.0);  glVertex3f(1,1,-1);
   glTexCoord2f(0.0,0.0);  glVertex3f(-1,1,-1);
   glTexCoord2f(0.0,rep);  glVertex3f(-1,1,1);
   glTexCoord2f(rep,rep);  glVertex3f(1,1,1);

   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 *  Draw a sphere
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated thx about the x axis
 *             thy about the y axis
 *             thz about the z axis
 *     with color (r,g,b)
 *     and texture t
 */
static void sphere(double x,double y,double z,
                   double dx,double dy,double dz,
                   double thx, double thy, double thz,
                   double r, double g, double b,
                   double t)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   const int d=5;
   int mh,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   if (t==0)
      glBindTexture(GL_TEXTURE_2D, none);
   else if (t==1)
      glBindTexture(GL_TEXTURE_2D, wood);
   else if (t==2)
      glBindTexture(GL_TEXTURE_2D, argile);
   else if (t==3)
      glBindTexture(GL_TEXTURE_2D, brick);
   else if (t==4)
      glBindTexture(GL_TEXTURE_2D, fire);
   else if (t==5)
      glBindTexture(GL_TEXTURE_2D, galaxy);
   else if (t==6)
      glBindTexture(GL_TEXTURE_2D, paisley);
   else
      glBindTexture(GL_TEXTURE_2D, water);

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      glColor3f(r,g,b);
      for (mh=0;mh<=360;mh+=d)
      {
         //glTexCoord2f(Cos(mh),Sin(2*(ph+90)));  
         Vertex(mh,ph);
         //glTexCoord2f(Cos(mh),Sin(2*(ph+d+90)));  
         Vertex(mh,ph+d);
      }
      glEnd();
   }
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 *  Draw a cone
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated thx about the x axis
 *             thy about the y axis
 *             thz about the z axis
 *     with color (r,g,b)
 *     and texture t
 */
static void cone(double x,double y, double z,
                 double dx,double dy,double dz,
                 double thx, double thy, double thz,
                 double r, double g, double b,
                 double t)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   glPushMatrix();

   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   if (t==0)
      glBindTexture(GL_TEXTURE_2D, none);
   else if (t==1)
      glBindTexture(GL_TEXTURE_2D, wood);
   else if (t==2)
      glBindTexture(GL_TEXTURE_2D, argile);
   else if (t==3)
      glBindTexture(GL_TEXTURE_2D, brick);
   else if (t==4)
      glBindTexture(GL_TEXTURE_2D, fire);
   else if (t==5)
      glBindTexture(GL_TEXTURE_2D, galaxy);
   else if (t==6)
      glBindTexture(GL_TEXTURE_2D, paisley);
   else
      glBindTexture(GL_TEXTURE_2D, water);

   // Cone
   glBegin(GL_TRIANGLE_FAN);
   glColor3f(r,g,b);
   glTexCoord2f(0.5,0.5); glVertex3f(0,1,0);
   for (int k=0;k<=360;k+=10)
   {
      glNormal3f(Cos(k),1,Sin(k));
      glTexCoord2f(rep/2*Cos(k)+0.5,rep/2*Sin(k)+0.5); glVertex3f(Cos(k),-1,Sin(k));
   }
   glEnd();

   // Base
   glBegin(GL_TRIANGLE_FAN);
   for (int k=0;k<=360;k+=15){
      glNormal3f(0,-1,0);
      glTexCoord2f(rep/2*Cos(k)+0.5,rep/2*Sin(k)+0.5); glVertex3f(Cos(k),-1,Sin(k));
   }
   glEnd();

   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 *  Draw a cylinder
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated thx about the x axis
 *             thy about the y axis
 *             thz about the z axis
 *     with color (r,g,b)
 *     and texture t
 */
static void cylinder(double x,double y, double z,
                     double dx,double dy,double dz,
                     double thx, double thy, double thz,
                     double r, double g, double b,
                     double t)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   glPushMatrix();

   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   if (t==0)
      glBindTexture(GL_TEXTURE_2D, none);
   else if (t==1)
      glBindTexture(GL_TEXTURE_2D, wood);
   else if (t==2)
      glBindTexture(GL_TEXTURE_2D, argile);
   else if (t==3)
      glBindTexture(GL_TEXTURE_2D, brick);
   else if (t==4)
      glBindTexture(GL_TEXTURE_2D, fire);
   else if (t==5)
      glBindTexture(GL_TEXTURE_2D, galaxy);
   else if (t==6)
      glBindTexture(GL_TEXTURE_2D, paisley);
   else
      glBindTexture(GL_TEXTURE_2D, water);
   
   // Sides
   glBegin(GL_QUAD_STRIP);
   glColor3f(r,g,b);
   for (int k=0;k<=360;k+=15){
      glNormal3f(Cos(k),0,Sin(k));
      glTexCoord2f(0,k/360.0);   glVertex3f(Cos(k),-1,Sin(k));
      glTexCoord2f(1,k/360.0);   glVertex3f(Cos(k),1,Sin(k));
   }
   glEnd();
   
   // Bottom
   glBegin(GL_TRIANGLE_FAN);
   for (int k=0;k<=360;k+=15){
      glNormal3f(0,-1,0);
      glTexCoord2f(rep/2*0.25*Cos(k)+0.5,rep/2*0.25*Sin(k)+0.5); glVertex3f(Cos(k),-1,Sin(k));
   }
   glEnd();

   // Top
   glBegin(GL_TRIANGLE_FAN);
   for (int k=0;k<=360;k+=15){
      glNormal3f(0,1,0);
      glTexCoord2f(rep/2*Cos(k)+0.5,rep/2*Sin(k)+0.5); glVertex3f(Cos(k),1,Sin(k));
   }
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}


/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*0,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(0.8,0.8,0);
   glMaterialf(GL_FRONT,GL_SHININESS,1);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=10)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*10)
      {
         Vertex(th,ph);
         Vertex(th,ph+10);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}