#pragma once

#include "math.hpp"
#include "config.hpp"

#include <cassert>

namespace sp
{
    //сворачивает сигнал input с окном и комплексной гармоникой, один раз
    template <class Window, int pow>//оконная функция (hann, gaussian), в ней математика интегрирования
    complex convolve(
            const real xCenter,          //на первом сэмпле время=0, каждый новый сэмпл это +1.0/g_frequency времени
            const TVReal &input,    //это вся дорожка, не кусок. В ней должно быть достаточно отсчетов чтобы
            const real period)      //целевой период
    {
        //итерировать сигнал, для каждого отчета накапливать свертку
        complex res;

        real xStart = xCenter-period*pow/2;
        real xStop = xCenter+period*pow/2;

        assert(xStart >= 0);

        //левая граница. Найти максимальный отсчет, меньше старта
        size_t indexFirst = size_t(xStart/g_sampleStep);
        assert(indexFirst+1 < input.size());

        {
            real x0 = (indexFirst+0) * g_sampleStep;
            real x1 = (indexFirst+1) * g_sampleStep;

            real y0 = input[indexFirst+0];
            real y1 = input[indexFirst+1];

            //линейная интерполяция от точки x0,y0 до x1,y1
            real x = xStart;
            real y = ((y0-y1)*x + (x0*y1 - x1*y0)) / (x0-x1);

            res += Window::template convolve<pow>(
                       xCenter,
                       x, y,
                       x1, y1,
                       period);
        }

        //правая граница
        size_t indexLast = size_t(xStop/g_sampleStep);
        assert(indexLast+1 < input.size());

        if(indexLast == indexFirst)// весь диапазон внутри одного сэмпла
        {
            real x0 = (indexLast+0) * g_sampleStep;
            real x1 = (indexLast+1) * g_sampleStep;

            real y0 = input[indexLast+0];
            real y1 = input[indexLast+1];

            //линейная интерполяция от точки x0,y0 до x1,y1
            real x = xStop;
            real y = ((y0-y1)*x + (x0*y1 - x1*y0)) / (x0-x1);

            res -= Window::template convolve<pow>(
                       xCenter,
                       x, y,
                       x1, y1,
                       period);
            return res;
        }

        {
            real x0 = (indexLast+0) * g_sampleStep;
            real x1 = (indexLast+1) * g_sampleStep;

            real y0 = input[indexLast+0];
            real y1 = input[indexLast+1];

            //линейная интерполяция от точки x0,y0 до x1,y1
            real x = xStop;
            real y = ((y0-y1)*x + (x0*y1 - x1*y0)) / (x0-x1);

            res += Window::template convolve<pow>(
                       xCenter,
                       x0, y0,
                       x, y,
                       period);
        }


        //центр
        for(size_t index(indexFirst+1); index<indexLast; ++index)
        {
            real x0 = (index+0) * g_sampleStep;
            real x1 = (index+1) * g_sampleStep;

            real y0 = input[index+0];
            real y1 = input[index+1];

            res += Window::template convolve<pow>(
                       xCenter,
                       x0, y0,
                       x1, y1,
                       period);
        }


        return res/real(pow);
    }


    //сворачивает сигнал input с окном и комплексной гармоникой, по сетке периодов
    template <class Window, int pow>//оконная функция (hann, gaussian), в ней математика интегрирования
    void convolve(
            const real xCenter,          //на первом сэмпле время=0, каждый новый сэмпл это +1.0/g_frequency времени
            const TVReal &input,    //это вся дорожка, не кусок. В ней должно быть достаточно отсчетов чтобы
            TVComplex &output)      //результат свертки, размерность по сетке периодов
    {
        //итерировать сетку, для каждого периода делать свертку
        output.resize(g_periodSteps);

        for(size_t k(0); k<g_periodSteps; k++)
        {
            output[k] = convolve<Window, pow>(xCenter, input, g_periodGrid[k]);
        }
    }
}
