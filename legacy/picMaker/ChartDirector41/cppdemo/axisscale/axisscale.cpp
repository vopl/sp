#include "chartdir.h"

void createChart(int img, const char *filename)
{
    // The data for the chart
    double data[] = {5.5, 3.5, -3.7, 1.7, -1.4, 3.3};
    const char *labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun"};

    // Create a XYChart object of size 200 x 180 pixels
    XYChart *c = new XYChart(200, 180);

    // Set the plot area at (30, 20) and of size 140 x 130 pixels
    c->setPlotArea(30, 20, 140, 130);

    // Configure the axis as according to the input parameter
    if (img == 0) {
        c->addTitle("No Axis Extension", "arial.ttf", 8);
    } else if (img == 1) {
        c->addTitle("Top/Bottom Extensions = 0/0", "arial.ttf", 8);
        // Reserve 20% margin at top of plot area when auto-scaling
        c->yAxis()->setAutoScale(0, 0);
    } else if (img == 2) {
        c->addTitle("Top/Bottom Extensions = 0.2/0.2", "arial.ttf", 8);
        // Reserve 20% margin at top and bottom of plot area when auto-scaling
        c->yAxis()->setAutoScale(0.2, 0.2);
    } else if (img == 3) {
        c->addTitle("Axis Top Margin = 15", "arial.ttf", 8);
        // Reserve 15 pixels at top of plot area
        c->yAxis()->setMargin(15);
    } else {
        c->addTitle("Manual Scale -5 to 10", "arial.ttf", 8);
        // Set the y axis to scale from -5 to 10, with ticks every 5 units
        c->yAxis()->setLinearScale(-5, 10, 5);
    }

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a color bar layer using the given data. Use a 1 pixel 3D border for the
    // bars.
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), IntArray(0, 0)
        )->setBorderColor(-1, 1);

    // output the chart
    c->makeChart(filename);

    //free up resources
    delete c;
}

int main(int argc, char *argv[])
{
    createChart(0, "axisscale0.png");
    createChart(1, "axisscale1.png");
    createChart(2, "axisscale2.png");
    createChart(3, "axisscale3.png");
    createChart(4, "axisscale4.png");
    return 0;
}

