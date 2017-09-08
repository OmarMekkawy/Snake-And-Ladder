#include <GL/glew.h>
#include <GL/glut.h>
#include <bits/stdc++.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <FreeImage.h>
#include "util.h"
#include "Serial.h"
int nxt[101];
int Menu_State=0;
int inMenState=0;
using namespace std;
const GLsizei WindowWidth=1000;
const GLsizei WindowHeight=900;
int xAng=0,yAng=0;
bool Keys[260];
bool Keys2[260];
vector<double> transMenu;
void init_nxt()
{
    transMenu.push_back(0);
    transMenu.push_back((double)-0.15);
    transMenu.push_back((double)-0.30);
    transMenu.push_back((double)-0.45);
    for(int i=1;i<=100;i++)
        nxt[i]=i;
    nxt[99]=37;
    nxt[40]=3;
    nxt[38]=19;
    nxt[64]=45;
    nxt[90]=54;
    nxt[29]=10;
    nxt[62]=81;
    nxt[39]=63;
    nxt[4]=25;
    nxt[15]=66;
    nxt[33]=41;
    nxt[50]=69;
}
GLuint tex1,tex2,tex3,tex4; //Texture IDS after building
vector<materials> mats[3]; //MTL MATRIX
GLuint bufs[6];
double add1=0,add2=0,add3=0,add4=0,add5=0,add6=0;
int curcell1=1,curcell2=1;
bool rolled=false;
double diff1=0,diff2=0,diff3=0;
double diff4=0,diff5=0,diff6=0;
int curpos1=1,curpos2=1;
int turn=0;
int idx=0;
CSerial ser;
bool Animated=false;
void Print_Menu()
{
    glClearColor(0,0,0,0);
    glColor3f(1,1,1);
    string ver="Snake and Ladder Version 2.0.4";
    glRasterPos3f(-0.9,0.75,-2);
    for(int i=0;i<ver.size();i++){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,ver[i]);
    }
    string Mnu="1. New Game",Mnu2="2.  Instructions",Mnu3="3.  About",Mnu4="4.  Exit\n\n";
    glRasterPos3f(-0.15,0.4,-2);
    for(int i=0;i<Mnu.size();i++){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,Mnu[i]);
    }
    glRasterPos3f(-0.15,0.25,-2);
    for(int i=0;i<Mnu2.size();i++){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,Mnu2[i]);
    }
    glRasterPos3f(-0.15,0.1,-2);
    for(int i=0;i<Mnu3.size();i++){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,Mnu3[i]);
    }
    glRasterPos3f(-0.15,-0.05,-2);
    for(int i=0;i<Mnu4.size();i++){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,Mnu4[i]);
    }
    glFlush();
}
struct cell
{
    double x,y,z;
};
string cur;
void PrScanned()
{
    glColor3f(0,0,0);
    glRasterPos3f(-0.2,1,-3);
    for(int i=0;i<cur.size();i++){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,cur[i]);
    }
    glFlush();
}
cell board[101];
void Establish_3D_Projection(GLsizei width,GLsizei height)
{
    //for changing window size
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,(double)width/height,0.1,200.0);
}
void initGL(GLsizei Width,GLsizei Height)
{
    Establish_3D_Projection(Width,Height);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.3, 0.4, 0.5,0.0);
    //for testing purposes
    glEnable(GL_DEPTH_TEST);
    //The order of drawing
    glDepthFunc(GL_LEQUAL);
    //optional ta7seen lel view law yenfa3
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  //  glEnable(GL_PERSPECTIVE_CORRECTION_HINT); //This one screws up textures for some reason
}
void PrintNumbers(int num,float x,float y,float z,int color)
{
    glColor3f(color,color,color);
    string cur="";
        cur.clear();
        int tmp=num;
        while(tmp){
            cur.push_back(tmp%10+'0');tmp/=10;
        }
        reverse(cur.begin(),cur.end());
    double add=0;
    if(num>9&&num<100)
        add=0.03;
    if(num<=9)
        add=0.06;
    glRasterPos3f(x+add,y-0.14,z);
    for(int i=0;i<cur.size();i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,cur[i]);
    glColor3f(1-color,1-color,1-color);
    glFlush();
}
void DrawSnake()
{
    glEnable(GL_TEXTURE_2D);
    glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[1]);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vec12), 0);
	glNormalPointer(GL_FLOAT, sizeof(vec12), (void*)12);
	glTexCoordPointer(2, GL_FLOAT, sizeof(vec12), (void*)24);
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
    for (int i = 0; i < mats[0].size(); i++)
	{
		set_material(mats[0][i].mtl);
		glDrawElements(GL_TRIANGLES, mats[0][i].count, GL_UNSIGNED_INT, (void*)(mats[0][i].startindex * sizeof(unsigned int)));
	}
    glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
}
void DrawLadder()
{
    glEnable(GL_TEXTURE_2D);
    glBindBuffer(GL_ARRAY_BUFFER, bufs[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[3]);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vec12), 0);
	glNormalPointer(GL_FLOAT, sizeof(vec12), (void*)12);
	glTexCoordPointer(2, GL_FLOAT, sizeof(vec12), (void*)24);
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
    for (int i = 0; i < mats[1].size(); i++)
	{
		set_material(mats[1][i].mtl);
		glDrawElements(GL_TRIANGLES, mats[1][i].count, GL_UNSIGNED_INT, (void*)(mats[1][i].startindex * sizeof(unsigned int)));
	}
    glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	//glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
}
void DrawPawn()
{
    glEnable(GL_TEXTURE_2D);
    glBindBuffer(GL_ARRAY_BUFFER, bufs[4]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[5]);
    glBindTexture(GL_TEXTURE_2D, tex3);
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vec12), 0);
	glNormalPointer(GL_FLOAT, sizeof(vec12), (void*)12);
	glTexCoordPointer(2, GL_FLOAT, sizeof(vec12), (void*)24);
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
    for (int i = 0; i < mats[2].size(); i++)
	{
		set_material(mats[2][i].mtl);
		glDrawElements(GL_TRIANGLES, mats[2][i].count, GL_UNSIGNED_INT, (void*)(mats[2][i].startindex * sizeof(unsigned int)));
	}
    glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	//glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
}
void DrawPawn2()
{
    glEnable(GL_TEXTURE_2D);
    glBindBuffer(GL_ARRAY_BUFFER, bufs[4]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[5]);
    glBindTexture(GL_TEXTURE_2D, tex4);
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vec12), 0);
	glNormalPointer(GL_FLOAT, sizeof(vec12), (void*)12);
	glTexCoordPointer(2, GL_FLOAT, sizeof(vec12), (void*)24);
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
    for (int i = 0; i < mats[2].size(); i++)
	{
		set_material(mats[2][i].mtl);
		glDrawElements(GL_TRIANGLES, mats[2][i].count, GL_UNSIGNED_INT, (void*)(mats[2][i].startindex * sizeof(unsigned int)));
	}
    glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	//glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
}
void DrawCube(int i,int j)
{
                glBegin(GL_QUADS);
            //front Face
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2+0.2,(float)1-(float)0.2*j);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2+0.2,(float)1-(float)0.2*j);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2,(float)1-(float)0.2*j);
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2,(float)1-(float)0.2*j);
            //back Face
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2+0.2,(float)1-(float)0.2*j-0.2);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2+0.2,(float)1-(float)0.2*j-0.2);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2,(float)1-(float)0.2*j-0.2);
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2,(float)1-(float)0.2*j-0.2);
            // TopFace
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2+0.2,(float)1-(float)0.2*j);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2+0.2,(float)1-(float)0.2*j);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2+0.2,(float)1-(float)0.2*j-0.2);
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2+0.2,(float)1-(float)0.2*j-0.2);
            //Bottom Face
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2,(float)1-(float)0.2*j);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2,(float)1-(float)0.2*j);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2,(float)1-(float)0.2*j-0.2);
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2,(float)1-(float)0.2*j-0.2);
            //LeftFace
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2+0.2,(float)1-(float)0.2*j);
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2+0.2,(float)1-(float)0.2*j-0.2);
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2,(float)1-(float)0.2*j-0.2);
            glVertex3f((float)-1+(float)0.2*i,-1+j*0.2,(float)1-(float)0.2*j);
            //Right Face
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2+0.2,(float)1-(float)0.2*j);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2+0.2,(float)1-(float)0.2*j-0.2);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2,(float)1-(float)0.2*j-0.2);
            glVertex3f((float)-1+(float)0.2*i+0.2,-1+j*0.2,(float)1-(float)0.2*j);
            glEnd();
}
int Addx=0,Addy=0,Addz=0;
int Rotatex=0,Rotatey=0,Rotatez=0;
void UpdateCoords()
{
    glTranslated(Addx,Addy,Addz);
    glRotated(Rotatex,1,0,0);
    glRotated(Rotatey,0,1,0);
    glRotated(Rotatez,0,0,1);
    cout<<"Adjusting Function Results : "<<Addx<<" "<<Addy<<" "<<Addz<<" "<<Rotatex<<" "<<Rotatey<<" "<<Rotatez<<endl;
    return ;
}
void Adjust_Shapre(int ax,int ay,int az,int rotx,int rotz,double scalex,double scaley,double scalez)
{
    glPushMatrix();
    glScaled(scalex,scaley,scalez);
    glTranslated(ax,ay,az);
    glRotated(rotx,1,0,0);
    glRotated(rotz,0,0,1);
}
void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    if(Menu_State==0){
        Print_Menu();
        glPushMatrix();
        glTranslated(0,transMenu[inMenState],0);
        glBegin(GL_QUADS);
        glColor3f(0.5,0.1,0.7);
        glVertex3f(-0.2,0.46,-2.01);
        glColor3f(0.2,0.8,0.9);
        glVertex3f(0.2,0.46,-2.01);
        glColor3f(0,0.9,0.3);
        glVertex3f(0.2,0.38,-2.01);
        glVertex3f(-0.2,0.38,-2.01);
        glEnd();
        glPopMatrix();
    }
    else if(Menu_State==1){
        glClearColor(0.3, 0.4, 0.5,0.0);
        glLoadIdentity();
        glTranslated(0,0,-3.5);
        glRotatef(xAng,1,0,0);
        glRotatef(yAng,0,1,0);
        //xAxis
        for(int i=0;i<10;i++){
                //zAxis
            for(int j=0;j<10;j++){
                if((i+j)%2)
                    glColor3f(1,1,1);
                else
                    glColor3f(1-0.07*j,min(0.5,1-0.8*j),0.08*j);
                DrawCube(i,j);
                PrintNumbers(j*10+((i+1)*(pow(-1,(j%2))))+(10*(j%2))+1*(j%2),(float)-1+(float)0.2*i,-1+j*0.2+0.2,(float)1-(float)0.2*j,1-((i+j)%2));
                board[j*10+((i+1)*((int)pow(-1,(j%2))))+(10*(j%2))+1*(j%2)].x=(double)-1+(double)0.2*i;
                board[j*10+((i+1)*((int)pow(-1,(j%2))))+(10*(j%2))+1*(j%2)].y=-1+(double)j*0.2+0.2;
                board[j*10+((i+1)*((int)pow(-1,(j%2))))+(10*(j%2))+1*(j%2)].z=(double)1-(double)0.2*j;

            }
        }
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glPushMatrix();
        glTranslated(diff1+add1,diff2+add2,diff3+add3);
        //cout<<add1<<" "<<add2<<" "<<add3<<endl;
        Adjust_Shapre(-188,-159,190,0,0,0.005,0.005,0.005);
        DrawPawn();
        glPopMatrix();
        glPushMatrix();
        glTranslated(diff4+add4,diff5+add5,diff6+add6);
        Adjust_Shapre(-172,-159,190,0,0,0.005,0.005,0.005);
        DrawPawn2();
        glPopMatrix();
        Adjust_Shapre(52,-13,24,0,-50,0.01,0.017,0.02);
        DrawLadder();
        glPopMatrix();
        Adjust_Shapre(-62,-3,11,-25,-44,0.01,0.04,0.02);
        DrawLadder();
        glPopMatrix();
        Adjust_Shapre(-28,-20,45,-43,-48,0.01,0.04,0.02);
        DrawLadder();
        glPopMatrix();
        Adjust_Shapre(-70,4,-4,-54,396,0.01,0.06,0.02);
        DrawLadder();
        glPopMatrix();
        Adjust_Shapre(82,-2,10,-45,45,0.01,0.06,0.02);
        DrawLadder();
        glPopMatrix();
        Adjust_Shapre(5,-11,33,0,0,0.015,0.04,0.02);
        DrawLadder();
        glPopMatrix();
        Adjust_Shapre(-9,10,27,0,21,0.02,0.02,0.02);
        DrawSnake();
        glPopMatrix();
        Adjust_Shapre(-14,-12,74,0,318,0.02,0.02,0.02);
        DrawSnake();
        glPopMatrix();
        Adjust_Shapre(21,14,74,0,318,0.02,0.02,0.02);
        DrawSnake();
        glPopMatrix();
        Adjust_Shapre(18,-8,107,0,393,0.02,0.02,0.02);
        DrawSnake();
        glPopMatrix();
        Adjust_Shapre(-17,10,45,0,21,0.02,0.05,0.02);
        DrawSnake();
        glPopMatrix();
        Adjust_Shapre(-35,-4,53,0,39,0.02,0.05,0.02);
        DrawSnake();
        glLoadIdentity();
        PrScanned();
        glPopMatrix();
        glLoadIdentity();
    }
    else if(Menu_State==2){
        glClearColor(0,0,0,0);
        glColor3f(1,1,1);
        glRasterPos3f(-0.9,0.75,-2);
        string pr="Game Instructions: ";
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);
        glColor3f(0,0.9,0);
        pr="    1-	Type “roll” to get the number your pawn will move according to.";
        glRasterPos3f(-0.9,0.65,-2);
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);
        pr="    2-	Type “play” to make the pawn move to its destination.";
        glRasterPos3f(-0.9,0.55,-2);
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);
        pr="    3-	Switch turns and Have Fun! The game is designed for more than one player!";
        glRasterPos3f(-0.9,0.45,-2);
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);

    }
    else if(Menu_State==3){
        glClearColor(0,0,0,0);
        glColor3f(1,1,1);
        glRasterPos3f(-0.9,0.75,-2);
        string pr="About the game : ";
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);
        glColor3f(0,0.9,0);
        pr="        this is a typical snakes and ladders game where you roll the dice, get a number";
        glRasterPos3f(-0.9,0.65,-2);
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);
        pr="        and the pawn will move according to this number.";
        glRasterPos3f(-0.9,0.55,-2);
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);
        pr="        If the pawn reaches a cell having a start of a ladder,it moves up to the cell";
        glRasterPos3f(-0.9,0.45,-2);
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);
        pr="        where the end of the ladder lies.";
        glRasterPos3f(-0.9,0.35,-2);
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);
        pr="        If the pawn reaches a cell with a snake’s head it will move down to the cell that have that snake’s tail.";
        glRasterPos3f(-0.9,0.25,-2);
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);
        pr="        The first player who reaches the 100 cell will be the winner; Gather your friends and let the fun Begin!!";
        glRasterPos3f(-0.9,0.15,-2);
        for(int i=0;i<pr.size();i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,pr[i]);

    }
    else
        exit(0);
    glFlush();
    glutSwapBuffers();

}
void init2()
{
	vector<vec12> newvec[3];
	vector<unsigned int> normaluns[3];
	tex1 = loadTexture("Green.png");
	tex2 = loadTexture("Texture_CS7.jpg");
	tex3 = loadTexture("eswed.png");
	tex4 = loadTexture("bony.png");
	load_obj("snakeMlawlew.obj", newvec[0], normaluns[0], mats[0]);
	load_obj("Ladder.obj", newvec[1], normaluns[1], mats[1]);
	load_obj("pawn.obj",newvec[2],normaluns[2],mats[2]);
	glGenBuffers(6, bufs);
	glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
	glBufferData(GL_ARRAY_BUFFER, newvec[0].size() * sizeof(vec12), &newvec[0][0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, normaluns[0].size() * sizeof(unsigned int), &normaluns[0][0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, bufs[2]);
	glBufferData(GL_ARRAY_BUFFER, newvec[1].size() * sizeof(vec12), &newvec[1][0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, normaluns[1].size() * sizeof(unsigned int), &normaluns[1][0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[4]);
	glBufferData(GL_ARRAY_BUFFER, newvec[2].size() * sizeof(vec12), &newvec[2][0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[5]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, normaluns[2].size() * sizeof(unsigned int), &normaluns[2][0], GL_STATIC_DRAW);
}
void HandleKeyActions()
{
    if(Keys2[' ']&&Menu_State==0){
        Menu_State=inMenState+1;
        inMenState=0;
        Keys2[' ']=false;
    }
    if(Keys2[27]){
        Menu_State=0;
        inMenState=0;
    }
    if(Keys[GLUT_KEY_UP])
        xAng+=1;
    if(Keys[GLUT_KEY_DOWN])
        xAng-=1;
    if(Keys[GLUT_KEY_LEFT])
        yAng-=1;
    if(Keys[GLUT_KEY_RIGHT])
        yAng+=1;
    if(Keys2['a'])
        Addx--;
    if(Keys2['d'])
        Addx++;
    if(Keys2['w']){
        Addy++;
        inMenState--;
        inMenState+=4;
        inMenState%=4;
        Keys2['w']=false;
    }
    if(Keys2['s']){
        inMenState++;
        inMenState%=4;
        Addy--;
        Keys2['s']=false;
    }
    if(Keys2['z'])
        Addz++;
    if(Keys2['x'])
        Addz--;
    if(Keys2['r'])
        Rotatex-=3;
    //if(Keys2['t'])
      //  Rotatey++;
    if(Keys2['y'])
        Rotatez+=3;
}
int randomed=0;
void timerLoop(int value)
{
    if(cur=="roll"||rolled){
        rolled=true;
        if(cur=="roll")
            randomed=rand()%6+1;
        if(cur=="roll"){
            if(turn==0){
                curcell1=curpos1;
                curcell2=curpos1+1;
            }
            else{
                curcell1=curpos2;
                curcell2=curpos2+1;
            }
        }
        if(cur=="roll")
        cur="";
        if(cur=="")
        cur.push_back((char)('0'+randomed));
        string tmp;
        tmp.push_back(cur[0]);
        tmp+="play";
        if(cur==tmp){
            Animated=true;
            if(turn == 0){
                if(fabs(board[curcell1].x+add1-board[curcell2].x)<1e-9&&
                   fabs(board[curcell1].y+add2-board[curcell2].y)<1e-9&&
                   fabs(board[curcell1].z+add3-board[curcell2].z)<1e-9){
                    add1=0;
                    add2=0;
                    add3=0;
                    curcell1++;
                    curcell2++;
                        diff1=(board[1].x-board[curcell1].x);
                        diff2=(board[1].y-board[curcell1].y);
                        diff3=(board[1].z-board[curcell1].z);
                        diff1*=-1;
                        diff2*=-1;
                        diff3*=-1;
                    if(curcell1==curpos1+randomed)
                        Animated=false;
                   }
                if(board[curcell1].x+add1<board[curcell2].x&&fabs(board[curcell1].x+add1-board[curcell2].x)>1e-9)
                add1+=0.005;
                else if(board[curcell1].x+add1>board[curcell2].x&&fabs(board[curcell1].x+add1-board[curcell2].x)>1e-9)
                add1-=0.005;
                if(board[curcell1].y+add2<board[curcell2].y&&fabs(board[curcell1].y+add2-board[curcell2].y)>1e-9)
                add2+=0.005;
                else if(board[curcell1].y+add2>board[curcell2].y&&fabs(board[curcell1].y+add2-board[curcell2].y)>1e-9)
                add2-=0.005;
                else if(fabs(board[curcell1].y+add2-board[curcell2].y)<1e-9){
                        if(board[curcell1].z+add3<board[curcell2].z)
                        add3+=0.005;
                        else if(board[curcell1].z+add3>board[curcell2].z)
                        add3-=0.005;
                }
            }
            else{
                if(fabs(board[curcell1].x+add4-board[curcell2].x)<1e-9&&
                   fabs(board[curcell1].y+add5-board[curcell2].y)<1e-9&&
                   fabs(board[curcell1].z+add6-board[curcell2].z)<1e-9){
                    add4=0;
                    add5=0;
                    add6=0;
                    curcell1++;
                    curcell2++;
                        diff4=(board[1].x-board[curcell1].x);
                        diff5=(board[1].y-board[curcell1].y);
                        diff6=(board[1].z-board[curcell1].z);
                        diff4*=-1;
                        diff5*=-1;
                        diff6*=-1;
                    if(curcell1==curpos2+randomed)
                        Animated=false;
                   }
                if(board[curcell1].x+add4<board[curcell2].x&&fabs(board[curcell1].x+add4-board[curcell2].x)>1e-9)
                add4+=0.005;
                else if(board[curcell1].x+add4>board[curcell2].x&&fabs(board[curcell1].x+add4-board[curcell2].x)>1e-9)
                add4-=0.005;
                if(board[curcell1].y+add5<board[curcell2].y&&fabs(board[curcell1].y+add5-board[curcell2].y)>1e-9)
                add5+=0.005;
                else if(board[curcell1].y+add5>board[curcell2].y&&fabs(board[curcell1].y+add5-board[curcell2].y)>1e-9)
                add5-=0.005;
                else if(fabs(board[curcell1].y+add5-board[curcell2].y)<1e-9){
                        if(board[curcell1].z+add6<board[curcell2].z)
                        add6+=0.005;
                        else if(board[curcell1].z+add6>board[curcell2].z)
                        add6-=0.005;
                }
            }
            if(!Animated){
                if(turn==0){
                    diff1=(board[1].x-board[nxt[curpos1+randomed]].x);
                    diff2=(board[1].y-board[nxt[curpos1+randomed]].y);
                    diff3=(board[1].z-board[nxt[curpos1+randomed]].z);
                    diff1*=-1;
                    diff2*=-1;
                    diff3*=-1;
                    curpos1=nxt[curpos1+randomed];
                /*    char Send[10];
                    sprintf(Send,"%d",curpos1);
                    ser.SendData(Send, strlen(Send));
                    char Send2[10];
                    sprintf(Send2,"%d",curpos1+randomed);
                    ser.SendData(Send2,strlen(Send2));
                    curpos1=curpos1+randomed;*/
                }
                else{
                    diff4=(board[1].x-board[nxt[curpos2+randomed]].x);
                    diff5=(board[1].y-board[nxt[curpos2+randomed]].y);
                    diff6=(board[1].z-board[nxt[curpos2+randomed]].z);
                    diff4*=-1;
                    diff5*=-1;
                    diff6*=-1;
                    curpos2=nxt[curpos2+randomed];
                 /*     char Send[10];
                    sprintf(Send,"%d",curpos2);
                    ser.SendData(Send, strlen(Send));
                    char Send2[10];
                    sprintf(Send2,"%d",curpos2+randomed);
                    ser.SendData(Send2,strlen(Send2));
                    curpos2=curpos2+randomed;*/
                }
                turn=1-turn;
                cur="";
                rolled=false;
            }
        }
    }
    HandleKeyActions();
    glutPostRedisplay();
    glutTimerFunc(10,timerLoop,0);
}
void KeyboardSpecialDown(int key,int x,int y)
{
    Keys[key]=true;
}
void KeyboardSepcialUp(int key,int x,int y)
{
    Keys[key]=false;
}
void glutkeyUp(unsigned char key,int x,int y)
{
    Keys2[key]=false;
}
void glutkeyDown(unsigned char key,int x,int y)
{
    Keys2[key]=true;
    if(key==' ')
        cur="";
    else
    cur.push_back(key);
}
int main(int argc, char **argv) {

	// init GLUT and create window
    ser.Open(3,9600);
    init_nxt();
	srand(time(NULL));
    FreeImage_Initialise();
	glutInit(&argc, argv);
    glutInitWindowSize(WindowWidth,WindowHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    int WindowId=glutCreateWindow("Ladder And Snake");
    initGL(WindowWidth,WindowHeight);
    glutFullScreen();
    assert(glewInit() == GLEW_OK);
    init2();
    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene);
    glutReshapeFunc(Establish_3D_Projection);
    glutSpecialFunc(KeyboardSpecialDown);
    glutSpecialUpFunc(KeyboardSepcialUp);
    glutKeyboardUpFunc(glutkeyUp);
    glutKeyboardFunc(glutkeyDown);
    glutTimerFunc(1,timerLoop,0);
    glutMainLoop();
    //we never get to this area, 3shan el loop mbt5lassh
	return 0;
}
