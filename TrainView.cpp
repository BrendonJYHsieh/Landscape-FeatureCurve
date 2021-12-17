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
Pnt3f Intersect(Pnt3f A, Pnt3f B,float length) {
	Pnt3f C;
	bool t = false;
	float m = (B.z - A.z) / (B.x - A.x);
	float _m = -1 / m;
	float bb = (_m * B.x) - B.z;
	float a = _m , b = -1, c = bb;
	float d = m, e = -1, f = -length * sqrt(m*m+1) + (m * B.x - B.z);

	float det = -_m + m;
	C.x = (c * e - b * f) / det;
	C.y = B.y;
	C.z = (-d * c + a * f) / det;
	return C;
}
Pnt3f _Intersect(Pnt3f A, Pnt3f B,float length) {
	Pnt3f C;
	bool t = false;
	float m = (B.z - A.z) / (B.x - A.x);
	float _m = -1 / m;
	float bb = (_m * B.x) - B.z;
	float a = _m , b = -1, c = bb;
	float d = m, e = -1, f = length*sqrt(m*m+1) + (m * B.x - B.z);

	float det = -_m + m;
	C.x = (c * e - b * f) / det;
	C.y = B.y;
	C.z = (-d * c + a * f) / det;
	return C;
}

glm::vec3 Rotate(glm::vec3 n, glm::vec3 v, float degree) {
	float theta = glm::radians(degree);
	n = glm::normalize(n);
	glm::mat3x3 T = {
		n.x * n.x * (1 - cos(theta)) + cos(theta)      , n.x * n.y * (1 - cos(theta)) - n.z*sin(theta)  , n.x * n.z * (1 - cos(theta)) + n.y * sin(theta),
		n.x * n.y * (1 - cos(theta)) + n.z * sin(theta), n.y * n.y * (1 - cos(theta)) + cos(theta)      , n.y * n.z* (1 - cos(theta)) - n.x *sin(theta),
		n.x * n.z * (1 - cos(theta)) - n.y * sin(theta), n.y * n.z * (1 - cos(theta)) + n.z * sin(theta), n.z*n.z * (1 - cos(theta)) + cos(theta)
	};
	return glm::vec3(v.x, v.y, v.z) * T;
}

glm::vec3 Pnt3_to_Vec3(Pnt3f a) {
	return glm::vec3(a.x, a.y, a.z);
}

Pnt3f Vec3_to_Pnt3(glm::vec3 a) {
	return Pnt3f(a.x, a.y, a.z);
}

void TrainView::addbufferdata(Pnt3f q0) {
	elevation_intersections.push_back(q0.x);
	elevation_intersections.push_back(q0.y);
	elevation_intersections.push_back(q0.z);
	elevation_intersections.push_back(q0.normal.x);
	elevation_intersections.push_back(q0.normal.y);
	elevation_intersections.push_back(q0.normal.z);
}

void TrainView::draw_elevation_map() {
	float vertices[] = {
		// positions                          // texture coords
		 1.0f,  1.0f, 0.0f,     1.0f, 1.0f,   // top right
		 1.0f, -1.0f, 0.0f,     1.0f, 0.0f,   // bottom right
		-1.0f,  1.0f, 0.0f,     0.0f, 1.0f,    // top left 
		 1.0f, -1.0f, 0.0f,     1.0f, 0.0f,   // bottom right
		-1.0f, -1.0f, 0.0f,     0.0f, 0.0f,   // bottom left
		-1.0f,  1.0f, 0.0f,     0.0f, 1.0f    // top left 
	};

	glGenFramebuffers(1, &this->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glActiveTexture(GL_TEXTURE0);
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
	
	/*VAO*/
	unsigned int VBO[2], VAO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    //Curve
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * elevation_intersections.size(), &elevation_intersections[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//Ground
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// render
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	// make sure we clear the framebuffer's content
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Curve
	elevation_shader->Use();
	glm::mat4 model = glm::mat4(1.0f);
	
	float wi, he;
	if ((static_cast<float>(w()) / static_cast<float>(h())) >= 1) {
		wi = 100;
		he = wi / (static_cast<float>(w()) / static_cast<float>(h()));
	}
	else {
		he = 100;
		wi = he * (static_cast<float>(w()) / static_cast<float>(h()));
	}
	glViewport(0, 0, w(), h());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-wi, wi, -he, he, 200, -200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-90, 1, 0, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
	glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(elevation_shader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(elevation_shader->Program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(elevation_shader->Program, "model"), 1, GL_FALSE, &model[0][0]);

	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, elevation_intersections.size());

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

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);

}
void TrainView::draw_gradient_map() {
	float vertices[] = {
		// positions                          // texture coords
		 1.0f,  1.0f, 0.0f,     1.0f, 1.0f,   // top right
		 1.0f, -1.0f, 0.0f,     1.0f, 0.0f,   // bottom right
		-1.0f,  1.0f, 0.0f,     0.0f, 1.0f,    // top left 
		 1.0f, -1.0f, 0.0f,     1.0f, 0.0f,   // bottom right
		-1.0f, -1.0f, 0.0f,     0.0f, 0.0f,   // bottom left
		-1.0f,  1.0f, 0.0f,     0.0f, 1.0f    // top left 
	};

	glGenFramebuffers(1, &this->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glActiveTexture(GL_TEXTURE1);
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

	/*VAO*/
	unsigned int VBO[2], VAO[2];
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	//Curve
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * elevation_intersections.size(), &elevation_intersections[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Ground
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// render
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	// make sure we clear the framebuffer's content
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Curve
	gradient_shader->Use();
	glm::mat4 model = glm::mat4(1.0f);

	float wi, he;
	if ((static_cast<float>(w()) / static_cast<float>(h())) >= 1) {
		wi = 100;
		he = wi / (static_cast<float>(w()) / static_cast<float>(h()));
	}
	else {
		he = 100;
		wi = he * (static_cast<float>(w()) / static_cast<float>(h()));
	}
	glViewport(0, 0, w(), h());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-wi, wi, -he, he, 200, -200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-90, 1, 0, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
	glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(gradient_shader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(gradient_shader->Program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(gradient_shader->Program, "model"), 1, GL_FALSE, &model[0][0]);

	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, elevation_intersections.size());

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

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);

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
	glPointSize(5);
	glBegin(GL_POINTS);
	glColor3ub(0, 255, 0);
	glVertex3f(100, 5, 0);
	glVertex3f(-100, 0, 0);
	glEnd();
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
	float aspect = (static_cast<float>(w()) / static_cast<float>(h()));

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 100;
			he = wi / aspect;
		} 
		else {
			he = 100;
			wi = he * aspect;
		}
		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		//glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
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
	elevation_intersections.clear();
	for (int i = 0; i < Curves.size(); i++) {
		float t = 0;
		float divide = tw->segment->value();
		float interval = tw->interval->value();
		float count = 0;
		Curves[i].arclength_points.clear();
		ControlPoint p1 = Curves[i].points[0];
		ControlPoint p2 = Curves[i].points[1];
		ControlPoint p3 = Curves[i].points[2];
		ControlPoint p4 = Curves[i].points[3];
		Curves[i].arclength_points.push_back(p1.pos);
		Pnt3f c;
		for (int j = 0; j < divide; j++) {
			Pnt3f Q0 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, t);
			Pnt3f Q1 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, t += 1/divide);
			count += (Q1 - Q0).length2D();
			if (count > interval) {
				count = 0;
				Curves[i].arclength_points.push_back(Q1);
			}	
		}
		Curves[i].arclength_points.push_back(p4.pos);
	}
	for (int i = 0; i < Curves.size(); i++) {
		Curves[i].arclength_points[Curves[i].arclength_points.size() - 1].r = Curves[i].points[3].pos.r;
		Curves[i].arclength_points[0].r = Curves[i].points[0].pos.r;

		float r_init = Curves[i].arclength_points[0].r;
		float r_interporate = (Curves[i].arclength_points[Curves[i].arclength_points.size() - 1].r - Curves[i].arclength_points[0].r)/ (Curves[i].arclength_points.size()-1);

		float a_init = Curves[i].arclength_points[0].a;
		float a_interporate = (Curves[i].arclength_points[Curves[i].arclength_points.size() - 1].a - Curves[i].arclength_points[0].a) / (Curves[i].arclength_points.size() - 1);

		float b_init = Curves[i].arclength_points[0].b;
		float b_interporate = (Curves[i].arclength_points[Curves[i].arclength_points.size() - 1].b - Curves[i].arclength_points[0].b)/ (Curves[i].arclength_points.size()-1);

		float phi_init = Curves[i].arclength_points[0].phi;
		float phi_interporate = (Curves[i].arclength_points[Curves[i].arclength_points.size() - 1].phi - Curves[i].arclength_points[0].phi) / (Curves[i].arclength_points.size() - 1);

		float theta_init = Curves[i].arclength_points[0].theta;
		float theta_interporate = (Curves[i].arclength_points[Curves[i].arclength_points.size() - 1].theta - Curves[i].arclength_points[0].theta) / (Curves[i].arclength_points.size() - 1);
		Pnt3f p2,_p2,p4,_p4, p6, _p6;
		for (int j = 0; j < Curves[i].arclength_points.size()-2; j++) {
			Pnt3f q0 = Curves[i].arclength_points[j], q1 = Curves[i].arclength_points[j+1],q2,q3,q4,q5,q6,q7;
			
			if (q0.x < q1.x) {
				q2 = Intersect(q0, q1, r_init + r_interporate * (j + 1));
				q3 = Intersect(q1, q0, r_init + r_interporate * j);
				q4 = q2;
				q5 = q3;
				q6 = Intersect(q0, q1, r_init + r_interporate * (j + 1) + b_init + b_interporate*(j+1));
				q7 = Intersect(q1, q0, r_init + r_interporate * j + b_init + b_interporate * j);
			}
			else {
				q2 = _Intersect(q0, q1, r_init + r_interporate * (j + 1));
				q3 = _Intersect(q1, q0, r_init + r_interporate * j);
				q4 = q2;
				q5 = q3;
				q6 = _Intersect(q0, q1, r_init + r_interporate * (j + 1) + b_init + b_interporate * (j + 1));
				q7 = _Intersect(q1, q0, r_init + r_interporate * j + b_init + b_interporate * j);
			}
			glm::vec3 Axis = glm::vec3(q3.x - q2.x, q3.y - q2.y, q3.z - q2.z);
			q6 = Vec3_to_Pnt3(((Rotate(Axis, Pnt3_to_Vec3(q6 - q2), -90 + phi_init + phi_interporate * (j + 1)))) + Pnt3_to_Vec3(q2));
			q7 = Vec3_to_Pnt3(((Rotate(Axis, Pnt3_to_Vec3(q7 - q3), -90 + phi_init + phi_interporate * (j)))) + Pnt3_to_Vec3(q3));

			glm::vec3 normal = glm::normalize(glm::cross(Pnt3_to_Vec3(q0)-Pnt3_to_Vec3(q1), Pnt3_to_Vec3(q0) - Pnt3_to_Vec3(q3)));

			q0.normal = normal;
			q1.normal = normal;
			q2.normal = normal;
			q3.normal = normal;
			q4.normal = Rotate(Axis, normal, -90 + phi_init + phi_interporate * (j + 1));
			q5.normal = Rotate(Axis, normal, -90 + phi_init + phi_interporate * j);
			q6.normal = glm::vec3(0, 0, 0);
			q7.normal = glm::vec3(0, 0, 0);

			//q0,q1,q2
			addbufferdata(q0);
			addbufferdata(q1);
			addbufferdata(q2);
			//q2,q3,q0
			addbufferdata(q2);
			addbufferdata(q3);
			addbufferdata(q0);
			//q2,q4,q3
			addbufferdata(q4);
			addbufferdata(q6);
			addbufferdata(q5);
			//q4,q5,q3
			addbufferdata(q6);
			addbufferdata(q7);
			addbufferdata(q5);
			if (j > 0) {
				//q0,q3,p2
				addbufferdata(q0);
				addbufferdata(q3);
				addbufferdata(p2);
				//q0,q3,p2
				addbufferdata(p4);
				addbufferdata(q5);
				addbufferdata(q7);
				//q0,q3,p2
				addbufferdata(q7);
				addbufferdata(p6);
				addbufferdata(p4);
			}
			p2 = q2;
			p4 = q4;
			p6 = q6;

			if (q0.x < q1.x) {
				q2 = _Intersect(q0, q1, r_init + r_interporate * (j + 1));
				q3 = _Intersect(q1, q0, r_init + r_interporate * j);
				q4 = q2;
				q5 = q3;
				q6 = _Intersect(q0, q1, r_init + r_interporate * (j + 1) + a_init + a_interporate * (j + 1));
				q7 = _Intersect(q1, q0, r_init + r_interporate * j + a_init + a_interporate * j);
			}
			else {
				q2 = Intersect(q0, q1, r_init + r_interporate * (j + 1));
				q3 = Intersect(q1, q0, r_init + r_interporate * j);
				q4 = q2;
				q5 = q3;
				q6 = Intersect(q0, q1, r_init + r_interporate * (j + 1) + a_init + a_interporate * (j + 1));
				q7 = Intersect(q1, q0, r_init + r_interporate * j + a_init + a_interporate * j);
			}
			Axis = glm::vec3(q3.x - q2.x, q3.y - q2.y, q3.z - q2.z);
			q6 = Vec3_to_Pnt3(((Rotate(Axis, Pnt3_to_Vec3(q6 - q2), 90 - theta_init - theta_interporate * (j + 1)))) + Pnt3_to_Vec3(q2));
			q7 = Vec3_to_Pnt3(((Rotate(Axis, Pnt3_to_Vec3(q7 - q3), 90 - theta_init - theta_interporate * (j)))) + Pnt3_to_Vec3(q3));

			q0.normal = normal;
			q1.normal = normal;
			q2.normal = normal;
			q3.normal = normal;
			q4.normal = Rotate(Axis, normal, 90 - theta_init - theta_interporate * (j + 1));
			q5.normal = Rotate(Axis, normal, 90 - theta_init - theta_interporate * j);
			q6.normal = glm::vec3(0, 0, 0);
			q7.normal = glm::vec3(0, 0, 0);

			//q0,q1,q2
			addbufferdata(q0);
			addbufferdata(q1);
			addbufferdata(q2);
			//q2,q3,q0
			addbufferdata(q2);
			addbufferdata(q3);
			addbufferdata(q0);
			//q4,q6,q5
			addbufferdata(q4);
			addbufferdata(q6);
			addbufferdata(q5);
			//q6,q7,q5
			addbufferdata(q6);
			addbufferdata(q7);
			addbufferdata(q5);

			if (j > 0) {
				//q0,q3,p2
				addbufferdata(q0);
				addbufferdata(q3);
				addbufferdata(_p2);
				//q0,q3,p2
				addbufferdata(_p4);
				addbufferdata(q5);
				addbufferdata(q7);
				//q0,q3,p2
				addbufferdata(q7);
				addbufferdata(_p6);
				addbufferdata(_p4);
			}
			_p2 = q2;
			_p4 = q4;
			_p6 = q6;
		}
	}

	if (!this->elevation_shader) {
			this->elevation_shader = new
				Shader(
					"../src/Shaders/elevation.vs",
					nullptr, nullptr, nullptr,
					"../src/Shaders/elevation.fs");
	}

	if (!this->gradient_shader) {
		this->gradient_shader = new
			Shader(
				"../src/Shaders/gradient.vs",
				nullptr, nullptr, nullptr,
				"../src/Shaders/gradient.fs");
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
	}

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * elevation_intersections.size(), &elevation_intersections[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Ground
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	draw_elevation_map();
	draw_gradient_map();


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();
	glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
	glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);

	

	//Ground
	screen_shader->Use();
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(200, 0, 0));
	trans = glm::scale(trans, glm::vec3(100, 0, 100));
	
	// pass transformation matrices to the shader
	glUniformMatrix4fv(glGetUniformLocation(screen_shader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(screen_shader->Program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(screen_shader->Program, "model"), 1, GL_FALSE, &trans[0][0]);
	glUniform1i(glGetUniformLocation(screen_shader->Program, "Texture"), 0);

	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glm::mat4 trans1 = glm::mat4(1.0f);
	trans1 = glm::translate(trans1, glm::vec3(-200, 0, 0));
	trans1 = glm::scale(trans1, glm::vec3(100, 0, 100));

	// pass transformation matrices to the shader
	glUniformMatrix4fv(glGetUniformLocation(screen_shader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(screen_shader->Program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(screen_shader->Program, "model"), 1, GL_FALSE, &trans1[0][0]);
	glUniform1i(glGetUniformLocation(screen_shader->Program, "Texture"), 1);

	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Curve
	gradient_shader->Use();
	glm::mat4 model = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(elevation_shader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(elevation_shader->Program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(elevation_shader->Program, "model"), 1, GL_FALSE, &model[0][0]);

	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, elevation_intersections.size());

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &textureColorbuffer);
	glDeleteRenderbuffers(1, &rbo);
	glUseProgram(0);

	if (!tw->trainCam->value()) {
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
		}
	}
}

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

		tw->radius->value(Curves[ii].points[temp].pos.r);
		tw->a->value(Curves[ii].points[temp].pos.a);
		tw->b->value(Curves[ii].points[temp].pos.b);
		tw->theta->value(Curves[ii].points[temp].pos.theta);
		tw->phi->value(Curves[ii].points[temp].pos.phi);

	} else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n",selectedCube);
}