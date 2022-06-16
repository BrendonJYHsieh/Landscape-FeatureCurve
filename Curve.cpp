/************************************************************************
     File:        Track.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Container for the "World"

						This provides a container for all of the "stuff" 
						in the world.

						It could have been all global variables, or it could 
						have just been
						contained in the window. The advantage of doing it 
						this way is that
						we might have multiple windows looking at the same 
						world. But, I don't	think we'll actually do that.

						See the readme for commentary on code style

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include "Curve.H"

#include <FL/fl_ask.h>

//****************************************************************************
//
// * Constructor
//============================================================================
Curve::
Curve() : trainU(0)
//============================================================================
{
	resetPoints();
}

//****************************************************************************
//
// * provide a default set of points
//============================================================================
void Curve::
resetPoints()
//============================================================================
{
	int r = rand() % 30 +10;
	points.clear();
	points.push_back(ControlPoint(Pnt3f( 0,0,-70)));
	points.push_back(ControlPoint(Pnt3f( 0,100,-30)));
	points.push_back(ControlPoint(Pnt3f( 0,100,30)));
	points.push_back(ControlPoint(Pnt3f( 0,0,70)));



	// we had better put the train back at the start of the track...
	trainU = 0.0;
}


