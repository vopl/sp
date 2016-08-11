#include "stdafx.h"
#include "fftw.h"
//#include "params.h"

#define PLANNER_FLAGS_OPT		FFTW_MEASURE
//#define PLANNER_FLAGS_OPT		FFTW_PATIENT
//#define PLANNER_FLAGS_OPT		FFTW_EXHAUSTIVE

#define PLANNER_FLAGS			FFTW_ESTIMATE




#define FWD_TRANSFORM			FFTW_R2HC
#define BWD_TRANSFORM			FFTW_HC2R

//#define FWD_TRANSFORM			FFTW_DHT
//#define BWD_TRANSFORM			FFTW_DHT

namespace ge
{
	fftw_plan Fftw::mkPlan(size_t oneSize, bool forw, bool optimize, bool inPlace)
	{
		PlanDescr pd = {oneSize, 1, forw, inPlace};
		TMPlans::iterator iter = _plans.find(pd);
		if(iter == _plans.end())
		{
			hel::TVReal inStub(oneSize);
			hel::TVReal outStub;
			hel::TVReal *pOutStub;
			if(inPlace)
			{
				pOutStub = &inStub;
			}
			else
			{
				outStub.resize(oneSize);
				pOutStub = &outStub;
			}
			fftw_plan p = NULL;
			
			if(optimize)
			{
				//fftw_plan_with_nthreads(params.getThreadsAmount());
				p = fftw_plan_r2r_1d(
					oneSize, 
					&inStub.front(), 
					&pOutStub->front(), 
					forw?FWD_TRANSFORM:BWD_TRANSFORM, 
					PLANNER_FLAGS_OPT);
				flushWisdom();
			}
			else
			{
				//fftw_plan_with_nthreads(params.getThreadsAmount());
				p = fftw_plan_r2r_1d(
					oneSize, 
					&inStub.front(), 
					&pOutStub->front(), 
					forw?FWD_TRANSFORM:BWD_TRANSFORM, 
					PLANNER_FLAGS);
			}

			_plans[pd] = p;
			iter = _plans.find(pd);
		}
		fftw_plan p=iter->second;
		return p;
	}

	fftw_plan Fftw::mkPlanMulti(
		size_t amount, 
		size_t oneSize, 
		bool forw, 
		bool optimize,
		bool inPlace)
	{
		PlanDescr pd = {oneSize, amount, forw};
		TMPlans::iterator iter = _plans.find(pd);
		if(iter == _plans.end())
		{
			hel::TVReal inStub(oneSize*amount);
			hel::TVReal outStub;
			hel::TVReal *pOutStub;
			if(inPlace)
			{
				pOutStub = &inStub;
			}
			else
			{
				outStub.resize(oneSize*amount);
				pOutStub = &outStub;
			}

			int n[1] = {oneSize};
			fftw_r2r_kind kind[1] = {forw?FWD_TRANSFORM:BWD_TRANSFORM};
			//fftw_r2r_kind kind[1] = {FFTW_DHT};
			fftw_plan p = NULL;

			if(optimize)
			{
				//fftw_plan_with_nthreads(params.getThreadsAmount());
				p = fftw_plan_many_r2r(
					1,				// 	int rank, 
					n,				// 	const int *n, 
					amount,			// 	int howmany,		howmany is the number of transforms to compute, where the k-th transform is of the arrays starting at in+k*idist and out+k*odist
					&inStub.front(),		// 	double *in, 
					NULL,			// 	const int *inembed,	The two nembed parameters (which should be arrays of length rank) indicate the sizes of the input and output array dimensions, respectively, where the transform is of a subarray of size n.
					1,				// 	int istride, 
					oneSize,		// 	int idist,
					&pOutStub->front(),	// 	double *out, 
					NULL,			// 	const int *onembed,
					1,				// 	int ostride, 
					oneSize,		// 	int odist,
					kind,			// 	const fftw_r2r_kind *kind, 
					PLANNER_FLAGS_OPT	// 	unsigned flags
					);
				flushWisdom();
			}
			else
			{
				//fftw_plan_with_nthreads(params.getThreadsAmount());
				p = fftw_plan_many_r2r(
					1,				// 	int rank, 
					n,				// 	const int *n, 
					amount,			// 	int howmany,		howmany is the number of transforms to compute, where the k-th transform is of the arrays starting at in+k*idist and out+k*odist
					&inStub.front(),		// 	double *in, 
					NULL,			// 	const int *inembed,	The two nembed parameters (which should be arrays of length rank) indicate the sizes of the input and output array dimensions, respectively, where the transform is of a subarray of size n.
					1,				// 	int istride, 
					oneSize,		// 	int idist,
					&pOutStub->front(),	// 	double *out, 
					NULL,			// 	const int *onembed,
					1,				// 	int ostride, 
					oneSize,		// 	int odist,
					kind,			// 	const fftw_r2r_kind *kind, 
					PLANNER_FLAGS	// 	unsigned flags
					);
			}

			_plans[pd] = p;
			return p;
		}

		return iter->second;
	}
	Fftw::Fftw()
	{
		assert(sizeof(double) == sizeof(hel::real));

		initThreads();

		if(boost::filesystem::exists("wisdom.txt"))
		{
			size_t wisdomSize = (size_t)boost::filesystem::file_size("wisdom.txt");
			std::vector<char> str(wisdomSize+1);
			std::ifstream wisdom("wisdom.txt", std::ios::binary);
			wisdom.read(&str[0], wisdomSize);
			str[wisdomSize] = 0;
			fftw_import_wisdom_from_string(&str[0]);
		}
	}
	Fftw::~Fftw()
	{
		destroyPlans();
		deinitThreads();
	}

	void Fftw::initThreads()
	{
		fftw_init_threads();
	}
	void Fftw::deinitThreads()
	{
		fftw_cleanup_threads();
	}

	void Fftw::eval(hel::TVReal &in, hel::TVReal &out, bool forw, bool optimize)
	{
		size_t size = in.size();
		out.resize(size);

		fftw_plan p = mkPlan(size, forw, optimize, &in.front() == &out.front());
		fftw_execute_r2r(p, &in[0], &out[0]);
	}

	void Fftw::evalMulti(
		hel::TVReal &in, 
		hel::TVReal &out, 
		size_t oneSize, 
		bool forw, 
		bool optimize)
	{
		size_t size = in.size();
		out.resize(size);

		fftw_plan p = mkPlanMulti(size/oneSize, oneSize, forw, optimize, &in.front() == &out.front());
		fftw_execute_r2r(p, &in[0], &out[0]);
	}

	void Fftw::flushWisdom()
	{
		char *str = fftw_export_wisdom_to_string();
		std::ofstream wisdom("wisdom.txt", std::ios::binary);
		wisdom.write(str, strlen(str));
		//fftw_free(str); глючит
	}

	void Fftw::destroyPlans()
	{
		TMPlans::iterator iter(_plans.begin());
		TMPlans::iterator end(_plans.end());

		for(; iter!=end; iter++)
		{
			fftw_destroy_plan(iter->second);
		}
		_plans.clear();
	}
	Fftw fftw;
}
