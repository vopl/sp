#include "chartdir.h"
#include <stdio.h>

void createChart(int img, const char *filename)
{
    char buffer[256];

    // the tilt angle of the pie
    int angle = img * 90 + 45;

    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // Create a PieChart object of size 100 x 110 pixels
    PieChart *c = new PieChart(100, 110);

    // Set the center of the pie at (50, 55) and the radius to 36 pixels
    c->setPieSize(50, 55, 36);

    // Set the depth, tilt angle and 3D mode of the 3D pie (-1 means auto depth,
    // "true" means the 3D effect is in shadow mode)
    c->set3D(-1, angle, true);

    // Add a title showing the shadow angle
    sprintf(buffer, "Shadow @ %d deg", angle);
    c->addTitle(buffer, "arial.ttf", 8);

    // Set the pie data
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])));

    // Disable the sector labels by setting the color to Transparent
    c->setLabelStyle("", 8, Chart::Transparent);

    // output the chart
    c->makeChart(filename);

    //free up resources
    delete c;
}

int main(int argc, char *argv[])
{
    createChart(0, "shadowpie0.png");
    createChart(1, "shadowpie1.png");
    createChart(2, "shadowpie2.png");
    createChart(3, "shadowpie3.png");
    return 0;
}
