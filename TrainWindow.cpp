/************************************************************************
     File:        TrainWindow.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						this class defines the window in which the project 
						runs - its the outer windows that contain all of 
						the widgets, including the "TrainView" which has the 
						actual OpenGL window in which the train is drawn

						You might want to modify this class to add new widgets
						for controlling	your train

						This takes care of lots of things - including installing 
						itself into the FlTk "idle" loop so that we get periodic 
						updates (if we're running the train).


     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <FL/fl.h>
#include <FL/Fl_Box.h>

// for using the real time clock
#include <time.h>

#include "TrainWindow.H"
#include "TrainView.H"
#include "CallBacks.H"



//************************************************************************
//
// * Constructor
//========================================================================
TrainWindow::
TrainWindow(const int x, const int y) 
	: Fl_Double_Window(x,y,800,600,"Train and Roller Coaster")
//========================================================================
{
	// make all of the widgets
	begin();	// add to this widget
	{
		int pty=5;			// where the last widgets were drawn

		trainView = new TrainView(5,5,590,590);
		trainView->tw = this;
		trainView->m_pTrack = &m_Track;
		this->resizable(trainView);

		// to make resizing work better, put all the widgets in a group
		widgets = new Fl_Group(600,5,190,590);
		widgets->begin();

		// runButton = new Fl_Button(605,pty,60,20,"Run");
		// togglify(runButton);

		// Fl_Button* fb = new Fl_Button(700,pty,25,20,"@>>");
		// fb->callback((Fl_Callback*)forwCB,this);
		// Fl_Button* rb = new Fl_Button(670,pty,25,20,"@<<");
		// rb->callback((Fl_Callback*)backCB,this);
		
		// arcLength = new Fl_Button(730,pty,65,20,"ArcLength");
		// togglify(arcLength,1);
  
		// pty+=25;
		// speed = new Fl_Value_Slider(655,pty,140,20,"speed");
		// speed->range(0,10);
		// speed->value(2);
		// speed->align(FL_ALIGN_LEFT);
		// speed->type(FL_HORIZONTAL);

		// pty += 30;

		// camera buttons - in a radio button group
		Fl_Group* camGroup = new Fl_Group(600,pty,195,20);
		camGroup->begin();
		worldCam = new Fl_Button(605, pty, 60, 20, "World");
        worldCam->type(FL_RADIO_BUTTON);		// radio button
        worldCam->value(1);			// turned on
        worldCam->selection_color((Fl_Color)3); // yellow when pressed
		worldCam->callback((Fl_Callback*)damageCB,this);
		trainCam = new Fl_Button(670, pty, 60, 20, "Train");
        trainCam->type(FL_RADIO_BUTTON);
        trainCam->value(0);
        trainCam->selection_color((Fl_Color)3);
		trainCam->callback((Fl_Callback*)damageCB,this);
		topCam = new Fl_Button(735, pty, 60, 20, "Top");
        topCam->type(FL_RADIO_BUTTON);
        topCam->value(0);
        topCam->selection_color((Fl_Color)3);
		topCam->callback((Fl_Callback*)damageCB,this);
		camGroup->end();

		pty += 30;
		// add and delete points
		Fl_Button* output = new Fl_Button(600, pty, 80, 30, "Output");
		output->callback((Fl_Callback*)Output, this);
		// // browser to select spline types
		// // TODO: make sure these choices are the same as what the code supports
		// splineBrowser = new Fl_Browser(605,pty,120,75,"Spline Type");
		// splineBrowser->type(2);		// select
		// splineBrowser->callback((Fl_Callback*)damageCB,this);
		// splineBrowser->add("Linear");
		// splineBrowser->add("Cardinal Cubic");
		// splineBrowser->add("Cubic B-Spline");
		// splineBrowser->select(2);

		pty += 50;

		// add and delete points
		Fl_Button* ac = new Fl_Button(600,pty,80,30,"Add Curve");
		ac->callback((Fl_Callback*)addCurve,this);
		Fl_Button* dc = new Fl_Button(690,pty,100,30,"Delete Curve");
		dc->callback((Fl_Callback*)deleteCurve,this);

		pty+=50;
		segment = new Fl_Value_Slider(655,pty,140,20,"Segment");
		segment->range(0,10000);
		segment->value(5000);
		segment->align(FL_ALIGN_LEFT);
		segment->type(FL_HORIZONTAL);
		segment->callback((Fl_Callback*)damageCB, this);

		pty += 50;
		interval = new Fl_Value_Slider(655, pty, 140, 20, "Interval");
		interval->range(0.1, 1);
		interval->value(0.1);
		interval->align(FL_ALIGN_LEFT);
		interval->type(FL_HORIZONTAL);
		interval->callback((Fl_Callback*)damageCB, this);

		pty += 50;
		height = new Fl_Value_Slider(655, pty, 140, 20, "Height");
		height->range(0, 255);
		height->value(5);
		height->align(FL_ALIGN_LEFT);
		height->type(FL_HORIZONTAL);
		height->callback((Fl_Callback*)heightCB, this);

		pty += 50;
		radius = new Fl_Value_Slider(655, pty, 140, 20, "Radius");
		radius->range(0, 100);
		radius->value(0.1);
		radius->align(FL_ALIGN_LEFT);
		radius->type(FL_HORIZONTAL);
		radius->callback((Fl_Callback*)radiusCB, this);

		pty += 50;
		a = new Fl_Value_Slider(655, pty, 140, 20, "a");
		a->range(0, 200);
		a->value(10);
		a->align(FL_ALIGN_LEFT);
		a->type(FL_HORIZONTAL);
		a->callback((Fl_Callback*)aCB, this);

		pty += 50;
		theta = new Fl_Value_Slider(655, pty, 140, 20, "Theta");
		theta->range(0, 90);
		theta->value(50);
		theta->align(FL_ALIGN_LEFT);
		theta->type(FL_HORIZONTAL);
		theta->callback((Fl_Callback*)thetaCB, this);

		pty += 50;
		b = new Fl_Value_Slider(655, pty, 140, 20, "b");
		b->range(0, 200);
		b->value(10);
		b->align(FL_ALIGN_LEFT);
		b->type(FL_HORIZONTAL);
		b->callback((Fl_Callback*)bCB, this);

		pty += 50;
		phi = new Fl_Value_Slider(655, pty, 140, 20, "Phi");
		phi->range(0, 90);
		phi->value(0);
		phi->align(FL_ALIGN_LEFT);
		phi->type(FL_HORIZONTAL);
		phi->callback((Fl_Callback*)phiCB, this);

		pty += 50;
		iteration = new Fl_Value_Slider(655, pty, 140, 20, "Iter");
		iteration->range(1, 5000);
		iteration->value(1);
		iteration->align(FL_ALIGN_LEFT);
		iteration->type(FL_HORIZONTAL);
		iteration->callback((Fl_Callback*)iterationCB, this);
		// // reset the points
		// resetButton = new Fl_Button(735,pty,60,20,"Reset");
		// resetButton->callback((Fl_Callback*)resetCB,this);
		// Fl_Button* loadb = new Fl_Button(605,pty,60,20,"Load");
		// loadb->callback((Fl_Callback*) loadCB, this);
		// Fl_Button* saveb = new Fl_Button(670,pty,60,20,"Save");
		// saveb->callback((Fl_Callback*) saveCB, this);

		// pty += 25;
		// // roll the points
		// Fl_Button* rx = new Fl_Button(605,pty,30,20,"R+X");
		// rx->callback((Fl_Callback*)rpxCB,this);
		// Fl_Button* rxp = new Fl_Button(635,pty,30,20,"R-X");
		// rxp->callback((Fl_Callback*)rmxCB,this);
		// Fl_Button* rz = new Fl_Button(670,pty,30,20,"R+Z");
		// rz->callback((Fl_Callback*)rpzCB,this);
		// Fl_Button* rzp = new Fl_Button(700,pty,30,20,"R-Z");
		// rzp->callback((Fl_Callback*)rmzCB,this);

		// pty+=30;

		// TODO: add widgets for all of your fancier features here
#ifdef EXAMPLE_SOLUTION
		makeExampleWidgets(this,pty);
#endif

		// we need to make a little phantom widget to have things resize correctly
		Fl_Box* resizebox = new Fl_Box(600,595,200,5);
		widgets->resizable(resizebox);

		widgets->end();
	}
	end();	// done adding to this widget

	// set up callback on idle
	Fl::add_idle((void (*)(void*))runButtonCB,this);
}

//************************************************************************
//
// * handy utility to make a button into a toggle
//========================================================================
void TrainWindow::
togglify(Fl_Button* b, int val)
//========================================================================
{
	b->type(FL_TOGGLE_BUTTON);		// toggle
	b->value(val);		// turned off
	b->selection_color((Fl_Color)3); // yellow when pressed	
	b->callback((Fl_Callback*)damageCB,this);
}

//************************************************************************
//
// *
//========================================================================
void TrainWindow::
damageMe()
//========================================================================
{
	// if (trainView->SelectedNode >= ((int)trainView->Curves[trainView->SelectedCurve].points.size()))
	// 	trainView->SelectedNode = 0;
	trainView->damage(1);
}

//************************************************************************
//
// * This will get called (approximately) 30 times per second
//   if the run button is pressed
//========================================================================
void TrainWindow::
advanceTrain(float dir)
//========================================================================
{
	//#####################################################################
	// TODO: make this work for your train
	//#####################################################################
#ifdef EXAMPLE_SOLUTION
	// note - we give a little bit more example code here than normal,
	// so you can see how this works

	if (arcLength->value()) {
		float vel = ew.physics->value() ? physicsSpeed(this) : dir * (float)speed->value();
		world.trainU += arclenVtoV(world.trainU, vel, this);
	} else {
		world.trainU +=  dir * ((float)speed->value() * .1f);
	}

	float nct = static_cast<float>(world.points.size());
	if (world.trainU > nct) world.trainU -= nct;
	if (world.trainU < 0) world.trainU += nct;
#endif
}