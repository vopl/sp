//-------------------------------------------------------------------------------------------------------
// peak Filter VST plug in
// Nov 6, 2005, Kevin Kuang
// http://ccrma.stanford.edu/~kuangzn
//-------------------------------------------------------------------------------------------------------

#ifndef __peak_H
#define __peak_H


// param IDs
enum {
	kGain,
	kQ,
	kPhi,
	kL,
};

//-------------------------------------------------------------------------------------------------------
class peak
{
public:
	peak ();
	~peak ();

	// Processes
	virtual void clear();
	virtual void process (double *inputs, double *outputs, long sampleFrames);
	virtual void processReplacing (double *inputs, double *outputs, long sampleFrames);

	// Parameter
	virtual void setParameter (long index, double value);
	virtual double getParameter (long index);

protected:


	// config
	enum { 
		kNumProgs	= 1,
		kNumInputs	= 1,
		kNumOutputs	= 1
	};

	double fGain, fQ, fPhi, fL, gain, Q, phi, l;
    double b0, b1, b2, a1, a2;
	double lastOutput1, lastOutput2;
	double temp1, temp2, temp3, temp4;
	double inputs1_[3],inputs2_[3], outputs1_[3],outputs2_[3]; 
	int		sampleCount;

};

#endif