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


		void drawStuff(bool doingShadows=false);

		void setProjection();
		void SetCamera();
		// Reset the Arc ball control
		void resetArcball();
		void doPick();
		void initElevationMap();
		void Rasterization_ElevationMap();
		void initGradientMap();
		void Rasterization_GradientMap();
		void initGradientMapDiffuse();
		void initTest();
		void Diffuse_GradientMap();
		void jacobi();



		void push_vertexDatas(Pnt3f, int Area);
	public:

		ArcBallCam		arcball;			// keep an ArcBall for the UI
		int				selectedCube = -1;  // simple - just remember which cube is selected

		TrainWindow*	tw;				// The parent of this display window
		CTrack*			m_pTrack;		// The track of the entire scene
		vector<Curve>   Curves;
		int 			SelectedCurve;
		int 			SelectedNode;

		float vertices[12] = {
			// positions           // texture coords
			1.0f,  1.0f, 
			1.0f, -1.0f, 
		   -1.0f,  1.0f, 
			1.0f, -1.0f, 
		   -1.0f, -1.0f, 
		   -1.0f,  1.0f, 		};

		float borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		Shader* elevation_shader = nullptr;
		Shader* background_shader = nullptr;
		Shader* screen_shader = nullptr;
		Shader* gradient_shader = nullptr;
		Shader* gradientcross_shader = nullptr;
		Shader* gradientnorm_shader = nullptr;
		Shader* heightmap_shader = nullptr;
		Shader* heightmap_shader1 = nullptr;
		Shader* overlay_shader = nullptr;
		Shader* diffuse_shader = nullptr;
		Shader* jacobi_shader = nullptr;

		Texture2D* mountain_texture = nullptr;

		Model* wave_model = nullptr;

		Texture2D* background_texture = nullptr;

		unsigned int framebuffer;


		unsigned int vao2D[1] = {0};
		unsigned int vbo2D[1];
		unsigned int vaoRasterization[1];
		unsigned int vboRasterization[1];

		/// <summary>
		/// Elevation
		/// </summary>
		unsigned int framebufferElevetionMap=0;
		unsigned int textureElevetionMap;
		unsigned int rboElevetionMap;
		/// <summary>
		/// Gradient
		/// </summary>
		unsigned int framebufferGradientMap =0;
		unsigned int textureGradientMap;
		unsigned int rboGradientMap;
		/// <summary>
		/// Diffuse Gradient
		/// </summary>
		GLuint framebufferDiffuse[2] = { 0 };
		unsigned int textureDiffuse[2];
		unsigned int rboDiffuse[2];

		/// <summary>
		/// Jacobi Gradient
		/// </summary>
		unsigned int framebufferJacobi[4];
		unsigned int textureJacobi[4];
		unsigned int rboJacobi[4];

		/// <summary>
		/// Gradient Cross
		/// </summary>
		unsigned int framebufferCross = -1;
		unsigned int textureCross;
		unsigned int rboCross;

		int iteration = 20;
		int finestSize = 2048;
		int coarsestSize = finestSize / 4;
		int middleSize = finestSize / 2;

		int CanvasHeight = 400;
		int CanvasWidth = 400;
		
		vector<float> vertexDatas;

		glm::mat4 view;
		glm::mat4 projection;
};