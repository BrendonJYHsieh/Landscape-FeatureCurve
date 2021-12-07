/************************************************************************
     File:        TrainView.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						The TrainView is the window that actually shows the 
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within 
						a TrainWindow
						that is the outer window with all the widgets. 
						The TrainView needs 
						to be aware of the window - since it might need to 
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know 
						about it (beware circular references)

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <iostream>
#include <Fl/fl.h>

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include "GL/glu.h"
#include <math.h>
#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"
#define STB_IMAGE_IMPLEMENTATION
#include "RenderUtilities/stb_image.h"
#include "RenderUtilities/Texture.h"
#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif

using namespace std;

Pnt3f GMT(Pnt3f p1, Pnt3f p2, Pnt3f p3, Pnt3f p4, float t) {
	Pnt3f q0;
	q0.x  = pow(1-t,3) * p1.x + 3*t * pow(1-t,2) * p2.x + 3*t*t*(1-t)* p3.x + t*t*t *p4.x;
	q0.y  = pow(1-t,3) * p1.y + 3*t * pow(1-t,2) * p2.y + 3*t*t*(1-t)* p3.y + t*t*t *p4.y;
	q0.z  = pow(1-t,3) * p1.z + 3*t * pow(1-t,2) * p2.z + 3*t*t*(1-t)* p3.z + t*t*t *p4.z;
	return q0;
}
void Test(Pnt3f A, Pnt3f B, Pnt3f &C,float length) {
	bool t = false;
	float m = (B.z - A.z) / (B.x - A.x);
	float _m = -1 / m;
	float bb = (_m * B.x) - B.z;
	float a = _m , b = -1, c = bb;
	float d = m, e = -1, f = -length*sqrt(m*m+1) + (m * B.x - B.z);


	float det = -_m + m;
	C.x = (c * e - b * f) / det;
	C.z = (-d * c + a * f) / det;

}
void Test1(Pnt3f A, Pnt3f B, Pnt3f &C,float length) {
	bool t = false;
	float m = (B.z - A.z) / (B.x - A.x);
	float _m = -1 / m;
	float bb = (_m * B.x) - B.z;
	float a = _m , b = -1, c = bb;
	float d = m, e = -1, f = length*sqrt(m*m+1) + (m * B.x - B.z);

	float det = -_m + m;
	C.x = (c * e - b * f) / det;
	C.z = (-d * c + a * f) / det;

	
	// double tmpx = (A.x - B.x) / (A.z - B.z) * (C.z - B.z) + B.x;
	// if (B.x<A.x) {
	// 	f = + length*sqrt(m*m+1) + (m * B.x - B.z);
	// 	float det = -_m + m;
	// 	C.x = (c * e - b * f) / det;
	// 	C.z = (-d * c + a * f) / det;
	// }
}
//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l) 
	: Fl_Gl_Window(x,y,w,h,l)
//========================================================================
{
	mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );

	resetArcball();

	//Test//
	Curves.push_back(Curve());
	//Curves.push_back(Curve());
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event)) 
			return 1;

	// remember what button was used
	static int last_push;

	switch(event) {
		// Mouse button being pushed event
		case FL_PUSH:
			last_push = Fl::event_button();
			// if the left button be pushed is left mouse button
			if (last_push == FL_LEFT_MOUSE  ) {
				doPick();
				damage(1);
				return 1;
			};
			break;

	   // Mouse button release event
		case FL_RELEASE: // button release
			damage(1);
			last_push = 0;
			return 1;

		// Mouse button drag event
		case FL_DRAG:

			// Compute the new control point position
			if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
				// int i=0;
				// int temp = selectedCube;
				// for (i = 0;i < Curves.size(); i++) {
				// 	if(temp <= Curves[i].points.size()-1){
				// 		break;
				// 	}
				// 	else{
				// 		temp -= Curves[i].points.size();
				// 	}
				// }
				ControlPoint *cp = &Curves[SelectedCurve].points[SelectedNode];
				
				double r1x, r1y, r1z, r2x, r2y, r2z;
				getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z, 
								static_cast<double>(cp->pos.x), 
								static_cast<double>(cp->pos.y),
								static_cast<double>(cp->pos.z),
								rx, ry, rz,
								(Fl::event_state() & FL_CTRL) != 0);

				cp->pos.x = (float) rx;
				cp->pos.y = (float) ry;
				cp->pos.z = (float) rz;
				damage(1);
			}
			break;

		// in order to get keyboard events, we need to accept focus
		case FL_FOCUS:
			return 1;

		// every time the mouse enters this window, aggressively take focus
		case FL_ENTER:	
			focus(this);
			break;

		case FL_KEYBOARD:
		 		int k = Fl::event_key();
				int ks = Fl::event_state();
				if (k == 'p') {
					// Print out the selected control point information
					if (selectedCube >= 0) 
						printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
								 selectedCube,
								 m_pTrack->points[selectedCube].pos.x,
								 m_pTrack->points[selectedCube].pos.y,
								 m_pTrack->points[selectedCube].pos.z,
								 m_pTrack->points[selectedCube].orient.x,
								 m_pTrack->points[selectedCube].orient.y,
								 m_pTrack->points[selectedCube].orient.z);
					else
						printf("Nothing Selected\n");

					return 1;
				};
				break;
	}

	return Fl_Gl_Window::handle(event);
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0,0,w(),h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,.3f,0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]			= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);



	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();
	glDisable(GL_LIGHTING);
	//drawFloor(200,200);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {
		//setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		/*if (aspect >= 1) {
			wi = 100;
			he = wi / aspect;
		} 
		else {
			he = 100;
			wi = he * aspect;
		}*/
		if (static_cast<float>(w()) / static_cast<float>(h()) >= 1) {
			he = 100 / static_cast<float>(w()) / static_cast<float>(h());
		}
		else {
			wi = 100 * static_cast<float>(w()) / static_cast<float>(h());
		}
		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		//glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
	} 
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif
	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		int ii = 0;
		int temp = selectedCube;
		for (ii = 0; ii < Curves.size(); ii++) {
			if (temp <= Curves[ii].points.size() - 1) {
				break;
			}
			else {
				temp -= Curves[ii].points.size();
			}
		}
		SelectedCurve = ii;
		SelectedNode = temp;
		for (int i = 0; i < Curves.size(); i++) {
			for (int j = 0; j < Curves[i].points.size(); j++) {
				if (!doingShadows) {
					if (SelectedCurve == i && j == SelectedNode)
						glColor3ub(240, 240, 30);
					else
						glColor3ub(240, 60, 60);
				}
				Curves[i].points[j].draw();
			}
		}
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################
	vector<float> vertexs;
	for (int i = 0; i < Curves.size(); i++) {
		float t = 0;
		ControlPoint p1 = Curves[i].points[0];
		ControlPoint p2 = Curves[i].points[1];
		ControlPoint p3 = Curves[i].points[2];
		ControlPoint p4 = Curves[i].points[3];
		Pnt3f c;
		for (int j = 0; j < 100; j++) {
			Pnt3f q0 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, t);
			Pnt3f q1 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, t += 0.01);
			glBegin(GL_LINES);
			if (!doingShadows) {
				glColor3ub(0, 255, 0);
			}
			glVertex3f(q0.x, q0.y, q0.z);
			glVertex3f(q1.x, q1.y, q1.z);
			glEnd();
			if (!doingShadows) {
				glColor3ub(255, 0, 0);
			}
			float d = rand() % 5 + 5;
			//right
			if (q0.x < q1.x) {
				vertexs.push_back(q0.x);
				vertexs.push_back(q0.z);
				vertexs.push_back(q0.y);
				vertexs.push_back(q1.x);
				vertexs.push_back(q1.z);
				vertexs.push_back(q1.y);
				Test(q0, q1, c, 3);
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q1.y);
				//
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q1.y);
				Test(q1, q0, c, 3);
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q0.y);
				vertexs.push_back(q0.x);
				vertexs.push_back(q0.z);
				vertexs.push_back(q0.y);
			}
			else {
				vertexs.push_back(q0.x);
				vertexs.push_back(q0.z);
				vertexs.push_back(q0.y);
				vertexs.push_back(q1.x);
				vertexs.push_back(q1.z);
				vertexs.push_back(q1.y);
				Test1(q0, q1, c, 3);
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q1.y);
				//
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q1.y);
				Test1(q1, q0, c, 3);
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q0.y);
				vertexs.push_back(q0.x);
				vertexs.push_back(q0.z);
				vertexs.push_back(q0.y);
			}
			//left
			if (!doingShadows) {
				glColor3ub(0, 255, 0);
			}
			if (q0.x < q1.x) {
				vertexs.push_back(q0.x);
				vertexs.push_back(q0.z);
				vertexs.push_back(q0.y);
				vertexs.push_back(q1.x);
				vertexs.push_back(q1.z);
				vertexs.push_back(q1.y);
				Test1(q0, q1, c, 3);
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q1.y);
				//
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q1.y);
				Test1(q1, q0, c, 3);
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q0.y);
				vertexs.push_back(q0.x);
				vertexs.push_back(q0.z);
				vertexs.push_back(q0.y);
			}
			else {
				vertexs.push_back(q0.x);
				vertexs.push_back(q0.z);
				vertexs.push_back(q0.y);
				vertexs.push_back(q1.x);
				vertexs.push_back(q1.z);
				vertexs.push_back(q1.y);
				Test(q0, q1, c, 3);
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q1.y);
				//
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q1.y);
				Test(q1, q0, c, 3);
				vertexs.push_back(c.x);
				vertexs.push_back(c.z);
				vertexs.push_back(q0.y);
				vertexs.push_back(q0.x);
				vertexs.push_back(q0.z);
				vertexs.push_back(q0.y);
			}

		}
	}

	if (!this->elevation_shader) {
			this->elevation_shader = new
				Shader(
					"../src/Shaders/elevation.vs",
					nullptr, nullptr, nullptr,
					"../src/Shaders/elevation.fs");
	}

	if (!this->background_shader) {
			this->background_shader = new
				Shader(
					"../src/Shaders/background.vs",
					nullptr, nullptr, nullptr,
					"../src/Shaders/background.fs");
	}

	if (!this->screen_shader) {
		this->screen_shader = new
			Shader(
				"../src/Shaders/screen.vs",
				nullptr, nullptr, nullptr,
				"../src/Shaders/screen.fs");
		
		glGenFramebuffers(1, &this->framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
		// create a color attachment texture
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->pixel_w(), this->pixel_h(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->pixel_w(), this->pixel_h()); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	}

	glm::mat4 view;
	glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);

	glm::mat4 projection;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);

	float vertices[] = {
    // positions                          // texture coords
     1.0f,  1.0f, 0.0f,     1.0f, 1.0f,   // top right
     1.0f, -1.0f, 0.0f,     1.0f, 0.0f,   // bottom right
	-1.0f,  1.0f, 0.0f,     0.0f, 1.0f,    // top left 
	 1.0f, -1.0f, 0.0f,     1.0f, 0.0f,   // bottom right
    -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,     0.0f, 1.0f    // top left 
	};


	unsigned int VBO[2], VAO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    //Curve
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexs.size(), &vertexs[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//Ground
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

   


	/*glViewport(0, 0, w(), h());
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float wi=0, he=0;
	if (static_cast<float>(w()) / static_cast<float>(h()) >= 1) {
		he = 100 / static_cast<float>(w()) / static_cast<float>(h());
	}
	else {
		wi = 100 * static_cast<float>(w()) / static_cast<float>(h());
	}
	glOrtho(-wi, wi, -he, he, 200, -200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-90, 1, 0, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
	glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);*/

	// render
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	// make sure we clear the framebuffer's content
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Curve
	elevation_shader->Use();
	glm::mat4 model = glm::mat4(1.0f);
	// pass transformation matrices to the shader
	glUniformMatrix4fv(glGetUniformLocation(elevation_shader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(elevation_shader->Program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(elevation_shader->Program, "model"), 1, GL_FALSE, &model[0][0]);

	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, vertexs.size());

	//Ground
	background_shader->Use();
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::scale(trans, glm::vec3(100, 0, 100));
	// pass transformation matrices to the shader
	glUniformMatrix4fv(glGetUniformLocation(background_shader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(background_shader->Program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(background_shader->Program, "model"), 1, GL_FALSE, &trans[0][0]);

	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	//screen
	screen_shader->Use();
	glm::mat4 trans1 = glm::mat4(1.0f);
	trans1 = glm::scale(trans1, glm::vec3(100, 0, 100));
	// pass transformation matrices to the shader
	glUniformMatrix4fv(glGetUniformLocation(screen_shader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(screen_shader->Program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(screen_shader->Program, "model"), 1, GL_FALSE, &trans1[0][0]);
	glUniform1i(glGetUniformLocation(screen_shader->Program, "Texture"), 0);
	
	glBindVertexArray(VAO[1]);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteFramebuffers(1, &framebuffer);
	glUseProgram(0);

	if (!tw->trainCam->value()) {
		int ii = 0;
		int temp = selectedCube;
		for (ii = 0; ii < Curves.size(); ii++) {
			if (temp <= Curves[ii].points.size() - 1) {
				break;
			}
			else {
				temp -= Curves[ii].points.size();
			}
		}
		SelectedCurve = ii;
		SelectedNode = temp;
		for (int i = 0; i < Curves.size(); i++) {
			for (int j = 0; j < Curves[i].points.size(); j++) {
				if (SelectedCurve == i && j == SelectedNode)
					glColor3ub(240, 240, 30);
				else
					glColor3ub(240, 60, 60);
				Curves[i].points[j].draw();
			}
		}
		}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################
	for (int i = 0; i < Curves.size(); i++) {
		float t = 0;
		ControlPoint p1 = Curves[i].points[0];
		ControlPoint p2 = Curves[i].points[1];
		ControlPoint p3 = Curves[i].points[2];
		ControlPoint p4 = Curves[i].points[3];
		Pnt3f c;
		for (int j = 0; j < 100; j++) {
			Pnt3f q0 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, t);
			Pnt3f q1 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, t += 0.01);
			glBegin(GL_LINES);
			glColor3ub(0, 255, 0);
			glVertex3f(q0.x, q0.y, q0.z);
			glVertex3f(q1.x, q1.y, q1.z);
			glEnd();
			if (!doingShadows) {
				glColor3ub(255, 0, 0);
			}
		}
	}
#ifdef EXAMPLE_SOLUTION
		drawTrack(this, doingShadows);
#endif

		// draw the train
		//####################################################################
		// TODO: 
		//	call your own train drawing code
		//####################################################################
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
		if (!tw->trainCam->value())
			drawTrain(this, doingShadows);
#endif
}
// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();		

	// where is the mouse?
	int mx = Fl::event_x(); 
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
						5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	int count = 0;
	for (int i = 0;i < Curves.size(); i++) {
		for(int j=0;j<Curves[i].points.size();j++){
			glLoadName((GLuint) ((count++)+1));
			Curves[i].points[j].draw();
		}
	}


	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n",selectedCube);
}