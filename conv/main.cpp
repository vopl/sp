#include <iostream>

#include <sp/math.hpp>
#include <sp/window_rect.hpp>
#include <sp/window_hann.hpp>
#include <sp/window_nutall.hpp>
#include <sp/window_hann2.hpp>
#include <sp/convolve.hpp>

#include <sp/responseModel.hpp>

using namespace std;





//hann, hann2















int main(int argc, char *argv[])
{
    cout.precision(20);
    cout.setf(std::ios::scientific);

//    sp::real x = 1.0/3;
//    cout << "Hello World! "<< sizeof(x)*8<< endl;

//    for(const auto &x : sp::g_periodGrid)
//    {
//        cout << x<< endl;
//    }

    sp::TVReal signal;

    signal.resize(100000);
    for(size_t index(0); index<signal.size(); ++index)
    {
        sp::real x = index * sp::g_sampleStep;
        signal[index] = cos((x-0.456)*sp::g_2pi/(sp::g_periodGrid[250]));//ровно посеридине нашей шкалы

        //cout << x<<","<<signal[index]<< endl;

    }

    if(0)
    {
        sp::TVComplex response;

        sp::convolve<sp::Window_rect, 10>(0.456, signal, response);

        for(size_t index(0); index<sp::g_periodSteps; ++index)
        {
            const auto r = response[index];
            cout << r.a() << endl;
        }
        return 0;
    }

    if(0)
    {
        sp::TVComplex response;

        sp::convolve<sp::Window_hann2, 10>(0.456, signal, response);

        for(size_t index(0); index<sp::g_periodSteps; ++index)
        {
            const auto r = response[index];
            cout << r.re()<<" " << r.im() << " ";


            sp::real rr,ri,ir,ii;

            sp::Window_hann2::kernel<10>(sp::g_periodGrid[index]/(sp::g_periodGrid[250]),rr,ri,ir,ii);
            cout << rr << " " << ri <<" " << ir <<" "<< ii <<endl;


        }
        return 0;
    }

    if(0)
    {
        for(size_t col(0); col<sp::g_periodSteps; ++col)
        {
            for(size_t row(0); row<sp::g_periodSteps; ++row)
            {
                const auto t = sp::g_periodGrid[col] / sp::g_periodGrid[row];

                sp::real rr,ri,ir,ii;

                sp::Window_hann::kernel<10>(t,rr,ri,ir,ii);
                cout << rr << " " << ir <<" ";
            }
            cout<<endl;
            for(size_t row(0); row<sp::g_periodSteps; ++row)
            {
                const auto t = sp::g_periodGrid[col] / sp::g_periodGrid[row];

                sp::real rr,ri,ir,ii;

                sp::Window_hann::kernel<10>(t,rr,ri,ir,ii);
                cout << ri << " " << ii <<" ";
            }
            cout<<endl;
        }

    }


    if(1)
    {
        sp::ResponseModel rm;

        sp::TVComplex response(sp::g_periodSteps);
        //sp::convolve<sp::Window_hann, 10>(0.456, signal, response);

        for(size_t i(0); i<sp::g_periodSteps; ++i)
        {
            response[i] = 0;

            for(size_t j(0); j<sp::g_periodSteps; j+=4)
            {
                response[i] += rm.eval(sp::g_periodGrid[i], sp::g_periodGrid[j], sp::complex(1,0));
            }
        }

        sp::TVReal logPeriodGrid(sp::g_periodGrid.size());
        for(size_t i(0); i<sp::g_periodSteps; ++i)
        {
            logPeriodGrid[i] = log(sp::g_periodGrid[i]);
        }


        sp::TVReal work;

        //для инициализации спектра нулем - mu=1e-10 лучший. При меньших значениях начинают артифакты появляться, при больших - медленно сходится
        for(int iters(1); iters<200; iters++)
        {
            sp::TVComplex spectr(response.size());
            //spectr = response;
            //spectr[250] = sp::complex(0.5,0);

            int res = rm.solve_v(1e-10,
                        response.size(),
                        &logPeriodGrid[0],
                    &response[0],
                    &spectr[0],
                    iters,
                   work);

            cerr<<iters<<": "<<res<<endl;
            for(size_t i(0); i<sp::g_periodSteps; ++i)
            {
                std::cout<<spectr[i].a()<<" ";
            }
            std::cout<<endl;
        }
    }

    return 0;
}
