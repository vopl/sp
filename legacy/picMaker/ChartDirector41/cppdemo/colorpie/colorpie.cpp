#include "chartdir.h"

void createChart(int img, const char *filename)
{
    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

    // Colors of the sectors if custom coloring is used
    int colors[] = {0xb8bc9c, 0xecf0b9, 0x999966, 0x333366, 0xc3c3e6, 0x594330,
        0xa0bdc4};

    // Create a PieChart object of size 280 x 240 pixels
    PieChart *c = new PieChart(280, 240);

    // Set the center of the pie at (140, 120) and the radius to 80 pixels
    c->setPieSize(140, 120, 80);

    // Draw the pie in 3D
    c->set3D();

    // Set the coloring schema
    if (img == 0) {
        c->addTitle("Custom Colors");
        // set the LineColor to light gray
        c->setColor(Chart::LineColor, 0xc0c0c0);
        // use given color array as the data colors (sector colors)
        c->setColors(Chart::DataColor, IntArray(colors,
            sizeof(colors)/sizeof(colors[0])));
    } else if (img == 1) {
        c->addTitle("Dark Background Colors");
        // use the standard white on black palette
        c->setColors(Chart::whiteOnBlackPalette);
    } else if (img == 2) {
        c->addTitle("Wallpaper As Background");
        c->setWallpaper("bg.png");
    } else {
        c->addTitle("Transparent Colors");
        c->setWallpaper("bg.png");
        // use semi-transparent colors to allow the background to be seen
        c->setColors(Chart::transparentPalette);
    }

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Explode the 1st sector (index = 0)
    c->setExplode(0);

    // output the chart
    c->makeChart(filename);

    //free up resources
    delete c;
}

int main(int argc, char *argv[])
{
    createChart(0, "colorpie0.gif");
    createChart(1, "colorpie1.gif");
    createChart(2, "colorpie2.gif");
    createChart(3, "colorpie3.gif");
    return 0;
}

