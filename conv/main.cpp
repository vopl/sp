#include <iostream>

#include <sp/math.hpp>
#include <sp/window_rect.hpp>
#include <sp/window_hann.hpp>
#include <sp/window_nutall.hpp>
#include <sp/window_hann2.hpp>
#include <sp/convolve.hpp>

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
        signal[index] = sin((x-0.456)*sp::g_2pi/(2.23942317539142285719e-03));//500 герц

        //cout << x<<","<<signal[index]<< endl;

    }

    if(0)
    {
        sp::TVComplex response;

        sp::convolve<sp::Window_rect, 10>(0.456, signal, response);

        for(size_t index(0); index<sp::g_periodSteps; ++index)
        {
            const auto r = response[index];
            cout << abs(r) << endl;
        }
        return 0;
    }

    if(0)
    {
        sp::TVComplex response;

        sp::convolve<sp::Window_hann, 10>(0.456, signal, response);

        for(size_t index(0); index<sp::g_periodSteps; ++index)
        {
            const auto r = response[index];
            cout << r.real()<<" " << r.imag() << " ";


            sp::real rr,ri,ir,ii;

            sp::Window_hann::kernel<10>(sp::g_periodGrid[index]/(2.23942317539142285719e-03),rr,ri,ir,ii);
            cout << rr << " " << ri <<" " << ir <<" "<< ii <<endl;


        }
        return 0;
    }

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

    return 0;
}
