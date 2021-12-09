#include "ControlPoint.H"
#include <vector>

using std::vector;

class Curve
{
    public:
        public:		
		// Constructor
		Curve();

	public:
		// when we want to clear the control points, we really "reset" them 
		// to have 4 default positions (since we should never have fewer
		// than 4 points)
		void resetPoints();
		vector<Pnt3f> arclength_points;

		// read and write to files
		// void readPoints(const char* filename);
		// void writePoints(const char* filename);

	public:
		// rather than have generic objects, we make a special case for these few
		// objects that we know that all implementations are going to need and that
		// we're going to have to handle specially
		vector<ControlPoint> points;

		//###################################################################
		// TODO: you might want to do this differently
		//###################################################################
		// the state of the train - basically, all I need to remember is where
		// it is in parameter space
		float trainU;
};