#ifndef _hel_lawPolyChebCos_h_
#define _hel_lawPolyChebCos_h_

#include "hel/math.h"
#include "hel/level.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	struct LawParams
	{
		//начальное количество периодов в окне перед оптимизацией
		real _periodsOnWin;
		real _periodOnNearLevels;
		real _periodMult;
	};

	//////////////////////////////////////////////////////////////////////////
	template <size_t maxPow>
	struct LawPolyChebCos
	{
		//////////////////////////////////////////////////////////////////////////
		//количество аргументов закона: 
		//1 период
		//1 фаза
		//1 амплитуда
		//maxPow+1 коэффициенты ряда полиномов
		static const size_t argumentsAmountOpt = 3 + (1+maxPow);

		//плюс 2 - диапазон
		static const size_t argumentsAmount = argumentsAmountOpt + 2;

		//////////////////////////////////////////////////////////////////////////
		//нормирование времени на [-1,1]
		static real normTime(real time, real a, real b)
		{
			return 2.0*(time - a)/(b - a) - 1.0;
		}

		//////////////////////////////////////////////////////////////////////////
		//закон для аппроксимации
		//cos(2*pi/period*t + phase)*a + poly
		static real eval(real time, const real args[argumentsAmountOpt], real a, real b)
		{
			real res = cos(time * c_2Pi / args[0] + args[1]) * args[2];

			double polyVal;
			double b1;
			double b2;
			int i;

			time = normTime(time, a, b);
			b1 = 0;
			b2 = 0;
			i = maxPow;
			do
			{
				polyVal = 2*time*b1-b2+args[3+i];
				b2 = b1;
				b1 = polyVal;
				i = i-1;
			}
			while(i>=0);
			polyVal = polyVal-time*b2;

			return res + polyVal;
		}
		static real eval(real time, const real args[argumentsAmount])
		{
			return eval(time, args, args[argumentsAmount-2], args[argumentsAmount-1]);
		}

		//////////////////////////////////////////////////////////////////////////
		//значение градиента в точке, используется в МНК
		static void grad(real time, const real args[argumentsAmountOpt], real *d, real a, real b)
		{
			//2*sin(2*Pi*time/period+phase)*Pi*time*a/period^2
			//-sin(2*Pi*time/period+phase)*a
			//cos(2*Pi*time/period+phase)
			//cheb<0>
			//cheb<1>
			//cheb<2>
			//...

			real hArg = c_2Pi / args[0] * time + args[1];
			real sinVal = sin(hArg);

			d[0] = sinVal * c_2Pi / args[0] / args[0] * time * args[2];
			//d[0] = 0;
			d[1] = -sinVal * args[2];
			d[2] = cos(hArg);

			//chebd<maxPow>(time, d+3);
			time = normTime(time, a, b);

			if(maxPow == 0)
			{
				d[3+0] = 1;
			}
			else if(maxPow == 1)
			{
				d[3+0] = 1;
				d[3+1] = time;
			}
			else
			{
				d[3+0] = 1;
				d[3+1] = time;

				for(size_t i=2; i<=maxPow; i++)
				{
					d[3+i] = 2.0*time*d[3+i-1] - d[3+i-2];
				}
			}
		}

		struct FindTimePred
		{
			bool operator()(const LeveledValue &left, real right)
			{
				return left.getTime() < right;
			}
			bool operator()(real left, const LeveledValue &right)
			{
				return left < right.getTime();
			}
			bool operator()(const LeveledValue &left, const LeveledValue &right)
			{
				return left.getTime() < right.getTime();
			}
		};
		//////////////////////////////////////////////////////////////////////////
		//подготовка аргументов к оптимизации
		static void prepareArgs(
			TVLeveledValue::const_iterator valuesIter, 
			size_t size, 
			const LawParams &params, 
			real args[argumentsAmount])
		{
			assert(size >= 2);
			if(size < 2)
			{
				return;
			}



			//////////////////////////////////////////////////////////////////////////
			//вычисление коэффициентов полинома
			{
				real a = valuesIter->getTime();
				real b = (valuesIter+(size-1))->getTime();

				real bma = 0.5 * (b - a);
				real bpa = 0.5 * (b + a);
				real fac = 2.0/(maxPow +1.0);

				real f[maxPow+1];

				for(size_t k = 0; k<=maxPow; k++)
				{
					real y = cos(c_pi * (k+0.5)/(maxPow+1));
					real t = y*bma + bpa;
					TVLeveledValue::const_iterator value = std::lower_bound(valuesIter, valuesIter+size, t, FindTimePred());
					if(value == valuesIter+size)
					{
						value--;
					}
					f[k] = value->getValue();
				}

				for(size_t j = 0; j<=maxPow; j++)
				{
					double sum = 0.0;
					for(size_t k = 0; k<=maxPow; k++)
					{
						sum += f[k]*cos(c_pi * j*(k+0.5)/(maxPow+1));
					}
					args[3+j] = fac * sum;
				}
				args[3] /= 2;

				args[argumentsAmount-2] = a;
				args[argumentsAmount-1] = b;
			}

// 			{
// 				args[0]=1;
// 				args[1]=args[2]=0;
// 
// 				std::ofstream tout("pair");
// 
// 				for(size_t i = 0; i < size; i++)
// 				{
// 					real rx = values[i].getTime();
// 					real ry = eval(rx, args);
// 
// 					tout<<values[i].getValue()<<"\t"<<ry<<std::endl;
// 				}
// 
// 				exit(0);
// 			}



			//подбор примерных гармонических параметров
			args[0]=1;
			args[1]=args[2]=0;
			real sum = 0;
			TVLeveledValue::const_iterator iter = valuesIter;
			for(size_t i = 0; i < size; i++, iter++)
			{
				real rx = iter->getTime();
				real ry = iter->getValue() - eval(rx, args);

				sum += fabs(ry);
			}

			real xRange = (valuesIter+(size-1))->getTime() - valuesIter->getTime();

			//чтобы получилось params._periodsAmount периодов в окне
			args[0] = xRange / params._periodsOnWin;
			//фаза
			args[1] = 0;
			//амплитуда
			args[2] = sum / size;
		}

		//////////////////////////////////////////////////////////////////////////
		//корректировка после оптимизации отрицательной частоты амплитуды, фазы
		static void correctArgs(TVLeveledValue::const_iterator valuesIter, size_t size, real *args)
		{
			if(args[0] < 0)
			{
				args[0] = -args[0];
				args[1] = -args[1];
			}

			if(args[2] < 0)
			{
				args[2] = -args[2];
				args[1] += c_pi;
			}

			args[1] = alignPhase(args[1]);
		}

	};
}

#endif
