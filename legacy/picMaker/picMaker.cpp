// picMaker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "timeMapper.h"
#include <chartdir.h>


#define WIDTH 1024
#define HEIGHT 768

#define TIMELENGTH 1*60*60*24
#define SPECTR_X_OFFSET (-50*60.0)
#define MIN_TDISTCONST -10
#define MIN_TDIST MIN_TDISTCONST
#define MAX_TDISTCONST 0.5
#define MAX_TDIST MAX_TDISTCONST

#define MUTE_EXTRAP 1

#define MIN_T (1.0*60*60*12*0)
#define MAX_T (1.0*60*60*24*5000)

TimeMapper tmap;
const real c_piDiv2	= (1.57079632679489661923132169163975144209858469968755291048747229615390820314310449931401741267105853);
const real c_pi		= (3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214);
const real c_2Pi	= (6.28318530717958647692528676655900576839433879875021164194988918461563281257241799725606965068423413);


void addLine(XYChart *c, real x0, real y0, real x1, real y1, int color=-1)
{
	LineLayer *l2 = c->addLineLayer();
	real ys[2] = {y0,y1};
	real xs[2] = {Chart::chartTime2(int(tmap.internal2External(x0)+0.5)),Chart::chartTime2(int(tmap.internal2External(x1)+0.5))};
	l2->addDataSet(DoubleArray(ys, 2), color);
	l2->setXData(DoubleArray(xs, 2));
}

//////////////////////////////////////////////////////////////////////////
bool mkOne(
	const char *fname, 
	real xStart, real xMiddle, real xStop,
	real *times,
	real *original,
	size_t s1,
	real *spectr_t,
	real *spectr_re,
	real *spectr_im,
	size_t s2)
{
	real xStart_c = Chart::chartTime2(int(tmap.internal2External(xStart)+0.5));
	real xMiddle_c = Chart::chartTime2(int(tmap.internal2External(xMiddle)+0.5));
	real xStop_c = Chart::chartTime2(int(tmap.internal2External(xStop)+0.5));
	vector<real> times_c(s1);
	real yMin = 10000000;
	real yMax = -10000000;
	for(size_t i(0); i<s1; i++)
	{
		times_c[i] = Chart::chartTime2(int(tmap.internal2External(times[i])+0.5));

		yMin = min(yMin, original[i]);
		yMax = max(yMax, original[i]);
	}
	XYChart *c = new XYChart(WIDTH, HEIGHT);
	c->setPlotArea(50,10,WIDTH-50, HEIGHT-60);
	c->xAxis()->setDateScale(xStart_c, xStop_c);
	c->xAxis()->setLabelFormat("{value|yyyy-mm-dd hh:nn}");
	LineLayer *orig = c->addLineLayer(DoubleArray(original, s1));
	orig->setXData(DoubleArray(&times_c.front(), s1));

	//значение оригинала в центре
	vector<real> ev(s1, 0);
	//перебрать спектр
	for(size_t i(0); i<s2; i++)
	{
		real re = spectr_re[i];
		real im = spectr_im[i];
		real t = spectr_t[i];

		if(t<MIN_T || t>MAX_T)
		{
			continue;
		}

		real a = sqrt(re*re+im*im);
		real p = atan2(im, re);

		for(size_t j(0); j<s1; j++)
		{
			real tDist = (times[j] - xMiddle)/t;
			if(tDist < MIN_TDIST || tDist > MAX_TDIST)
			{
				continue;
			}

			real w = 1;

			if(tDist < MIN_TDISTCONST)
			{
				w = (tDist - MIN_TDISTCONST)/(MIN_TDIST - MIN_TDISTCONST);
				w = cos(w*c_pi)/2+0.5;
			}
			if(tDist > MAX_TDISTCONST)
			{
				w = (tDist - MAX_TDISTCONST)/(MAX_TDIST - MAX_TDISTCONST);
				w = cos(w*c_pi)/2+0.5;
			}

			ev[j] += a*cos(c_2Pi*(times[j] - xMiddle)/t + p)*w;
		}
	}
	real delta = 0;
	real deltaAmount = 0;
	for(size_t i(0); i<s1/2; i++)
	{
		real w = 1;
		delta += (original[i] - ev[i])*w;
		deltaAmount += w;
	}
	delta /= deltaAmount;

#if MUTE_EXTRAP
	for(size_t i(s1/2+1); i<s1; i++)
	{
		ev[i] = ev[s1/2-1];
	}
#endif

	for(size_t i(0); i<s1; i++)
	{
		ev[i] += delta;
		yMin = min(yMin, ev[i]);
		yMax = max(yMax, ev[i]);
	}

	c->yAxis()->setLinearScale(yMin, yMax);

	LineLayer *extrap = c->addLineLayer(DoubleArray(&ev[0], s1));
	extrap->setXData(DoubleArray(&times_c.front(), s1));

	addLine(c, xMiddle, yMin, xMiddle, yMax, LineColor);
	

	bool res = c->makeChart(fname);

	delete c;
	return res;
}


real s2time(const char *csz)
{
	tm stm = {};
	if(6 != sscanf_s(csz, "%d-%d-%d %d:%d:%d", &stm.tm_year, &stm.tm_mon, &stm.tm_mday, &stm.tm_hour, &stm.tm_min, &stm.tm_sec))
	{
		return 0;
	}
	stm.tm_year -= 1900;
	stm.tm_mon -= 1;
	return tmap.external2Internal(real(_mkgmtime(&stm)));
}


int _tmain(int argc, _TCHAR* argv[])
{
	tmap.setup(0, 946684800, 0, 946684800, true, (5*24*60*60 - 1*60*60));

	vector<real> times;
	vector<real> original;
	//////////////////////////////////////////////////////////////////////////
	cout<<"load original...";
	{
		ifstream inOrig("EURXAG");
		//ifstream inOrig("P:\\finance\\quotes\\weather\\msk_1998_2009\\out");

		while(inOrig)
		{
			real v_=-234;
			char x_1[512];
			char x_2[512];
			inOrig>>x_1>>x_2>>v_;

			if(v_<-230)
			{
				break;
			}

			real x__ = s2time((string(x_1)+string(" ")+string(x_2)).c_str());
			times.push_back(x__);
			original.push_back(v_);
		}
	}
	cout<<original.size()<<endl;

	vector<real> spectr_x;
	vector<real> spectr_t;
	vector<real> spectr_re;
	vector<real> spectr_im;
	cout<<"load spectr...";
	{
		ifstream in("data10\\out_spectr");

		while(in)
		{
			real x=0;
			real t=0;
			real re=0;
			real im=0;
			in>>x>>t>>re>>im;

			if(!im)
			{
				break;
			}

			//x = tmap.external2Internal(x);
			//t = tmap.external2InternalOffset(t);
			spectr_x.push_back(x+SPECTR_X_OFFSET);
			spectr_t.push_back(t);
			spectr_re.push_back(re);
			spectr_im.push_back(im);
		}
	}
	cout<<spectr_x.size()<<endl;


	//перебрать спектр по времени
	size_t spectr_idx = 0;
	int frameIdx(0);
	while(spectr_idx < spectr_x.size())
	{
		size_t spectr_startIdx = spectr_idx;
		size_t spectr_stopIdx = spectr_idx;
		while(spectr_stopIdx < spectr_x.size() && spectr_x[spectr_startIdx] == spectr_x[spectr_stopIdx])
		{
			spectr_stopIdx++;
		}

		if(spectr_stopIdx == spectr_startIdx)
		{
			break;
		}

		if(! (frameIdx % 1))
		{
			//для точки спектра подобрать оригинал
			real xStart = spectr_x[spectr_startIdx] - TIMELENGTH;
			real xStop = spectr_x[spectr_startIdx] + TIMELENGTH;

			size_t original_startIdx = std::lower_bound(times.begin(), times.end(), xStart) - times.begin();
			size_t original_stopIdx = std::lower_bound(times.begin(), times.end(), xStop) - times.begin();

			char frameName[512];
			sprintf_s(frameName, "out\\%06d.png", frameIdx);

			mkOne(frameName, 
				xStart, spectr_x[spectr_startIdx], xStop,
				&times[original_startIdx],
				&original[original_startIdx],
				original_stopIdx - original_startIdx,
				&spectr_t[spectr_startIdx],
				&spectr_re[spectr_startIdx],
				&spectr_im[spectr_startIdx],
				spectr_stopIdx - spectr_startIdx);

			cout<<"frame "<<frameName<<std::endl;
		}
		frameIdx++;

		spectr_idx = spectr_stopIdx;
	}
	

	return 0;
}

