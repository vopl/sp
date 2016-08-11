// nhel2.spectr.smoother.cpp : Defines the entry point for the console application.
//



#include "stdafx.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <numeric>

#define ID_DIR "P:\\projects\\finance\\nhel.2\\39.middle\\console\\out_small_10\\spectr"
#define MINP 50
#define MAXP 50

#define MINT 1
#define MAXT 1

#define POW 10



struct TLine
{
	double time;
	std::deque<double> data;
};
typedef std::deque<TLine> TLines;
TLines g_data;
std::deque<double> g_periods;

//////////////////////////////////////////////////////////////////////////
void load(std::string fname)
{
	std::cout<<"load "<<fname<<std::endl;

	TLine l;
	double ltime = 0;
	std::ifstream in(fname.c_str());
	size_t ip(0);
	while(in)
	{
		double time=0, period,real, imag, stub=0;
		in>>time>>period>>real>>imag>>stub>>stub;

		if(stub)
		{
			if(ltime != time)
			{
				if(!l.data.empty())
				{
					l.time = ltime;
					g_data.push_back(l);
					l.data.clear();
					ip = 0;
				}
				ltime = time;
			}

		}
		l.data.push_back(sqrt(real*real+imag*imag));
		while(g_periods.size()<=ip)
		{
			g_periods.push_back(period);
		}
		ip++;
	}

	if(!l.data.empty())
	{
		l.time = ltime;
		g_data.push_back(l);
		l.data.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
void save(TLines &data)
{
	std::ofstream out(ID_DIR "\\_out");
	out.precision(20);
	out.setf(std::ios::scientific);


	//перебрать все времена
	for(size_t it = 0; it<data.size(); it++)
	{
		TLine &line = data[it];

		//перебрать все периоды
		for(size_t ip = 0; ip<line.data.size(); ip++)
		{
			out<<line.time<<"\t"<<g_periods[ip]<<"\t"<<line.data[ip]<<"\t"<<0.0<<"\n";
		}
	}
}

//////////////////////////////////////////////////////////////////////////
size_t findIt(double time2, size_t near)
{
	size_t res = near;
	while(res < g_data.size() && time2 > g_data[res].time)
	{
		res++;
	}
	while(res < g_data.size() && time2 < g_data[res].time)
	{
		res--;
	}
	
	return res;
}

//////////////////////////////////////////////////////////////////////////
void doSmoth()
{


	TLines sdata;

	//перебрать все времена
	for(size_t it = MINT+3; it<g_data.size()-MAXT-3; it++)
	{
		TLine &line = g_data[it];

		//перебрать все периоды
		for(size_t ip = MINP+3; ip<line.data.size()-MAXP-3; ip++)
		{
			double period = g_periods[ip];

			std::deque<double> nears;
			//собирать амплитуды на участке сглаживания
			for(size_t ip2(ip-MINP); ip2<=ip+MAXP; ip2++)
			{
				double period2 = g_periods[ip2];
				double time2 = line.time + (period2 - period)*POW;

				size_t it2_0 = findIt(time2, it);

				if(it2_0 < g_data.size())
				{
					for(size_t it2(it2_0-MINT); it2<=it2_0+MAXT; it2++)
					{
						if(it2 < g_data.size())
						{
							nears.push_back(g_data[it2].data[ip2]);
							//g_data[it2].data[ip2] = 1e-1;
						}
					}
				}
			}
			//g_data[it].data[ip] = 2e-0;


			//////////////////////////////////////////////////////////////////////////
			while(sdata.size() <= it)
			{
				sdata.push_back(TLine());
				sdata[sdata.size()-1].time = g_data[sdata.size()-1].time;
			}
			TLine &line2 = sdata[it];
			while(line2.data.size() <= ip)
			{
				line2.data.push_back(0.0);
			}

			//////////////////////////////////////////////////////////////////////////
			double median = 0;
			if(nears.size())
			{
				median = std::accumulate(nears.begin(), nears.end(), 0.0)/nears.size();
			}
			line2.data[ip] = g_data[it].data[ip] - median;
			//break;
		}
		//break;
	}

	save(sdata);

}

//////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	boost::filesystem::directory_iterator diter(ID_DIR);
	boost::filesystem::directory_iterator dend;
	for(; diter != dend; diter++)
	{
		boost::filesystem::path p = *diter;
		if(boost::filesystem::is_regular_file(p) && p.filename() != "_out")
		{
			load(p.string());
			//break;
		}
	}

	doSmoth();


	return 0;
}

