/************************************************************************
     File:        CallBacks.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Header file to define callback functions.
						define the callbacks for the TrainWindow

						these are little functions that get called when the 
						various widgets
						get accessed (or the fltk timer ticks). these 
						functions are used 
						when TrainWindow sets itself up.

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/
#pragma once

// Preclarify to prevent the compiler error
class TrainWindow;
class Fl_Widget;

// Reset the train window
void resetCB(Fl_Widget*, TrainWindow* tw);
// Something change and thus we need to update the view
void damageCB(Fl_Widget*, TrainWindow* tw);

// Callback that adds a new point to the spline
// idea: add the point AFTER the selected point
void addPointCB(Fl_Widget*, TrainWindow* tw);


// Callback that deletes a point from the spline
void deletePointCB(Fl_Widget*, TrainWindow* tw);


// Callbacks for advancing/pulling back train
void forwCB(Fl_Widget*, TrainWindow* tw);
void backCB(Fl_Widget*, TrainWindow* tw);

// Idle callback: for run the step of the window
void runButtonCB(TrainWindow* tw);

// For load and save buttons
void loadCB(Fl_Widget*, TrainWindow* tw);
void saveCB(Fl_Widget*, TrainWindow* tw);

//Landscape
void addCurve(Fl_Widget*, TrainWindow* tw);
void deleteCurve(Fl_Widget*, TrainWindow* tw);

void heightCB(Fl_Widget*, TrainWindow* tw);
void radiusCB(Fl_Widget*, TrainWindow* tw);
void aCB(Fl_Widget*, TrainWindow* tw);
void bCB(Fl_Widget*, TrainWindow* tw);
void thetaCB(Fl_Widget*, TrainWindow* tw);
void phiCB(Fl_Widget*, TrainWindow* tw);
void iterationCB(Fl_Widget*, TrainWindow* tw);