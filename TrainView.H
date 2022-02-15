/************************************************************************
     File:        TrainView.H

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

#pragma once

// Preclarify for preventing the compiler error
class TrainWindow;
class CTrack;
class Model;

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


// The TrainView also needs to handle its user interface events.
// since this is an FLTk Window, we need to include FlTk Headers
// the pragmas turn off the warnings from FlTk - our code should have no warnings,
// but their code doesn't meet that standard
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_Gl_Window.h>
#pragma warning(pop)
// this uses the old ArcBall Code
#include "Utilities/ArcBallCam.H"
#include "Curve.h"

#include "RenderUtilities/BufferObject.h"
#include "RenderUtilities/Shader.h"
#include "RenderUtilities/Texture.h"

class TrainView : public Fl_Gl_Window
{
	public:
		// note that we keep the "standard widget" constructor arguments
		TrainView(int x, int y, int w, int h, const char* l = 0);

		// overrides of important window things
		virtual int handle(int);
		virtual void draw();

		// all of the actual drawing happens in this routine
		// it has to be encapsulated, since we draw differently if
		// we're drawing shadows (no colors, for example)
		void drawStuff(bool doingShadows=false);

		// setup the projection - assuming that the projection stack has been
		// cleared for you
		void setProjection();

		// Reset the Arc ball control
		void resetArcball();

		// pick a point (for when the mouse goes down)
		void doPick();

		void draw_elevation_map();
		void draw_gradient_map();
		void jacobi();

		void push_gradient_data(Pnt3f);
		void push_elevation_data(Pnt3f);
	public:
		ArcBallCam		arcball;			// keep an ArcBall for the UI
		int				selectedCube = -1;  // simple - just remember which cube is selected

		TrainWindow*	tw;				// The parent of this display window
		CTrack*			m_pTrack;		// The track of the entire scene
		vector<Curve>   Curves;
		int 			SelectedCurve;
		int 			SelectedNode;

		Shader* elevation_shader = nullptr;
		Shader* background_shader = nullptr;
		Shader* screen_shader = nullptr;
		Shader* gradient_shader = nullptr;
		Shader* heightmap_shader = nullptr;
		Shader* heightmap_shader1 = nullptr;

		Model* wave_model = nullptr;

		Texture2D* background_texture = nullptr;

		unsigned int framebuffer;
		unsigned int textureColorbuffer;
		unsigned int rbo;
		unsigned int textureColorbuffer1;
		unsigned int rbo1;

		int gridsize = 512;
		int grid0_size = gridsize / 4;
		


		vector<float> gradient_data;
		vector<float> elevation_data;

		unsigned int textureColorbuffer2;
		float* grid0;
		float* grid1;
		float* data;


		uint8_t* ImageBuffer = new uint8_t[gridsize * gridsize * 4];
		uint8_t* ImageBuffer1 = new uint8_t[gridsize * gridsize * 4];

		glm::mat4 view;
		glm::mat4 projection;
};