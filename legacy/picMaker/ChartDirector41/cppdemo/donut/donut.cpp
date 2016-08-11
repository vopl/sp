#include "chartdir.h"

int main(int argc, char *argv[])
{
    // The data for the pie chart
    double data[] = {20, 10, 15, 12};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Facilities", "Production"};

    // Create a PieChart object of size 560 x 280 pixels, with a silver background,
    // black border, 1 pxiel 3D border effect and rounded corners
    PieChart *c = new PieChart(560, 280, Chart::silverColor(), 0x000000, 1);
    c->setRoundedFrame();

    // Add a title box using 15 pts Times Bold Italic font in white color, on a deep
    // blue (0000CC) background with reduced-glare glass effect
    c->addTitle("Donut Chart Demonstration", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x0000cc, 0x000000, Chart::glassEffect(Chart::ReducedGlare))
        ;

    // Set donut center at (280, 140), and outer/inner radii as 110/55 pixels
    c->setDonutSize(280, 140, 110, 55);

    // Set 3D effect with 3D depth of 20 pixels
    c->set3D(20);

    // Set the label box background color the same as the sector color, with
    // reduced-glare glass effect and rounded corners
    TextBox *t = c->setLabelStyle();
    t->setBackground(Chart::SameAsMainColor, Chart::Transparent, Chart::glassEffect(
        Chart::ReducedGlare));
    t->setRoundedCorners();

    // Set the sector label format. The label consists of two lines. The first line
    // is the sector name in Times Bold Italic font and is underlined. The second
    // line shows the data value and percentage.
    c->setLabelFormat(
        "<*block,halign=left*><*font=timesbi.ttf,size=12,underline=1*>{label}"
        "<*/font*><*br*>US$ {value}K ({percent}%)");

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Use the side label layout method
    c->setLabelLayout(Chart::SideLayout);

    // output the chart
    c->makeChart("donut.png");

    //free up resources
    delete c;
    return 0;
}

