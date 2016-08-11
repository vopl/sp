#ifndef _hel_responseModel_h_
#define _hel_responseModel_h_

#include "hel/types.h"
#include "hel/complex.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	class ResponseModel
	{
	public:
		struct VDS
		{
			real _rr;
			real _ri;
			real _ir;
			real _ii;
			real _drr;
			real _dri;
			real _dir;
			real _dii;
		};
		typedef std::vector<VDS> TVVDS;
		//////////////////////////////////////////////////////////////////////////
		//������� ��� t<1
		real	_l1_tStep;
		TVReal	_l1_t;//ti=_le1_t[i]
		TVVDS	_l1_vds;

		//������� ��� t>=1
		real	_ge1_loglogtStep;
		TVReal	_ge1_loglogt;//ti=exp(exp(_g1_loglogt)-1);
		TVVDS	_ge1_vds;


		size_t	_pow;

	public:
		ResponseModel();
		virtual ~ResponseModel(){}

		void setup(
			real maxT,
			size_t stepsT_l1,
			size_t stepsT_ge1,
			size_t pow);

		void evalVRaw(real t, real &rr, real &ri, real &ir, real &ii);
//		void evalVRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii);

		void evalVDRaw(
			real tm, real t, real tp, 
			real & rr, real & ri, real & ir, real & ii,
			real &drr, real &dri, real &dir, real &dii);
//		void evalDRaw(const TVReal &t, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii);
//		void evalVDRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii);

		//////////////////////////////////////////////////////////////////////////
		void evalVRaw_tabled(
			real logt, 
			real &rr, real &ri, real &ir, real &ii);
		void evalVDRaw_tabled(
			real logt, 
			real &rr, real &ri, real &ir, real &ii,
			real &drr, real &dri, real &dir, real &dii);

	public:
		Complex eval(real t, real st, const Complex &sv);
		Complex eval_tabled(real t, real st, const Complex &sv);

	public:
		//////////////////////////////////////////////////////////////////////////
		struct SFuncOptParams
		{
			//����� ��������� �������, ������ ���� ������ ������
			//����� n
			const real		*_elogt;

			//�������� �������, ���� ������, �� ����� ������� �� ������������ (����� �� �������� � dlrvmar_der)
			//����� n
			const Complex	*_ev;

			//�������� �������� ������� �������. ���� ������ �� ������ ������� �� ��������������
			//����� m
			const real		*_slogt;

			//����������, ���������� ����
			ResponseModel	*_rm;
		};
		//�������� s ��� ����� ��������� ����������� m ����
		//p(j*3+0)=st
		//p(j*3+1)=sr
		//p(j*3+2)=si
		static void evalFuncOpt(double *p, double *hx, int m, int n, void *_SFuncOptParams);
		static void jacoFuncOpt(double *p, double *jx, int m, int n, void *_SFuncOptParams);

		int solve_tv(
			size_t eSize, const real *elogt, const Complex *ev,//������
			size_t sSize,       real *slogt,       Complex *sv,//������
			size_t itMax,//���� ��������
			TVReal &work);
			
		int solve_v(
			size_t eSize, const real *elogt, const Complex *ev,//������
			size_t sSize, const real *slogt,       Complex *sv,//������
			size_t itMax,//���� ��������
			TVReal &work);

		void dump(const char *fname);
	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _l1_tStep;
			ar & _l1_t;
			ar & _l1_vds;

			ar & _ge1_loglogtStep;
			ar & _ge1_loglogt;
			ar & _ge1_vds;

			ar & _pow;
		}

	};

	typedef boost::shared_ptr<ResponseModel> ResponseModelPtr;

}
BOOST_CLASS_IMPLEMENTATION(hel::ResponseModel::VDS, boost::serialization::primitive_type);

#endif
