#include "chartdir.h"

void createChart(int img, const char *filename)
{
    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Facilities",
        "Production"};

    // Create a PieChart object of size 300 x 300 pixels
    PieChart *c = new PieChart(300, 300);

    if (img == 0) {
    //============================================================
    //    Draw a pie chart where the label is on top of the pie
    //============================================================

        // Set the center of the pie at (150, 150) and the radius to 120 pixels
        c->setPieSize(150, 150, 120);

        // Set the label position to -40 pixels from the perimeter of the pie (-ve
        // means label is inside the pie)
        c->setLabelPos(-40);

    } else {
    //============================================================
    //    Draw a pie chart where the label is outside the pie
    //============================================================

        // Set the center of the pie at (150, 150) and the radius to 80 pixels
        c->setPieSize(150, 150, 80);

        // Set the sector label position to be 20 pixels from the pie. Use a join
        // line to connect the labels to the sectors.
        c->setLabelPos(20, Chart::LineColor);

    }

    // Set the label format to three lines, showing the sector name, value, and
    // percentage. The value 999 will be formatted as US$999K.
    c->setLabelFormat("{label}\nUS${value}K\n({percent}%)");

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
    createChart(0, "circlelabelpie0.png");
    createChart(1, "circlelabelpie1.png");
    return 0;
}

