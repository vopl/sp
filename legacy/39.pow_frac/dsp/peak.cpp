//-------------------------------------------------------------------------------------------------------
// peak Filter VST plug in
// Nov 6, 2005, Kevin Kuang
// http://ccrma.stanford.edu/~kuangzn
//-------------------------------------------------------------------------------------------------------

#ifndef __peak_H
#include "peak.hpp"
#endif

#include <stdio.h>
#include <math.h>




int sign(double);

//-------------------------------------------------------------------------------------------------------
peak::peak ()
{
	fGain	= 0.5;						// default input gain = 0
	fQ		= 0.5;						// default 1/band width = 0
	fPhi	= 0.5;						// default trans freq = pi/2
	fL		= 0.5;						// default boost/decrease factor = 0
	sampleCount = 0;					// reset sample counter

}


//-------------------------------------------------------------------------------------------------------
peak::~peak ()
{
	// nothing to do here
}


//-----------------------------------------------------------------------------------------
void peak::setParameter (long index, double value)
{
	switch (index)
	{
	case kGain:
		fGain = value;
		gain = pow(10.0,(60.0*fGain-30.0)/20.0);
		break;
	case kQ:
		fQ = value;
		Q = pow(10.0,(60.0*fQ-30.0)/20.0);
		break;
	case kPhi:
		fPhi = value;
		phi = fPhi * 3.1415926535897932384626433832795;
		break;
	case kL:
		fL = value;
		l = pow(10.0,(60.0*fL)/20.0);
		break;
	default :
		break;
	}

}

//-----------------------------------------------------------------------------------------
double peak::getParameter (long index)
{

	switch (index)
	{
		case kGain:
			return fGain;
			break;
		case kQ:
			return fQ;
			break;
		case kPhi:
			return fPhi;
			break;
		case kL:
			return fL;
			break;
	default :
		return 0.0;
	}

}


void peak::clear()
{
	int i;
	for ( i=0; i<3; i++){
		inputs1_[i]=0, inputs2_[i]=0;}
}


//-----------------------------------------------------------------------------------------
void peak::process (double *inputs, double *outputs, long sampleFrames)
{
}

//-----------------------------------------------------------------------------------------
void peak::processReplacing (double *inputs, double *outputs, long sampleFrames)
{
    double *in1  =  inputs;
    double *out1 =  outputs;
	double pi = 3.1415926535897932384626433832795;
	clear();

   // using one pole one zero filter
        while (--sampleFrames >= 0)
    {

			// setup the BiQiad filter parameters
			a2 = (2*Q - sin(phi))/(2*Q + sin(phi));
			a1 = -(1+a2)*cos(phi);
			b1 = -(1+a2)*cos(phi);
			b0 = 1/2*(1+a2) + 1/2*(1-a2)*l;
			b2 = 1/2*(1+a2) - 1/2*(1-a2)*l;

			//BiQuad filter
			//Lelf channel
			inputs1_[0] = gain * (*in1++);
			outputs1_[0] = b0 * inputs1_[0] + b1 * inputs1_[1] + b2 * inputs1_[2];
			outputs1_[0] -= a2 * outputs1_[2] + a1 * outputs1_[1];
			inputs1_[2] = inputs1_[1];
			inputs1_[1] = inputs1_[0];
			outputs1_[2] = outputs1_[1];
			outputs1_[1] = outputs1_[0];

// 			//Right channel
// 			inputs2_[0] = gain * (*in2++);
// 			outputs2_[0] = b0 * inputs2_[0] + b1 * inputs2_[1] + b2 * inputs2_[2];
// 			outputs2_[0] -= a2 * outputs2_[2] + a1 * outputs2_[1];
// 			inputs2_[2] = inputs2_[1];
// 			inputs2_[1] = inputs2_[0];
// 			outputs2_[2] = outputs2_[1];
// 			outputs2_[1] = outputs2_[0];
			
			(*out1++) = double (outputs1_[0]);
// 			(*out2++) = double (outputs2_[0]);

		
		
    }
}
