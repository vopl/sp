#ifndef _hel_responseModel2_h_
#define _hel_responseModel2_h_

#include "hel/types.h"
#include "hel/complex.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	class ResponseModel2
	{
		size_t _pow;

		TVReal _t;

		TVReal _rr;
		TVReal _ri;
		TVReal _ir;
		TVReal _ii;

		TVReal _drr;
		TVReal _dri;
		TVReal _dir;
		TVReal _dii;

	public:
		ResponseModel2();
		virtual ~ResponseModel2(){}

		void setupPeriod(
			size_t pow,
			real minT,
			real maxT,
			size_t stepsT);
		void setup(
			real minT,
			real maxT,
			size_t stepsT,
			real minTd,
			real maxTd,
			size_t pow);
		void setup2(
			real minT,
			real maxT,
			size_t stepsT,
			real minTd,
			real maxTd,
			size_t pow);
		void evalVRaw(real t, real &rr, real &ri, real &ir, real &ii);
		void evalVRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii);

		void evalVDRaw(
			real t, 
			real & rr, real & ri, real & ir, real & ii,
			real &drr, real &dri, real &dir, real &dii);
		void evalDRaw(const TVReal &t, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii);
		void evalVDRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii);

	public:
		Complex eval(real t, real st, const Complex &sv);

	public:
		//////////////////////////////////////////////////////////////////////////
		struct SFuncOptParams
		{
			//����� �������, ������ ���� ������ ������
			//����� n
			const real		*_et;

			//�������� �������, ���� ������, �� ����� ������� �� ������������ (����� �� �������� � dlrvmar_der)
			//����� n
			const Complex	*_ev;

			//�������� ������� �������. ���� ������ �� ������ ������� �� ��������������
			//����� m
			const real		*_st;

			//����������, ���������� ����
			ResponseModel2	*_rm;
		};
		//�������� s ��� ����� ��������� ����������� m ����
		//p(j*3+0)=st
		//p(j*3+1)=sr
		//p(j*3+2)=si
		static void evalFuncOpt(double *p, double *hx, int m, int n, void *_SFuncOptParams);
		static void jacoFuncOpt(double *p, double *jx, int m, int n, void *_SFuncOptParams);

		int solve_tv(
			size_t eSize, const real *et, const Complex *ev,//������
			size_t sSize,       real *st,       Complex *sv,//������
			size_t itMax,//���� ��������
			TVReal &work);
			
		int solve_v(
			size_t eSize, const real *et, const Complex *ev,//������
			size_t sSize, const real *st,       Complex *sv,//������
			size_t itMax,//���� ��������
			TVReal &work);

	public:
		const TVReal &getTable_t();
		const TVReal &getTable_rr();
		const TVReal &getTable_ri();
		const TVReal &getTable_ir();
		const TVReal &getTable_ii();
	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
		}

	};

	typedef boost::shared_ptr<ResponseModel2> ResponseModel2Ptr;

}
#endif
