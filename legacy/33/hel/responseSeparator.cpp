#include "stdafx.h"
#include "hel/responseSeparator.h"
#include "hel/math.h"
#include "hel/levmar.h"

#include "alglib/minlm.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	ResponseSeparator::ResponseSeparator(
		const TVReal &tVectorIn, 
		const TVComplex &vectorIn,
		ResponseModelPtr responseModel,
		TVReal &tVectorOut, 
		TVComplex &vectorOut)
		: _responseModel(responseModel)
		, _tVectorOut(tVectorOut)
		, _vectorOut(vectorOut)

	{
		_input.resize(tVectorIn.size());
		for(size_t i(0); i<_input.size(); i++)
		{
			_input[i]._t = tVectorIn[i];
			_input[i]._ap = vectorIn[i];
		}
		_output.resize(tVectorOut.size());
		for(size_t i(0); i<_output.size(); i++)
		{
			_output[i]._t = tVectorOut[i];
			_output[i]._ap = vectorOut[i];
		}

		TVReal areimIn(vectorIn.size());
		for(size_t i(0); i<areimIn.size(); i++)
		{
			areimIn[i] += fabs(vectorIn[i].re());
			areimIn[i] += fabs(vectorIn[i].im());
			areimIn[i] /= 2;
		}

		llsq(&tVectorIn.front(), &areimIn.front(), tVectorIn.size(), _normaK, _normaB);
		//_normaK *= 2;
	}

	//////////////////////////////////////////////////////////////////////////
	ResponseSeparator::~ResponseSeparator()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	real ResponseSeparator::separate(
		TVReal &tVectorOutSmoothed, 
		TVComplex &vectorOutSmoothed)
	{
		std::cout.precision(20);

		size_t optIters = 500;
		size_t optItersGlobal = 100;
		real boundError = 0.0000001;
		size_t boundAmount = 550;//_input.size()/20;///20=pow

		real minCollapseDist = 0.00001;
		real minCollapseNorma = 0.00001;

		real minCollapseSmoothDist = 0.00001;
		real minCollapseSmoothNorma = 0.00001;

		real prMult = 2.5;
		real prMultAdd = 5.0;

		size_t bwdIters = 1;
		bool globalOptPeriod = true;



		TVTap inputTail(_input.size());
		TVTap nullTaps;
		real initInt = makeInputTail(_input, nullTaps, inputTail);

		real error = 1;


		if(_output.size())
		{
			for(size_t bwdIter(0); bwdIter<bwdIters; bwdIter++)
			{
				std::cout<<"initial bwd... ";
				for(size_t i(_output.size()-1); i<_output.size(); i--)
				{
					real minOptPeriod = _output[i]._t * pow(_responseModel->getMinSignificantPeriodMult(), prMult);
					real maxOptPeriod = _output[i]._t * pow(_responseModel->getMaxSignificantPeriodMult(), prMult);
					optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optIters, true);
				}
				error = makeInputTail(_input, _output, inputTail)/initInt;
				std::cout<<error<<", "<<_output.size()<<std::endl;
			}

			//////////////////////////////////////////////////////////////////////////
			std::cout<<"initial global... ";
			real minOptPeriod = _responseModel->getMinSignificantPeriod(std::min(_output.front()._t, _input.front()._t));
			real maxOptPeriod = _responseModel->getMaxSignificantPeriod(std::max(_output.back()._t, _input.back()._t));
			optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optItersGlobal, globalOptPeriod);
			error = makeInputTail(_input, _output, inputTail)/initInt;
			std::cout<<error<<", "<<_output.size()<<std::endl;

			boundAmount = _output.size()+10;
		}
		else
		{
			boundAmount = _output.size()+10;
		}

		boundAmount = std::min(_input.size()/3, boundAmount)+1;




		bool addWas = false;

		size_t cnt(0);
		while((error > boundError && _output.size() < boundAmount) && _output.size()*3 < _input.size() && cnt <10000000)
		//while(error > boundError && _output.size()*3 < _input.size() && cnt <10000000)
		{


			//////////////////////////////////////////////////////////////////////////
			size_t addStep = std::lower_bound(
				inputTail.begin(), 
				inputTail.end(), 
				inputTail.front()._t / _responseModel->getMinSignificantPeriodMult() * _responseModel->getMaxSignificantPeriodMult()) - inputTail.begin();

			bool fmmWas = false;
			for(size_t ui(0); ui<inputTail.size(); ui++)
			{

				size_t inIdx = 0;
				TAP tap = findMaxMatch(inputTail, ui, std::min(ui+addStep, inputTail.size()), inIdx);
				if(tap._t<0)
				{
					ui += addStep;
					continue;
				}

				TVTap tmpOut(_output);
				size_t newIdx = std::lower_bound(tmpOut.begin(), tmpOut.end(), tap)-tmpOut.begin();
				tmpOut.insert(tmpOut.begin()+newIdx, tap);





 				real minOptPeriod = tmpOut[newIdx]._t * pow(_responseModel->getMinSignificantPeriodMult(), prMultAdd);
 				real maxOptPeriod = tmpOut[newIdx]._t * pow(_responseModel->getMaxSignificantPeriodMult(), prMultAdd);
 				optimizeOut(_input, tmpOut, minOptPeriod, maxOptPeriod, optIters);
				real tmpError = makeInputTail(_input, tmpOut, inputTail)/initInt;

				if(tmpError < error)
				{
					std::cout<<std::lower_bound(inputTail.begin(), inputTail.end(), tap._t)-inputTail.begin()<<", ";
					std::cout<<"add: "<<tmpError<<", "<<tmpOut.size()<<std::endl;

					addWas = true;
					fmmWas = true;
					//fmmIdx += fmmStep;
					_output = tmpOut;
					error = tmpError;

					if(inIdx < addStep/2)
					{
						ui = 0;
					}
					else
					{
						ui = inIdx+1;
					}

				}
				else
				{
					error = makeInputTail(_input, _output, inputTail)/initInt;
					//break;
					ui += addStep;
				}

			}
			if(!fmmWas)
			{
				//std::cout<<"findMaxMatch filed"<<std::endl;
				break;
			}


		}


		if(addWas)
		{
			std::cout<<"adds ";
			error = makeInputTail(_input, _output, inputTail)/initInt;
			std::cout<<error<<", "<<_output.size()<<std::endl;
		}

		real t;
		while(_output.size()*3 >= _input.size() && (t = collapseOut_(_output, 1e100, minCollapseNorma)))
		{
			addWas = true;
			if(t>0)
			{
				real minOptPeriod = t * pow(_responseModel->getMinSignificantPeriodMult(), prMult);
				real maxOptPeriod = t * pow(_responseModel->getMaxSignificantPeriodMult(), prMult);
				optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optIters, true);
			}
			error = makeInputTail(_input, _output, inputTail)/initInt;
			std::cout<<"collapse superfluous "<<error<<", "<<_output.size()<<std::endl;
		}

		if(addWas)
		{

// 			real oldErr = error;
// 			do
// 			{
// 				std::cout<<"bwd... ";
// 				for(size_t i(_output.size()-1); i<_output.size(); i--)
// 				{
// 					real minOptPeriod = _output[i]._t * pow(_responseModel->getMinSignificantPeriodMult(), prMult);
// 					real maxOptPeriod = _output[i]._t * pow(_responseModel->getMaxSignificantPeriodMult(), prMult);
// 					optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optIters, true);
// 				}
// 				oldErr = error;
// 				error = makeInputTail(_input, _output, inputTail)/initInt;
// 				std::cout<<error<<", "<<_output.size()<<std::endl;
// 			}while (oldErr > error);

			//////////////////////////////////////////////////////////////////////////
			for(size_t bwdIter(0); bwdIter<bwdIters; bwdIter++)
			{
				std::cout<<"bwd... ";
				for(size_t i(_output.size()-1); i<_output.size(); i--)
				{
					real minOptPeriod = _output[i]._t * pow(_responseModel->getMinSignificantPeriodMult(), prMult);
					real maxOptPeriod = _output[i]._t * pow(_responseModel->getMaxSignificantPeriodMult(), prMult);
					optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optIters, true);
				}
				error = makeInputTail(_input, _output, inputTail)/initInt;
				std::cout<<error<<", "<<_output.size()<<std::endl;
			}





			//////////////////////////////////////////////////////////////////////////
			std::cout<<"global... ";
			real minOptPeriod = _responseModel->getMinSignificantPeriod(std::min(_output.front()._t, _input.front()._t));
			real maxOptPeriod = _responseModel->getMaxSignificantPeriod(std::max(_output.back()._t, _input.back()._t));
			optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optItersGlobal, globalOptPeriod);
			error = makeInputTail(_input, _output, inputTail)/initInt;
			std::cout<<error<<", "<<_output.size()<<std::endl;
		}

		//////////////////////////////////////////////////////////////////////////
		bool collapseWas = false;
		while(real t = collapseOut_(_output, minCollapseDist, minCollapseNorma))
		{
			collapseWas = true;
			if(t>0)
			{
				real minOptPeriod = t * pow(_responseModel->getMinSignificantPeriodMult(), prMult);
				real maxOptPeriod = t * pow(_responseModel->getMaxSignificantPeriodMult(), prMult);
				optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optIters, true);
			}
		}

		if(collapseWas)
		{
			std::cout<<"collapse... ";
			error = makeInputTail(_input, _output, inputTail)/initInt;
			std::cout<<error<<", "<<_output.size()<<std::endl;
		}




		//////////////////////////////////////////////////////////////////////////
		if(collapseWas)
		{
			//////////////////////////////////////////////////////////////////////////
			for(size_t bwdIter(0); bwdIter<bwdIters; bwdIter++)
			{
				std::cout<<"bwd... ";
				for(size_t i(_output.size()-1); i<_output.size(); i--)
				{
					real minOptPeriod = _output[i]._t * pow(_responseModel->getMinSignificantPeriodMult(), prMult);
					real maxOptPeriod = _output[i]._t * pow(_responseModel->getMaxSignificantPeriodMult(), prMult);
					optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optIters, true);
				}
				error = makeInputTail(_input, _output, inputTail)/initInt;
				std::cout<<error<<", "<<_output.size()<<std::endl;
			}




			//////////////////////////////////////////////////////////////////////////
			std::cout<<"global... ";
			real minOptPeriod = _responseModel->getMinSignificantPeriod(std::min(_output.front()._t, _input.front()._t));
			real maxOptPeriod = _responseModel->getMaxSignificantPeriod(std::max(_output.back()._t, _input.back()._t));
			optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optItersGlobal, false);
			error = makeInputTail(_input, _output, inputTail)/initInt;
			std::cout<<error<<", "<<_output.size()<<std::endl;
		}

		_tVectorOut.clear(); 
		_vectorOut.clear();

		//real minPeriod = _responseModel->getMaxSignificantPeriod(_input.front()._t);
		//real maxPeriod = _responseModel->getMinSignificantPeriod(_input.back()._t);
		for(size_t i(0); i<_output.size(); i++)
		{
			//if(_output[i]._t > minPeriod && _output[i]._t < maxPeriod)
			{
				_tVectorOut.push_back(_output[i]._t);
				_vectorOut.push_back(_output[i]._ap);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		while(real t = collapseOut_(_output, minCollapseSmoothDist, minCollapseSmoothNorma))
		{
			collapseWas = true;
			if(t>0)
			{
				real minOptPeriod = t * pow(_responseModel->getMinSignificantPeriodMult(), prMult);
				real maxOptPeriod = t * pow(_responseModel->getMaxSignificantPeriodMult(), prMult);
				optimizeOut(_input, _output, minOptPeriod, maxOptPeriod, optIters, true);
			}
			error = makeInputTail(_input, _output, inputTail)/initInt;
			std::cout<<"smooth "<<error<<", "<<_output.size()<<std::endl;
		}
		error = makeInputTail(_input, _output, inputTail)/initInt;
		std::cout<<"smooth "<<error<<", "<<_output.size()<<std::endl;

		tVectorOutSmoothed.clear();
		vectorOutSmoothed.clear();
		for(size_t i(0); i<_output.size(); i++)
		{
			//if(_output[i]._t > minPeriod && _output[i]._t < maxPeriod)
			{
				tVectorOutSmoothed.push_back(_output[i]._t);
				vectorOutSmoothed.push_back(_output[i]._ap);
			}
		}

		return error;
	}

	//////////////////////////////////////////////////////////////////////////
	TAP ResponseSeparator::findMaxMatch(const TVTap &vTap, size_t begin, size_t end, size_t &idx)
	{
		TVTap inputTail(vTap.size()), nullTaps;
		real maxA = makeInputTail(vTap, nullTaps, inputTail);
		TAP maxTap;
		maxTap._t = -1;

		for(size_t i(begin); i<end; i++)
		{

			TVTap tmp;
			tmp.push_back(vTap[i]);

			optimizeOut(vTap, tmp, tmp.front()._t, tmp.back()._t, 10, false);
			collapseOut_(tmp, 1e-100,1e-100);
			if(tmp.empty())
			{
				continue;
			}

			real v = makeInputTail(vTap, tmp, inputTail);
			if(v < maxA)
			{
				maxA = v;
				maxTap = tmp.back();
				idx = i;
			}
		}

 		if(maxTap._t>0)
 		{
 			TVTap tmp;
 			tmp.push_back(maxTap);
			optimizeOut(vTap, tmp, maxTap._t, maxTap._t, 50, true);
 			return tmp.back();
 		}

		return maxTap;
	}
	
	//////////////////////////////////////////////////////////////////////////
	real ResponseSeparator::makeInputTail(const TVTap &input, const TVTap &output, TVTap &inputTail)
	{
		real diff = 0;

		TVComplex mms(inputTail.size());

		for(size_t i(0); i<input.size(); i++)
		{
			Complex m;
			for(size_t j(0); j<output.size(); j++)
			{
				m += _responseModel->evalAP(input[i]._t, output[j]._t, output[j]._ap);
			}

			mms[i] = m;


			inputTail[i]._t = input[i]._t;
			inputTail[i]._ap = input[i]._ap - m;

			if(i)
			{
				diff += inputTail[i]._ap.a() / getNorma(inputTail[i]._t);// * (input[i]._t - input[i-1]._t);
			}
		}
		//diff /= 2;

		return diff;
	}

	//////////////////////////////////////////////////////////////////////////
	struct ParamsDlevmarFunc
	{
		real *_x;//две длины, на мнимую и вещественную
		real *_y;

		ResponseModel *_rm;

		bool _optPeriod;

	};

	void evalFuncOpt(double *p, double *hx, int m, int n, void *adata)
	{
		ParamsDlevmarFunc *params = (ParamsDlevmarFunc *)adata;

		for(int i(0); i<n/2; i++)
		{
			Complex c;
			for(int j(0); j<m/3; j++)
			{
				c += params->_rm->evalAP(params->_x[i], p[j*3+0], Complex(p[j*3+1], p[j*3+2]));
			}

			hx[i] = c.re();
			hx[n/2+i] = c.im();
		}
	}

	void jacoFuncOpt(double *p, double *jx, int m, int n, void *adata)
	{
		ParamsDlevmarFunc *params = (ParamsDlevmarFunc *)adata;

		Complex dt, da, dp;
		for(int i(0); i<n/2; i++)
		{
			for(int j(0); j<m/3; j++)
			{
				params->_rm->evalAP_grad(params->_x[i], p[j*3+0], Complex(p[j*3+1], p[j*3+2]),
					dt, da, dp);


				jx[(i)*m+j*3+0] = dt.re();
				jx[(i)*m+j*3+1] = da.re();
				jx[(i)*m+j*3+2] = dp.re();

				jx[(n/2+i)*m+j*3+0] = dt.im();
				jx[(n/2+i)*m+j*3+1] = da.im();
				jx[(n/2+i)*m+j*3+2] = dp.im();

				if(!params->_optPeriod)
				{
					jx[(i)*m+j*3+0] = 0;
					jx[(n/2+i)*m+j*3+0] = 0;
				}
			}
		}
	}




	//////////////////////////////////////////////////////////////////////////
	real ResponseSeparator::collapseOut_(TVTap &output, real minDist, real minValue)
	{
		for(size_t i(0); i<output.size(); i++)
		{
			if(
				output[i]._t > pow(_responseModel->getMaxSignificantPeriodMult(), 10)*_input.back()._t ||
				output[i]._t < pow(_responseModel->getMinSignificantPeriodMult(), 10)*_input.front()._t ||
				0)
			{
//				std::cout<<"\t\terase: "<<output[i]._t<<", "<<output[i]._ap.a()<<std::endl;
				output.erase(output.begin()+i);
				return -1;
			}
		}

		for(size_t i(0); i<output.size(); i++)
		{
			if(
				output[i]._ap.a() < getNorma(output[i]._t) * minValue)
			{
//				std::cout<<"\t\terase: "<<output[i]._t<<", "<<output[i]._ap.a()<<std::endl;
				real res = output[i]._t;
				output.erase(output.begin()+i);
				return res;
			}
		}


		//выбрать пару с наименьшей дистанцией
		real minDistLocal = std::numeric_limits<real>::max();
		size_t targetIdx1(-1), targetIdx2(-1);
		for(size_t i(0); i<output.size()-1; i++)
		{

			for(size_t j(i+1); j<output.size(); j++)
			{
				real hDist = fabs(output[i].hDist(output[j]));

				if(hDist > minDist)
				{
					continue;
				}

				if(hDist < minDistLocal)
				{
					targetIdx1 = i;
					targetIdx2 = j;
					minDist = hDist;
				}
			}
		}

		//std::cout<<"\t\t\t\tminDist: "<<minDist<<std::endl;
		if(targetIdx1 != -1 && targetIdx2 != -1)
		{
			real a1(output[targetIdx1]._ap.a());
			real a2(output[targetIdx2]._ap.a());
			real t1(output[targetIdx1]._t);
			real t2(output[targetIdx2]._t);

			output[targetIdx1]._t = (t1*a1 + t2*a2) / (a1+a2);
			output[targetIdx1]._ap += output[targetIdx2]._ap;


			output.erase(output.begin()+targetIdx2);
			//std::cout<<"\t\thDist: "<<minDist<<", "<<targetIdx1<<", "<<targetIdx2<<std::endl;
			return output[targetIdx1]._t;
		}

		return 0;
	}


































// 	//////////////////////////////////////////////////////////////////////////
// 	bool ResponseSeparator::optimizeOut(
// 		const TVTap &input, 
// 		TVTap &output, 
// 		real minPeriod, 
// 		real maxPeriod, 
// 		size_t maxIters)
// 	{
// 		if(!output.size())
// 		{
// 			return false;
// 		}
// 
// 		//////////////////////////////////////////////////////////////////////////
// 		//сформировать выход
// 		size_t beginOutputIdx = std::lower_bound(output.begin(), output.end(), minPeriod) - output.begin();
// 		if(beginOutputIdx < output.size()-1)
// 		{
// 			beginOutputIdx++;
// 		}
// 		size_t endOutputIdx = std::lower_bound(output.begin(), output.end(), maxPeriod) - output.begin();
// 		while(endOutputIdx <= beginOutputIdx)
// 		{
// 			endOutputIdx++;
// 		}
// 
// 		ap::real_1d_array args;
// 		args.setbounds(0, (endOutputIdx - beginOutputIdx)*3);
// 		for(size_t i(0); i<endOutputIdx - beginOutputIdx; i++)
// 		{
// 			args(i*3+0) = output[beginOutputIdx+i]._t;
// 			args(i*3+1) = output[beginOutputIdx+i]._ap.re();
// 			args(i*3+2) = output[beginOutputIdx+i]._ap.im();
// 		}
// // 		TVReal args((endOutputIdx - beginOutputIdx)*3);
// // 		for(size_t i(0); i<endOutputIdx - beginOutputIdx; i++)
// // 		{
// // 			args[i*3+0] = output[beginOutputIdx+i]._t;
// // 			args[i*3+1] = output[beginOutputIdx+i]._ap.re();
// // 			args[i*3+2] = output[beginOutputIdx+i]._ap.im();
// // 		}
// 
// 		//////////////////////////////////////////////////////////////////////////
// 		//сформировать вход
// 		real minInputPeriod = _responseModel->getMinSignificantPeriod(std::min(minPeriod, output[beginOutputIdx]._t));
// 		minInputPeriod *= 0.68;
// 		real maxInputPeriod = _responseModel->getMaxSignificantPeriod(std::max(maxPeriod, output[endOutputIdx-1]._t));
// 		maxInputPeriod /= 0.68;
// 
// 		size_t beginInputIdx = std::lower_bound(input.begin(), input.end(), minInputPeriod) - input.begin();
// 		size_t endInputIdx = std::lower_bound(input.begin(), input.end(), maxInputPeriod) - input.begin();
// 		if(endInputIdx < input.size())
// 		{
// 			endInputIdx++;
// 		}
// 
// 		while((endInputIdx - beginInputIdx)/3 < (endOutputIdx - beginOutputIdx))
// 		{
// 			if(!beginInputIdx && endInputIdx>=input.size())
// 			{
// 				return false;
// 			}
// 
// 			if(beginInputIdx)
// 			{
// 				beginInputIdx--;
// 			}
// 			if(endInputIdx<input.size())
// 			{
// 				endInputIdx++;
// 			}
// 		}
// 
// 		TVTap inputTail(endInputIdx - beginInputIdx);
// 
// 		for(size_t i(0); i<inputTail.size(); i++)
// 		{
// 			inputTail[i] = input[beginInputIdx+i];
// 
// 			for(size_t j(0); j<output.size(); j++)
// 			{
// 				if(j<beginOutputIdx || j>=endOutputIdx)
// 				{
// 					inputTail[i]._ap -= _responseModel->evalAP(inputTail[i]._t, output[j]._t, output[j]._ap);
// 				}
// 			}
// 		}
// 
// 		TVReal xdata(inputTail.size()*2);
// 		TVReal ydata(inputTail.size()*2);
// 		for(size_t i(0); i<inputTail.size(); i++)
// 		{
// 			xdata[i] = inputTail[i]._t;
// 			xdata[inputTail.size() +i] = inputTail[i]._t;
// 
// 			ydata[i] = inputTail[i]._ap.re();
// 			ydata[inputTail.size() +i] = inputTail[i]._ap.im();
// 		}
// 
// 
// 
// 
// 
// 
// 		//////////////////////////////////////////////////////////////////////////
// 		//////////////////////////////////////////////////////////////////////////
// 		//////////////////////////////////////////////////////////////////////////
// 		lmstate state;
// 		minlmfj(
// 			args.gethighbound(),
// 			xdata.size(),
// 			args,
// 			0,
// 			0,
// 			maxIters,
// 			state);
// 
// 		while(minlmiteration(state))
// 		{
// 			if(state.needfij)
// 			{
// 				state.fi;
// 				state.j;
// 
// 				for(int i(0); i<state.m/2; i++)
// 				{
// 					Complex v(-ydata[i], -ydata[i+state.m/2]);
// 					for(int j(0); j<state.n/3; j++)
// 					{
// 						v += _responseModel->evalAP(
// 							xdata[i], 
// 							state.x(j*3+0), 
// 							Complex(state.x(j*3+1), state.x(j*3+2)));
// 
// 						Complex dt;
// 						Complex dapRe;
// 						Complex dapIm;
// 						_responseModel->evalAP_grad(
// 							xdata[i], 
// 							state.x(j*3+0), 
// 							Complex(state.x(j*3+1), state.x(j*3+2)),
// 							dt,
// 							dapRe,
// 							dapIm);
// 
// 						state.j(i, j*3+0) = dt.re();
// 						state.j(i, j*3+1) = dapRe.re();
// 						state.j(i, j*3+2) = dapIm.re();
// 
// 						state.j(i+state.m/2, j*3+0) = dt.im();
// 						state.j(i+state.m/2, j*3+1) = dapRe.im();
// 						state.j(i+state.m/2, j*3+2) = dapIm.im();
// 					}
// 					state.fi(i) = v.re();
// 					state.fi(i+state.m/2) = v.im();
// 				}
// 			}
// 			else if(state.needf)
// 			{
// 				state.f = 0;
// 				for(int i(0); i<state.m/2; i++)
// 				{
// 					Complex v(-ydata[i], -ydata[i+state.m/2]);
// 					for(int j(0); j<state.n/3; j++)
// 					{
// 						v += _responseModel->evalAP(
// 							xdata[i], 
// 							state.x(j*3+0), 
// 							Complex(state.x(j*3+1), state.x(j*3+2)));
// 					}
// 					state.f += sqr(v.re()) + sqr(v.im());
// 				}
// 			}
// 			else if(state.needfg)
// 			{
// 				state.f;
// 				state.g;
// 
// 				for(int j(0); j<state.n; j++)
// 				{
// 					state.g(j) = 0;
// 				}
// 				state.f = 0;
// 
// 				for(int i(0); i<state.m/2; i++)
// 				{
// 					Complex v(-ydata[i], -ydata[i+state.m/2]);
// 					for(int j(0); j<state.n/3; j++)
// 					{
// 						v += _responseModel->evalAP(
// 							xdata[i], 
// 							state.x(j*3+0), 
// 							Complex(state.x(j*3+1), state.x(j*3+2)));
// 
// 						Complex dt;
// 						Complex dapRe;
// 						Complex dapIm;
// 						_responseModel->evalAP_grad(
// 							xdata[i], 
// 							state.x(j*3+0), 
// 							Complex(state.x(j*3+1), state.x(j*3+2)),
// 							dt,
// 							dapRe,
// 							dapIm);
// 
// 						state.g(j*3+0) += dt.re() + dt.im();
// 						state.g(j*3+1) += dapRe.re() + dapRe.im();
// 						state.g(j*3+2) += dapIm.re() + dapIm.im();
// 					}
// 					state.f += sqr(v.re()) + sqr(v.im());
// 				}
// 			}
// 			else
// 			{
// 				assert(0);
// 			}
// 		}
// 
// 		lmreport rep;
// 		minlmresults(state, args, rep);
// 
// 		//////////////////////////////////////////////////////////////////////////
// 		//////////////////////////////////////////////////////////////////////////
// 		//////////////////////////////////////////////////////////////////////////
// 
// 
// 
// 
// 
// 
// 
// 		//std::cout<<"\titers: "<<res<<"\t"<<args.size()/3<<"\t"<<ydata.size()/2<<std::endl;
// 
// 		for(size_t i(0); i<endOutputIdx - beginOutputIdx; i++)
// 		{
// 			 output[beginOutputIdx+i]._t = args(i*3+0);
// 			 output[beginOutputIdx+i]._ap.set(args(i*3+1), args(i*3+2));
// 		}
// 
// 		std::sort(output.begin(), output.end());
// 
// 		return true;
// 	}













	//////////////////////////////////////////////////////////////////////////
	bool ResponseSeparator::optimizeOut(
		const TVTap &input, 
		TVTap &output, 
		real minPeriod, 
		real maxPeriod, 
		size_t maxIters,
		bool optPeriod)
	{
		if(!output.size())
		{
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		//сформировать выход
		size_t beginOutputIdx = std::lower_bound(output.begin(), output.end(), minPeriod) - output.begin();
		if(beginOutputIdx >= output.size())
		{
			return false;
		}

		size_t endOutputIdx = std::lower_bound(output.begin(), output.end(), maxPeriod) - output.begin();
		if(endOutputIdx < output.size())
		{
			endOutputIdx++;
		}
		while(endOutputIdx <= beginOutputIdx && endOutputIdx < output.size())
		{
			endOutputIdx++;
		}

		TVReal args((endOutputIdx - beginOutputIdx)*3);
		for(size_t i(0); i<endOutputIdx - beginOutputIdx; i++)
		{
			args[i*3+0] = output[beginOutputIdx+i]._t;
			args[i*3+1] = output[beginOutputIdx+i]._ap.re();
			args[i*3+2] = output[beginOutputIdx+i]._ap.im();
		}

		//////////////////////////////////////////////////////////////////////////
		//сформировать вход
		real minInputPeriod = _responseModel->getMinSignificantPeriod(std::min(minPeriod, output[beginOutputIdx]._t));
		minInputPeriod *= 0.68;
		real maxInputPeriod = _responseModel->getMaxSignificantPeriod(std::max(maxPeriod, output[endOutputIdx-1]._t));
		maxInputPeriod /= 0.68;

		size_t beginInputIdx = std::lower_bound(input.begin(), input.end(), minInputPeriod) - input.begin();
		size_t endInputIdx = std::lower_bound(input.begin(), input.end(), maxInputPeriod) - input.begin();
		if(endInputIdx < input.size())
		{
			endInputIdx++;
		}

		while((endInputIdx - beginInputIdx)/3 < (endOutputIdx - beginOutputIdx))
		{
			if(!beginInputIdx && endInputIdx>=input.size())
			{
				std::cout<<"LOW DATA FOR OPTIMIZE"<<std::endl;
				return false;
			}

			if(beginInputIdx)
			{
				beginInputIdx--;
			}
			if(endInputIdx<input.size())
			{
				endInputIdx++;
			}
		}

		TVTap inputTail(endInputIdx - beginInputIdx);

		for(size_t i(0); i<inputTail.size(); i++)
		{
			inputTail[i] = input[beginInputIdx+i];

			for(size_t j(0); j<output.size(); j++)
			{
				if(j<beginOutputIdx || j>=endOutputIdx)
				{
					inputTail[i]._ap -= _responseModel->evalAP(inputTail[i]._t, output[j]._t, output[j]._ap);
				}
			}
		}

		TVReal xdata(inputTail.size()*2);
		TVReal ydata(inputTail.size()*2);
		for(size_t i(0); i<inputTail.size(); i++)
		{
			xdata[i] = inputTail[i]._t;
			xdata[inputTail.size() +i] = inputTail[i]._t;

			ydata[i] = inputTail[i]._ap.re();
			ydata[inputTail.size() +i] = inputTail[i]._ap.im();
		}


		TVReal threadStorage;

		ParamsDlevmarFunc params;
		params._x = &xdata[0];
		params._y = &ydata[0];
		params._rm = _responseModel.get();
		params._optPeriod = optPeriod;

// 		TVReal err(xdata.size());
// 		dlevmar_chkjac(
// 			evalFuncOpt, 
// 			jacoFuncOpt, 
// 			&args.front(), 
// 			args.size(), 
// 			ydata.size(),
// 			&params, 
// 			&err.front());
// 		real errVal = std::accumulate(err.begin(), err.end(), 0.0)/err.size();
// 		std::cout<<"dlevmar_chkjac: "<<errVal<<std::endl;
// 		exit(0);


		int res = dlevmar_der_autoMu(
			evalFuncOpt, 
			jacoFuncOpt, 
			&args.front(), 
			&ydata.front(),
			args.size(), 
			ydata.size(),
			threadStorage, 
			&params,
			int(maxIters));

		//std::cout<<"\titers: "<<res<<"\t"<<args.size()/3<<"\t"<<ydata.size()/2<<std::endl;

		for(size_t i(0); i<endOutputIdx - beginOutputIdx; i++)
		{
			 output[beginOutputIdx+i]._t = args[i*3+0];
			 output[beginOutputIdx+i]._ap.set(args[i*3+1], args[i*3+2]);
		}

		std::sort(output.begin(), output.end());

		return true;
	}



}

