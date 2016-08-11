
#include "stdafx.h"
#include <direct.h>
#include "hel/icore.h"
#include "hel/math.h"
#include "spectrStore.h"
#include "tmarket.h"
using namespace hel;

#define DUMP_STEP (60*60*24)

void processOne(TVTimedValue &serie, real x, real xStep, TVReal &ts, TVComplex &vs, bool doDump);

//////////////////////////////////////////////////////////////////////////
//перебрать файлы
//перебрать точки
//отработать каждую точку
void process(TVTimedValue &serie, std::string dir)
{
	boost::filesystem::directory_iterator iter(dir);
	boost::filesystem::directory_iterator end;

	std::vector<string> fileNames;
	for(; iter!=end; iter++)
	{
		fileNames.push_back(iter->string());
	}

	std::sort(fileNames.begin(), fileNames.end());

	real lastx(0);
	real lastDumpX(0);
	TVReal ts;
	TVComplex vs;
	for(size_t i(0); i<fileNames.size(); i++)
	{
		std::ifstream in(fileNames[i].c_str());
		while(in)
		{
#define LINLEN 1024
			char line[LINLEN];
			in.getline(line, LINLEN);

			real x,t,r,i;
			if(4 == sscanf_s(line, "%lg\t%lg\t%lg\t%lg", &x,&t,&r,&i))
			{
				if(lastx != x)
				{
					if(!vs.empty())
					{
						bool doDump = false;
						if(lastx - lastDumpX > DUMP_STEP)
						{
							doDump = true;
							lastDumpX = lastx;
						}
						processOne(serie, lastx, 1, ts, vs, doDump);

						ts.clear();
						vs.clear();
					}
					lastx = x;
				}
				ts.push_back(t);
				vs.push_back(Complex(r,i));
			}
		}
		in.close();
		std::cout<<fileNames[i]<<std::endl;

		if(!vs.empty())
		{
			bool doDump = false;
			if(lastx - lastDumpX > DUMP_STEP)
			{
				doDump = true;
				lastDumpX = lastx;
			}
			processOne(serie, lastx, 1, ts, vs, doDump);

			ts.clear();
			vs.clear();
		}
	}

}

//////////////////////////////////////////////////////////////////////////
//делать историю
//делать сделку по истории
hel_player::SpectrStore store(0.1, 120);
TVReal prevMarks;
std::vector<TMarket> tms;
void processOne(TVTimedValue &serie, real x, real xStep, TVReal &ts, TVComplex &vs, bool doDump)
{
	store.push(x, xStep, ts, vs);


	//////////////////////////////////////////////////////////////////////////
	//нейро экспорт
	{
#define HIST_TRANGE_SIZE 5
#define HIST_POWBASE 2
#define HIST_XRANGE_SIZE 2

		TVReal nnDataIn;
		TVReal nnDataOut;
		for(size_t ti(HIST_TRANGE_SIZE); ti<ts.size()-HIST_TRANGE_SIZE; ti++)
		{
			if(store.fillNNOut(ti, HIST_TRANGE_SIZE, HIST_POWBASE, HIST_XRANGE_SIZE, nnDataIn))
			{
				//out
				nnDataOut.resize(1);
				{
					size_t nowXIdx = std::lower_bound(serie.begin(), serie.end(), x) - serie.begin();
					size_t futXIdx = std::lower_bound(serie.begin(), serie.end(), x+ts[ti]/2) - serie.begin();

					nnDataOut[0] = serie[futXIdx]._value - serie[nowXIdx]._value;
				}

				//in
				char tmp[256];
				sprintf_s(tmp, "nnData\\stream_%04d.txt", ti);

				static std::map<string, bool> s_initFlags;

				bool &initFlag = s_initFlags[tmp];
				std::ofstream out;

				if(!initFlag)
				{
					out.open(tmp, std::ios::trunc);
					for(size_t i(0); i<nnDataIn.size(); i++)
					{
						out<<"in_"<<i<<"\t";
					}
					for(size_t i(0); i<nnDataOut.size(); i++)
					{
						out<<"out_"<<i<<"\t";
					}
					out<<std::endl;
					initFlag = true;
				}
				else
				{
					out.open(tmp, std::ios::app);
				}
				out.precision(16);
				out.setf(std::ios::scientific);

				for(size_t i(0); i<nnDataIn.size(); i++)
				{
					out<<nnDataIn[i]<<"\t";
				}
				for(size_t i(0); i<nnDataOut.size(); i++)
				{
					out<<nnDataOut[i]<<"\t";
				}
				out<<std::endl;
			}

		}

	}
	return;
	//////////////////////////////////////////////////////////////////////////

	if(tms.empty())
	{
		tms.resize(ts.size());
		for(size_t i(0); i<tms.size(); i++)
		{
			tms[i].setup(ts[i]);
		}
	}

	TVReal marks(ts.size());
	store.getMarks(marks);

	if(!prevMarks.empty())
	{
#define MAX_DIST 2
		for(size_t i(MAX_DIST); i<tms.size()-MAX_DIST; i++)
		{
			if(marks[i] && prevMarks[i])
			{
				if(
					marks[i] > prevMarks[i] && 
					marks[i-1]<marks[i] &&
					marks[i+1]<marks[i] &&
 					vs[i-MAX_DIST].a()<vs[i].a() &&
 					vs[i+MAX_DIST].a()<vs[i].a() &&
					1)
				{
					tms[i].step(serie, x, vs[i]);
				}
			}
		}
	}

	if(doDump)
	{
		static std::ofstream outt("t");
		static std::ofstream outx("x");
		static std::ofstream outm("m");
		static std::ofstream oute("e");
		static std::ofstream oute2("e2");
		static bool initDump=true;

		if(initDump)
		{
			outt.setf(std::ios::scientific); outt.precision(16);
			outx.setf(std::ios::scientific); outx.precision(16);
			outm.setf(std::ios::scientific); outm.precision(16);
			oute.setf(std::ios::scientific); oute.precision(16);
			oute2.setf(std::ios::scientific); oute2.precision(16);

			for(size_t i(0); i<ts.size(); i++)
			{
				outt<<ts[i]<<std::endl;
			}

			initDump = false;
		}

		for(size_t i(0); i<ts.size(); i++)
		{
			outm<<marks[i]<<"\t";
			oute<<tms[i].getEquity(serie, x)<<"\t";
			oute2<<tms[i].getEquity2(serie, x)<<"\t";
			//oute<<tms[i].getBalance()<<"\t";
		}
		outm<<std::endl;
		oute<<std::endl;
		oute2<<std::endl;
		outx<<x<<std::endl;

	}
	prevMarks.swap(marks);
}
