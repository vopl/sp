#include "chartdir.h"

int main(int argc, char *argv[])
{
    // The data for the bar chart
    double data0[] = {100, 125, 245, 147, 67};
    double data1[] = {85, 156, 179, 211, 123};
    double data2[] = {97, 87, 56, 267, 157};

    // The labels for the bar chart
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 500 x 280 pixels, using 0xffffcc as background
    // color, with a black border, and 1 pixel 3D border effect
    XYChart *c = new XYChart(500, 280, 0xffffcc, 0, 1);

    // Set the plotarea at (50, 45) and of size 320 x 200 pixels. Use white
    // (0xffffff) color as background.
    c->setPlotArea(50, 45, 320, 200, 0xffffff);

    // Add a legend box at (370, 45) using vertical layout and 8 points Arial Bold
    // font.
    LegendBox *legendBox = c->addLegend(370, 45, true, "arialbd.ttf", 8);

    // Set the legend box background and border to transparent
    legendBox->setBackground(Chart::Transparent, Chart::Transparent);

    // Set the legend box icon size to 16 x 32 pixels to match with custom icon size
    legendBox->setKeySize(16, 32);

    // Add a title to the chart using 14 points Times Bold Itatic font and white font
    // color, and 0x804020 as the background color
    c->addTitle("Weekly Product Sales", "timesbi.ttf", 14, 0xffffff)->setBackground(
        0x804020);

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a percentage bar layer and set the layer 3D depth to 8 pixels
    BarLayer *layer = c->addBarLayer(Chart::Percentage);

    // Add the three data sets to the bar layer, using icons images with labels as
    // data set names
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xddaa77,
        "<*block,valign=absmiddle*><*img=service.png*> Service<*/*>");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0xaadd77,
        "<*block,valign=absmiddle*><*img=software.png*> Software<*/*>");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0xaa77dd,
        "<*block,valign=absmiddle*><*img=computer.png*> Hardware<*/*>");

    // Disable bar borders by setting their colors to transparent
    layer->setBorderColor(Chart::Transparent);

    // Enable data label at the middle of the the bar
    layer->setDataLabelStyle()->setAlignment(Chart::Center);

    // For a vertical stacked chart with positive data only, the last data set is
    // always on top. However, in a vertical legend box, the last data set is at the
    // bottom. This can be reversed by using the setLegend method.
    layer->setLegend(Chart::ReverseLegend);

    // output the chart
    c->makeChart("percentbar.png");

    //free up resources
    delete c;
    return 0;
}

