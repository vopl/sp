#include "stdafx.h"
#include "mfccharts.h"
#include "chartdir.h"
#include "FinanceChart.h"

////////////////////////////////////////////////////////////////////////////////
//
//	Implementation of Demo Charts
//
////////////////////////////////////////////////////////////////////////////////

BaseChart *simplepie(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

    // Create a PieChart object of size 360 x 300 pixels
    PieChart *c = new PieChart(360, 300);

    // Set the center of the pie at (180, 140) and the radius to 100 pixels
    c->setPieSize(180, 140, 100);

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *threedpie(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

    // Create a PieChart object of size 360 x 300 pixels
    PieChart *c = new PieChart(360, 300);

    // Set the center of the pie at (180, 140) and the radius to 100 pixels
    c->setPieSize(180, 140, 100);

    // Add a title to the pie chart
    c->addTitle("Project Cost Breakdown");

    // Draw the pie in 3D
    c->set3D();

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Explode the 1st sector (index = 0)
    c->setExplode(0);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *multidepthpie(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {72, 18, 15, 12};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Machinery", "Facilities", "Computers"};

    // The depths for the sectors
    double depths[] = {30, 20, 10, 10};

    // Create a PieChart object of size 360 x 300 pixels, with a light blue (DDDDFF)
    // background and a 1 pixel 3D border
    PieChart *c = new PieChart(360, 300, 0xddddff, -1, 1);

    // Set the center of the pie at (180, 175) and the radius to 100 pixels
    c->setPieSize(180, 175, 100);

    // Add a title box using 15 pts Times Bold Italic font and blue (AAAAFF) as
    // background color
    c->addTitle("Project Cost Breakdown", "timesbi.ttf", 15)->setBackground(0xaaaaff)
        ;

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Draw the pie in 3D with variable 3D depths
    c->set3D(DoubleArray(depths, sizeof(depths)/sizeof(depths[0])));

    // Set the start angle to 225 degrees may improve layout when the depths of the
    // sector are sorted in descending order, because it ensures the tallest sector
    // is at the back.
    c->setStartAngle(225);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *sidelabelpie(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

    // Create a PieChart object of size 500 x 230 pixels
    PieChart *c = new PieChart(500, 230);

    // Set the center of the pie at (250, 120) and the radius to 100 pixels
    c->setPieSize(250, 120, 100);

    // Add a title box using 15 points Times Bold Italic as font
    c->addTitle("Project Cost Breakdown", "timesbi.ttf", 15);

    // Draw the pie in 3D
    c->set3D();

    // Use the side label layout method
    c->setLabelLayout(Chart::SideLayout);

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *smallsectorpie(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {35, 30, 25, 7, 6, 5, 4, 3, 2, 1};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Production", "Facilities", "Taxes", "Misc",
        "Legal", "Insurance", "Licenses", "Transport", "Interest"};

    // Create a PieChart object of size 560 x 270 pixels, with a golden background
    // and a 1 pixel 3D border
    PieChart *c = new PieChart(560, 270, Chart::goldColor(), -1, 1);

    // Add a title box using 15 pts Times Bold Italic font and metallic pink
    // background color
    c->addTitle("Project Cost Breakdown", "timesbi.ttf", 15)->setBackground(
        Chart::metalColor(0xff9999));

    // Set the center of the pie at (280, 135) and the radius to 110 pixels
    c->setPieSize(280, 135, 110);

    // Draw the pie in 3D with 20 pixels 3D depth
    c->set3D(20);

    // Use the side label layout method
    c->setLabelLayout(Chart::SideLayout);

    // Set the label box background color the same as the sector color, with glass
    // effect, and with 5 pixels rounded corners
    TextBox *t = c->setLabelStyle();
    t->setBackground(Chart::SameAsMainColor, Chart::Transparent, Chart::glassEffect()
        );
    t->setRoundedCorners(5);

    // Set the border color of the sector the same color as the fill color. Set the
    // line color of the join line to black (0x0)
    c->setLineColor(Chart::SameAsMainColor, 0x000000);

    // Set the start angle to 135 degrees may improve layout when there are many
    // small sectors at the end of the data array (that is, data sorted in descending
    // order). It is because this makes the small sectors position near the
    // horizontal axis, where the text label has the least tendency to overlap. For
    // data sorted in ascending order, a start angle of 45 degrees can be used
    // instead.
    c->setStartAngle(135);

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *circlelabelpie(int img, const char **imageMap)
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
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *legendpie(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

    // Create a PieChart object of size 450 x 240 pixels
    PieChart *c = new PieChart(450, 240);

    // Set the center of the pie at (150, 100) and the radius to 80 pixels
    c->setPieSize(150, 100, 80);

    // Add a title at the bottom of the chart using Arial Bold Italic font
    c->addTitle(Chart::Bottom, "Project Cost Breakdown", "arialbi.ttf");

    // Draw the pie in 3D
    c->set3D();

    // add a legend box where the top left corner is at (330, 40)
    c->addLegend(330, 40);

    // modify the label format for the sectors to $nnnK (pp.pp%)
    c->setLabelFormat("{label} ${value}K\n({percent}%)");

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Explode the 1st sector (index = 0)
    c->setExplode(0);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *iconpie(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {72, 18, 15, 12};

    // The depths for the sectors
    double depths[] = {30, 20, 10, 10};

    // The labels for the pie chart
    const char *labels[] = {"Sunny", "Cloudy", "Rainy", "Snowy"};

    // The icons for the sectors
    const char *icons[] = {"sun.png", "cloud.png", "rain.png", "snowy.png"};

    // Create a PieChart object of size 400 x 310 pixels, with a blue (CCCCFF)
    // vertical metal gradient background, black border, 1 pixel 3D border effect and
    // rounded corners
    PieChart *c = new PieChart(400, 310, Chart::metalColor(0xccccff, 0), 0x000000, 1)
        ;
    c->setRoundedFrame();

    // Set the center of the pie at (200, 180) and the radius to 100 pixels
    c->setPieSize(200, 180, 100);

    // Add a title box using 15 pts Times Bold Italic font, on a blue (CCCCFF)
    // background with glass effect
    c->addTitle("Weather Profile in Wonderland", "timesbi.ttf", 15)->setBackground(
        0xccccff, 0x000000, Chart::glassEffect());

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Add icons to the chart as a custom field
    c->addExtraField(StringArray(icons, sizeof(icons)/sizeof(icons[0])));

    // Configure the sector labels using CDML to include the icon images
    c->setLabelFormat(
        "<*block,valign=absmiddle*><*img={field0}*> <*block*>{label}\n{percent}%"
        "<*/*><*/*>");

    // Draw the pie in 3D with variable 3D depths
    c->set3D(DoubleArray(depths, sizeof(depths)/sizeof(depths[0])));

    // Set the start angle to 225 degrees may improve layout when the depths of the
    // sector are sorted in descending order, because it ensures the tallest sector
    // is at the back.
    c->setStartAngle(225);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: {value} days ({percent}%)'");

    return c;
}


BaseChart *iconpie2(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {28, 45, 5, 1, 12};

    // The labels for the pie chart
    const char *labels[] = {"Excellent", "Good", "Bad", "Very Bad", "Neutral"};

    // The icons for the sectors
    const char *icons[] = {"laugh.png", "smile.png", "sad.png", "angry.png",
        "nocomment.png"};

    // Create a PieChart object of size 560 x 300 pixels, with a silver background,
    // black border, 1 pxiel 3D border effect and rounded corners
    PieChart *c = new PieChart(560, 300, Chart::silverColor(), 0x000000, 1);
    c->setRoundedFrame();

    // Set the center of the pie at (280, 150) and the radius to 120 pixels
    c->setPieSize(280, 150, 120);

    // Add a title box with title written in CDML, on a sky blue (A0C8FF) background
    // with glass effect
    c->addTitle(
        "<*block,valign=absmiddle*><*img=doc.png*>"
        "<*font=timesbi.ttf,size=15,color=000080*> Customer Survey: "
        "<*font=timesi.ttf,color=000000*>Do you like our <*font,color=dd0000*>Hyper"
        "<*super*>TM<*/font*> molecules?")->setBackground(0xa0c8ff, 0x000000,
        Chart::glassEffect());

    // Add a logo to the chart written in CDML as the bottom title aligned to the
    // bottom right
    c->addTitle(Chart::BottomRight,
        "<*block,valign=absmiddle*><*img=molecule.png*> <*block*><*color=FF*>"
        "<*font=timesbi.ttf,size=12*>Molecular Engineering\n"
        "<*font=arial.ttf,size=10*>Creating better molecules");

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Set 3D style
    c->set3D();

    // Use the side label layout method
    c->setLabelLayout(Chart::SideLayout);

    // Set the label background color to transparent
    c->setLabelStyle()->setBackground(Chart::Transparent);

    // Add icons to the chart as a custom field
    c->addExtraField(StringArray(icons, sizeof(icons)/sizeof(icons[0])));

    // Configure the sector labels using CDML to include the icon images
    c->setLabelFormat(
        "<*block,valign=absmiddle*><*img={field0}*> {label} ({percent|0}%)");

    // Explode the 3rd and 4th sectors as a group (index = 2 and 3)
    c->setExplodeGroup(2, 3);

    // Set the start angle to 135 degrees may improve layout when there are many
    // small sectors at the end of the data array (that is, data sorted in descending
    // order). It is because this makes the small sectors position near the
    // horizontal axis, where the text label has the least tendency to overlap. For
    // data sorted in ascending order, a start angle of 45 degrees can be used
    // instead.
    c->setStartAngle(135);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: {value} responses ({percent}%)'");

    return c;
}


BaseChart *multipie(int img, const char **imageMap)
{
    // The data for the pie chart
    double data0[] = {25, 18, 15};
    double data1[] = {14, 32, 24};
    double data2[] = {25, 23, 9};

    // The labels for the pie chart
    const char *labels[] = {"Software", "Hardware", "Services"};

    // Create a PieChart object of size 180 x 160 pixels
    PieChart *c = new PieChart(180, 160);

    // Set the center of the pie at (90, 80) and the radius to 60 pixels
    c->setPieSize(90, 80, 60);

    // Set the border color of the sectors to white (ffffff)
    c->setLineColor(0xffffff);

    // Set the background color of the sector label to pale yellow (ffffc0) with a
    // black border (000000)
    c->setLabelStyle()->setBackground(0xffffc0, 0x000000);

    // Set the label to be slightly inside the perimeter of the circle
    c->setLabelLayout(Chart::CircleLayout, -10);

    // Set the title, data and colors according to which pie to draw
    if (img == 0) {
        c->addTitle("Alpha Division", "arialbd.ttf", 8);
        c->setData(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), StringArray(
            labels, sizeof(labels)/sizeof(labels[0])));
        int pattern1[] = {0xff3333, 0xff9999, 0xffcccc};
        c->setColors(Chart::DataColor, IntArray(pattern1,
            sizeof(pattern1)/sizeof(pattern1[0])));
    } else if (img == 1) {
        c->addTitle("Beta Division", "arialbd.ttf", 8);
        c->setData(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), StringArray(
            labels, sizeof(labels)/sizeof(labels[0])));
        int pattern2[] = {0x33ff33, 0x99ff99, 0xccffcc};
        c->setColors(Chart::DataColor, IntArray(pattern2,
            sizeof(pattern2)/sizeof(pattern2[0])));
    } else {
        c->addTitle("Gamma Division", "arialbd.ttf", 8);
        c->setData(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), StringArray(
            labels, sizeof(labels)/sizeof(labels[0])));
        int pattern3[] = {0x3333ff, 0x9999ff, 0xccccff};
        c->setColors(Chart::DataColor, IntArray(pattern3,
            sizeof(pattern3)/sizeof(pattern3[0])));
    }

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}M ({percent}%)'");

    return c;
}


BaseChart *donut(int img, const char **imageMap)
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
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *icondonut(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {72, 18, 15, 12};

    // The depths for the sectors
    double depths[] = {30, 20, 10, 10};

    // The labels for the pie chart
    const char *labels[] = {"Sunny", "Cloudy", "Rainy", "Snowy"};

    // The icons for the sectors
    const char *icons[] = {"sun.png", "cloud.png", "rain.png", "snowy.png"};

    // Create a PieChart object of size 400 x 300 pixels
    PieChart *c = new PieChart(400, 300);

    // Use the semi-transparent palette for this chart
    c->setColors(Chart::transparentPalette);

    // Set the background to metallic light blue (CCFFFF), with a black border and 1
    // pixel 3D border effect,
    c->setBackground(Chart::metalColor(0xccccff), 0x000000, 1);

    // Set donut center at (200, 175), and outer/inner radii as 100/50 pixels
    c->setDonutSize(200, 175, 100, 50);

    // Add a title box using 15 pts Times Bold Italic font and metallic blue (8888FF)
    // background color
    c->addTitle("Weather Profile in Wonderland", "timesbi.ttf", 15)->setBackground(
        Chart::metalColor(0x8888ff));

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Add icons to the chart as a custom field
    c->addExtraField(StringArray(icons, sizeof(icons)/sizeof(icons[0])));

    // Configure the sector labels using CDML to include the icon images
    c->setLabelFormat(
        "<*block,valign=absmiddle*><*img={field0}*> <*block*>{label}\n{percent}%"
        "<*/*><*/*>");

    // Draw the pie in 3D with variable 3D depths
    c->set3D(DoubleArray(depths, sizeof(depths)/sizeof(depths[0])));

    // Set the start angle to 225 degrees may improve layout when the depths of the
    // sector are sorted in descending order, because it ensures the tallest sector
    // is at the back.
    c->setStartAngle(225);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: {value} days ({percent}%)'");

    return c;
}


BaseChart *texturedonut(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {18, 45, 28};

    // The labels for the pie chart
    const char *labels[] = {"Marble", "Wood", "Granite"};

    // The icons for the sectors
    const char *texture[] = {"marble3.png", "wood.png", "rock.png"};

    // Create a PieChart object of size 400 x 330 pixels, with a metallic green
    // (88EE88) background, black border and 1 pixel 3D border effect
    PieChart *c = new PieChart(400, 330, Chart::metalColor(0x88ee88), 0x000000, 1);

    // Set donut center at (200, 160), and outer/inner radii as 120/60 pixels
    c->setDonutSize(200, 160, 120, 60);

    // Add a title box using 15 pts Times Bold Italic font and metallic deep green
    // (008000) background color
    c->addTitle("Material Composition", "timesbi.ttf", 15)->setBackground(
        Chart::metalColor(0x008000));

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Set the colors of the sectors to the 3 texture patterns
    c->setColor(Chart::DataColor + 0, c->patternColor(texture[0]));
    c->setColor(Chart::DataColor + 1, c->patternColor(texture[1]));
    c->setColor(Chart::DataColor + 2, c->patternColor(texture[2]));

    // Draw the pie in 3D with a 3D depth of 30 pixels
    c->set3D(30);

    // Use 12 pts Arial Bold Italic as the sector label font
    c->setLabelStyle("arialbi.ttf", 12);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: {value}kg ({percent}%)'");

    return c;
}


BaseChart *concentric(int img, const char **imageMap)
{
    // Data for outer ring
    double data[] = {88, 124, 96};

    // Data for inner ring
    double data2[] = {77, 87, 45};

    // Labels for the sectors
    const char *labels[] = {"Hardware", "Software", "Services"};

    // Colors for the sectors
    int colors[] = {0xff9999, 0x9999ff, 0x66ff66};

    //
    // Create the main chart, which contains the chart title, the outer ring, and the
    // legend box
    //

    // Create a PieChart object of size 450 x 360 pixels, with transparent background
    PieChart *c = new PieChart(450, 360);

    // Add a title to the chart with 18pts Times Bold Italic font
    c->addTitle("Concentric Donut Chart", "timesbi.ttf", 18);

    // Set donut center at (160, 200), and outer/inner radii as 150/100 pixels
    c->setDonutSize(160, 200, 150, 100);

    // Add a label at the bottom-right corner of the ring to label the outer ring Use
    // 12pts Arial Bold Italic font in white (ffffff) color, on a green (008800)
    // background, with soft lighting effect and 5 pixels rounded corners
    TextBox *t = c->addText(260, 300, " Year 2006 ", "arialbi.ttf", 12, 0xffffff);
    t->setBackground(0x008800, Chart::Transparent, Chart::softLighting());
    t->setRoundedCorners(5);

    // Set the legend box at (320, 50) with 12 pts Arial Bold Italic font, with no
    // border
    c->addLegend(320, 50, true, "arialbi.ttf", 13)->setBackground(Chart::Transparent,
        Chart::Transparent);

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Set the pie colors
    c->setColors(Chart::DataColor, IntArray(colors, sizeof(colors)/sizeof(colors[0]))
        );

    // Set pie border color to white (ffffff)
    c->setLineColor(0xffffff);

    // Set pie label to value in $###M format, percentage in (##.#%) format, in two
    // lines.
    c->setLabelFormat("${value}M<*br*>({percent|1}%)");

    // Use 10pts Airal Bold for the sector labels
    c->setLabelStyle("arialbd.ttf", 10);

    // Set the label position to -25 pixels from the sector (which would be internal
    // to the sector)
    c->setLabelPos(-25);

    //
    // Create the inner ring.
    //

    // Create a PieChart object of size 280 x 320 pixels, with transparent background
    PieChart *c2 = new PieChart(280, 320, Chart::Transparent);

    // Set donut center at (110, 110), and outer/inner radii as 100/50 pixels
    c2->setDonutSize(110, 110, 100, 50);

    // Add a label at the center of the ring to label the inner ring. Use 12pts Arial
    // Bold Italic font in white (ffffff) color, on a deep blue (0000cc) background,
    // with soft lighting effect and 5 pixels rounded corners
    TextBox *t2 = c2->addText(110, 110, " Year 2005 ", "arialbi.ttf", 12, 0xffffff,
        Chart::Center);
    t2->setBackground(0x0000cc, Chart::Transparent, Chart::softLighting());
    t2->setRoundedCorners(5);

    // Set the pie data and the pie labels
    c2->setData(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), StringArray(
        labels, sizeof(labels)/sizeof(labels[0])));

    // Set the pie colors
    c2->setColors(Chart::DataColor, IntArray(colors, sizeof(colors)/sizeof(colors[0])
        ));

    // Set pie border color to white (ffffff)
    c2->setLineColor(0xffffff);

    // Set pie label to value in $###M format, percentage in (##.#%) format, in two
    // lines.
    c2->setLabelFormat("${value}M<*br*>({percent|1}%)");

    // Use 10pts Airal Bold for the sector labels
    c2->setLabelStyle("arialbd.ttf", 10);

    // Set the label position to -25 pixels from the sector (which would be internal
    // to the sector)
    c2->setLabelPos(-25);

    // merge the inner ring into the outer ring at (50, 90)
    c->makeChart()->merge(c2->makeChart(), 50, 90, Chart::TopLeft, 0);

    // output the chart
    c->makeChart();

    //free up resources
    delete c2;

    return c;
}


BaseChart *fontpie(int img, const char **imageMap)
{
    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

    // Create a PieChart object of size 480 x 300 pixels
    PieChart *c = new PieChart(480, 300);

    // Use a blue marble pattern as the background wallpaper, with a black border,
    // and 1 pixel 3D border effect
    c->setBackground(c->patternColor("marble.png"), 0x000000, 1);

    // Set the center of the pie at (150, 150) and the radius to 100 pixels
    c->setPieSize(150, 150, 100);

    // Add a title to the pie chart using Times Bold Italic/15 points/deep blue
    // (0x000080) as font, with a wood pattern as the title background
    c->addTitle("Project Cost Breakdown", "timesbi.ttf", 15, 0x000080
        )->setBackground(c->patternColor("wood.png"));

    // Draw the pie in 3D
    c->set3D();

    // Add a legend box using Arial Bold Italic/11 points font. Use a pink marble
    // pattern as the background wallpaper, with a 1 pixel 3D border. The legend box
    // is top-right aligned relative to the point (465, 70)
    LegendBox *b = c->addLegend(465, 70, true, "arialbi.ttf", 11);
    b->setBackground(c->patternColor("marble2.png"), Chart::Transparent, 1);
    b->setAlignment(Chart::TopRight);

    // Set the default font for all sector labels to Arial Bold/8 pts/dark green
    // (0x008000).
    c->setLabelStyle("arialbd.ttf", 8, 0x008000);

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Explode the 3rd sector
    c->setExplode(2, 40);

    // Use Arial Bold/12 pts/red as label font for the 3rd sector
    c->sector(2)->setLabelStyle("arialbd.ttf", 12, 0xff0000);

    // Use Arial/8 pts/deep blue as label font for the 5th sector. Add a background
    // box using the sector fill color (SameAsMainColor), with a black (0x000000)
    // edge and 2 pixel 3D border.
    c->sector(4)->setLabelStyle("arial.ttf", 8, 0x000080)->setBackground(
        Chart::SameAsMainColor, 0x000000, 2);

    // Use Arial Italic/8 pts/light red (0xff9999) as label font for the 6th sector.
    // Add a dark blue (0x000080) background box with a 2 pixel 3D border.
    c->sector(0)->setLabelStyle("ariali.ttf", 8, 0xff9999)->setBackground(0x000080,
        Chart::Transparent, 2);

    // Use Times Bold Italic/8 pts/deep green (0x008000) as label font for 7th
    // sector. Add a yellow (0xFFFF00) background box with a black (0x000000) edge.
    c->sector(6)->setLabelStyle("timesbi.ttf", 8, 0x008000)->setBackground(0xffff00,
        0x000000);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *colorpie(int img, const char **imageMap)
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
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *threedanglepie(int img, const char **imageMap)
{
    char buffer[256];

    // the tilt angle of the pie
    int angle = img * 15;

    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

    // Create a PieChart object of size 100 x 110 pixels
    PieChart *c = new PieChart(100, 110);

    // Set the center of the pie at (50, 55) and the radius to 38 pixels
    c->setPieSize(50, 55, 38);

    // Set the depth and tilt angle of the 3D pie (-1 means auto depth)
    c->set3D(-1, angle);

    // Add a title showing the tilt angle
    sprintf(buffer, "Tilt = %d deg", angle);
    c->addTitle(buffer, "arial.ttf", 8);

    // Set the pie data
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Disable the sector labels by setting the color to Transparent
    c->setLabelStyle("", 8, Chart::Transparent);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *threeddepthpie(int img, const char **imageMap)
{
    char buffer[256];

    // the tilt angle of the pie
    int depth = img * 5 + 5;

    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

    // Create a PieChart object of size 100 x 110 pixels
    PieChart *c = new PieChart(100, 110);

    // Set the center of the pie at (50, 55) and the radius to 38 pixels
    c->setPieSize(50, 55, 38);

    // Set the depth of the 3D pie
    c->set3D(depth);

    // Add a title showing the depth
    sprintf(buffer, "Depth = %d pixels", depth);
    c->addTitle(buffer, "arial.ttf", 8);

    // Set the pie data
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Disable the sector labels by setting the color to Transparent
    c->setLabelStyle("", 8, Chart::Transparent);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *shadowpie(int img, const char **imageMap)
{
    char buffer[256];

    // the tilt angle of the pie
    int angle = img * 90 + 45;

    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

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
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Disable the sector labels by setting the color to Transparent
    c->setLabelStyle("", 8, Chart::Transparent);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *anglepie(int img, const char **imageMap)
{
    char buffer[256];

    // query string to determine the starting angle and direction
    int angle = 0;
    bool clockwise = true;
    if (img != 0) {
        angle = 90;
        clockwise = false;
    }

    // The data for the pie chart
    double data[] = {25, 18, 15, 12, 8, 30, 35};

    // The labels for the pie chart
    const char *labels[] = {"Labor", "Licenses", "Taxes", "Legal", "Insurance",
        "Facilities", "Production"};

    // Create a PieChart object of size 280 x 240 pixels
    PieChart *c = new PieChart(280, 240);

    // Set the center of the pie at (140, 130) and the radius to 80 pixels
    c->setPieSize(140, 130, 80);

    // Add a title to the pie to show the start angle and direction
    if (clockwise) {
        sprintf(buffer, "Start Angle = %d degrees\nDirection = Clockwise", angle);
        c->addTitle(buffer);
    } else {
        sprintf(buffer, "Start Angle = %d degrees\nDirection = AntiClockwise", angle)
            ;
        c->addTitle(buffer);
    }

    // Set the pie start angle and direction
    c->setStartAngle(angle, clockwise);

    // Draw the pie in 3D
    c->set3D();

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Explode the 1st sector (index = 0)
    c->setExplode(0);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *donutwidth(int img, const char **imageMap)
{
    char buffer[256];

    // query string to determine the donut inner radius, expressed as percentage of
    // outer radius
    int donutRadius = img * 25;

    // The data for the pie chart
    double data[] = {10, 10, 10, 10, 10};

    // The labels for the pie chart
    const char *labels[] = {"Marble", "Wood", "Granite", "Plastic", "Metal"};

    // Create a PieChart object of size 150 x 120 pixels, with a grey (EEEEEE)
    // background, black border and 1 pixel 3D border effect
    PieChart *c = new PieChart(150, 120, 0xeeeeee, 0x000000, 1);

    // Set donut center at (75, 65) and the outer radius to 50 pixels. Inner radius
    // is computed according donutWidth
    c->setDonutSize(75, 60, 50, 50 * donutRadius / 100);

    // Add a title to show the donut width
    sprintf(buffer, "Inner Radius = %d %%", donutRadius);
    c->addTitle(buffer, "Arial", 10)->setBackground(0xcccccc, 0);

    // Draw the pie in 3D
    c->set3D(12);

    // Set the pie data and the pie labels
    c->setData(DoubleArray(data, sizeof(data)/sizeof(data[0])), StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Disable the sector labels by setting the color to Transparent
    c->setLabelStyle("", 8, Chart::Transparent);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: {value}kg ({percent}%)'");

    return c;
}


BaseChart *simplebar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {85, 156, 179.5, 211, 123};

    // The labels for the bar chart
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 250 x 250 pixels
    XYChart *c = new XYChart(250, 250);

    // Set the plotarea at (30, 20) and of size 200 x 200 pixels
    c->setPlotArea(30, 20, 200, 200);

    // Add a bar chart layer using the given data
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])));

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "", "title='{xLabel}: US${value}K'");

    return c;
}


BaseChart *colorbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {85, 156, 179.5, 211, 123};

    // The labels for the bar chart
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // The colors for the bar chart
    int colors[] = {0xb8bc9c, 0xa0bdc4, 0x999966, 0x333366, 0xc3c3e6};

    // Create a XYChart object of size 300 x 220 pixels. Use golden background color.
    // Use a 2 pixel 3D border.
    XYChart *c = new XYChart(300, 220, Chart::goldColor(), -1, 2);

    // Add a title box using 10 point Arial Bold font. Set the background color to
    // metallic blue (9999FF) Use a 1 pixel 3D border.
    c->addTitle("Daily Network Load", "arialbd.ttf", 10)->setBackground(
        Chart::metalColor(0x9999ff), -1, 1);

    // Set the plotarea at (40, 40) and of 240 x 150 pixels in size
    c->setPlotArea(40, 40, 240, 150);

    // Add a multi-color bar chart layer using the given data and colors. Use a 1
    // pixel 3D border for the bars.
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), IntArray(colors,
        sizeof(colors)/sizeof(colors[0])))->setBorderColor(-1, 1);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: {value} GBytes'");

    return c;
}


BaseChart *softlightbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {450, 560, 630, 800, 1100, 1350, 1600, 1950, 2300, 2700};

    // The labels for the bar chart
    const char *labels[] = {"1996", "1997", "1998", "1999", "2000", "2001", "2002",
        "2003", "2004", "2005"};

    // Create a XYChart object of size 600 x 360 pixels
    XYChart *c = new XYChart(600, 360);

    // Add a title to the chart using 18pts Times Bold Italic font
    c->addTitle("Annual Revenue for Star Tech", "timesbi.ttf", 18);

    // Set the plotarea at (60, 40) and of size 500 x 280 pixels. Use a vertical
    // gradient color from light blue (eeeeff) to deep blue (0000cc) as background.
    // Set border and grid lines to white (ffffff).
    c->setPlotArea(60, 40, 500, 280, c->linearGradientColor(60, 40, 60, 280,
        0xeeeeff, 0x0000cc), -1, 0xffffff, 0xffffff);

    // Add a multi-color bar chart layer using the supplied data. Use soft lighting
    // effect with light direction from left.
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), IntArray(0, 0)
        )->setBorderColor(Chart::Transparent, Chart::softLighting(Chart::Left));

    // Set x axis labels using the given labels
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Draw the ticks between label positions (instead of at label positions)
    c->xAxis()->setTickOffset(0.5);

    // Add a title to the y axis with 10pts Arial Bold font
    c->yAxis()->setTitle("USD (millions)", "arialbd.ttf", 10);

    // Set axis label style to 8pts Arial Bold
    c->xAxis()->setLabelStyle("arialbd.ttf", 8);
    c->yAxis()->setLabelStyle("arialbd.ttf", 8);

    // Set axis line width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Year {xLabel}: US$ {value}M'");

    return c;
}


BaseChart *glasslightbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {450, 560, 630, 800, 1100, 1350, 1600, 1950, 2300, 2700};

    // The labels for the bar chart
    const char *labels[] = {"1996", "1997", "1998", "1999", "2000", "2001", "2002",
        "2003", "2004", "2005"};

    // Create a XYChart object of size 600 x 360 pixels
    XYChart *c = new XYChart(600, 360);

    // Set the plotarea at (60, 40) and of size 480 x 280 pixels. Use a vertical
    // gradient color from light blue (eeeeff) to deep blue (0000cc) as background.
    // Set border and grid lines to white (ffffff).
    c->setPlotArea(60, 40, 480, 280, c->linearGradientColor(60, 40, 60, 280,
        0xeeeeff, 0x0000cc), -1, 0xffffff, 0xffffff);

    // Add a title to the chart using 18pts Times Bold Italic font
    c->addTitle("Annual Revenue for Star Tech", "timesbi.ttf", 18);

    // Add a multi-color bar chart layer using the supplied data. Use glass lighting
    // effect with light direction from the left.
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), IntArray(0, 0)
        )->setBorderColor(Chart::Transparent, Chart::glassEffect(Chart::NormalGlare,
        Chart::Left));

    // Set the x axis labels
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Show the same scale on the left and right y-axes
    c->syncYAxis();

    // Set the left y-axis and right y-axis title using 10pt Arial Bold font
    c->yAxis()->setTitle("USD (millions)", "arialbd.ttf", 10);
    c->yAxis2()->setTitle("USD (millions)", "arialbd.ttf", 10);

    // Set all axes to transparent
    c->xAxis()->setColors(Chart::Transparent);
    c->yAxis()->setColors(Chart::Transparent);
    c->yAxis2()->setColors(Chart::Transparent);

    // Set the label styles of all axes to 8pt Arial Bold font
    c->xAxis()->setLabelStyle("arialbd.ttf", 8);
    c->yAxis()->setLabelStyle("arialbd.ttf", 8);
    c->yAxis2()->setLabelStyle("arialbd.ttf", 8);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Year {xLabel}: US$ {value}M'");

    return c;
}


BaseChart *cylinderlightbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {450, 560, 630, 800, 1100, 1350, 1600, 1950, 2300, 2700};

    // The labels for the bar chart
    const char *labels[] = {"1996", "1997", "1998", "1999", "2000", "2001", "2002",
        "2003", "2004", "2005"};

    // Create a XYChart object of size 600 x 360 pixels
    XYChart *c = new XYChart(600, 360);

    // Add a title to the chart using 18pts Times Bold Italic font
    c->addTitle("Annual Revenue for Star Tech", "timesbi.ttf", 18);

    // Set the plotarea at (60, 40) and of size 480 x 280 pixels. Use a vertical
    // gradient color from light green (eeffee) to dark green (008800) as background.
    // Set border and grid lines to white (ffffff).
    c->setPlotArea(60, 40, 480, 280, c->linearGradientColor(60, 40, 60, 280,
        0xeeffee, 0x008800), -1, 0xffffff, 0xffffff);

    // Add a multi-color bar chart layer using the supplied data. Set cylinder bar
    // shape.
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), IntArray(0, 0)
        )->setBarShape(Chart::CircleShape);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Show the same scale on the left and right y-axes
    c->syncYAxis();

    // Set the left y-axis and right y-axis title using 10pt Arial Bold font
    c->yAxis()->setTitle("USD (millions)", "arialbd.ttf", 10);
    c->yAxis2()->setTitle("USD (millions)", "arialbd.ttf", 10);

    // Set all axes to transparent
    c->xAxis()->setColors(Chart::Transparent);
    c->yAxis()->setColors(Chart::Transparent);
    c->yAxis2()->setColors(Chart::Transparent);

    // Set the label styles of all axes to 8pt Arial Bold font
    c->xAxis()->setLabelStyle("arialbd.ttf", 8);
    c->yAxis()->setLabelStyle("arialbd.ttf", 8);
    c->yAxis2()->setLabelStyle("arialbd.ttf", 8);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Year {xLabel}: US$ {value}M'");

    return c;
}


BaseChart *threedbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {85, 156, 179.5, 211, 123};

    // The labels for the bar chart
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 300 x 280 pixels
    XYChart *c = new XYChart(300, 280);

    // Set the plotarea at (45, 30) and of size 200 x 200 pixels
    c->setPlotArea(45, 30, 200, 200);

    // Add a title to the chart
    c->addTitle("Weekly Server Load");

    // Add a title to the y axis
    c->yAxis()->setTitle("MBytes");

    // Add a title to the x axis
    c->xAxis()->setTitle("Work Week 25");

    // Add a bar chart layer with green (0x00ff00) bars using the given data
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), 0x00ff00)->set3D(
        );

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: {value} MBytes'");

    return c;
}


BaseChart *cylinderbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {85, 156, 179.5, 211, 123};

    // The labels for the bar chart
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 400 x 240 pixels.
    XYChart *c = new XYChart(400, 240);

    // Add a title to the chart using 14 pts Times Bold Italic font
    c->addTitle("Weekly Server Load", "timesbi.ttf", 14);

    // Set the plotarea at (45, 40) and of 300 x 160 pixels in size. Use alternating
    // light grey (f8f8f8) / white (ffffff) background.
    c->setPlotArea(45, 40, 300, 160, 0xf8f8f8, 0xffffff);

    // Add a multi-color bar chart layer
    BarLayer *layer = c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])),
        IntArray(0, 0));

    // Set layer to 3D with 10 pixels 3D depth
    layer->set3D(10);

    // Set bar shape to circular (cylinder)
    layer->setBarShape(Chart::CircleShape);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a title to the y axis
    c->yAxis()->setTitle("MBytes");

    // Add a title to the x axis
    c->xAxis()->setTitle("Work Week 25");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: {value} MBytes'");

    return c;
}


BaseChart *polygonbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {85, 156, 179.5, 211, 123, 176, 195};

    // The labels for the bar chart
    const char *labels[] = {"Square", "Star(8)", "Polygon(6)", "Cross", "Cross2",
        "Diamond", "Custom"};

    // Create a XYChart object of size 500 x 280 pixels.
    XYChart *c = new XYChart(500, 280);

    // Set the plotarea at (50, 40) with alternating light grey (f8f8f8) / white
    // (ffffff) background
    c->setPlotArea(50, 40, 400, 200, 0xf8f8f8, 0xffffff);

    // Add a title to the chart using 14 pts Arial Bold Italic font
    c->addTitle("    Bar Shape Demonstration", "arialbi.ttf", 14);

    // Add a multi-color bar chart layer
    BarLayer *layer = c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])),
        IntArray(0, 0));

    // Set layer to 3D with 10 pixels 3D depth
    layer->set3D(10);

    // Set bar shape to circular (cylinder)
    layer->setBarShape(Chart::CircleShape);

    // Set the first bar (index = 0) to square shape
    layer->setBarShape(Chart::SquareShape, 0, 0);

    // Set the second bar to 8-pointed star
    layer->setBarShape(Chart::StarShape(8), 0, 1);

    // Set the third bar to 6-sided polygon
    layer->setBarShape(Chart::PolygonShape(6), 0, 2);

    // Set the next 3 bars to cross shape, X shape and diamond shape
    layer->setBarShape(Chart::CrossShape(), 0, 3);
    layer->setBarShape(Chart::Cross2Shape(), 0, 4);
    layer->setBarShape(Chart::DiamondShape, 0, 5);

    // Set the last bar to a custom shape, specified as an array of (x, y) points in
    // normalized coordinates
    int pattern1[] = {-500, 0, 0, 500, 500, 0, 500, 1000, 0, 500, -500, 1000};
    layer->setBarShape(IntArray(pattern1, sizeof(pattern1)/sizeof(pattern1[0])), 0, 6
        );

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a title to the y axis
    c->yAxis()->setTitle("Frequency");

    // Add a title to the x axis
    c->xAxis()->setTitle("Shapes");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "", "title='{xLabel}: {value}'");

    return c;
}


BaseChart *stackedbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data0[] = {100, 125, 245, 147, 67};
    double data1[] = {85, 156, 179, 211, 123};
    double data2[] = {97, 87, 56, 267, 157};

    // The labels for the bar chart
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 500 x 320 pixels
    XYChart *c = new XYChart(500, 320);

    // Set the plotarea at (100, 40) and of size 280 x 240 pixels
    c->setPlotArea(100, 40, 280, 240);

    // Add a legend box at (400, 100)
    c->addLegend(400, 100);

    // Add a title to the chart using 14 points Times Bold Itatic font
    c->addTitle("Weekday Network Load", "timesbi.ttf", 14);

    // Add a title to the y axis. Draw the title upright (font angle = 0)
    c->yAxis()->setTitle("Average\nWorkload\n(MBytes\nPer Hour)")->setFontAngle(0);

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a stacked bar layer and set the layer 3D depth to 8 pixels
    BarLayer *layer = c->addBarLayer(Chart::Stack, 8);

    // Add the three data sets to the bar layer
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xff8080,
        "Server # 1");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x80ff80,
        "Server # 2");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0x8080ff,
        "Server # 3");

    // Enable bar label for the whole bar
    layer->setAggregateLabelStyle();

    // Enable bar label for each segment of the stacked bar
    layer->setDataLabelStyle();

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} on {xLabel}: {value} MBytes/hour'");

    return c;
}


BaseChart *percentbar(int img, const char **imageMap)
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
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} revenue on {xLabel}: US${value}K ({percent}%)'");

    return c;
}


BaseChart *multibar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data0[] = {100, 125, 245, 147, 67};
    double data1[] = {85, 156, 179, 211, 123};
    double data2[] = {97, 87, 56, 267, 157};
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 400 x 240 pixels
    XYChart *c = new XYChart(400, 240);

    // Add a title to the chart using 10 pt Arial font
    c->addTitle("         Average Weekday Network Load", "", 10);

    // Set the plot area at (50, 25) and of size 320 x 180. Use two alternative
    // background colors (0xffffc0 and 0xffffe0)
    c->setPlotArea(50, 25, 320, 180, 0xffffc0, 0xffffe0);

    // Add a legend box at (55, 18) using horizontal layout. Use 8 pt Arial font,
    // with transparent background
    c->addLegend(55, 18, false, "", 8)->setBackground(Chart::Transparent);

    // Add a title to the y-axis
    c->yAxis()->setTitle("Throughput (MBytes Per Hour)");

    // Reserve 20 pixels at the top of the y-axis for the legend box
    c->yAxis()->setTopMargin(20);

    // Set the x axis labels
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a multi-bar layer with 3 data sets and 3 pixels 3D depth
    BarLayer *layer = c->addBarLayer(Chart::Side, 3);
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xff8080,
        "Server #1");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x80ff80,
        "Server #2");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0x8080ff,
        "Server #3");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} on {xLabel}: {value} MBytes/hour'");

    return c;
}


BaseChart *softmultibar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data0[] = {100, 125, 245, 147, 67};
    double data1[] = {85, 156, 179, 211, 123};
    double data2[] = {97, 87, 56, 267, 157};
    const char *labels[] = {"Mon", "Tue", "Wed", "Thur", "Fri"};

    // Create a XYChart object of size 540 x 375 pixels
    XYChart *c = new XYChart(540, 375);

    // Add a title to the chart using 18 pts Times Bold Italic font
    c->addTitle("Average Weekly Network Load", "timesbi.ttf", 18);

    // Set the plotarea at (50, 55) and of 440 x 280 pixels in size. Use a vertical
    // gradient color from light red (ffdddd) to dark red (880000) as background. Set
    // border and grid lines to white (ffffff).
    c->setPlotArea(50, 55, 440, 280, c->linearGradientColor(0, 55, 0, 335, 0xffdddd,
        0x880000), -1, 0xffffff, 0xffffff);

    // Add a legend box at (50, 25) using horizontal layout. Use 10pts Arial Bold as
    // font, with transparent background.
    c->addLegend(50, 25, false, "arialbd.ttf", 10)->setBackground(Chart::Transparent)
        ;

    // Set the x axis labels
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Draw the ticks between label positions (instead of at label positions)
    c->xAxis()->setTickOffset(0.5);

    // Set axis label style to 8pts Arial Bold
    c->xAxis()->setLabelStyle("arialbd.ttf", 8);
    c->yAxis()->setLabelStyle("arialbd.ttf", 8);

    // Set axis line width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add axis title
    c->yAxis()->setTitle("Throughput (MBytes Per Hour)");

    // Add a multi-bar layer with 3 data sets and 4 pixels 3D depth
    BarLayer *layer = c->addBarLayer(Chart::Side, 4);
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xffff00,
        "Server #1");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x00ff00,
        "Server #2");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0x9999ff,
        "Server #3");

    // Set bar border to transparent. Use soft lighting effect with light direction
    // from top.
    layer->setBorderColor(Chart::Transparent, Chart::softLighting(Chart::Top));

    // Configure the bars within a group to touch each others (no gap)
    layer->setBarGap(0.2, Chart::TouchBar);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} on {xLabel}: {value} MBytes/hour'");

    return c;
}


BaseChart *glassmultibar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data0[] = {100, 125, 245, 147, 67};
    double data1[] = {85, 156, 179, 211, 123};
    double data2[] = {97, 87, 56, 267, 157};
    const char *labels[] = {"Mon", "Tue", "Wed", "Thur", "Fri"};

    // Create a XYChart object of size 540 x 375 pixels
    XYChart *c = new XYChart(540, 375);

    // Add a title to the chart using 18 pts Times Bold Italic font
    c->addTitle("Average Weekly Network Load", "timesbi.ttf", 18);

    // Set the plotarea at (50, 55) and of 440 x 280 pixels in size. Use a vertical
    // gradient color from light blue (f9f9ff) to blue (6666ff) as background. Set
    // border and grid lines to white (ffffff).
    c->setPlotArea(50, 55, 440, 280, c->linearGradientColor(0, 55, 0, 335, 0xf9f9ff,
        0x6666ff), -1, 0xffffff, 0xffffff);

    // Add a legend box at (50, 28) using horizontal layout. Use 10pts Arial Bold as
    // font, with transparent background.
    c->addLegend(50, 28, false, "arialbd.ttf", 10)->setBackground(Chart::Transparent)
        ;

    // Set the x axis labels
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Draw the ticks between label positions (instead of at label positions)
    c->xAxis()->setTickOffset(0.5);

    // Set axis label style to 8pts Arial Bold
    c->xAxis()->setLabelStyle("arialbd.ttf", 8);
    c->yAxis()->setLabelStyle("arialbd.ttf", 8);

    // Set axis line width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add axis title
    c->yAxis()->setTitle("Throughput (MBytes Per Hour)");

    // Add a multi-bar layer with 3 data sets
    BarLayer *layer = c->addBarLayer(Chart::Side);
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xff0000,
        "Server #1");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x00ff00,
        "Server #2");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0xff8800,
        "Server #3");

    // Set bar border to transparent. Use glass lighting effect with light direction
    // from left.
    layer->setBorderColor(Chart::Transparent, Chart::glassEffect(Chart::NormalGlare,
        Chart::Left));

    // Configure the bars within a group to touch each others (no gap)
    layer->setBarGap(0.2, Chart::TouchBar);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} on {xLabel}: {value} MBytes/hour'");

    return c;
}


BaseChart *multicylinder(int img, const char **imageMap)
{
    // Data for the chart
    double data0[] = {100, 125, 245, 147, 67, 96, 160, 145, 97, 167, 220, 125};
    double data1[] = {85, 156, 179, 211, 123, 225, 127, 99, 111, 260, 175, 156};
    double data2[] = {97, 87, 56, 267, 157, 157, 67, 156, 77, 87, 197, 87};
    const char *labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
        "Sept", "Oct", "Nov", "Dec"};

    // Create a XYChart object of size 560 x 280 pixels.
    XYChart *c = new XYChart(560, 280);

    // Add a title to the chart using 14 pts Arial Bold Italic font
    c->addTitle("     Average Weekly Network Load", "arialbi.ttf", 14);

    // Set the plotarea at (50, 50) and of 500 x 200 pixels in size. Use alternating
    // light grey (f8f8f8) / white (ffffff) background. Set border to transparent and
    // use grey (CCCCCC) dotted lines as horizontal and vertical grid lines
    c->setPlotArea(50, 50, 500, 200, 0xffffff, 0xf8f8f8, Chart::Transparent,
        c->dashLineColor(0xcccccc, Chart::DotLine), c->dashLineColor(0xcccccc,
        Chart::DotLine));

    // Add a legend box at (50, 22) using horizontal layout. Use 10 pt Arial Bold
    // Italic font, with transparent background
    c->addLegend(50, 22, false, "arialbi.ttf", 10)->setBackground(Chart::Transparent)
        ;

    // Set the x axis labels
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Draw the ticks between label positions (instead of at label positions)
    c->xAxis()->setTickOffset(0.5);

    // Add axis title
    c->yAxis()->setTitle("Throughput (MBytes Per Hour)");

    // Set axis line width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add a multi-bar layer with 3 data sets
    BarLayer *layer = c->addBarLayer(Chart::Side);
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xff0000,
        "Server #1");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x00ff00,
        "Server #2");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0x0000ff,
        "Server #3");

    // Set bar shape to circular (cylinder)
    layer->setBarShape(Chart::CircleShape);

    // Configure the bars within a group to touch each others (no gap)
    layer->setBarGap(0.2, Chart::TouchBar);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} on {xLabel}: {value} MBytes/hour'");

    return c;
}


BaseChart *multishapebar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data0[] = {100, 125, 245, 147};
    double data1[] = {85, 156, 179, 211};
    double data2[] = {97, 87, 56, 267};
    const char *labels[] = {"1st Quarter", "2nd Quarter", "3rd Quarter",
        "4th Quarter"};

    // Create a XYChart object of size 600 x 350 pixels
    XYChart *c = new XYChart(600, 350);

    // Add a title to the chart using 14 pts Arial Bold Italic font
    c->addTitle("Annual Product Revenue", "arialbi.ttf", 14);

    // Set the plot area at (50, 60) and of size 500 x 240. Use two alternative
    // background colors (f8f8f8 and ffffff)
    c->setPlotArea(50, 60, 500, 240, 0xf8f8f8, 0xffffff);

    // Add a legend box at (55, 22) using horizontal layout, with transparent
    // background
    c->addLegend(55, 22, false)->setBackground(Chart::Transparent);

    // Set the x axis labels
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Draw the ticks between label positions (instead of at label positions)
    c->xAxis()->setTickOffset(0.5);

    // Add a multi-bar layer with 3 data sets and 9 pixels 3D depth
    BarLayer *layer = c->addBarLayer(Chart::Side, 9);
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), -1,
        "Product A");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), -1,
        "Product B");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), -1,
        "Product C");

    // Set data set 1 to use a bar shape of a 6-pointed star
    layer->setBarShape(Chart::StarShape(6), 0);

    // Set data set 2 to use a bar shapre of a 6-sided polygon
    layer->setBarShape(Chart::PolygonShape(6), 1);

    // Set data set 3 to use an X bar shape
    layer->setBarShape(Chart::Cross2Shape(), 2);

    // Add a title to the y-axis
    c->yAxis()->setTitle("Revenue (USD in millions)");

    // Add a title to the x axis
    c->xAxis()->setTitle("Year 2005");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName}: {xLabel} Revenue = {value} millions'");

    return c;
}


BaseChart *overlapbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data0[] = {100, 125, 156, 147, 87, 124, 178, 109, 140, 106, 192, 122};
    double data1[] = {122, 156, 179, 211, 198, 177, 160, 220, 190, 188, 220, 270};
    double data2[] = {167, 190, 213, 267, 250, 320, 212, 199, 245, 267, 240, 310};
    const char *labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
        "Sept", "Oct", "Nov", "Dec"};

    // Create a XYChart object of size 580 x 280 pixels
    XYChart *c = new XYChart(580, 280);

    // Add a title to the chart using 14 pts Arial Bold Italic font
    c->addTitle("Product Revenue For Last 3 Years", "arialbi.ttf", 14);

    // Set the plot area at (50, 50) and of size 500 x 200. Use two alternative
    // background colors (f8f8f8 and ffffff)
    c->setPlotArea(50, 50, 500, 200, 0xf8f8f8, 0xffffff);

    // Add a legend box at (50, 25) using horizontal layout. Use 8pts Arial as font,
    // with transparent background.
    c->addLegend(50, 25, false, "arial.ttf", 8)->setBackground(Chart::Transparent);

    // Set the x axis labels
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Draw the ticks between label positions (instead of at label positions)
    c->xAxis()->setTickOffset(0.5);

    // Add a multi-bar layer with 3 data sets
    BarLayer *layer = c->addBarLayer(Chart::Side);
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xff8080,
        "Year 2003");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x80ff80,
        "Year 2004");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0x8080ff,
        "Year 2005");

    // Set 50% overlap between bars
    layer->setOverlapRatio(0.5);

    // Add a title to the y-axis
    c->yAxis()->setTitle("Revenue (USD in millions)");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel} Revenue on {dataSetName}: {value} millions'");

    return c;
}


BaseChart *multistackbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data0[] = {44, 55, 100};
    double data1[] = {97, 87, 167};
    double data2[] = {156, 78, 147};
    double data3[] = {125, 118, 211};

    // The labels for the bar chart. The labels contains embedded images as icons.
    const char *labels[] = {"<*img=service.png*><*br*>Service",
        "<*img=software.png*><*br*>Software", "<*img=computer.png*><*br*>Hardware"};

    // Create a XYChart object of size 600 x 350 pixels, using 0xe0e0ff as the
    // background color, 0xccccff as the border color, with 1 pixel 3D border effect.
    XYChart *c = new XYChart(600, 350, 0xe0e0ff, 0xccccff, 1);

    // Add a title to the chart using 14 points Times Bold Itatic font and light blue
    // (0x9999ff) as the background color
    c->addTitle("Business Results 2001 vs 2002", "timesbi.ttf", 14)->setBackground(
        0x9999ff);

    // Set the plotarea at (60, 45) and of size 500 x 210 pixels, using white
    // (0xffffff) as the background
    c->setPlotArea(60, 45, 500, 210, 0xffffff);

    // Swap the x and y axes to create a horizontal bar chart
    c->swapXY();

    // Add a title to the y axis using 11 pt Times Bold Italic as font
    c->yAxis()->setTitle("Revenue (millions)", "timesbi.ttf", 11);

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Disable x-axis ticks by setting the tick length to 0
    c->xAxis()->setTickLength(0);

    // Add a stacked bar layer to the chart
    BarLayer *layer = c->addBarLayer(Chart::Stack);

    // Add the first two data sets to the chart as a stacked bar group
    layer->addDataGroup("2001");
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xaaaaff,
        "Local");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x6666ff,
        "International");

    // Add the remaining data sets to the chart as another stacked bar group
    layer->addDataGroup("2002");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0xffaaaa,
        "Local");
    layer->addDataSet(DoubleArray(data3, sizeof(data3)/sizeof(data3[0])), 0xff6666,
        "International");

    // Set the sub-bar gap to 0, so there is no gap between stacked bars with a group
    layer->setBarGap(0.2, 0);

    // Set the bar border to transparent
    layer->setBorderColor(Chart::Transparent);

    // Set the aggregate label format
    layer->setAggregateLabelFormat("Year {dataGroupName}\n{value} millions");

    // Set the aggregate label font to 8 point Arial Bold Italic
    layer->setAggregateLabelStyle("arialbi.ttf", 8);

    // Reverse 20% space at the right during auto-scaling to allow space for the
    // aggregate bar labels
    c->yAxis()->setAutoScale(0.2);

    // Add a legend box at (310, 300) using TopCenter alignment, with 2 column grid
    // layout, and use 8 pts Arial Bold Italic as font
    LegendBox *legendBox = c->addLegend2(310, 300, 2, "arialbi.ttf", 8);
    legendBox->setAlignment(Chart::TopCenter);

    // Set the format of the text displayed in the legend box
    legendBox->setText("Year {dataGroupName} {dataSetName} Revenue");

    // Set the background and border of the legend box to transparent
    legendBox->setBackground(Chart::Transparent, Chart::Transparent);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Year {dataGroupName} {dataSetName} {xLabel} Revenue: {value} "
        "millions'");

    return c;
}


BaseChart *depthbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data0[] = {100, 125, 245, 147, 67};
    double data1[] = {85, 156, 179, 211, 123};
    double data2[] = {97, 87, 56, 267, 157};

    // The labels for the bar chart
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 500 x 320 pixels
    XYChart *c = new XYChart(500, 320);

    // Set the plotarea at (100, 40) and of size 280 x 240 pixels
    c->setPlotArea(100, 40, 280, 240);

    // Add a legend box at (405, 100)
    c->addLegend(405, 100);

    // Add a title to the chart
    c->addTitle("Weekday Network Load");

    // Add a title to the y axis. Draw the title upright (font angle = 0)
    c->yAxis()->setTitle("Average\nWorkload\n(MBytes\nPer Hour)")->setFontAngle(0);

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add three bar layers, each representing one data set. The bars are drawn in
    // semi-transparent colors.
    c->addBarLayer(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0x808080FF,
        "Server # 1", 5);
    c->addBarLayer(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x80FF0000,
        "Server # 2", 5);
    c->addBarLayer(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0x8000FF00,
        "Server # 3", 5);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} on {xLabel}: {value} MBytes/hour'");

    return c;
}


BaseChart *posnegbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {-6.3, 2.3, 0.7, -3.4, 2.2, -2.9, -0.1, -0.1, 3.3, 6.2, 4.3, 1.6}
        ;

    // The labels for the bar chart
    const char *labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"};

    // Create a XYChart object of size 500 x 320 pixels
    XYChart *c = new XYChart(500, 320);

    // Add a title to the chart using Arial Bold Italic font
    c->addTitle("Productivity Change - Year 2005", "arialbi.ttf");

    // Set the plotarea at (50, 30) and of size 400 x 250 pixels
    c->setPlotArea(50, 30, 400, 250);

    // Add a bar layer to the chart using the Overlay data combine method
    BarLayer *layer = c->addBarLayer(Chart::Overlay);

    // Select positive data and add it as data set with blue (6666ff) color
    layer->addDataSet(ArrayMath(DoubleArray(data, sizeof(data)/sizeof(data[0]))
        ).selectGEZ(DoubleArray(), Chart::NoValue), 0x6666ff);

    // Select negative data and add it as data set with orange (ff6600) color
    layer->addDataSet(ArrayMath(DoubleArray(data, sizeof(data)/sizeof(data[0]))
        ).selectLTZ(DoubleArray(), Chart::NoValue), 0xff6600);

    // Add labels to the top of the bar using 8 pt Arial Bold font. The font color is
    // configured to be red (0xcc3300) below zero, and blue (0x3333ff) above zero.
    layer->setAggregateLabelStyle("arialbd.ttf", 8, layer->yZoneColor(0, 0xcc3300,
        0x3333ff));

    // Set the labels on the x axis and use Arial Bold as the label font
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        )->setFontStyle("arialbd.ttf");

    // Draw the y axis on the right of the plot area
    c->setYAxisOnRight(true);

    // Use Arial Bold as the y axis label font
    c->yAxis()->setLabelStyle("arialbd.ttf");

    // Add a title to the y axis
    c->yAxis()->setTitle("Percentage");

    // Add a light blue (0xccccff) zone for positive part of the plot area
    c->yAxis()->addZone(0, 9999, 0xccccff);

    // Add a pink (0xffffcc) zone for negative part of the plot area
    c->yAxis()->addZone(-9999, 0, 0xffcccc);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "", "title='{xLabel}: {value}%'");

    return c;
}


BaseChart *hbar(int img, const char **imageMap)
{
    // The data for the bar chart
    double data[] = {3.9, 8.1, 10.9, 14.2, 18.1, 19.0, 21.2, 23.2, 25.7, 36};

    // The labels for the bar chart
    const char *labels[] = {"Bastic Group", "Simpa", "YG Super", "CID", "Giga Tech",
        "Indo Digital", "Supreme", "Electech", "THP Thunder", "Flash Light"};

    // Create a XYChart object of size 600 x 250 pixels
    XYChart *c = new XYChart(600, 250);

    // Add a title to the chart using Arial Bold Italic font
    c->addTitle("Revenue Estimation - Year 2002", "arialbi.ttf");

    // Set the plotarea at (100, 30) and of size 400 x 200 pixels. Set the plotarea
    // border, background and grid lines to Transparent
    c->setPlotArea(100, 30, 400, 200, Chart::Transparent, Chart::Transparent,
        Chart::Transparent, Chart::Transparent, Chart::Transparent);

    // Add a bar chart layer using the given data. Use a gradient color for the bars,
    // where the gradient is from dark green (0x008000) to white (0xffffff)
    BarLayer *layer = c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])),
        c->gradientColor(100, 0, 500, 0, 0x008000, 0xffffff));

    // Swap the axis so that the bars are drawn horizontally
    c->swapXY(true);

    // Set the bar gap to 10%
    layer->setBarGap(0.1);

    // Use the format "US$ xxx millions" as the bar label
    layer->setAggregateLabelFormat("US$ {value} millions");

    // Set the bar label font to 10 pts Times Bold Italic/dark red (0x663300)
    layer->setAggregateLabelStyle("timesbi.ttf", 10, 0x663300);

    // Set the labels on the x axis
    TextBox *textbox = c->xAxis()->setLabels(StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Set the x axis label font to 10pt Arial Bold Italic
    textbox->setFontStyle("arialbi.ttf");
    textbox->setFontSize(10);

    // Set the x axis to Transparent, with labels in dark red (0x663300)
    c->xAxis()->setColors(Chart::Transparent, 0x663300);

    // Set the y axis and labels to Transparent
    c->yAxis()->setColors(Chart::Transparent, Chart::Transparent);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: US${value} millions'");

    return c;
}


BaseChart *dualhbar(int img, const char **imageMap)
{
    // The age groups
    const char *labels[] = {"0 - 4", "5 - 9", "10 - 14", "15 - 19", "20 - 24",
        "24 - 29", "30 - 34", "35 - 39", "40 - 44", "44 - 49", "50 - 54", "55 - 59",
        "60 - 64", "65 - 69", "70 - 74", "75 - 79", "80+"};

    // The male population (in thousands)
    double male[] = {215, 238, 225, 236, 235, 260, 286, 340, 363, 305, 259, 164, 135,
        127, 102, 68, 66};

    // The female population (in thousands)
    double female[] = {194, 203, 201, 220, 228, 271, 339, 401, 384, 304, 236, 137,
        116, 122, 112, 85, 110};


    //=============================================================
    //    Draw the right bar chart
    //=============================================================

    // Create a XYChart object of size 320 x 300 pixels
    XYChart *c = new XYChart(320, 300);

    // Set the plotarea at (50, 0) and of size 250 x 255 pixels. Use pink (0xffdddd)
    // as the background.
    c->setPlotArea(50, 0, 250, 255, 0xffdddd);

    // Add a custom text label at the top right corner of the right bar chart
    c->addText(300, 0, "Female", "timesbi.ttf", 12, 0xa07070)->setAlignment(
        Chart::TopRight);

    // Add the pink (0xf0c0c0) bar chart layer using the female data
    BarLayer *femaleLayer = c->addBarLayer(DoubleArray(female,
        sizeof(female)/sizeof(female[0])), 0xf0c0c0, "Female");

    // Swap the axis so that the bars are drawn horizontally
    c->swapXY(true);

    // Set the bar to touch each others
    femaleLayer->setBarGap(Chart::TouchBar);

    // Set the border style of the bars to 1 pixel 3D border
    femaleLayer->setBorderColor(-1, 1);

    // Add a Transparent line layer to the chart using the male data. As it is
    // Transparent, only the female bar chart can be seen. We need to put both male
    // and female data in both left and right charts, because we want auto-scaling to
    // produce the same scale for both chart.
    c->addLineLayer(DoubleArray(male, sizeof(male)/sizeof(male[0])),
        Chart::Transparent);

    // Set the y axis label font to Arial Bold
    c->yAxis()->setLabelStyle("arialbd.ttf");

    // Set the labels between the two bar charts, which can be considered as the
    // x-axis labels for the right chart
    TextBox *tb = c->xAxis()->setLabels(StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Use a fix width of 50 for the labels (height = automatic) with center
    // alignment
    tb->setSize(50, 0);
    tb->setAlignment(Chart::Center);

    // Set the label font to Arial Bold
    tb->setFontStyle("arialbd.ttf");

    // Disable ticks on the x-axis by setting the tick length to 0
    c->xAxis()->setTickLength(0);

    //=============================================================
    //    Draw the left bar chart
    //=============================================================

    // Create a XYChart object of size 280 x 300 pixels with a transparent
    // background.
    XYChart *c2 = new XYChart(280, 300, Chart::Transparent);

    // Set the plotarea at (20, 0) and of size 250 x 255 pixels. Use pale blue
    // (0xddddff) as the background.
    c2->setPlotArea(20, 0, 250, 255, 0xddddff);

    // Add a custom text label at the top left corner of the left bar chart
    c2->addText(20, 0, "Male", "timesbi.ttf", 12, 0x7070a0);

    // Add the pale blue (0xaaaaff) bar chart layer using the male data
    BarLayer *maleLayer = c2->addBarLayer(DoubleArray(male,
        sizeof(male)/sizeof(male[0])), 0xaaaaff, "Male");

    // Swap the axis so that the bars are drawn horizontally
    c2->swapXY(true);

    // Reverse the direction of the y-axis so it runs from right to left
    c2->yAxis()->setReverse();

    // Set the bar to touch each others
    maleLayer->setBarGap(Chart::TouchBar);

    // Set the border style of the bars to 1 pixel 3D border
    maleLayer->setBorderColor(-1, 1);

    // Add a Transparent line layer to the chart using the female data. As it is
    // Transparent, only the male bar chart can be seen. We need to put both male and
    // female data in both left and right charts, because we want auto-scaling to
    // produce the same scale for both chart.
    c2->addLineLayer(DoubleArray(female, sizeof(female)/sizeof(female[0])),
        Chart::Transparent);

    // Set the y axis label font to Arial Bold
    c2->yAxis()->setLabelStyle("arialbd.ttf");

    // Set the x-axis labels for tool tip purposes.
    c2->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Hide the x-axis labels by setting them to Transparent. We only need to display
    // the x-axis labels for the right chart.
    c2->xAxis()->setColors(0x000000, Chart::Transparent, -1, Chart::Transparent);

    //=============================================================
    //    Use a MultiChart to contain both bar charts
    //=============================================================

    // Create a MultiChart object of size 590 x 320 pixels.
    MultiChart *m = new MultiChart(590, 320);

    // Add a title to the chart using Arial Bold Italic font
    m->addTitle("Demographics Hong Kong Year 2002", "arialbi.ttf");

    // Add another title at the bottom using Arial Bold Italic font
    m->addTitle(Chart::Bottom, "Population (in thousands)", "arialbi.ttf", 10);

    // Put the right chatr at (270, 25)
    m->addChart(270, 25, c);

    // Put the left char at (0, 25)
    m->addChart(0, 25, c2);

    // output the chart
    m->makeChart();

    //include tool tip for the chart
    *imageMap = m->getHTMLImageMap("clickable", "",
        "title='{dataSetName} (Age {xLabel}): Population {value}K'");

    //free up resources
    delete c;
    delete c2;

    return m;
}


BaseChart *pareto(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {40, 15, 7, 5, 2};

    // The labels for the chart
    const char *labels[] = {"Hard Disk", "PCB", "Printer", "CDROM", "Keyboard"};

    // Create a XYChart object of size 400 x 225 pixels. Use golden background color,
    // with a 2 pixel 3D border.
    XYChart *c = new XYChart(400, 225, Chart::goldColor(), -1, 2);

    // Add a title box using Arial Bold/11 pt font. Set the background color to
    // metallic blue (9999FF). Use a 1 pixel 3D border.
    c->addTitle("Hardware Defects", "arialbd.ttf", 11)->setBackground(
        Chart::metalColor(0x9999ff), -1, 1);

    // Set the plotarea at (50, 40) and of 300 x 150 pixels in size, with a silver
    // background color.
    c->setPlotArea(50, 40, 300, 150, Chart::silverColor());

    // Add a line layer for the pareto line
    LineLayer *lineLayer = c->addLineLayer();

    // Compute the pareto line by accumulating the data
    ArrayMath lineData = ArrayMath(DoubleArray(data, sizeof(data)/sizeof(data[0])));
    lineData.acc();

    // Set a scaling factor such as the maximum point of the line is scaled to 100
    double scaleFactor = 100 / lineData.maxValue();

    // Add the pareto line using the scaled data. Use deep blue (0x80) as the line
    // color, with light blue (0x9999ff) diamond symbols
    lineLayer->addDataSet(lineData.mul(scaleFactor), 0x000080)->setDataSymbol(
        Chart::DiamondSymbol, 9, 0x9999ff);

    // Set the line width to 2 pixel
    lineLayer->setLineWidth(2);

    // Tool tip for the line layer
    lineLayer->setHTMLImageMap("", "", "title='Top {={x}+1} items: {value|2}%'");

    // Add a multi-color bar layer using the given data.
    BarLayer *barLayer = c->addBarLayer(DoubleArray(data,
        sizeof(data)/sizeof(data[0])), IntArray(0, 0));

    // Bind the layer to the secondary (right) y-axis.
    barLayer->setUseYAxis2();

    // Set soft lighting for the bars with light direction from the right
    barLayer->setBorderColor(Chart::Transparent, Chart::softLighting(Chart::Right));


    // Tool tip for the bar layer
    barLayer->setHTMLImageMap("", "", "title='{xLabel}: {value} pieces'");

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Set the primary y-axis scale as 0 - 100 with a tick every 20 units
    c->yAxis()->setLinearScale(0, 100, 20);

    // Set the label format of the y-axis label to include a percentage sign
    c->yAxis()->setLabelFormat("{value}%");

    // Add a title to the secondary y-axis
    c->yAxis2()->setTitle("Frequency");

    // Set the secondary y-axis label foramt to show no decimal point
    c->yAxis2()->setLabelFormat("{value|0}");

    // Set the relationship between the two y-axes, which only differ by a scaling
    // factor
    c->syncYAxis(1 / scaleFactor);

    // Output the chart
    c->makeChart();

    // Include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable");

    return c;
}


BaseChart *gapbar(int img, const char **imageMap)
{
    char buffer[256];

    double bargap = img * 0.25 - 0.25;

    // The data for the bar chart
    double data[] = {100, 125, 245, 147, 67};

    // The labels for the bar chart
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 150 x 150 pixels
    XYChart *c = new XYChart(150, 150);

    // Set the plotarea at (27, 20) and of size 120 x 100 pixels
    c->setPlotArea(27, 20, 120, 100);

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    if (bargap >= 0) {
        // Add a title to display to bar gap using 8 pts Arial font
        sprintf(buffer, "      Bar Gap = %g", bargap);
        c->addTitle(buffer, "arial.ttf", 8);
    } else {
        // Use negative value to mean TouchBar
        c->addTitle("      Bar Gap = TouchBar", "arial.ttf", 8);
        bargap = Chart::TouchBar;
    }

    // Add a bar chart layer using the given data and set the bar gap
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])))->setBarGap(bargap
        );

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Production on {xLabel}: {value} kg'");

    return c;
}


BaseChart *simpleline(int img, const char **imageMap)
{
    // The data for the line chart
    double data[] = {30, 28, 40, 55, 75, 68, 54, 60, 50, 62, 75, 65, 75, 91, 60, 55,
        53, 35, 50, 66, 56, 48, 52, 65, 62};

    // The labels for the line chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 250 x 250 pixels
    XYChart *c = new XYChart(250, 250);

    // Set the plotarea at (30, 20) and of size 200 x 200 pixels
    c->setPlotArea(30, 20, 200, 200);

    // Add a line chart layer using the given data
    c->addLineLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])));

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis.
    c->xAxis()->setLabelStep(3);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Hour {xLabel}: Traffic {value} GBytes'");

    return c;
}


BaseChart *compactline(int img, const char **imageMap)
{
    //
    //    We use a random number generator to simulate the data from 9:30am to 4:30pm
    //    with one data point every 4 minutes. The total number of points during that
    //    period is 106.  (7 hours x 15 points/hour + 1)
    //
    int noOfPoints = 106;

    // Assume we have not reached the end of the day yet, and only 85 points are
    // available. Create a random table object of 1 col x 85 rows, using 9 as seed.
    RanTable *rantable = new RanTable(9, 1, 85);

    // Set the 1st column to start with 1800 and with random delta from -5 to 5.
    rantable->setCol(0, 1800, -5, 5);

    // Get the data as the 1st column of the random table
    DoubleArray data = rantable->getCol(0);

    // The x-axis labels for the chart
    const char *labels[] = {"-", "10am", "-", " ", "-", "12am", "-", " ", "-", "2pm",
        "-", " ", "-", "4pm", "-"};

    //
    //    Now we obtain the data into arrays, we can start to draw the chart using
    //    ChartDirector
    //

    // Create a XYChart object of size 180 x 180 pixels with a blue background
    // (0x9c9cce)
    XYChart *c = new XYChart(180, 180, 0x9c9cce);

    // Add titles to the top and bottom of the chart using 7.5pt Arial font. The text
    // is white 0xffffff on a deep blue 0x31319C background.
    c->addTitle(Chart::Top, "STAR TECH INDEX  2003-01-28", "arial.ttf", 7.5,
        0xffffff, 0x31319c);
    c->addTitle(Chart::Bottom, "LATEST  STI:1809.41 (+14.51)", "arial.ttf", 7.5,
        0xffffff, 0x31319c);

    // Set the plotarea at (31, 21) and of size 145 x 124 pixels, with a pale yellow
    // (0xffffc8) background.
    c->setPlotArea(31, 21, 145, 124, 0xffffc8);

    // Add custom text at (176, 21) (top right corner of plotarea) using 11pt Times
    // Bold Italic font/red (0xc09090) color
    c->addText(176, 21, "Chart Demo", "timesbi.ttf", 11, 0xc09090)->setAlignment(
        Chart::TopRight);

    // Use 7.5 pts Arial as the y axis label font
    c->yAxis()->setLabelStyle("", 7.5);

    // Set the labels on the x axis by spreading the labels evenly between the first
    // point (index = 0) and the last point (index = noOfPoints - 1)
    c->xAxis()->setLinearScale(0, noOfPoints - 1, StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Use 7.5 pts Arial as the x axis label font
    c->xAxis()->setLabelStyle("", 7.5);

    // Add a deep blue (0x000080) line layer to the chart
    c->addLineLayer(data, 0x000080);

    // output the chart
    c->makeChart();

    // Include tool tip for the chart. The chart starts at 9:30am and each point
    // spans 240 seconds, so we can compute the time as {x}*240+9.5*3600.
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{={x}*240+9.5*3600|h:nna}: {value|2}'");

    //free up resources
    delete rantable;

    return c;
}


BaseChart *threedline(int img, const char **imageMap)
{
    // The data for the line chart
    double data[] = {30, 28, 40, 55, 75, 68, 54, 60, 50, 62, 75, 65, 75, 91, 60, 55,
        53, 35, 50, 66, 56, 48, 52, 65, 62};

    // The labels for the line chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 300 x 280 pixels
    XYChart *c = new XYChart(300, 280);

    // Set the plotarea at (45, 30) and of size 200 x 200 pixels
    c->setPlotArea(45, 30, 200, 200);

    // Add a title to the chart using 12 pts Arial Bold Italic font
    c->addTitle("Daily Server Utilization", "arialbi.ttf", 12);

    // Add a title to the y axis
    c->yAxis()->setTitle("MBytes");

    // Add a title to the x axis
    c->xAxis()->setTitle("June 12, 2001");

    // Add a blue (0x6666ff) 3D line chart layer using the give data
    c->addLineLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), 0x6666ff
        )->set3D();

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis.
    c->xAxis()->setLabelStep(3);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Hour {xLabel}: {value} MBytes'");

    return c;
}


BaseChart *multiline(int img, const char **imageMap)
{
    // The data for the line chart
    double data0[] = {42, 49, 33, 38, 51, 46, 29, 41, 44, 57, 59, 52, 37, 34, 51, 56,
        56, 60, 70, 76, 63, 67, 75, 64, 51};
    double data1[] = {50, 55, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67,
        58, 59, 73, 77, 84, 82, 80, 84, 98};
    double data2[] = {36, 28, 25, 33, 38, 20, 22, 30, 25, 33, 30, 24, 28, 15, 21, 26,
        46, 42, 48, 45, 43, 52, 64, 60, 70};

    // The labels for the line chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create an XYChart object of size 600 x 300 pixels, with a light blue (EEEEFF)
    // background, black border, 1 pxiel 3D border effect and rounded corners
    XYChart *c = new XYChart(600, 300, 0xeeeeff, 0x000000, 1);
    c->setRoundedFrame();

    // Set the plotarea at (55, 58) and of size 520 x 195 pixels, with white
    // background. Turn on both horizontal and vertical grid lines with light grey
    // color (0xcccccc)
    c->setPlotArea(55, 58, 520, 195, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);

    // Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 9
    // pts Arial Bold font. Set the background and border color to Transparent.
    c->addLegend(50, 30, false, "arialbd.ttf", 9)->setBackground(Chart::Transparent);

    // Add a title box to the chart using 15 pts Times Bold Italic font, on a light
    // blue (CCCCFF) background with glass effect. white (0xffffff) on a dark red
    // (0x800000) background, with a 1 pixel 3D border.
    c->addTitle("Application Server Throughput", "timesbi.ttf", 15)->setBackground(
        0xccccff, 0x000000, Chart::glassEffect());

    // Add a title to the y axis
    c->yAxis()->setTitle("MBytes per hour");

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis.
    c->xAxis()->setLabelStep(3);

    // Add a title to the x axis
    c->xAxis()->setTitle("Jun 12, 2006");

    // Add a line layer to the chart
    LineLayer *layer = c->addLineLayer();

    // Set the default line width to 2 pixels
    layer->setLineWidth(2);

    // Add the three data sets to the line layer. For demo purpose, we use a dash
    // line color for the last line
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xff0000,
        "Server #1");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x008800,
        "Server #2");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])),
        c->dashLineColor(0x3333ff, Chart::DashLine), "Server #3");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{dataSetName}] Hour {xLabel}: {value} MBytes'");

    return c;
}


BaseChart *symbolline(int img, const char **imageMap)
{
    // The data for the line chart
    double data0[] = {60.2, 51.7, 81.3, 48.6, 56.2, 68.9, 52.8};
    double data1[] = {30.0, 32.7, 33.9, 29.5, 32.2, 28.4, 29.8};
    const char *labels[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    // Create a XYChart object of size 300 x 180 pixels, with a pale yellow
    // (0xffffc0) background, a black border, and 1 pixel 3D border effect.
    XYChart *c = new XYChart(300, 180, 0xffffc0, 0x000000, 1);

    // Set the plotarea at (45, 35) and of size 240 x 120 pixels, with white
    // background. Turn on both horizontal and vertical grid lines with light grey
    // color (0xc0c0c0)
    c->setPlotArea(45, 35, 240, 120, 0xffffff, -1, -1, 0xc0c0c0, -1);

    // Add a legend box at (45, 12) (top of the chart) using horizontal layout and 8
    // pts Arial font Set the background and border color to Transparent.
    c->addLegend(45, 12, false, "", 8)->setBackground(Chart::Transparent);

    // Add a title to the chart using 9 pts Arial Bold/white font. Use a 1 x 2 bitmap
    // pattern as the background.
    int pattern1[] = {0x004000, 0x008000};
    c->addTitle("Server Load (Jun 01 - Jun 07)", "arialbd.ttf", 9, 0xffffff
        )->setBackground(c->patternColor(IntArray(pattern1,
        sizeof(pattern1)/sizeof(pattern1[0])), 2));

    // Set the y axis label format to nn%
    c->yAxis()->setLabelFormat("{value}%");

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a line layer to the chart
    LineLayer *layer = c->addLineLayer();

    // Add the first line. Plot the points with a 7 pixel square symbol
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xcf4040,
        "Peak")->setDataSymbol(Chart::SquareSymbol, 7);

    // Add the second line. Plot the points with a 9 pixel dismond symbol
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x40cf40,
        "Average")->setDataSymbol(Chart::DiamondSymbol, 9);

    // Enable data label on the data points. Set the label format to nn%.
    layer->setDataLabelFormat("{value|0}%");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: {dataSetName} {value}%'");

    return c;
}


BaseChart *symbolline2(int img, const char **imageMap)
{
    // The data for the line chart
    double data0[] = {42, 49, 33, 38, 64, 56, 29, 41, 44, 57, 59, 42};
    double data1[] = {65, 75, 47, 34, 42, 49, 73, 62, 90, 69, 66, 78};
    double data2[] = {36, 28, 25, 28, 38, 20, 22, 30, 25, 33, 30, 24};
    const char *labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"};

    // Create a XYChart object of size 600 x 375 pixels
    XYChart *c = new XYChart(600, 375);

    // Add a title to the chart using 18 pts Times Bold Italic font
    c->addTitle("Product Line Global Revenue", "timesbi.ttf", 18);

    // Set the plotarea at (50, 55) and of 500 x 280 pixels in size. Use a vertical
    // gradient color from light blue (f9f9ff) to sky blue (aaccff) as background.
    // Set border to transparent and grid lines to white (ffffff).
    c->setPlotArea(50, 55, 500, 280, c->linearGradientColor(0, 55, 0, 335, 0xf9fcff,
        0xaaccff), -1, Chart::Transparent, 0xffffff);

    // Add a legend box at (50, 28) using horizontal layout. Use 10pts Arial Bold as
    // font, with transparent background.
    c->addLegend(50, 28, false, "arialbd.ttf", 10)->setBackground(Chart::Transparent)
        ;

    // Set the x axis labels
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Set y-axis tick density to 30 pixels. ChartDirector auto-scaling will use this
    // as the guideline when putting ticks on the y-axis.
    c->yAxis()->setTickDensity(30);

    // Set axis label style to 8pts Arial Bold
    c->xAxis()->setLabelStyle("arialbd.ttf", 8);
    c->yAxis()->setLabelStyle("arialbd.ttf", 8);

    // Set axis line width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add axis title using 10pts Arial Bold Italic font
    c->yAxis()->setTitle("Revenue in US millions", "arialbi.ttf", 10);

    // Add a line layer to the chart
    LineLayer *layer = c->addLineLayer();

    // Set the line width to 3 pixels
    layer->setLineWidth(3);

    // Add the three data sets to the line layer, using circles, diamands and X
    // shapes as symbols
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xff0000,
        "Quantum Computer")->setDataSymbol(Chart::CircleSymbol, 9);
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x00ff00,
        "Atom Synthesizer")->setDataSymbol(Chart::DiamondSymbol, 11);
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0xff6600,
        "Proton Cannon")->setDataSymbol(Chart::Cross2Shape(), 11);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Revenue of {dataSetName} in {xLabel}: US$ {value}M'");

    return c;
}


BaseChart *splineline(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {32, 39, 23, 28, 41, 38, 26, 35, 29};
    double data1[] = {50, 55, 47, 34, 47, 53, 38, 40, 51};

    // The labels for the chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

    // Create a XYChart object of size 600 x 300 pixels, with a pale red (ffdddd)
    // background, black border, 1 pixel 3D border effect and rounded corners.
    XYChart *c = new XYChart(600, 300, 0xffdddd, 0x000000, 1);
    c->setRoundedFrame();

    // Set the plotarea at (55, 58) and of size 520 x 195 pixels, with white (ffffff)
    // background. Set horizontal and vertical grid lines to grey (cccccc).
    c->setPlotArea(55, 58, 520, 195, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);

    // Add a legend box at (55, 32) (top of the chart) with horizontal layout. Use 9
    // pts Arial Bold font. Set the background and border color to Transparent.
    c->addLegend(55, 32, false, "arialbd.ttf", 9)->setBackground(Chart::Transparent);

    // Add a title box to the chart using 15 pts Times Bold Italic font. The title is
    // in CDML and includes embedded images for highlight. The text is white (ffffff)
    // on a dark red (880000) background, with soft lighting effect from the right
    // side.
    c->addTitle(
        "<*block,valign=absmiddle*><*img=star.png*><*img=star.png*> Performance "
        "Enhancer <*img=star.png*><*img=star.png*><*/*>", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x880000, -1, Chart::softLighting(Chart::Right));

    // Add a title to the y axis
    c->yAxis()->setTitle("Energy Concentration (KJ per liter)");

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a title to the x axis using CMDL
    c->xAxis()->setTitle(
        "<*block,valign=absmiddle*><*img=clock.png*>  Elapsed Time (hour)<*/*>");

    // Set the axes width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add a spline layer to the chart
    SplineLayer *layer = c->addSplineLayer();

    // Set the default line width to 2 pixels
    layer->setLineWidth(2);

    // Add a data set to the spline layer, using blue (0000c0) as the line color,
    // with yellow (ffff00) circle symbols.
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x0000c0,
        "Target Group")->setDataSymbol(Chart::CircleSymbol, 9, 0xffff00);

    // Add a data set to the spline layer, using brown (982810) as the line color,
    // with pink (f040f0) diamond symbols.
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0x982810,
        "Control Group")->setDataSymbol(Chart::DiamondSymbol, 11, 0xf040f0);

    // Add a custom CDML text at the bottom right of the plot area as the logo
    c->addText(575, 250,
        "<*block,valign=absmiddle*><*img=small_molecule.png*> <*block*>"
        "<*font=timesbi.ttf,size=10,color=804040*>Molecular\nEngineering<*/*>"
        )->setAlignment(Chart::BottomRight);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} at t = {xLabel} hour: {value} KJ/liter'");

    return c;
}


BaseChart *stepline(int img, const char **imageMap)
{
    // The data for the chart
    double dataY0[] = {4, 4.5, 5, 5.25, 5.75, 5.25, 5, 4.5, 4, 3, 2.5, 2.5};
    double dataX0[] = {chartTime(1997, 1, 1), chartTime(1998, 6, 25), chartTime(1999,
        9, 6), chartTime(2000, 2, 6), chartTime(2000, 9, 21), chartTime(2001, 3, 4),
        chartTime(2001, 6, 8), chartTime(2002, 2, 4), chartTime(2002, 5, 19),
        chartTime(2002, 8, 16), chartTime(2002, 12, 1), chartTime(2003, 1, 1)};

    double dataY1[] = {7, 6.5, 6, 5, 6.5, 7, 6, 5.5, 5, 4, 3.5, 3.5};
    double dataX1[] = {chartTime(1997, 1, 1), chartTime(1997, 7, 1), chartTime(1997,
        12, 1), chartTime(1999, 1, 15), chartTime(1999, 6, 9), chartTime(2000, 3, 3),
        chartTime(2000, 8, 13), chartTime(2001, 5, 5), chartTime(2001, 9, 16),
        chartTime(2002, 3, 16), chartTime(2002, 6, 1), chartTime(2003, 1, 1)};

    // Create a XYChart object of size 500 x 270 pixels, with a pale blue (e0e0ff)
    // background, black border, 1 pixel 3D border effect and rounded corners
    XYChart *c = new XYChart(600, 300, 0xe0e0ff, 0x000000, 1);
    c->setRoundedFrame();

    // Set the plotarea at (55, 60) and of size 520 x 200 pixels, with white (ffffff)
    // background. Set horizontal and vertical grid lines to grey (cccccc).
    c->setPlotArea(50, 60, 525, 200, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);

    // Add a legend box at (55, 32) (top of the chart) with horizontal layout. Use 9
    // pts Arial Bold font. Set the background and border color to Transparent.
    c->addLegend(55, 32, false, "arialbd.ttf", 9)->setBackground(Chart::Transparent);

    // Add a title box to the chart using 15 pts Times Bold Italic font. The text is
    // white (ffffff) on a deep blue (000088) background, with soft lighting effect
    // from the right side.
    c->addTitle("Long Term Interest Rates", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x000088, -1, Chart::softLighting(Chart::Right));

    // Set the y axis label format to display a percentage sign
    c->yAxis()->setLabelFormat("{value}%");

    // Add a red (ff0000) step line layer to the chart and set the line width to 2
    // pixels
    StepLineLayer *layer0 = c->addStepLineLayer(DoubleArray(dataY0,
        sizeof(dataY0)/sizeof(dataY0[0])), 0xff0000, "Country AAA");
    layer0->setXData(DoubleArray(dataX0, sizeof(dataX0)/sizeof(dataX0[0])));
    layer0->setLineWidth(2);

    // Add a blue (0000ff) step line layer to the chart and set the line width to 2
    // pixels
    StepLineLayer *layer1 = c->addStepLineLayer(DoubleArray(dataY1,
        sizeof(dataY1)/sizeof(dataY1[0])), 0x0000ff, "Country BBB");
    layer1->setXData(DoubleArray(dataX1, sizeof(dataX1)/sizeof(dataX1[0])));
    layer1->setLineWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} change to {value}% on {x|mmm dd, yyyy}'");

    return c;
}


BaseChart *linecompare(int img, const char **imageMap)
{
    // The data for the upper and lower bounding lines
    double upperY[] = {60, 60, 100, 100, 60, 60};
    double lowerY[] = {40, 40, 80, 80, 40, 40};
    double zoneX[] = {0, 2.5, 3.5, 5.5, 6.5, 10};

    // The data for the spline curve
    double curveY[] = {50, 44, 54, 48, 58, 50, 90, 85, 104, 82, 96, 90, 74, 52, 35,
        58, 46, 54, 48, 52, 50};
    double curveX[] = {0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7,
        7.5, 8, 8.5, 9, 9.5, 10};

    // Create a XYChart object of size 600 x 300 pixels, with a light grey (cccccc)
    // background, black border, and 1 pixel 3D border effect.
    XYChart *c = new XYChart(600, 300, 0xcccccc, 0x000000, 1);

    // Set the plotarea at (55, 58) and of size 520 x 195 pixels, with white
    // background. Turn on both horizontal and vertical grid lines with light grey
    // color (cccccc)
    c->setPlotArea(55, 58, 520, 195, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);

    // Add a legend box at (55, 32) (top of the chart) with horizontal layout. Use 9
    // pts Arial Bold font. Set the background and border color to Transparent.
    c->addLegend(55, 32, false, "arialbd.ttf", 9)->setBackground(Chart::Transparent);

    // Add a title box to the chart using 15 pts Times Bold Italic font. The title is
    // in CDML and includes embedded images for highlight. The text is white (ffffff)
    // on a black background, with a 1 pixel 3D border.
    c->addTitle(
        "<*block,valign=absmiddle*><*img=star.png*><*img=star.png*> Performance "
        "Enhancer <*img=star.png*><*img=star.png*><*/*>", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x000000, -1, 1);

    // Add a title to the y axis
    c->yAxis()->setTitle("Temperature");

    // Add a title to the x axis using CMDL
    c->xAxis()->setTitle(
        "<*block,valign=absmiddle*><*img=clock.png*>  Elapsed Time (hour)<*/*>");

    // Set the axes width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add a purple (800080) spline layer to the chart with a line width of 2 pixels
    SplineLayer *splineLayer = c->addSplineLayer(DoubleArray(curveY,
        sizeof(curveY)/sizeof(curveY[0])), 0x800080, "Molecular Temperature");
    splineLayer->setXData(DoubleArray(curveX, sizeof(curveX)/sizeof(curveX[0])));
    splineLayer->setLineWidth(2);

    // Add a line layer to the chart with two dark green (338033) data sets, and a
    // line width of 2 pixels
    LineLayer *lineLayer = c->addLineLayer();
    lineLayer->addDataSet(DoubleArray(upperY, sizeof(upperY)/sizeof(upperY[0])),
        0x338033, "Target Zone");
    lineLayer->addDataSet(DoubleArray(lowerY, sizeof(lowerY)/sizeof(lowerY[0])),
        0x338033);
    lineLayer->setXData(DoubleArray(zoneX, sizeof(zoneX)/sizeof(zoneX[0])));
    lineLayer->setLineWidth(2);

    // Color the zone between the upper zone line and lower zone line as
    // semi-transparent light green (8099ff99)
    c->addInterLineLayer(lineLayer->getLine(0), lineLayer->getLine(1), 0x8099FF99,
        0x8099FF99);

    // If the spline line gets above the upper zone line, color to area between the
    // lines red (ff0000)
    c->addInterLineLayer(splineLayer->getLine(0), lineLayer->getLine(0), 0xff0000,
        Chart::Transparent);

    // If the spline line gets below the lower zone line, color to area between the
    // lines blue (0000ff)
    c->addInterLineLayer(splineLayer->getLine(0), lineLayer->getLine(1),
        Chart::Transparent, 0x0000ff);

    // Add a custom CDML text at the bottom right of the plot area as the logo
    c->addText(575, 250,
        "<*block,valign=absmiddle*><*img=small_molecule.png*> <*block*>"
        "<*font=timesbi.ttf,size=10,color=804040*>Molecular\nEngineering<*/*>"
        )->setAlignment(Chart::BottomRight);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Temperature at hour {x}: {value} C'");

    return c;
}


BaseChart *errline(int img, const char **imageMap)
{
    // The data with error information
    double data[] = {42, 49, 33, 38, 51, 46, 29, 41, 44, 57, 59, 52, 37, 34, 51, 56,
        56, 60, 70, 76, 63, 67, 75, 64, 51};
    double errData[] = {5, 6, 5.1, 6.5, 6.6, 8, 5.4, 5.1, 4.6, 5.0, 5.2, 6.0, 4.9,
        5.6, 4.8, 6.2, 7.4, 7.1, 6.0, 6.6, 7.1, 5.3, 5.5, 7.9, 6.1};

    // The labels for the chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 600 x 300 pixels, with a light grey (eeeeee)
    // background, black border, 1 pixel 3D border effect and rounded corners.
    XYChart *c = new XYChart(600, 300, 0xeeeeee, 0x000000, 1);
    c->setRoundedFrame();

    // Set the plotarea at (55, 55) and of size 520 x 195 pixels, with white (ffffff)
    // background. Set horizontal and vertical grid lines to grey (cccccc).
    c->setPlotArea(55, 55, 520, 195, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);

    // Add a title box to the chart using 15 pts Times Bold Italic font. The title is
    // in CDML and includes embedded images for highlight. The text is on a light
    // grey (dddddd) background, with glass lighting effect.
    c->addTitle(
        "<*block,valign=absmiddle*><*img=star.png*><*img=star.png*> Molecular "
        "Temperature Control <*img=star.png*><*img=star.png*><*/*>", "timesbi.ttf",
        15)->setBackground(0xdddddd, 0, Chart::glassEffect());

    // Add a title to the y axis
    c->yAxis()->setTitle("Temperature");

    // Add a title to the x axis using CMDL
    c->xAxis()->setTitle(
        "<*block,valign=absmiddle*><*img=clock.png*>  Elapsed Time (hour)<*/*>");

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis. Show minor ticks for remaining
    // labels.
    c->xAxis()->setLabelStep(3, 1);

    // Set the axes width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add a line layer to the chart
    LineLayer *lineLayer = c->addLineLayer();

    // Add a blue (0xff) data set to the line layer, with yellow (0xffff80) diamond
    // symbols
    lineLayer->addDataSet(DoubleArray(data, sizeof(data)/sizeof(data[0])), 0x0000ff
        )->setDataSymbol(Chart::DiamondSymbol, 12, 0xffff80);

    // Set the line width to 2 pixels
    lineLayer->setLineWidth(2);

    // Add a box whisker layer to the chart. Use the upper and lower mark of the box
    // whisker layer to act as error zones. The upper and lower marks are computed
    // using the ArrayMath object.
    BoxWhiskerLayer *errLayer = c->addBoxWhiskerLayer(DoubleArray(), DoubleArray(),
        ArrayMath(DoubleArray(data, sizeof(data)/sizeof(data[0]))).add(DoubleArray(
        errData, sizeof(errData)/sizeof(errData[0]))), ArrayMath(DoubleArray(data,
        sizeof(data)/sizeof(data[0]))).sub(DoubleArray(errData,
        sizeof(errData)/sizeof(errData[0]))), DoubleArray(data,
        sizeof(data)/sizeof(data[0])), Chart::Transparent, 0xbb6633);

    // Set the line width to 2 pixels
    errLayer->setLineWidth(2);

    // Set the error zone to occupy half the space between the symbols
    errLayer->setDataGap(0.5);

    // Add a custom CDML text at the bottom right of the plot area as the logo
    c->addText(575, 247,
        "<*block,valign=absmiddle*><*img=small_molecule.png*> <*block*>"
        "<*font=timesbi.ttf,size=10,color=804040*>Molecular\nEngineering<*/*>"
        )->setAlignment(Chart::BottomRight);

    // output the chart
    c->makeChart();

    // Include tool tip for the chart. We only need to show the tool tip for the box
    // whisker layer.
    *imageMap = errLayer->getHTMLImageMap("clickable", "",
        "title='Temperature at hour {xLabel}: {med} +/- {={med}-{min}} C'");

    return c;
}


BaseChart *multisymbolline(int img, const char **imageMap)
{
    // In this example, the data points are unevenly spaced on the x-axis
    double dataY[] = {4.7, 4.7, 6.6, 2.2, 4.7, 4.0, 4.0, 5.1, 4.5, 4.5, 6.8, 4.5, 4,
        2.1, 3, 2.5, 2.5, 3.1};
    double dataX[] = {chartTime(1999, 7, 1), chartTime(2000, 1, 1), chartTime(2000,
        2, 1), chartTime(2000, 4, 1), chartTime(2000, 5, 8), chartTime(2000, 7, 5),
        chartTime(2001, 3, 5), chartTime(2001, 4, 7), chartTime(2001, 5, 9),
        chartTime(2002, 2, 4), chartTime(2002, 4, 4), chartTime(2002, 5, 8),
        chartTime(2002, 7, 7), chartTime(2002, 8, 30), chartTime(2003, 1, 2),
        chartTime(2003, 2, 16), chartTime(2003, 11, 6), chartTime(2004, 1, 4)};

    // Data points are assigned different symbols based on point type
    double pointType[] = {0, 1, 0, 1, 2, 1, 0, 0, 1, 1, 2, 2, 1, 0, 2, 1, 2, 0};

    // Create a XYChart object of size 600 x 300 pixels, with a light purple (ffccff)
    // background, black border, 1 pixel 3D border effect and rounded corners.
    XYChart *c = new XYChart(600, 300, 0xffccff, 0x000000, 1);
    c->setRoundedFrame();

    // Set the plotarea at (55, 58) and of size 520 x 195 pixels, with white (ffffff)
    // background. Set horizontal and vertical grid lines to grey (cccccc).
    c->setPlotArea(55, 58, 520, 195, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);

    // Add a legend box at (55, 30) (top of the chart) with horizontal layout. Use 10
    // pts Arial Bold Italic font. Set the background and border color to
    // Transparent.
    c->addLegend(55, 30, false, "arialbi.ttf", 10)->setBackground(Chart::Transparent)
        ;

    // Add a title box to the chart using 15 pts Times Bold Italic font. The text is
    // white (ffffff) on a purple (400040) background, with soft lighting effect from
    // the right side.
    c->addTitle("Multi-Symbol Line Chart Demo", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x400040, -1, Chart::softLighting(Chart::Right));

    // Set the y axis label format to display a percentage sign
    c->yAxis()->setLabelFormat("{value}%");

    // Set axis titles to use 9pt Arial Bold Italic font
    c->yAxis()->setTitle("Axis Title Placeholder", "arialbi.ttf", 9);
    c->xAxis()->setTitle("Axis Title Placeholder", "arialbi.ttf", 9);

    // Set axis labels to use Arial Bold font
    c->yAxis()->setLabelStyle("arialbd.ttf");
    c->xAxis()->setLabelStyle("arialbd.ttf");

    // We add the different data symbols using scatter layers. The scatter layers are
    // added before the line layer to make sure the data symbols stay on top of the
    // line layer.

    // We select the points with pointType = 0 (the non-selected points will be set
    // to NoValue), and use yellow (ffff00) 15 pixels high 5 pointed star shape
    // symbols for the points. (This example uses both x and y coordinates. For
    // charts that have no x explicitly coordinates, use an empty array as dataX.)
    c->addScatterLayer(DoubleArray(dataX, sizeof(dataX)/sizeof(dataX[0])), ArrayMath(
        DoubleArray(dataY, sizeof(dataY)/sizeof(dataY[0]))).selectEQZ(DoubleArray(
        pointType, sizeof(pointType)/sizeof(pointType[0])), Chart::NoValue),
        "Point Type 0", Chart::StarShape(5), 15, 0xffff00);

    // Similar to above, we select the points with pointType - 1 = 0 and use green
    // (ff00) 13 pixels high six-sided polygon as symbols.
    c->addScatterLayer(DoubleArray(dataX, sizeof(dataX)/sizeof(dataX[0])), ArrayMath(
        DoubleArray(dataY, sizeof(dataY)/sizeof(dataY[0]))).selectEQZ(ArrayMath(
        DoubleArray(pointType, sizeof(pointType)/sizeof(pointType[0]))).sub(1),
        Chart::NoValue), "Point Type 1", Chart::PolygonShape(6), 13, 0x00ff00);

    // Similar to above, we select the points with pointType - 2 = 0 and use red
    // (ff0000) 13 pixels high X shape as symbols.
    c->addScatterLayer(DoubleArray(dataX, sizeof(dataX)/sizeof(dataX[0])), ArrayMath(
        DoubleArray(dataY, sizeof(dataY)/sizeof(dataY[0]))).selectEQZ(ArrayMath(
        DoubleArray(pointType, sizeof(pointType)/sizeof(pointType[0]))).sub(2),
        Chart::NoValue), "Point Type 2", Chart::Cross2Shape(), 13, 0xff0000);

    // Finally, add a blue (0000ff) line layer with line width of 2 pixels
    LineLayer *layer = c->addLineLayer(DoubleArray(dataY,
        sizeof(dataY)/sizeof(dataY[0])), 0x0000ff);
    layer->setXData(DoubleArray(dataX, sizeof(dataX)/sizeof(dataX[0])));
    layer->setLineWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{x|mmm dd, yyyy}: {value}%'");

    return c;
}


BaseChart *customsymbolline(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {600, 800, 1200, 1500, 1800, 1900, 2000, 1950};
    double data1[] = {300, 450, 500, 1000, 1500, 1600, 1650, 1600};

    // The labels for the chart
    const char *labels[] = {"1995", "1996", "1997", "1998", "1999", "2000", "2001",
        "2002"};

    // Create a XYChart object of size 450 x 250 pixels, with a pale yellow
    // (0xffffc0) background, a black border, and 1 pixel 3D border effect.
    XYChart *c = new XYChart(450, 250, 0xffffc0, 0, 1);

    // Set the plotarea at (60, 45) and of size 360 x 170 pixels, using white
    // (0xffffff) as the plot area background color. Turn on both horizontal and
    // vertical grid lines with light grey color (0xc0c0c0)
    c->setPlotArea(60, 45, 360, 170, 0xffffff, -1, -1, 0xc0c0c0, -1);

    // Add a legend box at (60, 20) (top of the chart) with horizontal layout. Use 8
    // pts Arial Bold font. Set the background and border color to Transparent.
    c->addLegend(60, 20, false, "arialbd.ttf", 8)->setBackground(Chart::Transparent);

    // Add a title to the chart using 12 pts Arial Bold/white font. Use a 1 x 2
    // bitmap pattern as the background.
    int pattern1[] = {0x000040, 0x000080};
    c->addTitle("Information Resource Usage", "arialbd.ttf", 12, 0xffffff
        )->setBackground(c->patternColor(IntArray(pattern1,
        sizeof(pattern1)/sizeof(pattern1[0])), 2));

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Reserve 8 pixels margins at both side of the x axis to avoid the first and
    // last symbols drawing outside of the plot area
    c->xAxis()->setMargin(8, 8);

    // Add a title to the y axis
    c->yAxis()->setTitle("Population");

    // Add a line layer to the chart
    LineLayer *layer = c->addLineLayer();

    // Add the first line using small_user.png as the symbol.
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xcf4040,
        "Users")->setDataSymbol("small_user.png");

    // Add the first line using small_computer.png as the symbol.
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x40cf40,
        "Computers")->setDataSymbol("small_computer.png");

    // Set the line width to 3 pixels
    layer->setLineWidth(3);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Number of {dataSetName} at {xLabel}: {value}'");

    return c;
}


BaseChart *rotatedline(int img, const char **imageMap)
{
    //
    //    We use a random table to simulate generating 12 months of data
    //

    // Create the random table object with 4 cols * 12 rows, using 3 as seed
    RanTable *rantable = new RanTable(3, 4, 12);

    // Set the 1st column to be the 12 months of year 2002
    rantable->setDateCol(0, chartTime(2002, 1, 1), 86400 * 30);

    // Set the 2nd, 3rd and 4th columns to be random numbers starting from 125, 75,
    // and 100 respectively. The change between rows is set to -35 to + 35. The
    // minimum value of any cell is 0.
    rantable->setCol(1, 125, -35, 35, 0);
    rantable->setCol(2, 75, -35, 35, 0);
    rantable->setCol(3, 100, -35, 35, 0);

    // Get the 1st column (time) as the x data
    DoubleArray dataX = rantable->getCol(0);

    // Get the 2nd, 3rd and 4th columns as 3 data sets
    DoubleArray dataY0 = rantable->getCol(1);
    DoubleArray dataY1 = rantable->getCol(2);
    DoubleArray dataY2 = rantable->getCol(3);

    // Create a XYChart object of size 360 x 400 pixels
    XYChart *c = new XYChart(360, 400);

    // Add a title to the chart
    c->addTitle("<*underline=2*>Rotated Line Chart Demo", "timesbi.ttf", 14);

    // Set the plotarea at (60, 75) and of size 190 x 320 pixels. Turn on both
    // horizontal and vertical grid lines with light grey color (0xc0c0c0)
    c->setPlotArea(60, 75, 190, 320)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a legend box at (270, 75)
    c->addLegend(270, 75);

    // Swap the x and y axis to become a rotated chart
    c->swapXY();

    // Set the y axis on the top side (right + rotated = top)
    c->setYAxisOnRight();

    // Add a title to the y axis
    c->yAxis()->setTitle("Throughput (MBytes)");

    // Reverse the x axis so it is pointing downwards
    c->xAxis()->setReverse();

    // Add a line chart layer using the given data
    LineLayer *layer = c->addLineLayer();
    layer->setXData(dataX);
    layer->addDataSet(dataY0, 0xff0000, "Server A");
    layer->addDataSet(dataY1, 0x338033, "Server B");
    layer->addDataSet(dataY2, 0x0000ff, "Server C");

    // Set the line width to 2 pixels
    layer->setLineWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{dataSetName}] {x|mm/yyyy}: {value|0} MByte'");

    //free up resources
    delete rantable;

    return c;
}


BaseChart *xyline(int img, const char **imageMap)
{
    // The (x, y) data for the first line
    double dataX0[] = {20, 90, 40, 30, 12};
    double dataY0[] = {10, 40, 75, 54, 20};

    // The (x, y) data for the second line
    double dataX1[] = {10, 40, 75, 54, 60};
    double dataY1[] = {50, 90, 40, 30, 10};

    // Create a XYChart object of size 450 x 450 pixels
    XYChart *c = new XYChart(450, 450);

    // Set the plotarea at (55, 65) and of size 350 x 300 pixels, with white
    // background and a light grey border (0xc0c0c0). Turn on both horizontal and
    // vertical grid lines with light grey color (0xc0c0c0)
    c->setPlotArea(55, 65, 350, 300, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 12
    // pts Times Bold Italic font. Set the background and border color to
    // Transparent.
    c->addLegend(50, 30, false, "timesbi.ttf", 12)->setBackground(Chart::Transparent)
        ;

    // Add a title to the chart using 18 pts Times Bold Itatic font
    c->addTitle("Reaction Path", "timesbi.ttf", 18);

    // Add a title to the y axis using 12 pts Arial Bold Italic font
    c->yAxis()->setTitle("Temperature (Celcius)", "arialbi.ttf", 12);

    // Set the y axis line width to 3 pixels
    c->yAxis()->setWidth(3);

    // Add a title to the x axis using 12 pts Arial Bold Italic font
    c->xAxis()->setTitle("Pressure (Pa)", "arialbi.ttf", 12);

    // Set the x axis line width to 3 pixels
    c->xAxis()->setWidth(3);

    // Add a red (0xff3333) line layer using dataX0 and dataY0
    LineLayer *layer1 = c->addLineLayer(DoubleArray(dataY0,
        sizeof(dataY0)/sizeof(dataY0[0])), 0xff3333, "Compound AAA");
    layer1->setXData(DoubleArray(dataX0, sizeof(dataX0)/sizeof(dataX0[0])));

    // Set the line width to 3 pixels
    layer1->setLineWidth(3);

    // Use 9 pixel square symbols for the data points
    layer1->getDataSet(0)->setDataSymbol(Chart::SquareSymbol, 9);

    // Add custom text labels to the first and last point on the scatter plot using
    // Arial Bold font
    layer1->addCustomDataLabel(0, 0, "Start", "arialbd.ttf");
    layer1->addCustomDataLabel(0, 4, "End", "arialbd.ttf");

    // Add a green (0x33ff33) line layer using dataX1 and dataY1
    LineLayer *layer2 = c->addLineLayer(DoubleArray(dataY1,
        sizeof(dataY1)/sizeof(dataY1[0])), 0x33ff33, "Compound BBB");
    layer2->setXData(DoubleArray(dataX1, sizeof(dataX1)/sizeof(dataX1[0])));

    // Set the line width to 3 pixels
    layer2->setLineWidth(3);

    // Use 11 pixel diamond symbols for the data points
    layer2->getDataSet(0)->setDataSymbol(Chart::DiamondSymbol, 11);

    // Add custom text labels to the first and last point on the scatter plot using
    // Arial Bold font
    layer2->addCustomDataLabel(0, 0, "Start", "arialbd.ttf");
    layer2->addCustomDataLabel(0, 4, "End", "arialbd.ttf");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{dataSetName}] Pressure = {x} Pa, Temperature = {value} C'");

    return c;
}


BaseChart *discontline(int img, const char **imageMap)
{
    char buffer[256];

    //===================================================================
    //    For demo purpose, use random numbers as data for the chart
    //===================================================================

    // Use a random table to create the data. The random table contains 4 cols x 31
    // rows, using 9 as seed.
    RanTable *rantable = new RanTable(9, 4, 31);

    // Set the 1st col to be the series 1, 2, 3, ....
    rantable->setCol(0, 1, 1, 1);

    // Set the 2nd, 3rd and 4th col to be random number starting from 40, 50 and 60.
    // The change between rows is set to -5 to 5. The minimum value of any cell is 0.
    rantable->setCol(1, 40, -5, 5, 0);
    rantable->setCol(2, 50, -5, 5, 0);
    rantable->setCol(3, 60, -5, 5, 0);

    // Use the 1st col as the axis label
    DoubleArray labels = rantable->getCol(0);

    // Use the 2nd, 3rd and 4th columns for 3 lines
    DoubleArray data0 = rantable->getCol(1);
    DoubleArray data1 = rantable->getCol(2);
    DoubleArray data2 = rantable->getCol(3);

    // Simulate some data points have no data value
    int i;
    for(i = 1; i < 30; i += 7) {
        ((double*)data0.data)[i] = Chart::NoValue;
        ((double*)data1.data)[i] = Chart::NoValue;
        ((double*)data2.data)[i] = Chart::NoValue;
    }

    //===================================================================
    //    Now we have the data ready. Actually drawing the chart.
    //===================================================================

    // Create a XYChart object of size 600 x 220 pixels
    XYChart *c = new XYChart(600, 220);

    // Set the plot area at (100, 25) and of size 450 x 150 pixels. Enabled both
    // vertical and horizontal grids by setting their colors to light grey (0xc0c0c0)
    c->setPlotArea(100, 25, 450, 150)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a legend box (92, 0) (top of plot area) using horizontal layout. Use 8 pts
    // Arial font. Disable bounding box (set border to transparent).
    c->addLegend(92, 0, false, "", 8)->setBackground(Chart::Transparent);

    // Add a title to the y axis. Draw the title upright (font angle = 0)
    c->yAxis()->setTitle("Average\nUtilization\n(MBytes)")->setFontAngle(0);

    // Use manually scaling of y axis from 0 to 100, with ticks every 10 units
    c->yAxis()->setLinearScale(0, 100, 10);

    // Set the labels on the x axis
    c->xAxis()->setLabels(labels);

    // Set the title on the x axis
    c->xAxis()->setTitle("Jun - 2001");

    // Add x axis (vertical) zones to indicate Saturdays and Sundays
    for(i = 0; i < 29; i += 7) {
        c->xAxis()->addZone(i, i + 2, 0xc0c0c0);
    }

    // Add a line layer to the chart
    LineLayer *layer = c->addLineLayer();

    // Set the default line width to 2 pixels
    layer->setLineWidth(2);

    // Add the three data sets to the line layer
    layer->addDataSet(data0, 0xcf4040, "Server #1");
    layer->addDataSet(data1, 0x40cf40, "Server #2");
    layer->addDataSet(data2, 0x4040cf, "Server #3");

    // Layout the chart to fix the y axis scaling. We can then use getXCoor and
    // getYCoor to determine the position of custom objects.
    c->layout();

    // Add the "week n" custom text boxes at the top of the plot area.
    for(i = 0; i < 4; ++i) {
        // Add the "week n" text box using 8 pt Arial font with top center alignment.
        sprintf(buffer, "Week %d", i);
        TextBox *textbox = c->addText(layer->getXCoor(i * 7 + 2), 25, buffer,
            "arialbd.ttf", 8, 0x000000, Chart::TopCenter);

        // Set the box width to cover five days
        textbox->setSize(layer->getXCoor(i * 7 + 7) - layer->getXCoor(i * 7 + 2) + 1,
            0);

        // Set box background to pale yellow 0xffff80, with a 1 pixel 3D border
        textbox->setBackground(0xffff80, Chart::Transparent, 1);
    }

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{dataSetName}] {xLabel} Jun 2001: {value|2} MBytes'");

    //free up resources
    delete rantable;

    return c;
}


BaseChart *trendline(int img, const char **imageMap)
{
    // The data for the line chart
    double data[] = {50, 55, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67,
        58, 59, 73, 77, 84, 82, 80, 91};

    // The labels for the line chart
    const char *labels[] = {"Jan 2000", "Feb 2000", "Mar 2000", "Apr 2000",
        "May 2000", "Jun 2000", "Jul 2000", "Aug 2000", "Sep 2000", "Oct 2000",
        "Nov 2000", "Dec 2000", "Jan 2001", "Feb 2001", "Mar 2001", "Apr 2001",
        "May 2001", "Jun 2001", "Jul 2001", "Aug 2001", "Sep 2001", "Oct 2001",
        "Nov 2001", "Dec 2001"};

    // Create a XYChart object of size 500 x 320 pixels, with a pale purpule
    // (0xffccff) background, a black border, and 1 pixel 3D border effect.
    XYChart *c = new XYChart(500, 320, 0xffccff, 0x000000, 1);

    // Set the plotarea at (55, 45) and of size 420 x 210 pixels, with white
    // background. Turn on both horizontal and vertical grid lines with light grey
    // color (0xc0c0c0)
    c->setPlotArea(55, 45, 420, 210, 0xffffff, -1, -1, 0xc0c0c0, -1);

    // Add a legend box at (55, 25) (top of the chart) with horizontal layout. Use 8
    // pts Arial font. Set the background and border color to Transparent.
    c->addLegend(55, 22, false, "", 8)->setBackground(Chart::Transparent);

    // Add a title box to the chart using 13 pts Times Bold Italic font. The text is
    // white (0xffffff) on a purple (0x800080) background, with a 1 pixel 3D border.
    c->addTitle("Long Term Server Load", "timesbi.ttf", 13, 0xffffff)->setBackground(
        0x800080, -1, 1);

    // Add a title to the y axis
    c->yAxis()->setTitle("MBytes");

    // Set the labels on the x axis. Rotate the font by 90 degrees.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        )->setFontAngle(90);

    // Add a line layer to the chart
    LineLayer *lineLayer = c->addLineLayer();

    // Add the data to the line layer using light brown color (0xcc9966) with a 7
    // pixel square symbol
    lineLayer->addDataSet(DoubleArray(data, sizeof(data)/sizeof(data[0])), 0xcc9966,
        "Server Utilization")->setDataSymbol(Chart::SquareSymbol, 7);

    // Set the line width to 2 pixels
    lineLayer->setLineWidth(2);

    // tool tip for the line layer
    lineLayer->setHTMLImageMap("", "", "title='{xLabel}: {value} MBytes'");

    // Add a trend line layer using the same data with a dark green (0x008000) color.
    // Set the line width to 2 pixels
    TrendLayer *trendLayer = c->addTrendLayer(DoubleArray(data,
        sizeof(data)/sizeof(data[0])), 0x008000, "Trend Line");
    trendLayer->setLineWidth(2);

    // tool tip for the trend layer
    trendLayer->setHTMLImageMap("", "",
        "title='Change rate: {slope|2} MBytes/per month'");

    // output the chart
    c->makeChart();

    // include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable");

    return c;
}


BaseChart *scattertrend(int img, const char **imageMap)
{
    // The XY data of the first data series
    double dataX0[] = {50, 55, 37, 24, 42, 49, 63, 72, 83, 59};
    double dataY0[] = {3.6, 2.8, 2.5, 2.3, 3.8, 3.0, 3.8, 5.0, 6.0, 3.3};

    // The XY data of the second data series
    double dataX1[] = {50, 55, 37, 24, 42, 49, 63, 72, 83, 59};
    double dataY1[] = {1.6, 1.8, 0.8, 0.5, 1.3, 1.5, 2.3, 2.4, 2.9, 1.5};

    // Tool tip formats for data points and trend lines
    const char *scatterToolTip =
        "title='{dataSetName}: Response time at {x} TPS: {value} sec'";
    const char *trendToolTip =
        "title='Slope = {slope|4} sec/TPS; Intercept = {intercept|4} sec'";

    // Create a XYChart object of size 450 x 420 pixels
    XYChart *c = new XYChart(450, 420);

    // Set the plotarea at (55, 65) and of size 350 x 300 pixels, with white
    // background and a light grey border (0xc0c0c0). Turn on both horizontal and
    // vertical grid lines with light grey color (0xc0c0c0)
    c->setPlotArea(55, 65, 350, 300, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 12
    // pts Times Bold Italic font. Set the background and border color to
    // Transparent.
    c->addLegend(50, 30, false, "timesbi.ttf", 12)->setBackground(Chart::Transparent)
        ;

    // Add a title to the chart using 18 point Times Bold Itatic font.
    c->addTitle("Server Performance", "timesbi.ttf", 18);

    // Add a title to the y axis using 12 pts Arial Bold Italic font
    c->yAxis()->setTitle("Response Time (sec)", "arialbi.ttf", 12);

    // Set the y axis line width to 3 pixels
    c->yAxis()->setWidth(3);

    // Set the y axis label format to show 1 decimal point
    c->yAxis()->setLabelFormat("{value|1}");

    // Add a title to the x axis using 12 pts Arial Bold Italic font
    c->xAxis()->setTitle("Server Load (TPS)", "arialbi.ttf", 12);

    // Set the x axis line width to 3 pixels
    c->xAxis()->setWidth(3);

    // Add a scatter layer using (dataX0, dataY0)
    ScatterLayer *scatter1 = c->addScatterLayer(DoubleArray(dataX0,
        sizeof(dataX0)/sizeof(dataX0[0])), DoubleArray(dataY0,
        sizeof(dataY0)/sizeof(dataY0[0])), "Server AAA", Chart::DiamondSymbol, 11,
        0x008000);
    scatter1->setHTMLImageMap("", "", scatterToolTip);

    // Add a trend line layer for (dataX0, dataY0)
    TrendLayer *trend1 = c->addTrendLayer(DoubleArray(dataX0,
        sizeof(dataX0)/sizeof(dataX0[0])), DoubleArray(dataY0,
        sizeof(dataY0)/sizeof(dataY0[0])), 0x008000);
    trend1->setLineWidth(3);
    trend1->setHTMLImageMap("", "", trendToolTip);

    // Add a scatter layer for (dataX1, dataY1)
    ScatterLayer *scatter2 = c->addScatterLayer(DoubleArray(dataX1,
        sizeof(dataX1)/sizeof(dataX1[0])), DoubleArray(dataY1,
        sizeof(dataY1)/sizeof(dataY1[0])), "Server BBB", Chart::TriangleSymbol, 9,
        0x6666ff);
    scatter2->setHTMLImageMap("", "", scatterToolTip);

    // Add a trend line layer for (dataX1, dataY1)
    TrendLayer *trend2 = c->addTrendLayer(DoubleArray(dataX1,
        sizeof(dataX1)/sizeof(dataX1[0])), DoubleArray(dataY1,
        sizeof(dataY1)/sizeof(dataY1[0])), 0x6666ff);
    trend2->setLineWidth(3);
    trend2->setHTMLImageMap("", "", trendToolTip);

    // output the chart
    c->makeChart();

    // include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable");

    return c;
}


BaseChart *confidenceband(int img, const char **imageMap)
{
    char buffer[256];

    // The XY data of the first data series
    double dataX[] = {50, 55, 37, 24, 42, 49, 63, 72, 83, 59};
    double dataY[] = {3.6, 2.8, 2.5, 2.3, 3.8, 3.0, 3.8, 5.0, 6.0, 3.3};

    // Create a XYChart object of size 450 x 420 pixels
    XYChart *c = new XYChart(450, 420);

    // Set the plotarea at (55, 65) and of size 350 x 300 pixels, with white
    // background and a light grey border (0xc0c0c0). Turn on both horizontal and
    // vertical grid lines with light grey color (0xc0c0c0)
    c->setPlotArea(55, 65, 350, 300, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a title to the chart using 18 point Times Bold Itatic font.
    c->addTitle("Server Performance", "timesbi.ttf", 18);

    // Add a title to the y axis using 12 pts Arial Bold Italic font
    c->yAxis()->setTitle("Response Time (sec)", "arialbi.ttf", 12);

    // Set the y axis line width to 3 pixels
    c->yAxis()->setWidth(3);

    // Set the y axis label format to show 1 decimal point
    c->yAxis()->setLabelFormat("{value|1}");

    // Add a title to the x axis using 12 pts Arial Bold Italic font
    c->xAxis()->setTitle("Server Load (TPS)", "arialbi.ttf", 12);

    // Set the x axis line width to 3 pixels
    c->xAxis()->setWidth(3);

    // Add a scatter layer using (dataX, dataY)
    ScatterLayer *scatterLayer = c->addScatterLayer(DoubleArray(dataX,
        sizeof(dataX)/sizeof(dataX[0])), DoubleArray(dataY,
        sizeof(dataY)/sizeof(dataY[0])), "", Chart::DiamondSymbol, 11, 0x008000);

    // tool tip for scatter layer
    scatterLayer->setHTMLImageMap("", "",
        "title='Response time at {x} TPS: {value} sec'");

    // Add a trend line layer for (dataX, dataY)
    TrendLayer *trendLayer = c->addTrendLayer(DoubleArray(dataX,
        sizeof(dataX)/sizeof(dataX[0])), DoubleArray(dataY,
        sizeof(dataY)/sizeof(dataY[0])), 0x008000);

    // Set the line width to 3 pixels
    trendLayer->setLineWidth(3);

    // Add a 95% confidence band for the line
    trendLayer->addConfidenceBand(0.95, 0x806666FF);

    // Add a 95% confidence band (prediction band) for the points
    trendLayer->addPredictionBand(0.95, 0x8066FF66);

    // tool tip for trend layer
    trendLayer->setHTMLImageMap("", "",
        "title='Slope = {slope|4} sec/TPS; Intercept = {intercept|4} sec'");

    // Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 10
    // pts Arial Bold Italic font. Set the background and border color to
    // Transparent.
    LegendBox *legendBox = c->addLegend(50, 30, false, "arialbi.ttf", 10);
    legendBox->setBackground(Chart::Transparent);

    // Add entries to the legend box
    legendBox->addKey("95% Line Confidence", 0x806666FF);
    legendBox->addKey("95% Point Confidence", 0x8066FF66);

    // Display the trend line parameters as a text table formatted using CDML
    sprintf(buffer,
        "<*block*>Slope\nIntercept\nCorrelation\nStd Error<*/*>   <*block*>%.4f "
        "sec/tps\n%.4f sec\n%.4f\n%.4f sec<*/*>", trendLayer->getSlope(),
        trendLayer->getIntercept(), trendLayer->getCorrelation(),
        trendLayer->getStdError());
    TextBox *textbox = c->addText(56, 65, buffer, "arialbd.ttf", 8);

    // Set the background of the text box to light grey, with a black border, and 1
    // pixel 3D border
    textbox->setBackground(0xc0c0c0, 0, 1);

    // output the chart
    c->makeChart();

    // include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable");

    return c;
}


BaseChart *curvefitting(int img, const char **imageMap)
{
    // Use random table to generate a random series. The random table is set to 1 col
    // x 51 rows, with 9 as the seed
    RanTable *rantable = new RanTable(9, 1, 51);

    // Set the 1st column to start from 100, with changes between rows from -5 to +5
    rantable->setCol(0, 100, -5, 5);

    // Get the 1st column of the random table as the data set
    DoubleArray data = rantable->getCol(0);

    // Create a XYChart object of size 600 x 300 pixels
    XYChart *c = new XYChart(600, 300);

    // Set the plotarea at (50, 35) and of size 500 x 240 pixels. Enable both the
    // horizontal and vertical grids by setting their colors to grey (0xc0c0c0)
    c->setPlotArea(50, 35, 500, 240)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a title to the chart using 18 point Times Bold Itatic font.
    c->addTitle("LOWESS Generic Curve Fitting Algorithm", "timesbi.ttf", 18);

    // Set the y axis line width to 3 pixels
    c->yAxis()->setWidth(3);

    // Add a title to the x axis using 12 pts Arial Bold Italic font
    c->xAxis()->setTitle("Server Load (TPS)", "arialbi.ttf", 12);

    // Set the x axis line width to 3 pixels
    c->xAxis()->setWidth(3);

    // Set the x axis scale from 0 - 50, with major tick every 5 units and minor tick
    // every 1 unit
    c->xAxis()->setLinearScale(0, 50, 5, 1);

    // Add a blue layer to the chart
    LineLayer *layer = c->addLineLayer();

    // Add a red (0x80ff0000) data set to the chart with square symbols
    layer->addDataSet(data, 0x80FF0000)->setDataSymbol(Chart::SquareSymbol);

    // Set the line width to 2 pixels
    layer->setLineWidth(2);

    // Use lowess for curve fitting, and plot the fitted data using a spline layer
    // with line width set to 3 pixels
    c->addSplineLayer(ArrayMath(data).lowess(), 0x0000ff)->setLineWidth(3);

    // Set zero affinity to 0 to make sure the line is displayed in the most detail
    // scale
    c->yAxis()->setAutoScale(0, 0, 0);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "", "title='({x}, {value|2})'");

    //free up resources
    delete rantable;

    return c;
}


BaseChart *scatter(int img, const char **imageMap)
{
    // The XY points for the scatter chart
    double dataX0[] = {10, 15, 6, 12, 14, 8, 13, 13, 16, 12, 10.5};
    double dataY0[] = {130, 150, 80, 110, 110, 105, 130, 115, 170, 125, 125};

    double dataX1[] = {6, 12, 4, 3.5, 7, 8, 9, 10, 12, 11, 8};
    double dataY1[] = {65, 80, 40, 45, 70, 80, 80, 90, 100, 105, 60};

    // Create a XYChart object of size 450 x 420 pixels
    XYChart *c = new XYChart(450, 420);

    // Set the plotarea at (55, 65) and of size 350 x 300 pixels, with a light grey
    // border (0xc0c0c0). Turn on both horizontal and vertical grid lines with light
    // grey color (0xc0c0c0)
    c->setPlotArea(55, 65, 350, 300, -1, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 12
    // pts Times Bold Italic font. Set the background and border color to
    // Transparent.
    c->addLegend(50, 30, false, "timesbi.ttf", 12)->setBackground(Chart::Transparent)
        ;

    // Add a title to the chart using 18 pts Times Bold Itatic font.
    c->addTitle("Genetically Modified Predator", "timesbi.ttf", 18);

    // Add a title to the y axis using 12 pts Arial Bold Italic font
    c->yAxis()->setTitle("Length (cm)", "arialbi.ttf", 12);

    // Add a title to the x axis using 12 pts Arial Bold Italic font
    c->xAxis()->setTitle("Weight (kg)", "arialbi.ttf", 12);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);

    // Add an orange (0xff9933) scatter chart layer, using 13 pixel diamonds as
    // symbols
    c->addScatterLayer(DoubleArray(dataX0, sizeof(dataX0)/sizeof(dataX0[0])),
        DoubleArray(dataY0, sizeof(dataY0)/sizeof(dataY0[0])),
        "Genetically Engineered", Chart::DiamondSymbol, 13, 0xff9933);

    // Add a green (0x33ff33) scatter chart layer, using 11 pixel triangles as
    // symbols
    c->addScatterLayer(DoubleArray(dataX1, sizeof(dataX1)/sizeof(dataX1[0])),
        DoubleArray(dataY1, sizeof(dataY1)/sizeof(dataY1[0])), "Natural",
        Chart::TriangleSymbol, 11, 0x33ff33);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{dataSetName}] Weight = {x} kg, Length = {value} cm'");

    return c;
}


BaseChart *builtinsymbols(int img, const char **imageMap)
{
    // Some ChartDirector built-in symbols
    int symbols[] = {Chart::SquareSymbol, Chart::DiamondSymbol,
        Chart::TriangleSymbol, Chart::RightTriangleSymbol, Chart::LeftTriangleSymbol,
        Chart::InvertedTriangleSymbol, Chart::CircleSymbol, Chart::StarShape(3),
        Chart::StarShape(4), Chart::StarShape(5), Chart::StarShape(6),
        Chart::StarShape(7), Chart::StarShape(8), Chart::StarShape(9),
        Chart::StarShape(10), Chart::PolygonShape(5), Chart::Polygon2Shape(5),
        Chart::PolygonShape(6), Chart::Polygon2Shape(6), Chart::PolygonShape(7),
        Chart::PolygonShape(8), Chart::CrossShape(0.1), Chart::CrossShape(0.2),
        Chart::CrossShape(0.3), Chart::CrossShape(0.4), Chart::CrossShape(0.5),
        Chart::CrossShape(0.6), Chart::CrossShape(0.7), Chart::Cross2Shape(0.1),
        Chart::Cross2Shape(0.2), Chart::Cross2Shape(0.3), Chart::Cross2Shape(0.4),
        Chart::Cross2Shape(0.5), Chart::Cross2Shape(0.6), Chart::Cross2Shape(0.7)};

    // Create a XYChart object of size 450 x 400 pixels
    XYChart *c = new XYChart(450, 400);

    // Set the plotarea at (55, 40) and of size 350 x 300 pixels, with a light grey
    // border (0xc0c0c0). Turn on both horizontal and vertical grid lines with light
    // grey color (0xc0c0c0)
    c->setPlotArea(55, 40, 350, 300, -1, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a title to the chart using 18 pts Times Bold Itatic font.
    c->addTitle("Built-in Symbols", "timesbi.ttf", 18);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);

    // Add each symbol as a separate scatter layer.
    int i;
    for(i = 0; i < sizeof(symbols) / sizeof(symbols[0]); ++i) {
        double coor1[] = {i % 5 + 1};
        double coor2[] = {i / 5 + 1};
        c->addScatterLayer(DoubleArray(coor1, sizeof(coor1)/sizeof(coor1[0])),
            DoubleArray(coor2, sizeof(coor2)/sizeof(coor2[0])), "", symbols[i], 15);
    }

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "", "title='(x, y) = ({x}, {value})'"
        );

    return c;
}


BaseChart *scattersymbols(int img, const char **imageMap)
{
    // The XY points for the scatter chart
    double dataX[] = {200, 400, 300, 250, 500};
    double dataY[] = {40, 100, 50, 150, 250};

    // The custom symbols for the points
    const char *symbols[] = {"robot1.png", "robot2.png", "robot3.png", "robot4.png",
        "robot5.png"};

    // Create a XYChart object of size 450 x 400 pixels
    XYChart *c = new XYChart(450, 400);

    // Set the plotarea at (55, 40) and of size 350 x 300 pixels, with a light grey
    // border (0xc0c0c0). Turn on both horizontal and vertical grid lines with light
    // grey color (0xc0c0c0)
    c->setPlotArea(55, 40, 350, 300, -1, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a title to the chart using 18 pts Times Bold Itatic font.
    c->addTitle("Battle Robots", "timesbi.ttf", 18);

    // Add a title to the y axis using 12 pts Arial Bold Italic font
    c->yAxis()->setTitle("Speed (km/s)", "arialbi.ttf", 12);

    // Add a title to the y axis using 12 pts Arial Bold Italic font
    c->xAxis()->setTitle("Range (km)", "arialbi.ttf", 12);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);

    // Add each point of the data as a separate scatter layer, so that they can have
    // a different symbol
    int i;
    for(i = 0; i < sizeof(dataX) / sizeof(dataX[0]); ++i) {
        double coor1[] = {dataX[i]};
        double coor2[] = {dataY[i]};
        c->addScatterLayer(DoubleArray(coor1, sizeof(coor1)/sizeof(coor1[0])),
            DoubleArray(coor2, sizeof(coor2)/sizeof(coor2[0])))->getDataSet(0
            )->setDataSymbol(symbols[i]);
    }

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Range = {x} km, Speed = {value} km/s'");

    return c;
}


BaseChart *scatterlabels(int img, const char **imageMap)
{
    // The XY points for the scatter chart
    double dataX[] = {150, 400, 300, 1500, 800};
    double dataY[] = {0.6, 8, 5.4, 2, 4};

    // The labels for the points
    const char *labels[] = {"Nano\n100", "SpeedTron\n200 Lite", "SpeedTron\n200",
        "Marathon\nExtra", "Marathon\n2000"};

    // Create a XYChart object of size 450 x 400 pixels
    XYChart *c = new XYChart(450, 400);

    // Set the plotarea at (55, 40) and of size 350 x 300 pixels, with a light grey
    // border (0xc0c0c0). Turn on both horizontal and vertical grid lines with light
    // grey color (0xc0c0c0)
    c->setPlotArea(55, 40, 350, 300, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a title to the chart using 18 pts Times Bold Itatic font.
    c->addTitle("Product Comparison Chart", "timesbi.ttf", 18);

    // Add a title to the y axis using 12 pts Arial Bold Italic font
    c->yAxis()->setTitle("Capacity (tons)", "arialbi.ttf", 12);

    // Add a title to the x axis using 12 pts Arial Bold Italic font
    c->xAxis()->setTitle("Range (miles)", "arialbi.ttf", 12);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);

    // Add the data as a scatter chart layer, using a 15 pixel circle as the symbol
    ScatterLayer *layer = c->addScatterLayer(DoubleArray(dataX,
        sizeof(dataX)/sizeof(dataX[0])), DoubleArray(dataY,
        sizeof(dataY)/sizeof(dataY[0])), "Server BBB", Chart::CircleSymbol, 15,
        0xff3333, 0xff3333);

    // Add labels to the chart as an extra field
    layer->addExtraField(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Set the data label format to display the extra field
    layer->setDataLabelFormat("{field0}");

    // Use 8pts Arial Bold to display the labels
    TextBox *textbox = layer->setDataLabelStyle("arialbd.ttf", 8);

    // Set the background to purple with a 1 pixel 3D border
    textbox->setBackground(0xcc99ff, Chart::Transparent, 1);

    // Put the text box 4 pixels to the right of the data point
    textbox->setAlignment(Chart::Left);
    textbox->setPos(4, 0);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Range = {x} miles, Capacity = {value} tons'");

    return c;
}


BaseChart *bubble(int img, const char **imageMap)
{
    // The XYZ points for the bubble chart
    double dataX0[] = {150, 300, 1000, 1700};
    double dataY0[] = {12, 60, 25, 65};
    double dataZ0[] = {20, 50, 50, 85};

    double dataX1[] = {500, 1000, 1300};
    double dataY1[] = {35, 50, 75};
    double dataZ1[] = {30, 55, 95};

    // Create a XYChart object of size 450 x 420 pixels
    XYChart *c = new XYChart(450, 420);

    // Set the plotarea at (55, 65) and of size 350 x 300 pixels, with a light grey
    // border (0xc0c0c0). Turn on both horizontal and vertical grid lines with light
    // grey color (0xc0c0c0)
    c->setPlotArea(55, 65, 350, 300, -1, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 12
    // pts Times Bold Italic font. Set the background and border color to
    // Transparent.
    c->addLegend(50, 30, false, "timesbi.ttf", 12)->setBackground(Chart::Transparent)
        ;

    // Add a title to the chart using 18 pts Times Bold Itatic font.
    c->addTitle("Product Comparison Chart", "timesbi.ttf", 18);

    // Add a title to the y axis using 12 pts Arial Bold Italic font
    c->yAxis()->setTitle("Capacity (tons)", "arialbi.ttf", 12);

    // Add a title to the x axis using 12 pts Arial Bold Italic font
    c->xAxis()->setTitle("Range (miles)", "arialbi.ttf", 12);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);

    // Add (dataX0, dataY0) as a scatter layer with semi-transparent red (0x80ff3333)
    // circle symbols, where the circle size is modulated by dataZ0. This creates a
    // bubble effect.
    c->addScatterLayer(DoubleArray(dataX0, sizeof(dataX0)/sizeof(dataX0[0])),
        DoubleArray(dataY0, sizeof(dataY0)/sizeof(dataY0[0])), "Technology AAA",
        Chart::CircleSymbol, 9, 0x80FF3333, 0x80FF3333)->setSymbolScale(DoubleArray(
        dataZ0, sizeof(dataZ0)/sizeof(dataZ0[0])));

    // Add (dataX1, dataY1) as a scatter layer with semi-transparent green
    // (0x803333ff) circle symbols, where the circle size is modulated by dataZ1.
    // This creates a bubble effect.
    c->addScatterLayer(DoubleArray(dataX1, sizeof(dataX1)/sizeof(dataX1[0])),
        DoubleArray(dataY1, sizeof(dataY1)/sizeof(dataY1[0])), "Technology BBB",
        Chart::CircleSymbol, 9, 0x803333FF, 0x803333FF)->setSymbolScale(DoubleArray(
        dataZ1, sizeof(dataZ1)/sizeof(dataZ1[0])));

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{dataSetName}] Range = {x} miles, Capacity = {value} tons, Length "
        "= {z} meters'");

    return c;
}


BaseChart *bubblescale(int img, const char **imageMap)
{
    // The XY points for the bubble chart. The bubble chart has independent bubble
    // size on the X and Y direction.
    double dataX0[] = {1000, 1500, 1700};
    double dataY0[] = {25, 20, 65};
    double dataZX0[] = {500, 200, 600};
    double dataZY0[] = {15, 30, 20};

    double dataX1[] = {500, 1000, 1300};
    double dataY1[] = {35, 50, 75};
    double dataZX1[] = {800, 300, 500};
    double dataZY1[] = {8, 27, 25};

    double dataX2[] = {150, 300};
    double dataY2[] = {20, 60};
    double dataZX2[] = {160, 400};
    double dataZY2[] = {30, 20};

    // Create a XYChart object of size 450 x 420 pixels
    XYChart *c = new XYChart(450, 420);

    // Set the plotarea at (55, 65) and of size 350 x 300 pixels, with a light grey
    // border (0xc0c0c0). Turn on both horizontal and vertical grid lines with light
    // grey color (0xc0c0c0)
    c->setPlotArea(55, 65, 350, 300, -1, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 12
    // pts Times Bold Italic font. Set the background and border color to
    // Transparent.
    c->addLegend(50, 30, false, "timesbi.ttf", 12)->setBackground(Chart::Transparent)
        ;

    // Add a title to the chart using 18 pts Times Bold Itatic font.
    c->addTitle("Plasma Battery Comparison", "timesbi.ttf", 18);

    // Add titles to the axes using 12 pts Arial Bold Italic font
    c->yAxis()->setTitle("Operating Current", "arialbi.ttf", 12);
    c->xAxis()->setTitle("Operating Voltage", "arialbi.ttf", 12);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);

    // Add (dataX0, dataY0) as a standard scatter layer, and also as a "bubble"
    // scatter layer, using circles as symbols. The "bubble" scatter layer has symbol
    // size modulated by (dataZX0, dataZY0) using the scale on the x and y axes.
    c->addScatterLayer(DoubleArray(dataX0, sizeof(dataX0)/sizeof(dataX0[0])),
        DoubleArray(dataY0, sizeof(dataY0)/sizeof(dataY0[0])), "Vendor A",
        Chart::CircleSymbol, 9, 0xff3333, 0xff3333);
    c->addScatterLayer(DoubleArray(dataX0, sizeof(dataX0)/sizeof(dataX0[0])),
        DoubleArray(dataY0, sizeof(dataY0)/sizeof(dataY0[0])), "",
        Chart::CircleSymbol, 9, 0x80FF3333, 0x80FF3333)->setSymbolScale(DoubleArray(
        dataZX0, sizeof(dataZX0)/sizeof(dataZX0[0])), Chart::XAxisScale, DoubleArray(
        dataZY0, sizeof(dataZY0)/sizeof(dataZY0[0])), Chart::YAxisScale);

    // Add (dataX1, dataY1) as a standard scatter layer, and also as a "bubble"
    // scatter layer, using squares as symbols. The "bubble" scatter layer has symbol
    // size modulated by (dataZX1, dataZY1) using the scale on the x and y axes.
    c->addScatterLayer(DoubleArray(dataX1, sizeof(dataX1)/sizeof(dataX1[0])),
        DoubleArray(dataY1, sizeof(dataY1)/sizeof(dataY1[0])), "Vendor B",
        Chart::SquareSymbol, 7, 0x3333ff, 0x3333ff);
    c->addScatterLayer(DoubleArray(dataX1, sizeof(dataX1)/sizeof(dataX1[0])),
        DoubleArray(dataY1, sizeof(dataY1)/sizeof(dataY1[0])), "",
        Chart::SquareSymbol, 9, 0x803333FF, 0x803333FF)->setSymbolScale(DoubleArray(
        dataZX1, sizeof(dataZX1)/sizeof(dataZX1[0])), Chart::XAxisScale, DoubleArray(
        dataZY1, sizeof(dataZY1)/sizeof(dataZY1[0])), Chart::YAxisScale);

    // Add (dataX2, dataY2) as a standard scatter layer, and also as a "bubble"
    // scatter layer, using diamonds as symbols. The "bubble" scatter layer has
    // symbol size modulated by (dataZX2, dataZY2) using the scale on the x and y
    // axes.
    c->addScatterLayer(DoubleArray(dataX2, sizeof(dataX2)/sizeof(dataX2[0])),
        DoubleArray(dataY2, sizeof(dataY2)/sizeof(dataY2[0])), "Vendor C",
        Chart::DiamondSymbol, 9, 0x00ff00, 0x00ff00);
    c->addScatterLayer(DoubleArray(dataX2, sizeof(dataX2)/sizeof(dataX2[0])),
        DoubleArray(dataY2, sizeof(dataY2)/sizeof(dataY2[0])), "",
        Chart::DiamondSymbol, 9, 0x8033FF33, 0x8033FF33)->setSymbolScale(DoubleArray(
        dataZX2, sizeof(dataZX2)/sizeof(dataZX2[0])), Chart::XAxisScale, DoubleArray(
        dataZY2, sizeof(dataZY2)/sizeof(dataZY2[0])), Chart::YAxisScale);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Voltage = {x} +/- {={zx}/2} V, Current = {value} +/- {={zy}/2} A'");

    return c;
}


BaseChart *vector(int img, const char **imageMap)
{
    // The (x, y) coordinates of the starting point of the vectors
    double dataX[] = {20, 40, 60, 80, 100, 20, 40, 60, 80, 100, 20, 40, 58, 62, 80,
        100, 20, 40, 60, 80, 100, 20, 40, 60, 80, 100};
    double dataY[] = {20, 20, 20, 20, 20, 40, 40, 40, 40, 40, 60, 60, 60, 60, 60, 60,
        80, 80, 80, 80, 80, 100, 100, 100, 100, 100};

    // The lengths (radii) and directions (angles) of the vectors
    double dataR[] = {6, 6, 9, 6, 6, 6, 9, 12, 9, 6, 9, 12, 12, 12, 12, 9, 6, 9, 12,
        9, 6, 6, 6, 9, 6, 6};
    double dataA[] = {-45, -30, 0, 30, 45, -60, -45, 0, 45, 60, -90, -90, -90, 90,
        90, 90, -120, -135, 180, 135, 120, -135, -150, 180, 150, 135};

    // Create a XYChart object of size 450 x 390 pixels
    XYChart *c = new XYChart(450, 390);

    // Set the plotarea at (55, 40) and of size 350 x 300 pixels, with a light grey
    // border (0xc0c0c0). Turn on both horizontal and vertical grid lines with light
    // grey color (0xc0c0c0)
    c->setPlotArea(55, 40, 350, 300, -1, -1, 0xc0c0c0, 0xc0c0c0, -1);

    // Add a title to the chart using 18 pts Times Bold Itatic font.
    c->addTitle("Vector Chart Demonstration", "timesbi.ttf", 18);

    // Add a title to the y axis using 12 pts Arial Bold Italic font
    c->yAxis()->setTitle("Y Axis Title", "arialbi.ttf", 12);

    // Add a title to the x axis using 12 pts Arial Bold Italic font
    c->xAxis()->setTitle("X Axis Title", "arialbi.ttf", 12);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);

    // Add a vector layer to the chart using blue (0000CC) color, with vector arrow
    // size set to 11 pixels
    c->addVectorLayer(DoubleArray(dataX, sizeof(dataX)/sizeof(dataX[0])),
        DoubleArray(dataY, sizeof(dataY)/sizeof(dataY[0])), DoubleArray(dataR,
        sizeof(dataR)/sizeof(dataR[0])), DoubleArray(dataA,
        sizeof(dataA)/sizeof(dataA[0])), Chart::YAxisScale, 0x0000cc)->setArrowHead(
        11);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Vector at ({x}, {value}): Length = {len}, Angle = {dir} deg'");

    return c;
}


BaseChart *simplearea(int img, const char **imageMap)
{
    // The data for the area chart
    double data[] = {30, 28, 40, 55, 75, 68, 54, 60, 50, 62, 75, 65, 75, 89, 60, 55,
        53, 35, 50, 66, 56, 48, 52, 65, 62};

    // The labels for the area chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 250 x 250 pixels
    XYChart *c = new XYChart(250, 250);

    // Set the plotarea at (30, 20) and of size 200 x 200 pixels
    c->setPlotArea(30, 20, 200, 200);

    // Add an area chart layer using the given data
    c->addAreaLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])));

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis.
    c->xAxis()->setLabelStep(3);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Hour {xLabel}: Traffic {value} GBytes'");

    return c;
}


BaseChart *enhancedarea(int img, const char **imageMap)
{
    // The data for the area chart
    double data[] = {30, 28, 40, 55, 75, 68, 54, 60, 50, 62, 75, 65, 75, 89, 60, 55,
        53, 35, 50, 66, 56, 48, 52, 65, 62};

    // The labels for the area chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 600 x 300 pixels, with a pale blue (eeeeff)
    // background, black border, 1 pixel 3D border effect and rounded corners.
    XYChart *c = new XYChart(600, 300, 0xeeeeff, 0x000000, 1);
    c->setRoundedFrame();

    // Set the plotarea at (55, 55) and of size 520 x 195 pixels, with white (ffffff)
    // background. Set horizontal and vertical grid lines to grey (cccccc).
    c->setPlotArea(55, 55, 520, 195, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);

    // Add a title box to the chart using 15 pts Times Bold Italic font. The text is
    // white (ffffff) on a deep blue (000088) background, with soft lighting effect
    // from the right side.
    c->addTitle(
        "<*block,valign=absmiddle*><*img=star.png*><*img=star.png*> Performance "
        "Enhancer <*img=star.png*><*img=star.png*><*/*>", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x000088, -1, Chart::softLighting(Chart::Right));

    // Add a title to the y axis
    c->yAxis()->setTitle("Energy Concentration (KJ per liter)");

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis.
    c->xAxis()->setLabelStep(3);

    // Add a title to the x axis using CDML
    c->xAxis()->setTitle(
        "<*block,valign=absmiddle*><*img=clock.png*>  Elapsed Time (hour)<*/*>");

    // Set the axes width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add an area layer to the chart using a gradient color that changes vertically
    // from semi-transparent red (80ff0000) to semi-transparent white (80ffffff)
    c->addAreaLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])),
        c->linearGradientColor(0, 50, 0, 255, 0x80FF0000, 0x80FFFFFF));

    // Add a custom CDML text at the bottom right of the plot area as the logo
    c->addText(575, 245,
        "<*block,valign=absmiddle*><*img=small_molecule.png*> <*block*>"
        "<*font=timesbi.ttf,size=10,color=804040*>Molecular\nEngineering<*/*>"
        )->setAlignment(Chart::BottomRight);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Hour {xLabel}: {value} KJ/liter'");

    return c;
}


BaseChart *threedarea(int img, const char **imageMap)
{
    // The data for the area chart
    double data[] = {30, 28, 40, 55, 75, 68, 54, 60, 50, 62, 75, 65, 75, 89, 60, 55,
        53, 35, 50, 66, 56, 48, 52, 65, 62};

    // The labels for the area chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 300 x 300 pixels
    XYChart *c = new XYChart(300, 300);

    // Set the plotarea at (45, 30) and of size 200 x 200 pixels
    c->setPlotArea(45, 30, 200, 200);

    // Add a title to the chart using 12 pts Arial Bold Italic font
    c->addTitle("Daily Server Utilization", "arialbi.ttf", 12);

    // Add a title to the y axis
    c->yAxis()->setTitle("MBytes");

    // Add a title to the x axis
    c->xAxis()->setTitle("June 12, 2001");

    // Add a green (0x00ff00) 3D area chart layer using the give data
    c->addAreaLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), 0x00ff00
        )->set3D();

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis.
    c->xAxis()->setLabelStep(3);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Hour {xLabel}: Traffic {value} MBytes'");

    return c;
}


BaseChart *patternarea(int img, const char **imageMap)
{
    // The data for the area chart
    double data[] = {3.0, 2.8, 4.0, 5.5, 7.5, 6.8, 5.4, 6.0, 5.0, 6.2, 7.5, 6.5, 7.5,
        8.1, 6.0, 5.5, 5.3, 3.5, 5.0, 6.6, 5.6, 4.8, 5.2, 6.5, 6.2};

    // The labels for the area chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 300 x 180 pixels. Set the background to pale
    // yellow (0xffffa0) with a black border (0x0)
    XYChart *c = new XYChart(300, 180, 0xffffa0, 0x000000);

    // Set the plotarea at (45, 35) and of size 240 x 120 pixels. Set the background
    // to white (0xffffff). Set both horizontal and vertical grid lines to black
    // (&H0&) dotted lines (pattern code 0x0103)
    c->setPlotArea(45, 35, 240, 120, 0xffffff, -1, -1, c->dashLineColor(0x000000,
        0x000103), c->dashLineColor(0x000000, 0x000103));

    // Add a title to the chart using 10 pts Arial Bold font. Use a 1 x 2 bitmap
    // pattern as the background. Set the border to black (0x0).
    int pattern1[] = {0xb0b0f0, 0xe0e0ff};
    c->addTitle("Snow Percipitation (Dec 12)", "arialbd.ttf", 10)->setBackground(
        c->patternColor(IntArray(pattern1, sizeof(pattern1)/sizeof(pattern1[0])), 2),
        0x000000);

    // Add a title to the y axis
    c->yAxis()->setTitle("mm per hour");

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis.
    c->xAxis()->setLabelStep(3);

    // Add an area layer to the chart
    AreaLayer *layer = c->addAreaLayer();

    // Load a snow pattern from an external file "snow.png".
    int snowPattern = c->patternColor("snow.png");

    // Add a data set to the area layer using the snow pattern as the fill color. Use
    // deep blue (0x0000ff) as the area border line color (&H0000ff&)
    layer->addDataSet(DoubleArray(data, sizeof(data)/sizeof(data[0])))->setDataColor(
        snowPattern, 0x0000ff);

    // Set the line width to 2 pixels to highlight the line
    layer->setLineWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}:00 - {value} mm/hour'");

    return c;
}


BaseChart *stackedarea(int img, const char **imageMap)
{
    // The data for the area chart
    double data0[] = {42, 49, 33, 38, 51, 46, 29, 41, 44, 57, 59, 52, 37, 34, 51, 56,
        56, 60, 70, 76, 63, 67, 75, 64, 51};
    double data1[] = {50, 45, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67,
        58, 59, 73, 77, 84, 82, 80, 84, 89};
    double data2[] = {61, 79, 85, 66, 53, 39, 24, 21, 37, 56, 37, 22, 21, 33, 13, 17,
        4, 23, 16, 25, 9, 10, 5, 7, 16};
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 300 x 210 pixels. Set the background to pale
    // yellow (0xffffc0) with a black border (0x0)
    XYChart *c = new XYChart(300, 210, 0xffffc0, 0x000000);

    // Set the plotarea at (50, 30) and of size 240 x 140 pixels. Use white
    // (0xffffff) background.
    c->setPlotArea(50, 30, 240, 140)->setBackground(0xffffff);

    // Add a legend box at (50, 185) (below of plot area) using horizontal layout.
    // Use 8 pts Arial font with Transparent background.
    c->addLegend(50, 185, false, "", 8)->setBackground(Chart::Transparent);

    // Add a title box to the chart using 8 pts Arial Bold font, with yellow
    // (0xffff40) background and a black border (0x0)
    c->addTitle("Sales Volume", "arialbd.ttf", 8)->setBackground(0xffff40, 0);

    // Set the y axis label format to US$nnnn
    c->yAxis()->setLabelFormat("US${value}");

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
    // labels.
    c->xAxis()->setLabelStep(2, 1);

    // Add an stack area layer with three data sets
    AreaLayer *layer = c->addAreaLayer(Chart::Stack);
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0x4040ff,
        "Store #1");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0xff4040,
        "Store #2");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0x40ff40,
        "Store #3");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} sales at hour {xLabel}: US${value}K'");

    return c;
}


BaseChart *threedstackarea(int img, const char **imageMap)
{
    // The data for the area chart
    double data0[] = {42, 49, 33, 38, 51, 46, 29, 41, 44, 57, 59, 52, 37, 34, 51, 56,
        56, 60, 70, 76, 63, 67, 75, 64, 51};
    double data1[] = {50, 45, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67,
        58, 59, 73, 77, 84, 82, 80, 84, 89};
    double data2[] = {61, 79, 85, 66, 53, 39, 24, 21, 37, 56, 37, 22, 21, 33, 13, 17,
        4, 23, 16, 25, 9, 10, 5, 7, 16};
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 500 x 300 pixels
    XYChart *c = new XYChart(500, 300);

    // Set the plotarea at (90, 30) and of size 300 x 240 pixels.
    c->setPlotArea(90, 30, 300, 240);

    // Add a legend box at (405, 100)
    c->addLegend(405, 100);

    // Add a title to the chart
    c->addTitle("Daily System Load");

    // Add a title to the y axis. Draw the title upright (font angle = 0)
    c->yAxis()->setTitle("Database\nQueries\n(per sec)")->setFontAngle(0);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
    // labels.
    c->xAxis()->setLabelStep(2, 1);

    // Add an area layer
    AreaLayer *layer = c->addAreaLayer();

    // Draw the area layer in 3D
    layer->set3D();

    // Add the three data sets to the area layer
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), -1,
        "Server # 1");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), -1,
        "Server # 2");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), -1,
        "Server # 3");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} load at hour {xLabel}: {value} queries/sec'");

    return c;
}


BaseChart *percentarea(int img, const char **imageMap)
{
    // The data for the area chart
    double data0[] = {42, 49, 33, 38, 51, 46, 29, 41, 44, 57, 59, 52, 37, 34, 51, 56,
        56, 60, 70, 76, 63, 67, 75, 64, 51};
    double data1[] = {50, 55, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67,
        58, 59, 73, 77, 84, 82, 80, 84, 98};
    double data2[] = {87, 89, 85, 66, 53, 39, 24, 21, 37, 56, 37, 23, 21, 33, 13, 17,
        14, 23, 16, 25, 29, 30, 45, 47, 46};

    // The timestamps on the x-axis
    double labels[] = {chartTime(1996, 1, 1), chartTime(1996, 4, 1), chartTime(1996,
        7, 1), chartTime(1996, 10, 1), chartTime(1997, 1, 1), chartTime(1997, 4, 1),
        chartTime(1997, 7, 1), chartTime(1997, 10, 1), chartTime(1998, 1, 1),
        chartTime(1998, 4, 1), chartTime(1998, 7, 1), chartTime(1998, 10, 1),
        chartTime(1999, 1, 1), chartTime(1999, 4, 1), chartTime(1999, 7, 1),
        chartTime(1999, 10, 1), chartTime(2000, 1, 1), chartTime(2000, 4, 1),
        chartTime(2000, 7, 1), chartTime(2000, 10, 1), chartTime(2001, 1, 1),
        chartTime(2001, 4, 1), chartTime(2001, 7, 1), chartTime(2001, 10, 1),
        chartTime(2002, 1, 1)};

    // Create a XYChart object of size 500 x 280 pixels, using 0xffffcc as background
    // color, with a black border, and 1 pixel 3D border effect
    XYChart *c = new XYChart(500, 280, 0xffffcc, 0, 1);

    // Set the plotarea at (50, 45) and of size 320 x 200 pixels with white
    // background. Enable horizontal and vertical grid lines using the grey
    // (0xc0c0c0) color.
    c->setPlotArea(50, 45, 320, 200, 0xffffff)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a legend box at (370, 45) using vertical layout and 8 points Arial Bold
    // font.
    LegendBox *legendBox = c->addLegend(370, 45, true, "arialbd.ttf", 8);

    // Set the legend box background and border to transparent
    legendBox->setBackground(Chart::Transparent, Chart::Transparent);

    // Set the legend box icon size to 16 x 32 pixels to match with custom icon size
    legendBox->setKeySize(16, 32);

    // Add a title to the chart using 14 points Times Bold Itatic font and white font
    // color, and 0x804020 as the background color
    c->addTitle("Quarterly Product Sales", "timesbi.ttf", 14, 0xffffff
        )->setBackground(0x804020);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(DoubleArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Set multi-style axis label formatting. Start of year labels are displayed as
    // yyyy. For other labels, just show minor tick.
    c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(), "{value|yyyy}",
        Chart::AllPassFilter(), "-");

    // Add a percentage area layer to the chart
    AreaLayer *layer = c->addAreaLayer(Chart::Percentage);

    // Add the three data sets to the area layer, using icons images with labels as
    // data set names
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0x40ddaa77,
        "<*block,valign=absmiddle*><*img=service.png*> Service<*/*>");
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x40aadd77,
        "<*block,valign=absmiddle*><*img=software.png*> Software<*/*>");
    layer->addDataSet(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0x40aa77dd,
        "<*block,valign=absmiddle*><*img=computer.png*> Hardware<*/*>");

    // For a vertical stacked chart with positive data only, the last data set is
    // always on top. However, in a vertical legend box, the last data set is at the
    // bottom. This can be reversed by using the setLegend method.
    layer->setLegend(Chart::ReverseLegend);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} sales at {xLabel|yyyy} Q{=({xLabel|m}+2)/3|0}: "
        "US${value}K ({percent}%)'");

    return c;
}


BaseChart *deptharea(int img, const char **imageMap)
{
    // The data for the area chart
    double data0[] = {42, 49, 33, 38, 51, 46, 29, 41, 44, 57, 59, 52, 37, 34, 51, 56,
        56, 60, 70, 76, 63, 67, 75, 64, 51};
    double data1[] = {50, 55, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67,
        58, 59, 73, 77, 84, 82, 80, 84, 89};
    double data2[] = {87, 89, 85, 66, 53, 39, 24, 21, 37, 56, 37, 22, 21, 33, 13, 17,
        4, 23, 16, 25, 9, 10, 5, 7, 6};
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 350 x 230 pixels
    XYChart *c = new XYChart(350, 230);

    // Set the plotarea at (50, 30) and of size 250 x 150 pixels.
    c->setPlotArea(50, 30, 250, 150);

    // Add a legend box at (55, 0) (top of the chart) using 8 pts Arial Font. Set
    // background and border to Transparent.
    c->addLegend(55, 0, false, "", 8)->setBackground(Chart::Transparent);

    // Add a title to the x axis
    c->xAxis()->setTitle("Network Load for Jun 12");

    // Add a title to the y axis
    c->yAxis()->setTitle("MBytes");

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
    // labels.
    c->xAxis()->setLabelStep(2, 1);

    // Add three area layers, each representing one data set. The areas are drawn in
    // semi-transparent colors.
    c->addAreaLayer(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0x808080FF,
        "Server #1", 3);
    c->addAreaLayer(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0x80FF0000,
        "Server #2", 3);
    c->addAreaLayer(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x8000FF00,
        "Server #3", 3);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} load at hour {xLabel}: {value} MBytes'");

    return c;
}


BaseChart *rotatedarea(int img, const char **imageMap)
{
    // The data for the area chart
    double data[] = {30, 28, 40, 55, 75, 68, 54, 60, 50, 62, 75, 65, 75, 89, 60, 55,
        53, 35, 50, 66, 56, 48, 52, 65, 62};

    // The labels for the area chart
    double labels[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24};

    // Create a XYChart object of size 320 x 320 pixels
    XYChart *c = new XYChart(320, 320);

    // Swap the x and y axis to become a rotated chart
    c->swapXY();

    // Set the y axis on the top side (right + rotated = top)
    c->setYAxisOnRight();

    // Reverse the x axis so it is pointing downwards
    c->xAxis()->setReverse();

    // Set the plotarea at (50, 50) and of size 200 x 200 pixels. Enable horizontal
    // and vertical grids by setting their colors to grey (0xc0c0c0).
    c->setPlotArea(50, 50, 250, 250)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a line chart layer using the given data
    c->addAreaLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])),
        c->gradientColor(50, 0, 300, 0, 0xffffff, 0x0000ff));

    // Set the labels on the x axis. Append "m" after the value to show the unit.
    c->xAxis()->setLabels(DoubleArray(labels, sizeof(labels)/sizeof(labels[0])),
        "{value} m");

    // Display 1 out of 3 labels.
    c->xAxis()->setLabelStep(3);

    // Add a title to the x axis
    c->xAxis()->setTitle("Depth");

    // Add a title to the y axis
    c->yAxis()->setTitle("Carbon Dioxide Concentration (ppm)");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Carbon dioxide concentration at {xLabel}: {value} ppm'");

    return c;
}


BaseChart *boxwhisker(int img, const char **imageMap)
{
    // Sample data for the Box-Whisker chart. Represents the minimum, 1st quartile,
    // medium, 3rd quartile and maximum values of some quantities
    double Q0Data[] = {40, 45, 40, 30, 20, 50, 25, 44};
    double Q1Data[] = {55, 60, 50, 40, 38, 60, 51, 60};
    double Q2Data[] = {62, 70, 60, 50, 48, 70, 62, 70};
    double Q3Data[] = {70, 80, 65, 60, 53, 78, 69, 76};
    double Q4Data[] = {80, 90, 75, 70, 60, 85, 80, 84};

    // The labels for the chart
    const char *labels[] = {"Group A", "Group B", "Group C", "Group D", "Group E",
        "Group F", "Group G", "Group H"};

    // Create a XYChart object of size 550 x 250 pixels
    XYChart *c = new XYChart(550, 250);

    // Set the plotarea at (50, 25) and of size 450 x 200 pixels. Enable both
    // horizontal and vertical grids by setting their colors to grey (0xc0c0c0)
    c->setPlotArea(50, 25, 450, 200)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a title to the chart
    c->addTitle("Computer Vision Test Scores");

    // Set the labels on the x axis and the font to Arial Bold
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        )->setFontStyle("arialbd.ttf");

    // Set the font for the y axis labels to Arial Bold
    c->yAxis()->setLabelStyle("arialbd.ttf");

    // Add a Box Whisker layer using light blue 0x9999ff as the fill color and blue
    // (0xcc) as the line color. Set the line width to 2 pixels
    c->addBoxWhiskerLayer(DoubleArray(Q3Data, sizeof(Q3Data)/sizeof(Q3Data[0])),
        DoubleArray(Q1Data, sizeof(Q1Data)/sizeof(Q1Data[0])), DoubleArray(Q4Data,
        sizeof(Q4Data)/sizeof(Q4Data[0])), DoubleArray(Q0Data,
        sizeof(Q0Data)/sizeof(Q0Data[0])), DoubleArray(Q2Data,
        sizeof(Q2Data)/sizeof(Q2Data[0])), 0x9999ff, 0x0000cc)->setLineWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: min/med/max = {min}/{med}/{max}\nInter-quartile range: "
        "{bottom} to {top}'");

    return c;
}


BaseChart *boxwhisker2(int img, const char **imageMap)
{
    // Sample data for the Box-Whisker chart. Represents the minimum, 1st quartile,
    // medium, 3rd quartile and maximum values of some quantities
    double Q0Data[] = {40, 45, 35};
    double Q1Data[] = {55, 60, 50};
    double Q2Data[] = {62, 70, 60};
    double Q3Data[] = {70, 80, 65};
    double Q4Data[] = {80, 90, 75};

    // The labels for the chart
    const char *labels[] = {"<*img=robot1.png*><*br*>Bipedal Type",
        "<*img=robot2.png*><*br*>Wolf Type", "<*img=robot5.png*><*br*>Bird Type"};

    // Create a XYChart object of size 540 x 320 pixels
    XYChart *c = new XYChart(540, 320);

    // swap the x and y axes to create a horizontal box-whisker chart
    c->swapXY();

    // Set the plotarea at (75, 25) and of size 440 x 270 pixels. Enable both
    // horizontal and vertical grids by setting their colors to grey (0xc0c0c0)
    c->setPlotArea(75, 25, 440, 270)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a title to the chart
    c->addTitle("           Robot Shooting Accuracy Scores");

    // Set the labels on the x axis and the font to Arial Bold
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        )->setFontStyle("arialbd.ttf");

    // Disable x axis ticks by setting the length to 0
    c->xAxis()->setTickLength(0);

    // Set the font for the y axis labels to Arial Bold
    c->yAxis()->setLabelStyle("arialbd.ttf");

    // Add a Box Whisker layer using light blue 0x9999ff as the fill color and blue
    // (0xcc) as the line color. Set the line width to 2 pixels
    c->addBoxWhiskerLayer2(DoubleArray(Q3Data, sizeof(Q3Data)/sizeof(Q3Data[0])),
        DoubleArray(Q1Data, sizeof(Q1Data)/sizeof(Q1Data[0])), DoubleArray(Q4Data,
        sizeof(Q4Data)/sizeof(Q4Data[0])), DoubleArray(Q0Data,
        sizeof(Q0Data)/sizeof(Q0Data[0])), DoubleArray(Q2Data,
        sizeof(Q2Data)/sizeof(Q2Data[0])))->setLineWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: min/med/max = {min}/{med}/{max}\nInter-quartile range: "
        "{bottom} to {top}'");

    return c;
}


BaseChart *floatingbox(int img, const char **imageMap)
{
    // Sample data for the Box-Whisker chart. Represents the minimum, 1st quartile,
    // medium, 3rd quartile and maximum values of some quantities
    double Q0Data[] = {40, 45, 40, 30, 20, 50, 25, 44};
    double Q1Data[] = {55, 60, 50, 40, 38, 60, 51, 60};
    double Q2Data[] = {62, 70, 60, 50, 48, 70, 62, 70};
    double Q3Data[] = {70, 80, 65, 60, 53, 78, 69, 76};
    double Q4Data[] = {80, 90, 75, 70, 60, 85, 80, 84};

    // The labels for the chart
    const char *labels[] = {"Group A", "Group B", "Group C", "Group D", "Group E",
        "Group F", "Group G", "Group H"};

    // Create a XYChart object of size 550 x 250 pixels
    XYChart *c = new XYChart(550, 275);

    // Set the plotarea at (50, 25) and of size 450 x 200 pixels. Enable both
    // horizontal and vertical grids by setting their colors to grey (0xc0c0c0)
    c->setPlotArea(50, 50, 450, 200)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a title to the chart
    c->addTitle("Computer Vision Test Scores");

    // Set the labels on the x axis and the font to Arial Bold
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        )->setFontStyle("arialbd.ttf");

    // Set the font for the y axis labels to Arial Bold
    c->yAxis()->setLabelStyle("arialbd.ttf");

    // Add a Box Whisker layer using light blue 0x9999ff as the fill color and blue
    // (0xcc) as the line color. Set the line width to 2 pixels
    c->addBoxLayer(DoubleArray(Q4Data, sizeof(Q4Data)/sizeof(Q4Data[0])),
        DoubleArray(Q3Data, sizeof(Q3Data)/sizeof(Q3Data[0])), 0x00ff00, "Top 25%");
    c->addBoxLayer(DoubleArray(Q3Data, sizeof(Q3Data)/sizeof(Q3Data[0])),
        DoubleArray(Q2Data, sizeof(Q2Data)/sizeof(Q2Data[0])), 0x9999ff, "25% - 50%")
        ;
    c->addBoxLayer(DoubleArray(Q2Data, sizeof(Q2Data)/sizeof(Q2Data[0])),
        DoubleArray(Q1Data, sizeof(Q1Data)/sizeof(Q1Data[0])), 0xffff00, "50% - 75%")
        ;
    c->addBoxLayer(DoubleArray(Q1Data, sizeof(Q1Data)/sizeof(Q1Data[0])),
        DoubleArray(Q0Data, sizeof(Q0Data)/sizeof(Q0Data[0])), 0xff0000, "Bottom 25%"
        );

    // Add legend box at top center above the plot area using 10 pts Arial Bold Font
    LegendBox *b = c->addLegend(50 + 225, 22, false, "arialbd.ttf", 10);
    b->setAlignment(Chart::TopCenter);
    b->setBackground(Chart::Transparent);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel} ({dataSetName}): {bottom} to {top} points'");

    return c;
}


BaseChart *waterfall(int img, const char **imageMap)
{
    // 4 data points to represent the cash flow for the Q1 - Q4
    double data[] = {230, 140, 220, 330, 150};

    // We want to plot a waterfall chart showing the 4 quarters as well as the total
    const char *labels[] = {"Product 1", "Product 2", "Product 3", "Product 4",
        "Product 5", "Total"};

    // The top side of the bars in a waterfall chart is the accumulated data. We use
    // the ChartDirector ArrayMath utility to accumulate the data. The "total" is
    // handled by inserting a zero point at the end before accumulation (after
    // accumulation it will become the total).
    ArrayMath boxTop = ArrayMath(DoubleArray(data, sizeof(data)/sizeof(data[0]))
        ).insert(0, 1).acc();

    // The botom side of the bars is just the top side of the previous bar. So we
    // shifted the top side data to obtain the bottom side data.
    ArrayMath boxBottom = ArrayMath(boxTop).shift(1, 0);

    // The last point (total) is different. Its bottom side is always 0.
    boxBottom.trim(0, sizeof(data) / sizeof(data[0])).insert(0, 1);

    // Create a XYChart object of size 500 x 280 pixels. Set background color to
    // light blue (ccccff), with 1 pixel 3D border effect.
    XYChart *c = new XYChart(500, 290, 0xccccff, 0x000000, 1);

    // Add a title to the chart using 13 points Arial Bold Itatic font, with white
    // (ffffff) text on a deep blue (0x80) background
    c->addTitle("Product Revenue - Year 2004", "arialbi.ttf", 13, 0xffffff
        )->setBackground(0x000080);

    // Set the plotarea at (55, 50) and of size 430 x 215 pixels. Use alternative
    // white/grey background.
    c->setPlotArea(55, 45, 430, 215, 0xffffff, 0xeeeeee);

    // Set the labels on the x axis using Arial Bold font
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        )->setFontStyle("arialbd.ttf");

    // Set the x-axis ticks and grid lines to be between the bars
    c->xAxis()->setTickOffset(0.5);

    // Use Arial Bold as the y axis label font
    c->yAxis()->setLabelStyle("arialbd.ttf");

    // Add a title to the y axis
    c->yAxis()->setTitle("USD (in millions)");

    // Add a multi-color box-whisker layer to represent the waterfall bars
    BoxWhiskerLayer *layer = c->addBoxWhiskerLayer2(boxTop, boxBottom);

    // Put data labels on the bars to show the cash flow using Arial Bold font
    layer->setDataLabelFormat("{={top}-{bottom}}M");
    layer->setDataLabelStyle("arialbd.ttf")->setAlignment(Chart::Center);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: {={top}-{bottom}} millions'");

    return c;
}


BaseChart *posnegwaterfall(int img, const char **imageMap)
{
    // 4 data points to represent the cash flow for the Q1 - Q4
    double data[] = {230, -140, 220, 330};

    // We want to plot a waterfall chart showing the 4 quarters as well as the total
    const char *labels[] = {"1st Quarter", "2nd Quarter", "3rd Quarter",
        "4th Quarter", "Total"};

    // The top side of the bars in a waterfall chart is the accumulated data. We use
    // the ChartDirector ArrayMath utility to accumulate the data. The "total" is
    // handled by inserting a zero point at the end before accumulation (after
    // accumulation it will become the total).
    ArrayMath boxTop = ArrayMath(DoubleArray(data, sizeof(data)/sizeof(data[0]))
        ).insert(0, 1, sizeof(data) / sizeof(data[0])).acc();

    // The botom side of the bars is just the top side of the previous bar. So we
    // shifted the top side data to obtain the bottom side data.
    ArrayMath boxBottom = ArrayMath(boxTop).shift(1, 0);

    // The last point (total) is different. Its bottom side is always 0.
    boxBottom.trim(0, sizeof(data) / sizeof(data[0])).insert(0, 1);

    // In this example, we want to use different colors depending on the data is
    // positive or negative.
    int posColor = 0x00ff00;
    int negColor = 0xff0000;

    // Create a XYChart object of size 500 x 280 pixels. Set background color to
    // light blue (ccccff), with 1 pixel 3D border effect.
    XYChart *c = new XYChart(500, 300, 0xccccff, 0x000000, 1);

    // Add a title to the chart using 13 points Arial Bold Itatic font, with white
    // (ffffff) text on a deep blue (0x80) background
    c->addTitle("Corporate Cash Flow - Year 2004", "arialbi.ttf", 13, 0xffffff
        )->setBackground(0x000080);

    // Set the plotarea at (55, 50) and of size 430 x 215 pixels. Use alternative
    // white/grey background.
    c->setPlotArea(55, 50, 430, 215, 0xffffff, 0xeeeeee);

    // Add a legend box at (55, 25) using 8 pts Arial Bold font with horizontal
    // layout, with transparent background and border color.
    LegendBox *b = c->addLegend(55, 25, false, "arialbd.ttf", 8);
    b->setBackground(Chart::Transparent, Chart::Transparent);

    // Add keys to show the colors for positive and negative cash flows
    b->addKey("Positive Cash Flow", posColor);
    b->addKey("Negative Cash Flow", negColor);

    // Set the labels on the x axis using Arial Bold font
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        )->setFontStyle("arialbd.ttf");

    // Set the x-axis ticks and grid lines to be between the bars
    c->xAxis()->setTickOffset(0.5);

    // Use Arial Bold as the y axis label font
    c->yAxis()->setLabelStyle("arialbd.ttf");

    // Add a title to the y axis
    c->yAxis()->setTitle("USD (in millions)");

    // Add a box-whisker layer to represent the waterfall bars
    BoxWhiskerLayer *layer = c->addBoxWhiskerLayer(boxTop, boxBottom);

    DoubleArray boxTopData = boxTop.result();
    DoubleArray boxBottomData = boxBottom.result();
    for (int i = 0; i < boxTopData.len; ++i)
    {
        if (boxTopData[i] >= boxBottomData[i])
            layer->setBoxColor(i, posColor);
        else
            layer->setBoxColor(i, negColor);
    }

    // Put data labels on the bars to show the cash flow using Arial Bold font
    layer->setDataLabelFormat("{={top}-{bottom}}M");
    layer->setDataLabelStyle("arialbd.ttf")->setAlignment(Chart::Center);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: {={top}-{bottom}} millions'");

    return c;
}


BaseChart *gantt(int img, const char **imageMap)
{
    // data for the gantt chart, representing the start date, end date and names for
    // various activities
    double startDate[] = {chartTime(2004, 8, 16), chartTime(2004, 8, 30), chartTime(
        2004, 9, 13), chartTime(2004, 9, 20), chartTime(2004, 9, 27), chartTime(2004,
        10, 4), chartTime(2004, 10, 25), chartTime(2004, 11, 1), chartTime(2004, 11,
        8)};
    double endDate[] = {chartTime(2004, 8, 30), chartTime(2004, 9, 13), chartTime(
        2004, 9, 27), chartTime(2004, 10, 4), chartTime(2004, 10, 11), chartTime(
        2004, 11, 8), chartTime(2004, 11, 8), chartTime(2004, 11, 22), chartTime(
        2004, 11, 22)};
    const char *labels[] = {"Market Research", "Define Specifications",
        "Overall Archiecture", "Project Planning", "Detail Design",
        "Software Development", "Test Plan", "Testing and QA", "User Documentation"};

    // Create a XYChart object of size 620 x 280 pixels. Set background color to
    // light blue (ccccff), with 1 pixel 3D border effect.
    XYChart *c = new XYChart(620, 280, 0xccccff, 0x000000, 1);

    // Add a title to the chart using 15 points Times Bold Itatic font, with white
    // (ffffff) text on a deep blue (000080) background
    c->addTitle("Simple Gantt Chart Demo", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x000080);

    // Set the plotarea at (140, 55) and of size 460 x 200 pixels. Use alternative
    // white/grey background. Enable both horizontal and vertical grids by setting
    // their colors to grey (c0c0c0). Set vertical major grid (represents month
    // boundaries) 2 pixels in width
    c->setPlotArea(140, 55, 460, 200, 0xffffff, 0xeeeeee, Chart::LineColor, 0xc0c0c0,
        0xc0c0c0)->setGridWidth(2, 1, 1, 1);

    // swap the x and y axes to create a horziontal box-whisker chart
    c->swapXY();

    // Set the y-axis scale to be date scale from Aug 16, 2004 to Nov 22, 2004, with
    // ticks every 7 days (1 week)
    c->yAxis()->setDateScale(chartTime(2004, 8, 16), chartTime(2004, 11, 22), 86400 *
        7);

    // Set multi-style axis label formatting. Month labels are in Arial Bold font in
    // "mmm d" format. Weekly labels just show the day of month and use minor tick
    // (by using '-' as first character of format string).
    c->yAxis()->setMultiFormat(Chart::StartOfMonthFilter(),
        "<*font=arialbd.ttf*>{value|mmm d}", Chart::StartOfDayFilter(), "-{value|d}")
        ;

    // Set the y-axis to shown on the top (right + swapXY = top)
    c->setYAxisOnRight();

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Reverse the x-axis scale so that it points downwards.
    c->xAxis()->setReverse();

    // Set the horizontal ticks and grid lines to be between the bars
    c->xAxis()->setTickOffset(0.5);

    // Add a green (33ff33) box-whisker layer showing the box only.
    c->addBoxWhiskerLayer(DoubleArray(startDate,
        sizeof(startDate)/sizeof(startDate[0])), DoubleArray(endDate,
        sizeof(endDate)/sizeof(endDate[0])), DoubleArray(), DoubleArray(),
        DoubleArray(), 0x00cc00, Chart::SameAsMainColor, Chart::SameAsMainColor);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: {top|mmm dd, yyyy} to {bottom|mmm dd, yyyy}'");

    return c;
}


BaseChart *colorgantt(int img, const char **imageMap)
{
    // The tasks for the gantt chart
    const char *labels[] = {"Market Research", "Define Specifications",
        "Overall Archiecture", "Project Planning", "Detail Design",
        "Software Development", "Test Plan", "Testing and QA", "User Documentation"};

    // The task index, start date, end date and color for each bar
    double taskNo[] = {0, 0, 1, 2, 3, 4, 5, 6, 6, 7, 8, 8};
    double startDate[] = {chartTime(2004, 8, 16), chartTime(2004, 10, 4), chartTime(
        2004, 8, 30), chartTime(2004, 9, 13), chartTime(2004, 9, 20), chartTime(2004,
        9, 27), chartTime(2004, 10, 4), chartTime(2004, 10, 4), chartTime(2004, 10,
        25), chartTime(2004, 11, 1), chartTime(2004, 10, 18), chartTime(2004, 11, 8)}
        ;
    double endDate[] = {chartTime(2004, 8, 30), chartTime(2004, 10, 18), chartTime(
        2004, 9, 13), chartTime(2004, 9, 27), chartTime(2004, 10, 4), chartTime(2004,
        10, 11), chartTime(2004, 11, 8), chartTime(2004, 10, 18), chartTime(2004, 11,
        8), chartTime(2004, 11, 22), chartTime(2004, 11, 1), chartTime(2004, 11, 22)}
        ;
    int colors[] = {0x00cc00, 0x00cc00, 0x00cc00, 0x0000cc, 0x0000cc, 0xcc0000,
        0xcc0000, 0x0000cc, 0xcc0000, 0xcc0000, 0x00cc00, 0xcc0000};

    // Create a XYChart object of size 620 x 325 pixels. Set background color to
    // light red (0xffcccc), with 1 pixel 3D border effect.
    XYChart *c = new XYChart(620, 325, 0xffcccc, 0x000000, 1);

    // Add a title to the chart using 15 points Times Bold Itatic font, with white
    // (ffffff) text on a dark red (800000) background
    c->addTitle("Mutli-Color Gantt Chart Demo", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x800000);

    // Set the plotarea at (140, 55) and of size 460 x 200 pixels. Use alternative
    // white/grey background. Enable both horizontal and vertical grids by setting
    // their colors to grey (c0c0c0). Set vertical major grid (represents month
    // boundaries) 2 pixels in width
    c->setPlotArea(140, 55, 460, 200, 0xffffff, 0xeeeeee, Chart::LineColor, 0xc0c0c0,
        0xc0c0c0)->setGridWidth(2, 1, 1, 1);

    // swap the x and y axes to create a horziontal box-whisker chart
    c->swapXY();

    // Set the y-axis scale to be date scale from Aug 16, 2004 to Nov 22, 2004, with
    // ticks every 7 days (1 week)
    c->yAxis()->setDateScale(chartTime(2004, 8, 16), chartTime(2004, 11, 22), 86400 *
        7);

    // Set multi-style axis label formatting. Month labels are in Arial Bold font in
    // "mmm d" format. Weekly labels just show the day of month and use minor tick
    // (by using '-' as first character of format string).
    c->yAxis()->setMultiFormat(Chart::StartOfMonthFilter(),
        "<*font=arialbd.ttf*>{value|mmm d}", Chart::StartOfDayFilter(), "-{value|d}")
        ;

    // Set the y-axis to shown on the top (right + swapXY = top)
    c->setYAxisOnRight();

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Reverse the x-axis scale so that it points downwards.
    c->xAxis()->setReverse();

    // Set the horizontal ticks and grid lines to be between the bars
    c->xAxis()->setTickOffset(0.5);

    // Add some symbols to the chart to represent milestones. The symbols are added
    // using scatter layers. We need to specify the task index, date, name, symbol
    // shape, size and color.
    double coor1[] = {1};
    double date1[] = {chartTime(2004, 9, 13)};
    c->addScatterLayer(DoubleArray(coor1, sizeof(coor1)/sizeof(coor1[0])),
        DoubleArray(date1, sizeof(date1)/sizeof(date1[0])), "Milestone 1",
        Chart::Cross2Shape(), 13, 0xffff00)->setHTMLImageMap("{disable}");
    double coor2[] = {3};
    double date2[] = {chartTime(2004, 10, 4)};
    c->addScatterLayer(DoubleArray(coor2, sizeof(coor2)/sizeof(coor2[0])),
        DoubleArray(date2, sizeof(date2)/sizeof(date2[0])), "Milestone 2",
        Chart::StarShape(5), 15, 0xff00ff)->setHTMLImageMap("{disable}");
    double coor3[] = {5};
    double date3[] = {chartTime(2004, 11, 8)};
    c->addScatterLayer(DoubleArray(coor3, sizeof(coor3)/sizeof(coor3[0])),
        DoubleArray(date3, sizeof(date3)/sizeof(date3[0])), "Milestone 3",
        Chart::TriangleSymbol, 13, 0xff9933)->setHTMLImageMap("{disable}");

    // Add a multi-color box-whisker layer to represent the gantt bars
    BoxWhiskerLayer *layer = c->addBoxWhiskerLayer2(DoubleArray(startDate,
        sizeof(startDate)/sizeof(startDate[0])), DoubleArray(endDate,
        sizeof(endDate)/sizeof(endDate[0])), DoubleArray(), DoubleArray(),
        DoubleArray(), IntArray(colors, sizeof(colors)/sizeof(colors[0])));
    layer->setXData(DoubleArray(taskNo, sizeof(taskNo)/sizeof(taskNo[0])));
    layer->setBorderColor(Chart::SameAsMainColor);

    // Divide the plot area height ( = 200 in this chart) by the number of tasks to
    // get the height of each slot. Use 80% of that as the bar height.
    layer->setDataWidth(200 * 4 / 5 / (sizeof(labels) / sizeof(labels[0])));

    // Add a legend box at (140, 265) - bottom of the plot area. Use 8 pts Arial Bold
    // as the font with auto-grid layout. Set the width to the same width as the plot
    // area. Set the backgorund to grey (dddddd).
    LegendBox *legendBox = c->addLegend2(140, 265, Chart::AutoGrid, "arialbd.ttf", 8)
        ;
    legendBox->setWidth(461);
    legendBox->setBackground(0xdddddd);

    // The keys for the scatter layers (milestone symbols) will automatically be
    // added to the legend box. We just need to add keys to show the meanings of the
    // bar colors.
    legendBox->addKey("Market Team", 0x00cc00);
    legendBox->addKey("Planning Team", 0x0000cc);
    legendBox->addKey("Development Team", 0xcc0000);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel}: {top|mmm dd, yyyy} to {bottom|mmm dd, yyyy}'");

    return c;
}


BaseChart *layergantt(int img, const char **imageMap)
{
    // the names of the tasks
    const char *labels[] = {"Market Research", "Define Specifications",
        "Overall Archiecture", "Project Planning", "Detail Design",
        "Software Development", "Test Plan", "Testing and QA", "User Documentation"};

    // the planned start dates and end dates for the tasks
    double startDate[] = {chartTime(2004, 8, 16), chartTime(2004, 8, 30), chartTime(
        2004, 9, 13), chartTime(2004, 9, 20), chartTime(2004, 9, 27), chartTime(2004,
        10, 4), chartTime(2004, 10, 25), chartTime(2004, 11, 1), chartTime(2004, 11,
        8)};
    double endDate[] = {chartTime(2004, 8, 30), chartTime(2004, 9, 13), chartTime(
        2004, 9, 27), chartTime(2004, 10, 4), chartTime(2004, 10, 11), chartTime(
        2004, 11, 8), chartTime(2004, 11, 8), chartTime(2004, 11, 22), chartTime(
        2004, 11, 22)};

    // the actual start dates and end dates for the tasks up to now
    double actualStartDate[] = {chartTime(2004, 8, 16), chartTime(2004, 8, 27),
        chartTime(2004, 9, 9), chartTime(2004, 9, 18), chartTime(2004, 9, 22)};
    double actualEndDate[] = {chartTime(2004, 8, 27), chartTime(2004, 9, 9),
        chartTime(2004, 9, 27), chartTime(2004, 10, 2), chartTime(2004, 10, 8)};

    // Create a XYChart object of size 620 x 280 pixels. Set background color to
    // light green (ccffcc) with 1 pixel 3D border effect.
    XYChart *c = new XYChart(620, 280, 0xccffcc, 0x000000, 1);

    // Add a title to the chart using 15 points Times Bold Itatic font, with white
    // (ffffff) text on a dark green (0x6000) background
    c->addTitle("Mutli-Layer Gantt Chart Demo", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x006000);

    // Set the plotarea at (140, 55) and of size 460 x 200 pixels. Use alternative
    // white/grey background. Enable both horizontal and vertical grids by setting
    // their colors to grey (c0c0c0). Set vertical major grid (represents month
    // boundaries) 2 pixels in width
    c->setPlotArea(140, 55, 460, 200, 0xffffff, 0xeeeeee, Chart::LineColor, 0xc0c0c0,
        0xc0c0c0)->setGridWidth(2, 1, 1, 1);

    // swap the x and y axes to create a horziontal box-whisker chart
    c->swapXY();

    // Set the y-axis scale to be date scale from Aug 16, 2004 to Nov 22, 2004, with
    // ticks every 7 days (1 week)
    c->yAxis()->setDateScale(chartTime(2004, 8, 16), chartTime(2004, 11, 22), 86400 *
        7);

    // Add a red (ff0000) dash line to represent the current day
    c->yAxis()->addMark(chartTime(2004, 10, 8), c->dashLineColor(0xff0000,
        Chart::DashLine));

    // Set multi-style axis label formatting. Month labels are in Arial Bold font in
    // "mmm d" format. Weekly labels just show the day of month and use minor tick
    // (by using '-' as first character of format string).
    c->yAxis()->setMultiFormat(Chart::StartOfMonthFilter(),
        "<*font=arialbd.ttf*>{value|mmm d}", Chart::StartOfDayFilter(), "-{value|d}")
        ;

    // Set the y-axis to shown on the top (right + swapXY = top)
    c->setYAxisOnRight();

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Reverse the x-axis scale so that it points downwards.
    c->xAxis()->setReverse();

    // Set the horizontal ticks and grid lines to be between the bars
    c->xAxis()->setTickOffset(0.5);

    // Use blue (0000aa) as the color for the planned schedule
    int plannedColor = 0x0000aa;

    // Use a red hash pattern as the color for the actual dates. The pattern is
    // created as a 4 x 4 bitmap defined in memory as an array of colors.
    int pattern1[] = {0xffffff, 0xffffff, 0xffffff, 0xff0000, 0xffffff, 0xffffff,
        0xff0000, 0xffffff, 0xffffff, 0xff0000, 0xffffff, 0xffffff, 0xff0000,
        0xffffff, 0xffffff, 0xffffff};
    int actualColor = c->patternColor(IntArray(pattern1,
        sizeof(pattern1)/sizeof(pattern1[0])), 4);

    // Add a box whisker layer to represent the actual dates. We add the actual dates
    // layer first, so it will be the top layer.
    BoxWhiskerLayer *actualLayer = c->addBoxLayer(DoubleArray(actualStartDate,
        sizeof(actualStartDate)/sizeof(actualStartDate[0])), DoubleArray(
        actualEndDate, sizeof(actualEndDate)/sizeof(actualEndDate[0])), actualColor,
        "Actual");

    // Set the bar height to 8 pixels so they will not block the bottom bar
    actualLayer->setDataWidth(8);

    // Add a box-whisker layer to represent the planned schedule date
    c->addBoxLayer(DoubleArray(startDate, sizeof(startDate)/sizeof(startDate[0])),
        DoubleArray(endDate, sizeof(endDate)/sizeof(endDate[0])), plannedColor,
        "Planned")->setBorderColor(Chart::SameAsMainColor);

    // Add a legend box on the top right corner (595, 60) of the plot area with 8 pt
    // Arial Bold font. Use a semi-transparent grey (80808080) background.
    LegendBox *b = c->addLegend(595, 60, false, "arialbd.ttf", 8);
    b->setAlignment(Chart::TopRight);
    b->setBackground(0x80808080, -1, 2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel} ({dataSetName}): {top|mmm dd, yyyy} to {bottom|mmm dd, "
        "yyyy}'");

    return c;
}


BaseChart *hloc(int img, const char **imageMap)
{
    //
    // Sample data for the HLOC chart.
    //
    double highData[] = {2043, 2039, 2076, 2064, 2048, 2058, 2070, 2033, 2027, 2029,
        2071, 2085, 2034, 2031, 2056, 2128, 2180, 2183, 2192, 2213, 2230, 2281, 2272}
        ;

    double lowData[] = {1931, 1921, 1985, 2028, 1986, 1994, 1999, 1958, 1943, 1944,
        1962, 2011, 1975, 1962, 1928, 2059, 2112, 2103, 2151, 2127, 2123, 2152, 2212}
        ;

    double openData[] = {2000, 1957, 1993, 2037, 2018, 2021, 2045, 2009, 1959, 1985,
        2008, 2048, 2006, 2010, 1971, 2080, 2116, 2137, 2170, 2172, 2171, 2191, 2240}
        ;

    double closeData[] = {1950, 1991, 2026, 2029, 2004, 2053, 2011, 1962, 1987, 2019,
        2040, 2016, 1996, 1985, 2006, 2113, 2142, 2167, 2158, 2201, 2188, 2231, 2242}
        ;

    // The labels for the HLOC chart
    const char *labels[] = {"Mon 1", "Tue 2", "Wed 3", "Thu 4", "Fri 5", "Mon 8",
        "Tue 9", "Wed 10", "Thu 11", "Fri 12", "Mon 15", "Tue 16", "Wed 17",
        "Thu 18", "Fri 19", "Mon 22", "Tue 23", "Wed 24", "Thu 25", "Fri 26",
        "Mon 29", "Tue 30", "Wed 31"};

    // Create a XYChart object of size 600 x 350 pixels
    XYChart *c = new XYChart(600, 350);

    // Set the plotarea at (50, 25) and of size 500 x 250 pixels. Enable both the
    // horizontal and vertical grids by setting their colors to grey (0xc0c0c0)
    c->setPlotArea(50, 25, 500, 250)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a title to the chart
    c->addTitle("Universal Stock Index on Jan 2001");

    // Add a custom text at (50, 25) (the upper left corner of the plotarea). Use 12
    // pts Arial Bold/blue (4040c0) as the font.
    c->addText(50, 25, "(c) Global XYZ ABC Company", "arialbd.ttf", 12, 0x4040c0);

    // Add a title to the x axis
    c->xAxis()->setTitle("Jan 2001");

    // Set the labels on the x axis. Rotate the labels by 45 degrees.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        )->setFontAngle(45);

    // Add a title to the y axis
    c->yAxis()->setTitle("Universal Stock Index");

    // Draw the y axis on the right hand side of the plot area
    c->setYAxisOnRight(true);

    // Add a HLOC layer to the chart using green (008000) for up days and red
    // (ff0000) for down days
    HLOCLayer *layer = c->addHLOCLayer(DoubleArray(highData,
        sizeof(highData)/sizeof(highData[0])), DoubleArray(lowData,
        sizeof(lowData)/sizeof(lowData[0])), DoubleArray(openData,
        sizeof(openData)/sizeof(openData[0])), DoubleArray(closeData,
        sizeof(closeData)/sizeof(closeData[0])), 0x008000, 0xff0000);

    // Set the line width to 2 pixels
    layer->setLineWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel} Jan 2001\nHigh:{high}\nOpen:{open}\nClose:{close}\n"
        "Low:{low}'");

    return c;
}


BaseChart *candlestick(int img, const char **imageMap)
{
    //
    // Sample data for the CandleStick chart.
    //
    double highData[] = {2043, 2039, 2076, 2064, 2048, 2058, 2070, 2033, 2027, 2029,
        2071, 2085, 2034, 2031, 2056, 2128, 2180, 2183, 2192, 2213, 2230, 2281, 2272}
        ;

    double lowData[] = {1931, 1921, 1985, 2028, 1986, 1994, 1999, 1958, 1943, 1944,
        1962, 2011, 1975, 1962, 1928, 2059, 2112, 2103, 2151, 2127, 2123, 2152, 2212}
        ;

    double openData[] = {2000, 1957, 1993, 2037, 2018, 2021, 2045, 2009, 1959, 1985,
        2008, 2048, 2006, 2010, 1971, 2080, 2116, 2137, 2170, 2172, 2171, 2191, 2240}
        ;

    double closeData[] = {1950, 1991, 2026, 2029, 2004, 2053, 2011, 1962, 1987, 2019,
        2040, 2016, 1996, 1985, 2006, 2113, 2142, 2167, 2158, 2201, 2188, 2231, 2242}
        ;

    // The labels for the CandleStick chart
    const char *labels[] = {"Mon 1", "Tue 2", "Wed 3", "Thu 4", "Fri 5", "Mon 8",
        "Tue 9", "Wed 10", "Thu 11", "Fri 12", "Mon 15", "Tue 16", "Wed 17",
        "Thu 18", "Fri 19", "Mon 22", "Tue 23", "Wed 24", "Thu 25", "Fri 26",
        "Mon 29", "Tue 30", "Wed 31"};

    // Create a XYChart object of size 600 x 350 pixels
    XYChart *c = new XYChart(600, 350);

    // Set the plotarea at (50, 25) and of size 500 x 250 pixels. Enable both the
    // horizontal and vertical grids by setting their colors to grey (0xc0c0c0)
    c->setPlotArea(50, 25, 500, 250)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a title to the chart
    c->addTitle("Universal Stock Index on Jan 2001");

    // Add a custom text at (50, 25) (the upper left corner of the plotarea). Use 12
    // pts Arial Bold/blue (4040c0) as the font.
    c->addText(50, 25, "(c) Global XYZ ABC Company", "arialbd.ttf", 12, 0x4040c0);

    // Add a title to the x axis
    c->xAxis()->setTitle("Jan 2001");

    // Set the labels on the x axis. Rotate the labels by 45 degrees.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        )->setFontAngle(45);

    // Add a title to the y axis
    c->yAxis()->setTitle("Universal Stock Index");

    // Draw the y axis on the right hand side of the plot area
    c->setYAxisOnRight(true);

    // Reserve 10% margin at the top and bottom of the plot area during auto-scaling.
    // This is to leave space for the high and low data labels.
    c->yAxis()->setAutoScale(0.1, 0.1);

    // Add a CandleStick layer to the chart using green (00ff00) for up candles and
    // red (ff0000) for down candles
    CandleStickLayer *layer = c->addCandleStickLayer(DoubleArray(highData,
        sizeof(highData)/sizeof(highData[0])), DoubleArray(lowData,
        sizeof(lowData)/sizeof(lowData[0])), DoubleArray(openData,
        sizeof(openData)/sizeof(openData[0])), DoubleArray(closeData,
        sizeof(closeData)/sizeof(closeData[0])), 0x00ff00, 0xff0000);

    // Set the line width to 2 pixels
    layer->setLineWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{xLabel} Jan 2001\nHigh:{high}\nOpen:{open}\nClose:{close}\n"
        "Low:{low}'");

    return c;
}


BaseChart *finance(int img, const char **imageMap)
{
    // Create a finance chart demo containing 100 days of data
    int noOfDays = 100;

    // To compute moving averages starting from the first day, we need to get extra
    // data points before the first day
    int extraDays = 30;

    // In this exammple, we use a random number generator utility to simulate the
    // data. We set up the random table to create 6 cols x (noOfDays + extraDays)
    // rows, using 9 as the seed.
    RanTable *rantable = new RanTable(9, 6, noOfDays + extraDays);

    // Set the 1st col to be the timeStamp, starting from Sep 4, 2002, with each row
    // representing one day, and counting week days only (jump over Sat and Sun)
    rantable->setDateCol(0, chartTime(2002, 9, 4), 86400, true);

    // Set the 2nd, 3rd, 4th and 5th columns to be high, low, open and close data.
    // The open value starts from 100, and the daily change is random from -5 to 5.
    rantable->setHLOCCols(1, 100, -5, 5);

    // Set the 6th column as the vol data from 5 to 25 million
    rantable->setCol(5, 50000000, 250000000);

    // Now we read the data from the table into arrays
    DoubleArray timeStamps = rantable->getCol(0);
    DoubleArray highData = rantable->getCol(1);
    DoubleArray lowData = rantable->getCol(2);
    DoubleArray openData = rantable->getCol(3);
    DoubleArray closeData = rantable->getCol(4);
    DoubleArray volData = rantable->getCol(5);

    // Create a FinanceChart object of width 600 pixels
    FinanceChart *c = new FinanceChart(600);

    // Add a title to the chart
    c->addTitle("Finance Chart Demonstration");

    // Set the data into the finance chart object
    c->setData(timeStamps, highData, lowData, openData, closeData, volData, extraDays
        );

    // Add the main chart with 210 pixels in height
    c->addMainChart(210);

    // Add a 5 period simple moving average to the main chart, using brown color
    c->addSimpleMovingAvg(5, 0x663300);

    // Add a 20 period simple moving average to the main chart, using purple color
    c->addSimpleMovingAvg(20, 0x9900ff);

    // Add an HLOC symbols to the main chart, using green/red for up/down days
    c->addHLOC(0x008000, 0xcc0000);

    // Add 20 days bollinger band to the main chart, using light blue (9999ff) as the
    // border and semi-transparent blue (c06666ff) as the fill color
    c->addBollingerBand(20, 2, 0x9999ff, 0xC06666FF);

    // Add a 70 pixels volume bars sub-chart to the bottom of the main chart, using
    // green/red/grey for up/down/flat days
    c->addVolBars(70, 0x99ff99, 0xff9999, 0x808080);

    // Append a 14-days RSI indicator chart (70 pixels height) after the main chart.
    // The main RSI line is purple (800080). Set threshold region to +/- 20 (that is,
    // RSI = 50 +/- 25). The upper/lower threshold regions will be filled with red
    // (ff0000)/blue (0000ff).
    c->addRSI(70, 14, 0x800080, 20, 0xff0000, 0x0000ff);

    // Append a 12-days momentum indicator chart (70 pixels height) using blue
    // (0000ff) color.
    c->addMomentum(70, 12, 0x0000ff);

    // output the chart
    c->makeChart();

    //free up resources
    delete rantable;

    return c;
}


BaseChart *finance2(int img, const char **imageMap)
{
    // Create a finance chart demo containing 100 days of data
    int noOfDays = 100;

    // To compute moving averages starting from the first day, we need to get extra
    // data points before the first day
    int extraDays = 30;

    // In this exammple, we use a random number generator utility to simulate the
    // data. We set up the random table to create 6 cols x (noOfDays + extraDays)
    // rows, using 9 as the seed.
    RanTable *rantable = new RanTable(9, 6, noOfDays + extraDays);

    // Set the 1st col to be the timeStamp, starting from Sep 4, 2002, with each row
    // representing one day, and counting week days only (jump over Sat and Sun)
    rantable->setDateCol(0, chartTime(2002, 9, 4), 86400, true);

    // Set the 2nd, 3rd, 4th and 5th columns to be high, low, open and close data.
    // The open value starts from 100, and the daily change is random from -5 to 5.
    rantable->setHLOCCols(1, 100, -5, 5);

    // Set the 6th column as the vol data from 5 to 25 million
    rantable->setCol(5, 50000000, 250000000);

    // Now we read the data from the table into arrays
    DoubleArray timeStamps = rantable->getCol(0);
    DoubleArray highData = rantable->getCol(1);
    DoubleArray lowData = rantable->getCol(2);
    DoubleArray openData = rantable->getCol(3);
    DoubleArray closeData = rantable->getCol(4);
    DoubleArray volData = rantable->getCol(5);

    // Create a FinanceChart object of width 600 pixels
    FinanceChart *c = new FinanceChart(600);

    // Add a title to the chart
    c->addTitle("Finance Chart Demonstration");

    // Set the data into the finance chart object
    c->setData(timeStamps, highData, lowData, openData, closeData, volData, extraDays
        );

    // Add a slow stochastic chart with %K = 14 and %D = 3
    c->addSlowStochastic(70, 14, 3, 0x006060, 0x606000);

    // Add the main chart with 210 pixels in height
    c->addMainChart(210);

    // Add a 10 period simple moving average to the main chart, using brown color
    c->addSimpleMovingAvg(10, 0x663300);

    // Add a 20 period simple moving average to the main chart, using purple color
    c->addSimpleMovingAvg(20, 0x9900ff);

    // Add an HLOC symbols to the main chart, using green/red for up/down days
    c->addCandleStick(0x00ff00, 0xff0000);

    // Add 20 days donchian channel to the main chart, using light blue (9999ff) as
    // the border and semi-transparent blue (c06666ff) as the fill color
    c->addDonchianChannel(20, 0x9999ff, 0xC06666FF);

    // Add a 70 pixels volume bars sub-chart to the bottom of the main chart, using
    // green/red/grey for up/down/flat days
    c->addVolBars(70, 0x99ff99, 0xff9999, 0x808080);

    // Append a MACD(26, 12) indicator chart (70 pixels height) after the main chart,
    // using 9 days for computing divergence.
    c->addMACD(70, 26, 12, 9, 0x0000ff, 0xff00ff, 0x008000);

    // output the chart
    c->makeChart();

    //free up resources
    delete rantable;

    return c;
}


BaseChart *markzone(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {40, 45, 37, 24, 32, 39, 53, 52, 63, 49, 46, 40, 54, 50, 57, 57,
        48, 49, 63, 67, 74, 72, 70, 89, 74};
    const char *labels[] = {"0\nJun 4", "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22",
        "23", "0\nJun 5"};

    // Create a XYChart object of size 400 x 270 pixels
    XYChart *c = new XYChart(400, 270);

    // Set the plotarea at (80, 60) and of size 300 x 200 pixels. Turn off the grid
    // lines by setting their colors to Transparent.
    c->setPlotArea(80, 28, 300, 200)->setGridColor(Chart::Transparent);

    // Add a title to the y axis
    TextBox *textbox = c->yAxis()->setTitle("Temperature");

    // Set the y axis title upright (font angle = 0)
    textbox->setFontAngle(0);

    // Put the y axis title on top of the axis
    textbox->setAlignment(Chart::TopLeft2);

    // Add green (0x99ff99), yellow (0xffff99) and red (0xff9999) zones to the y axis
    // to represent the ranges 0 - 50, 50 - 80, and 80 - max.
    c->yAxis()->addZone(0, 50, 0x99ff99);
    c->yAxis()->addZone(50, 80, 0xffff99);
    c->yAxis()->addZone(80, 9999, 0xff9999);

    // Add a purple (0x800080) mark at y = 70 using a line width of 2.
    c->yAxis()->addMark(70, 0x800080, "Alert = 70")->setLineWidth(2);

    // Add a green (0x008000) mark at y = 40 using a line width of 2.
    c->yAxis()->addMark(40, 0x008000, "Watch = 40")->setLineWidth(2);

    // Add a legend box at (165, 0) (top right of the chart) using 8 pts Arial font.
    // and horizontal layout.
    LegendBox *legend = c->addLegend(165, 0, false, "arialbd.ttf", 8);

    // Disable the legend box boundary by setting the colors to Transparent
    legend->setBackground(Chart::Transparent, Chart::Transparent);

    // Add 3 custom entries to the legend box to represent the 3 zones
    legend->addKey("Normal", 0x80ff80);
    legend->addKey("Warning", 0xffff80);
    legend->addKey("Critical", 0xff8080);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis. Show minor ticks for remaining
    // labels.
    c->xAxis()->setLabelStep(3, 1);

    // Add a 3D bar layer with the given data
    BarLayer *layer = c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])),
        0xbbbbff);

    // Set the bar gap to 0 so that the bars are packed tightly
    layer->setBarGap(0);

    // Set the border color of the bars same as the fill color, with 1 pixel 3D
    // border effect.
    layer->setBorderColor(Chart::SameAsMainColor, 1);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Temperature at {x}:00 = {value} C'");

    return c;
}


BaseChart *markzone2(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {50, 55, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67,
        58, 59, 73, 77, 84, 82, 80, 84, 89};

    // The labels for the chart. Note the "-" means a minor tick.
    const char *labels[] = {"0\nJun 4", "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22",
        "23", "0\nJun 5"};

    // Create a XYChart object of size 400 x 270 pixels
    XYChart *c = new XYChart(400, 270);

    // Set the plotarea at (80, 25) and of size 300 x 200 pixels. Use alternate color
    // background (0xeeeeee) and (0xffffff). Set border and grid colors to grey
    // (0xc0c0c0).
    c->setPlotArea(50, 25, 300, 200, 0xeeeeee, 0xffffff, 0xc0c0c0, 0xc0c0c0, 0xc0c0c0
        );

    // Add a title to the chart using 14 pts Times Bold Italic font
    c->addTitle("Server Monitor", "timesbi.ttf", 14);

    // Add a title to the y axis
    c->yAxis()->setTitle("Server Load (MBytes)");

    // Set the y axis width to 2 pixels
    c->yAxis()->setWidth(2);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis. Show minor ticks for remaining
    // labels.
    c->xAxis()->setLabelStep(3, 1);

    // Set the x axis width to 2 pixels
    c->xAxis()->setWidth(2);

    // Add a horizontal red (0x800080) mark line at y = 80
    Mark *yMark = c->yAxis()->addMark(80, 0xff0000, "Critical Threshold Set Point");

    // Set the mark line width to 2 pixels
    yMark->setLineWidth(2);

    // Put the mark label at the top center of the mark line
    yMark->setAlignment(Chart::TopCenter);

    // Add an orange (0xffcc66) zone from x = 18 to x = 20
    c->xAxis()->addZone(18, 20, 0xffcc66);

    // Add a vertical brown (0x995500) mark line at x = 18
    Mark *xMark1 = c->xAxis()->addMark(18, 0x995500, "Backup Start");

    // Set the mark line width to 2 pixels
    xMark1->setLineWidth(2);

    // Put the mark label at the left of the mark line
    xMark1->setAlignment(Chart::Left);

    // Rotate the mark label by 90 degrees so it draws vertically
    xMark1->setFontAngle(90);

    // Add a vertical brown (0x995500) mark line at x = 20
    Mark *xMark2 = c->xAxis()->addMark(20, 0x995500, "Backup End");

    // Set the mark line width to 2 pixels
    xMark2->setLineWidth(2);

    // Put the mark label at the right of the mark line
    xMark2->setAlignment(Chart::Right);

    // Rotate the mark label by 90 degrees so it draws vertically
    xMark2->setFontAngle(90);

    // Add a green (0x00cc00) line layer with line width of 2 pixels
    c->addLineLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), 0x00cc00
        )->setLineWidth(2);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Load at {x}:00 = {value} MBytes'");

    return c;
}


BaseChart *yzonecolor(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {30, 28, 40, 55, 75, 68, 54, 60, 50, 62, 75, 65, 75, 89, 60, 55,
        53, 35, 50, 66, 56, 48, 52, 65, 62};

    // The labels for the chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 500 x 300 pixels, with a pale yellow
    // (0xffffc0) background, a black border, and 1 pixel 3D border effect
    XYChart *c = new XYChart(500, 300, 0xffffc0, 0x000000, 1);

    // Set the plotarea at (55, 50) and of size 420 x 205 pixels, with white
    // background. Turn on both horizontal and vertical grid lines with light grey
    // color (0xc0c0c0)
    c->setPlotArea(55, 50, 420, 205, 0xffffff)->setGridColor(0xc0c0c0, 0xc0c0c0);

    // Add a legend box at (55, 25) (top of the chart) with horizontal layout. Use 8
    // pts Arial font. Set the background and border color to Transparent.
    LegendBox *legendBox = c->addLegend(55, 25, false, "", 8);
    legendBox->setBackground(Chart::Transparent);

    // Add keys to the legend box to explain the color zones
    legendBox->addKey("Normal Zone", 0x8033FF33);
    legendBox->addKey("Alert Zone", 0x80FF3333);

    // Add a title box to the chart using 13 pts Arial Bold Italic font. The title is
    // in CDML and includes embedded images for highlight. The text is white
    // (0xffffff) on a black background, with a 1 pixel 3D border.
    c->addTitle(
        "<*block,valign=absmiddle*><*img=star.png*><*img=star.png*> Y Zone Color "
        "Demo <*img=star.png*><*img=star.png*><*/*>", "arialbi.ttf", 13, 0xffffff
        )->setBackground(0x000000, -1, 1);

    // Add a title to the y axis
    c->yAxis()->setTitle("Energy Concentration (KJ per liter)");

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis.
    c->xAxis()->setLabelStep(3);

    // Add a title to the x axis using CDML
    c->xAxis()->setTitle(
        "<*block,valign=absmiddle*><*img=clock.png*>  Elapsed Time (hour)<*/*>");

    // Set the axes width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add an area layer to the chart. The area is using a y zone color, where the
    // color is semi-transparent green below 60, and semi-transparent red above 60.
    c->addAreaLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), c->yZoneColor(
        60, 0x8033FF33, 0x80FF3333));

    // Add a custom CDML text at the bottom right of the plot area as the logo
    c->addText(475, 255,
        "<*block,valign=absmiddle*><*img=small_molecule.png*> <*block*>"
        "<*font=timesbi.ttf,size=10,color=804040*>Molecular\nEngineering<*/*>"
        )->setAlignment(Chart::BottomRight);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Hour {xLabel}: {value} KJ/liter'");

    return c;
}


BaseChart *xzonecolor(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {50, 55, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67,
        58, 59, 73, 77, 84, 82, 80, 84, 89};

    // The error data representing the error band around the data points
    double errData[] = {5, 6, 5.1, 6.5, 6.6, 8, 5.4, 5.1, 4.6, 5.0, 5.2, 6.0, 4.9,
        5.6, 4.8, 6.2, 7.4, 7.1, 6.5, 9.6, 12.1, 15.3, 18.5, 20.9, 24.1};

    // The timestamps for the data
    double labels[] = {chartTime(2001, 1, 1), chartTime(2001, 2, 1), chartTime(2001,
        3, 1), chartTime(2001, 4, 1), chartTime(2001, 5, 1), chartTime(2001, 6, 1),
        chartTime(2001, 7, 1), chartTime(2001, 8, 1), chartTime(2001, 9, 1),
        chartTime(2001, 10, 1), chartTime(2001, 11, 1), chartTime(2001, 12, 1),
        chartTime(2002, 1, 1), chartTime(2002, 2, 1), chartTime(2002, 3, 1),
        chartTime(2002, 4, 1), chartTime(2002, 5, 1), chartTime(2002, 6, 1),
        chartTime(2002, 7, 1), chartTime(2002, 8, 1), chartTime(2002, 9, 1),
        chartTime(2002, 10, 1), chartTime(2002, 11, 1), chartTime(2002, 12, 1),
        chartTime(2003, 1, 1)};

    // Create a XYChart object of size 550 x 220 pixels
    XYChart *c = new XYChart(550, 220);

    // Set the plot area at (50, 10) and of size 480 x 180 pixels. Enabled both
    // vertical and horizontal grids by setting their colors to light grey (cccccc)
    c->setPlotArea(50, 10, 480, 180)->setGridColor(0xcccccc, 0xcccccc);

    // Add a legend box (50, 10) (top of plot area) using horizontal layout. Use 8
    // pts Arial font. Disable bounding box (set border to transparent).
    LegendBox *legendBox = c->addLegend(50, 10, false, "", 8);
    legendBox->setBackground(Chart::Transparent);

    // Add keys to the legend box to explain the color zones
    legendBox->addKey("Historical", 0x9999ff);
    legendBox->addKey("Forecast", 0xff9966);

    // Add a title to the y axis.
    c->yAxis()->setTitle("Energy Consumption");

    // Set the labels on the x axis
    c->xAxis()->setLabels(DoubleArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Set multi-style axis label formatting. Use Arial Bold font for yearly labels
    // and display them as "yyyy". Use default font for monthly labels and display
    // them as "mmm". Replace some labels with minor ticks to ensure the labels are
    // at least 3 units apart.
    c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(),
        "<*font=arialbd.ttf*>{value|yyyy}", Chart::StartOfMonthFilter(),
        "{value|mmm}", 3);

    // Add a line layer to the chart
    LineLayer *layer = c->addLineLayer();

    // Create the color to draw the data line. The line is blue (0x333399) to the
    // left of x = 18, and become a red (0xd04040) dash line to the right of x = 18.
    int lineColor = layer->xZoneColor(18, 0x333399, c->dashLineColor(0xd04040,
        Chart::DashLine));

    // Add the data line
    layer->addDataSet(DoubleArray(data, sizeof(data)/sizeof(data[0])), lineColor,
        "Average");

    // We are not showing the data set name in the legend box. The name is for
    // showing in tool tips only.
    layer->setLegend(Chart::NoLegend);

    // Create the color to draw the err zone. The color is semi-transparent blue
    // (0x809999ff) to the left of x = 18, and become semi-transparent red
    // (0x80ff9966) to the right of x = 18.
    int errColor = layer->xZoneColor(18, 0x809999FF, 0x80FF9966);

    // Add the upper border of the err zone
    layer->addDataSet(ArrayMath(DoubleArray(data, sizeof(data)/sizeof(data[0]))).add(
        DoubleArray(errData, sizeof(errData)/sizeof(errData[0]))), errColor,
        "Upper bound");

    // Add the lower border of the err zone
    layer->addDataSet(ArrayMath(DoubleArray(data, sizeof(data)/sizeof(data[0]))).sub(
        DoubleArray(errData, sizeof(errData)/sizeof(errData[0]))), errColor,
        "Lower bound");

    // Set the default line width to 2 pixels
    layer->setLineWidth(2);

    // In this example, we are not showing the data set name in the legend box
    layer->setLegend(Chart::NoLegend);

    // Color the region between the err zone lines
    c->addInterLineLayer(layer->getLine(1), layer->getLine(2), errColor);

    // output the chart
    c->makeChart();

    // Include tool tip for the chart.
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} on {xLabel|mmm yyyy}: {value} MJoule'");

    return c;
}


BaseChart *dualyaxis(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {0.05, 0.06, 0.48, 0.1, 0.01, 0.05};
    double data1[] = {100, 125, 265, 147, 67, 105};
    const char *labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun"};

    // Create a XYChart object of size 300 x 180 pixels
    XYChart *c = new XYChart(300, 180);

    // Set the plot area at (50, 20) and of size 200 x 130 pixels
    c->setPlotArea(50, 20, 200, 130);

    // Add a title to the chart using 8 pts Arial Bold font
    c->addTitle("Independent Y-Axis Demo", "arialbd.ttf", 8);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a title to the primary (left) y axis
    c->yAxis()->setTitle("Packet Drop Rate (pps)");

    // Set the axis, label and title colors for the primary y axis to red (0xc00000)
    // to match the first data set
    c->yAxis()->setColors(0xc00000, 0xc00000, 0xc00000);

    // Add a title to the secondary (right) y axis
    c->yAxis2()->setTitle("Throughtput (MBytes)");

    // set the axis, label and title colors for the primary y axis to green
    // (0x008000) to match the second data set
    c->yAxis2()->setColors(0x008000, 0x008000, 0x008000);

    // Add a line layer to for the first data set using red (0xc00000) color with a
    // line width to 3 pixels
    LineLayer *lineLayer = c->addLineLayer(DoubleArray(data0,
        sizeof(data0)/sizeof(data0[0])), 0xc00000);
    lineLayer->setLineWidth(3);

    // tool tip for the line layer
    lineLayer->setHTMLImageMap("", "",
        "title='Packet Drop Rate on {xLabel}: {value} pps'");

    // Add a bar layer to for the second data set using green (0x00C000) color. Bind
    // the second data set to the secondary (right) y axis
    BarLayer *barLayer = c->addBarLayer(DoubleArray(data1,
        sizeof(data1)/sizeof(data1[0])), 0x00c000);
    barLayer->setUseYAxis2();

    // tool tip for the bar layer
    barLayer->setHTMLImageMap("", "",
        "title='Throughput on {xLabel}: {value} MBytes'");

    // output the chart
    c->makeChart();

    // include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable");

    return c;
}


BaseChart *dualxaxis(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {42, 49, 33, 38, 51, 46, 29, 41, 44, 57, 59, 52, 37, 34, 51, 56,
        56, 60, 70, 76, 63, 67, 75, 64, 51};
    double data1[] = {50, 55, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67,
        58, 59, 73, 77, 84, 82, 80, 84, 98};

    // The labels for the bottom x axis. Note the "-" means a minor tick.
    const char *labels0[] = {"0\nJun 4", "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22",
        "23", "0\nJun 5"};

    // The labels for the top x axis. Note that "-" means a minor tick.
    const char *labels1[] = {"Jun 3\n12", "13", "14", "15", "16", "17", "18", "19",
        "20", "21", "22", "23", "Jun 4\n0", "1", "2", "3", "4", "5", "6", "7", "8",
        "9", "10", "11", "12"};

    // Create a XYChart object of size 310 x 310 pixels
    XYChart *c = new XYChart(310, 310);

    // Set the plotarea at (50, 50) and of size 200 x 200 pixels
    c->setPlotArea(50, 50, 200, 200);

    // Add a title to the primary (left) y axis
    c->yAxis()->setTitle("US Dollars");

    // Set the tick length to -4 pixels (-ve means ticks inside the plot area)
    c->yAxis()->setTickLength(-4);

    // Add a title to the secondary (right) y axis
    c->yAxis2()->setTitle("HK Dollars (1 USD = 7.8 HKD)");

    // Set the tick length to -4 pixels (-ve means ticks inside the plot area)
    c->yAxis2()->setTickLength(-4);

    // Synchronize the y-axis such that y2 = 7.8 x y1
    c->syncYAxis(7.8);

    // Add a title to the bottom x axis
    c->xAxis()->setTitle("Hong Kong Time");

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels0, sizeof(labels0)/sizeof(labels0[0])));

    // Display 1 out of 3 labels on the x-axis. Show minor ticks for remaining
    // labels.
    c->xAxis()->setLabelStep(3, 1);

    // Set the major tick length to -4 pixels and minor tick length to -2 pixels (-ve
    // means ticks inside the plot area)
    c->xAxis()->setTickLength(-4, -2);

    // Set the distance between the axis labels and the axis to 6 pixels
    c->xAxis()->setLabelGap(6);

    // Add a title to the top x-axis
    c->xAxis2()->setTitle("New York Time");

    // Set the labels on the x axis.
    c->xAxis2()->setLabels(StringArray(labels1, sizeof(labels1)/sizeof(labels1[0])));

    // Display 1 out of 3 labels on the x-axis. Show minor ticks for remaining
    // labels.
    c->xAxis2()->setLabelStep(3, 1);

    // Set the major tick length to -4 pixels and minor tick length to -2 pixels (-ve
    // means ticks inside the plot area)
    c->xAxis2()->setTickLength(-4, -2);

    // Set the distance between the axis labels and the axis to 6 pixels
    c->xAxis2()->setLabelGap(6);

    // Add a line layer to the chart with a line width of 2 pixels
    c->addLineLayer(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), -1,
        "Red Transactions")->setLineWidth(2);

    // Add an area layer to the chart with no area boundary line
    c->addAreaLayer(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), -1,
        "Green Transactions")->setLineWidth(0);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName}\nHKT Jun {=3.5+{x}/24|0} {={x}%24}:00 (NYT Jun "
        "{=3+{x}/24|0} {=({x}+12)%24}:00)\nHKD {={value}*7.8} (USD {value})'");

    return c;
}


BaseChart *multiaxes(int img, const char **imageMap)
{
    // Data for the chart
    double data0[] = {1700, 3900, 2900, 3800, 4100, 4600, 2900, 4100, 4400, 5700,
        5900, 5200, 3700, 3400, 5100, 5600, 5600, 6000, 7000, 7600, 6300, 6700, 7500,
        6400, 8800};
    double data1[] = {500, 550, 670, 990, 820, 730, 800, 720, 730, 790, 860, 800,
        840, 680, 740, 890, 680, 790, 730, 770, 840, 820, 800, 840, 670};
    double data2[] = {46, 68, 35, 33, 38, 20, 12, 18, 15, 23, 30, 24, 28, 15, 21, 26,
        46, 42, 38, 25, 23, 32, 24, 20, 25};
    double data3[] = {0.84, 0.82, 0.82, 0.38, 0.25, 0.52, 0.54, 0.52, 0.38, 0.51,
        0.46, 0.29, 0.5, 0.55, 0.47, 0.34, 0.52, 0.33, 0.21, 0.3, 0.25, 0.15, 0.18,
        0.22, 0.14};

    // Labels for the chart
    const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
        "24"};

    // Create a XYChart object of size 600 x 300 pixels, with a grey (eeeeee)
    // background, a black border, and 1 pixel 3D border effect
    XYChart *c = new XYChart(600, 300, 0xeeeeee, 0x000000, 1);

    // Add a title box to the chart using 15 pts Arial Bold Italic font, with blue
    // (aaaaff) background
    c->addTitle("Multiple Axes Demonstration", "arialbi.ttf", 15)->setBackground(
        0xaaaaff);

    // Set the plotarea at (100, 70) and of size 400 x 180 pixels, with white
    // background. Turn on both horizontal and vertical grid lines with light grey
    // color (cccccc)
    c->setPlotArea(100, 70, 400, 180, 0xffffff)->setGridColor(0xcccccc, 0xcccccc);

    // Add a legend box at (300, 70) (top center of the chart) with horizontal
    // layout. Use 8 pts Arial Bold font. Set the background and border color to
    // Transparent.
    LegendBox *legendBox = c->addLegend(300, 70, false, "arialbd.ttf", 8);
    legendBox->setAlignment(Chart::BottomCenter);
    legendBox->setBackground(Chart::Transparent, Chart::Transparent);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Display 1 out of 3 labels on the x-axis.
    c->xAxis()->setLabelStep(3);

    // Add a title to the x-axis
    c->xAxis()->setTitle("Hour of Day");

    // Add a title on top of the primary (left) y axis.
    c->yAxis()->setTitle("Power\n(Watt)")->setAlignment(Chart::TopLeft2);
    // Set the axis, label and title colors for the primary y axis to red (c00000) to
    // match the first data set
    c->yAxis()->setColors(0xcc0000, 0xcc0000, 0xcc0000);

    // Add a title on top of the secondary (right) y axis.
    c->yAxis2()->setTitle("Load\n(Mbps)")->setAlignment(Chart::TopRight2);
    // Set the axis, label and title colors for the secondary y axis to green
    // (00800000) to match the second data set
    c->yAxis2()->setColors(0x008000, 0x008000, 0x008000);

    // Add the third y-axis at 50 pixels to the left of the plot area
    Axis *leftAxis = c->addAxis(Chart::Left, 50);
    // Add a title on top of the third y axis.
    leftAxis->setTitle("Temp\n(C)")->setAlignment(Chart::TopLeft2);
    // Set the axis, label and title colors for the third y axis to blue (0000cc) to
    // match the third data set
    leftAxis->setColors(0x0000cc, 0x0000cc, 0x0000cc);

    // Add the fouth y-axis at 50 pixels to the right of the plot area
    Axis *rightAxis = c->addAxis(Chart::Right, 50);
    // Add a title on top of the fourth y axis.
    rightAxis->setTitle("Error\n(%)")->setAlignment(Chart::TopRight2);
    // Set the axis, label and title colors for the fourth y axis to purple (880088)
    // to match the fourth data set
    rightAxis->setColors(0x880088, 0x880088, 0x880088);

    // Add a line layer to for the first data set using red (c00000) color, with a
    // line width of 2 pixels
    LineLayer *layer0 = c->addLineLayer(DoubleArray(data0,
        sizeof(data0)/sizeof(data0[0])), 0xcc0000, "Power");
    layer0->setLineWidth(2);

    // Add a line layer to for the second data set using green (00c0000) color, with
    // a line width of 2 pixels. Bind the layer to the secondary y-axis.
    LineLayer *layer1 = c->addLineLayer(DoubleArray(data1,
        sizeof(data1)/sizeof(data1[0])), 0x008000, "Load");
    layer1->setLineWidth(2);
    layer1->setUseYAxis2();

    // Add a line layer to for the third data set using blue (0000cc) color, with a
    // line width of 2 pixels. Bind the layer to the third y-axis.
    LineLayer *layer2 = c->addLineLayer(DoubleArray(data2,
        sizeof(data2)/sizeof(data2[0])), 0x0000cc, "Temperature");
    layer2->setLineWidth(2);
    layer2->setUseYAxis(leftAxis);

    // Add a line layer to for the fourth data set using purple (880088) color, with
    // a line width of 2 pixels. Bind the layer to the fourth y-axis.
    LineLayer *layer3 = c->addLineLayer(DoubleArray(data3,
        sizeof(data3)/sizeof(data3[0])), 0x880088, "Error Rate");
    layer3->setLineWidth(2);
    layer3->setUseYAxis(rightAxis);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} at hour {xLabel} = {value}'");

    return c;
}


BaseChart *fourq(int img, const char **imageMap)
{
    // XY points for the scatter chart
    double dataX0[] = {10, 15, 6, -12, 14, -8, 13, -13, 16, 12, 10.5};
    double dataY0[] = {130, 150, 80, 110, -110, -105, -130, 115, -170, 125, 125};

    double dataX1[] = {6, 7, -4, 3.5, 7, 8, -9, -10, -12, 11, 8};
    double dataY1[] = {65, -40, -40, 45, -70, -80, 80, 10, -100, 105, 60};

    double dataX2[] = {-10, -12, 11, 8, 6, 12, -4, 3.5, 7, 8, -9};
    double dataY2[] = {65, -80, -40, 45, -70, -80, 80, 90, -100, 105, 60};

    // Create a XYChart object of size 600 x 300 pixels, with a light blue (ccccff)
    // background, a black border, and 1 pixel 3D border effect
    XYChart *c = new XYChart(600, 480, 0xccccff, 0x000000, 1);

    // Add a title box to the chart using 16 pts Arial Bold Italic font, with white
    // text on deep blue background
    c->addTitle("Four Quadrant Chart Demonstration", "arialbi.ttf", 16, 0xffffff
        )->setBackground(0x000080);

    // Set the plotarea at (20, 60) and of size 560 x 360 pixels, with grey (808080)
    // border, and light grey (c0c0c0) horizontal and vertical grid lines. Set 4
    // quadrant coloring, where the colors of the quadrants alternate between lighter
    // and deeper grey (dddddd/eeeeee)
    c->setPlotArea(20, 60, 560, 360, -1, -1, 0x808080, 0xc0c0c0, 0xc0c0c0
        )->set4QBgColor(0xdddddd, 0xeeeeee, 0xdddddd, 0xeeeeee);

    // Set 4 quadrant mode, with both x and y axes symetrical around the origin
    c->setAxisAtOrigin(Chart::XYAxisAtOrigin, Chart::XAxisSymmetric +
        Chart::YAxisSymmetric);

    // Add a legend box at (300, 460) (bottom center of the chart) with horizontal
    // layout. Use 8 pts Arial Bold font.
    LegendBox *legendBox = c->addLegend(300, 460, false, "arialbd.ttf", 8);
    legendBox->setAlignment(Chart::BottomCenter);

    // Set legend box background to light grey (dddddd) with a black border
    legendBox->setBackground(0xdddddd, 0);

    // Set left/right margin to 20 pixels and top/bottom margin to 5 pixels
    legendBox->setMargin(20, 20, 5, 5);

    // Add a titles to axes
    c->xAxis()->setTitle("Alpha Index");
    c->yAxis()->setTitle("Beta Index");

    // Set axes width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // Add scatter layer, using 15 pixels red (ff33333) X shape symbols
    c->addScatterLayer(DoubleArray(dataX0, sizeof(dataX0)/sizeof(dataX0[0])),
        DoubleArray(dataY0, sizeof(dataY0)/sizeof(dataY0[0])), "Group A",
        Chart::Cross2Shape(), 15, 0xff3333);

    // Add scatter layer, using 15 pixels green (33ff33) 6-sided polygon symbols
    c->addScatterLayer(DoubleArray(dataX1, sizeof(dataX1)/sizeof(dataX1[0])),
        DoubleArray(dataY1, sizeof(dataY1)/sizeof(dataY1[0])), "Group B",
        Chart::PolygonShape(6), 15, 0x33ff33);

    // Add scatter layer, using 15 pixels blue (3333ff) triangle symbols
    c->addScatterLayer(DoubleArray(dataX2, sizeof(dataX2)/sizeof(dataX2[0])),
        DoubleArray(dataY2, sizeof(dataY2)/sizeof(dataY2[0])), "Group C",
        Chart::TriangleSymbol, 15, 0x3333ff);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{dataSetName}] Alpha = {x}, Beta = {value}'");

    return c;
}


BaseChart *fontxy(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {100, 125, 245, 147, 67};
    double data1[] = {85, 156, 179, 211, 123};
    double data2[] = {97, 87, 56, 267, 157};
    const char *labels[] = {"Mon Jun 4", "Tue Jun 5", "Wed Jun 6", "Thu Jun 7",
        "Fri Jun 8"};

    // Create a XYChart object of size 540 x 350 pixels
    XYChart *c = new XYChart(540, 350);

    // Set the plot area to start at (120, 40) and of size 280 x 240 pixels
    c->setPlotArea(120, 40, 280, 240);

    // Add a title to the chart using 20 pts Times Bold Italic (timesbi.ttf) font and
    // using a deep blue color (0x000080)
    c->addTitle("Weekly Server Load", "timesbi.ttf", 20, 0x000080);

    // Add a legend box at (420, 100) (right of plot area) using 12 pts Times Bold
    // font. Sets the background of the legend box to light grey 0xd0d0d0 with a 1
    // pixel 3D border.
    c->addLegend(420, 100, true, "timesbd.ttf", 12)->setBackground(0xd0d0d0,
        0xd0d0d0, 1);

    // Add a title to the y-axis using 12 pts Arial Bold/deep blue (0x000080) font.
    // Set the background to yellow (0xffff00) with a 2 pixel 3D border.
    c->yAxis()->setTitle("Throughput (per hour)", "arialbd.ttf", 12, 0x000080
        )->setBackground(0xffff00, 0xffff00, 2);

    // Use 10 pts Arial Bold/orange (0xcc6600) font for the y axis labels
    c->yAxis()->setLabelStyle("arialbd.ttf", 10, 0xcc6600);

    // Set the axis label format to "nnn MBytes"
    c->yAxis()->setLabelFormat("{value} MBytes");

    // Use 10 pts Arial Bold/green (0x008000) font for the x axis labels. Set the
    // label angle to 45 degrees.
    c->xAxis()->setLabelStyle("arialbd.ttf", 10, 0x008000)->setFontAngle(45);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a 3D stack bar layer with a 3D depth of 5 pixels
    BarLayer *layer = c->addBarLayer(Chart::Stack, 5);

    // Use Arial Italic as the default data label font in the bars
    layer->setDataLabelStyle("ariali.ttf");

    // Use 10 pts Times Bold Italic (timesbi.ttf) as the aggregate label font. Set
    // the background to flesh (0xffcc66) color with a 1 pixel 3D border.
    layer->setAggregateLabelStyle("timesbi.ttf", 10)->setBackground(0xffcc66,
        Chart::Transparent, 1);

    // Add the first data set to the stacked bar layer
    layer->addDataSet(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), -1,
        "Server #1");

    // Add the second data set to the stacked bar layer
    layer->addDataSet(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), -1,
        "Server #2");

    // Add the third data set to the stacked bar layer, and set its data label font
    // to Arial Bold Italic.
    TextBox *textbox = layer->addDataSet(DoubleArray(data2,
        sizeof(data2)/sizeof(data2[0])), -1, "Server #3")->setDataLabelStyle(
        "arialbi.ttf");

    // Set the data label font color for the third data set to yellow (0xffff00)
    textbox->setFontColor(0xffff00);

    // Set the data label background color to the same color as the bar segment, with
    // a 1 pixel 3D border.
    textbox->setBackground(Chart::SameAsMainColor, Chart::Transparent, 1);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} on {xLabel}: {value} MBytes/hour'");

    return c;
}


BaseChart *background(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {85, 156, 179.5, 211, 123};
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 270 x 270 pixels
    XYChart *c = new XYChart(270, 270);

    // Set the plot area at (40, 32) and of size 200 x 200 pixels
    PlotArea *plotarea = c->setPlotArea(40, 32, 200, 200);

    // Set the background style based on the input parameter
    if (img == 0) {
        // Has wallpaper image
        c->setWallpaper("tile.gif");
    } else if (img == 1) {
        // Use a background image as the plot area background
        plotarea->setBackground("bg.png");
    } else if (img == 2) {
        // Use white (0xffffff) and grey (0xe0e0e0) as two alternate plotarea
        // background colors
        plotarea->setBackground(0xffffff, 0xe0e0e0);
    } else {
        // Use a dark background palette
        c->setColors(Chart::whiteOnBlackPalette);
    }

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a color bar layer using the given data. Use a 1 pixel 3D border for the
    // bars.
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), IntArray(0, 0)
        )->setBorderColor(-1, 1);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Revenue for {xLabel}: US${value}K'");

    return c;
}


BaseChart *logaxis(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {100, 125, 260, 147, 67};
    const char *labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

    // Create a XYChart object of size 200 x 180 pixels
    XYChart *c = new XYChart(200, 180);

    // Set the plot area at (30, 10) and of size 140 x 130 pixels
    c->setPlotArea(30, 10, 140, 130);

    // Ise log scale axis if required
    if (img == 1) {
        c->yAxis()->setLogScale();
    }

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a color bar layer using the given data. Use a 1 pixel 3D border for the
    // bars.
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), IntArray(0, 0)
        )->setBorderColor(-1, 1);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Mileage on {xLabel}: {value} miles'");

    return c;
}


BaseChart *axisscale(int img, const char **imageMap)
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
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='ROI for {xLabel}: {value}%'");

    return c;
}


BaseChart *ticks(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {100, 125, 265, 147, 67, 105};
    const char *labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun"};

    // Create a XYChart object of size 250 x 250 pixels
    XYChart *c = new XYChart(250, 250);

    // Set the plot area at (27, 25) and of size 200 x 200 pixels
    c->setPlotArea(27, 25, 200, 200);

    if (img == 1) {
        // High tick density, uses 10 pixels as tick spacing
        c->addTitle("Tick Density = 10 pixels");
        c->yAxis()->setTickDensity(10);
    } else {
        // Normal tick density, just use the default setting
        c->addTitle("Default Tick Density");
    }

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));

    // Add a color bar layer using the given data. Use a 1 pixel 3D border for the
    // bars.
    c->addBarLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), IntArray(0, 0)
        )->setBorderColor(-1, 1);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Revenue for {xLabel}: US${value}M'");

    return c;
}


BaseChart *simpleradar(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {90, 60, 65, 75, 40};

    // The labels for the chart
    const char *labels[] = {"Speed", "Reliability", "Comfort", "Safety", "Efficiency"
        };

    // Create a PolarChart object of size 450 x 350 pixels
    PolarChart *c = new PolarChart(450, 350);

    // Set center of plot area at (225, 185) with radius 150 pixels
    c->setPlotArea(225, 185, 150);

    // Add an area layer to the polar chart
    c->addAreaLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])), 0x9999ff);

    // Set the labels to the angular axis as spokes
    c->angularAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        );

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{label}: score = {value}'");

    return c;
}


BaseChart *multiradar(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {90, 60, 85, 75, 55};
    double data1[] = {60, 80, 70, 80, 85};

    // The labels for the chart
    const char *labels[] = {"Speed", "Reliability", "Comfort", "Safety", "Efficiency"
        };

    // Create a PolarChart object of size 480 x 380 pixels. Set background color to
    // gold, with 1 pixel 3D border effect
    PolarChart *c = new PolarChart(480, 380, Chart::goldColor(), 0x000000, 1);

    // Add a title to the chart using 15 pts Times Bold Italic font. The title text
    // is white (ffffff) on a deep blue (000080) background
    c->addTitle("Space Travel Vehicles Compared", "timesbi.ttf", 15, 0xffffff
        )->setBackground(0x000080);

    // Set plot area center at (240, 210), with 150 pixels radius, and a white
    // (ffffff) background.
    c->setPlotArea(240, 210, 150, 0xffffff);

    // Add a legend box at top right corner (470, 35) using 10 pts Arial Bold font.
    // Set the background to silver, with 1 pixel 3D border effect.
    LegendBox *b = c->addLegend(470, 35, true, "arialbd.ttf", 10);
    b->setAlignment(Chart::TopRight);
    b->setBackground(Chart::silverColor(), Chart::Transparent, 1);

    // Add an area layer to the chart using semi-transparent blue (0x806666cc). Add a
    // blue (0x6666cc) line layer using the same data with 3 pixel line width to
    // highlight the border of the area.
    c->addAreaLayer(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0x806666CC,
        "Model Saturn");
    c->addLineLayer(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0x6666cc
        )->setLineWidth(3);

    // Add an area layer to the chart using semi-transparent red (0x80cc6666). Add a
    // red (0xcc6666) line layer using the same data with 3 pixel line width to
    // highlight the border of the area.
    c->addAreaLayer(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0x80CC6666,
        "Model Jupiter");
    c->addLineLayer(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0xcc6666
        )->setLineWidth(3);

    // Set the labels to the angular axis as spokes.
    c->angularAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        );

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{dataSetName}] {label}: score = {value}'");

    return c;
}


BaseChart *stackradar(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {100, 100, 100, 100, 100};
    double data1[] = {90, 85, 85, 80, 70};
    double data2[] = {80, 65, 65, 75, 45};

    // The labels for the chart
    const char *labels[] = {"Population<*br*><*font=arial.ttf*>6 millions",
        "GDP<*br*><*font=arial.ttf*>120 billions",
        "Export<*br*><*font=arial.ttf*>25 billions",
        "Import<*br*><*font=arial.ttf*>24 billions",
        "Investments<*br*><*font=arial.ttf*>20 billions"};

    // Create a PolarChart object of size 480 x 460 pixels. Set background color to
    // silver, with 1 pixel 3D border effect
    PolarChart *c = new PolarChart(480, 460, Chart::silverColor(), 0x000000, 1);

    // Add a title to the chart using 15 pts Times Bold Italic font. The title text
    // is white (ffffff) on a deep green (008000) background
    c->addTitle("Economic Growth", "timesbi.ttf", 15, 0xffffff)->setBackground(
        0x008000);

    // Set plot area center at (240, 270), with 150 pixels radius
    c->setPlotArea(240, 270, 150);

    // Use 1 pixel width semi-transparent black (c0000000) lines as grid lines
    c->setGridColor(0xC0000000, 1, 0xC0000000, 1);

    // Add a legend box at top-center of plot area (240, 35) using horizontal layout.
    // Use 10 pts Arial Bold font, with silver background and 1 pixel 3D border
    // effect.
    LegendBox *b = c->addLegend(240, 35, false, "arialbd.ttf", 10);
    b->setAlignment(Chart::TopCenter);
    b->setBackground(Chart::silverColor(), Chart::Transparent, 1);

    // Add area layers of different colors to represent the data
    c->addAreaLayer(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), 0xcc8880,
        "Year 2004");
    c->addAreaLayer(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), 0xffd080,
        "Year 1994");
    c->addAreaLayer(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), 0xa0bce0,
        "Year 1984");

    // Set the labels to the angular axis as spokes.
    c->angularAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        );

    // Set radial axis from 0 - 100 with a tick every 20 units
    c->radialAxis()->setLinearScale(0, 100, 20);

    // Just show the radial axis as a grid line. Hide the axis labels by setting the
    // label color to Transparent
    c->radialAxis()->setColors(0xC0000000, Chart::Transparent);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Current {label}: {value}% in {dataSetName}'");

    return c;
}


BaseChart *polarline(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {90, 25, 40, 55, 68, 44, 79, 85, 50};
    double angles0[] = {15, 60, 110, 180, 230, 260, 260, 310, 340};

    double data1[] = {80, 91, 66, 80, 92, 87};
    double angles1[] = {40, 65, 88, 110, 150, 200};

    // Create a PolarChart object of size 460 x 500 pixels, with a grey (e0e0e0)
    // background and 1 pixel 3D border
    PolarChart *c = new PolarChart(460, 500, 0xe0e0e0, 0x000000, 1);

    // Add a title to the chart at the top left corner using 15pts Arial Bold Italic
    // font. Use a wood pattern as the title background.
    c->addTitle("Polar Line Chart Demo", "arialbi.ttf", 15)->setBackground(
        c->patternColor("wood.png"));

    // Set center of plot area at (230, 280) with radius 180 pixels, and white
    // (ffffff) background.
    c->setPlotArea(230, 280, 180, 0xffffff);

    // Set the grid style to circular grid, with grids below the chart layers
    c->setGridStyle(false, false);

    // Add a legend box at top-center of plot area (230, 35) using horizontal layout.
    // Use 10 pts Arial Bold font, with 1 pixel 3D border effect.
    LegendBox *b = c->addLegend(230, 35, false, "arialbd.ttf", 9);
    b->setAlignment(Chart::TopCenter);
    b->setBackground(Chart::Transparent, Chart::Transparent, 1);

    // Set angular axis as 0 - 360, with a spoke every 30 units
    c->angularAxis()->setLinearScale(0, 360, 30);

    // Add a blue (0xff) line layer to the chart using (data0, angle0)
    PolarLineLayer *layer0 = c->addLineLayer(DoubleArray(data0,
        sizeof(data0)/sizeof(data0[0])), 0x0000ff, "Close Loop Line");
    layer0->setAngles(DoubleArray(angles0, sizeof(angles0)/sizeof(angles0[0])));

    // Set the line width to 2 pixels
    layer0->setLineWidth(2);

    // Use 11 pixel triangle symbols for the data points
    layer0->setDataSymbol(Chart::TriangleSymbol, 11);

    // Enable data label and set its format
    layer0->setDataLabelFormat("({value},{angle})");

    // Set the data label text box with light blue (0x9999ff) backgruond color and 1
    // pixel 3D border effect
    layer0->setDataLabelStyle()->setBackground(0x9999ff, Chart::Transparent, 1);

    // Add a red (0xff0000) line layer to the chart using (data1, angle1)
    PolarLineLayer *layer1 = c->addLineLayer(DoubleArray(data1,
        sizeof(data1)/sizeof(data1[0])), 0xff0000, "Open Loop Line");
    layer1->setAngles(DoubleArray(angles1, sizeof(angles1)/sizeof(angles1[0])));

    // Set the line width to 2 pixels
    layer1->setLineWidth(2);

    // Use 11 pixel diamond symbols for the data points
    layer1->setDataSymbol(Chart::DiamondSymbol, 11);

    // Set the line to open loop
    layer1->setCloseLoop(false);

    // Enable data label and set its format
    layer1->setDataLabelFormat("({value},{angle})");

    // Set the data label text box with light red (0xff9999) backgruond color and 1
    // pixel 3D border effect
    layer1->setDataLabelStyle()->setBackground(0xff9999, Chart::Transparent, 1);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{dataSetName}] ({radius}, {angle})'");

    return c;
}


BaseChart *polararea(int img, const char **imageMap)
{
    // Data for the chart
    double data0[] = {5, 3, 10, 4, 3, 5, 2, 5};
    double data1[] = {12, 6, 17, 6, 7, 9, 4, 7};
    double data2[] = {17, 7, 22, 7, 18, 13, 5, 11};

    const char *labels[] = {"North", "North<*br*>East", "East", "South<*br*>East",
        "South", "South<*br*>West", "West", "North<*br*>West"};

    // Create a PolarChart object of size 460 x 500 pixels, with a grey (e0e0e0)
    // background and 1 pixel 3D border
    PolarChart *c = new PolarChart(460, 500, 0xe0e0e0, 0x000000, 1);

    // Add a title to the chart at the top left corner using 15pts Arial Bold Italic
    // font. Use a wood pattern as the title background.
    c->addTitle("Polar Area Chart Demo", "arialbi.ttf", 15)->setBackground(
        c->patternColor("wood.png"));

    // Set center of plot area at (230, 280) with radius 180 pixels, and white
    // (ffffff) background.
    c->setPlotArea(230, 280, 180, 0xffffff);

    // Set the grid style to circular grid
    c->setGridStyle(false);

    // Add a legend box at top-center of plot area (230, 35) using horizontal layout.
    // Use 10 pts Arial Bold font, with 1 pixel 3D border effect.
    LegendBox *b = c->addLegend(230, 35, false, "arialbd.ttf", 9);
    b->setAlignment(Chart::TopCenter);
    b->setBackground(Chart::Transparent, Chart::Transparent, 1);

    // Set angular axis using the given labels
    c->angularAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        );

    // Specify the label format for the radial axis
    c->radialAxis()->setLabelFormat("{value}%");

    // Set radial axis label background to semi-transparent grey (40cccccc)
    c->radialAxis()->setLabelStyle()->setBackground(0x40cccccc, 0);

    // Add the data as area layers
    c->addAreaLayer(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])), -1,
        "5 m/s or above");
    c->addAreaLayer(DoubleArray(data1, sizeof(data1)/sizeof(data1[0])), -1,
        "1 - 5 m/s");
    c->addAreaLayer(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])), -1,
        "less than 1 m/s");

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='[{label}] {dataSetName}: {value}%'");

    return c;
}


BaseChart *polarspline(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {5.1, 2.6, 1.5, 2.2, 5.1, 4.3, 4.0, 9.0, 1.7, 8.8, 9.9, 9.5,
        9.4, 1.8, 2.1, 2.3, 3.5, 7.7, 8.8, 6.1, 5.0, 3.1, 6.0, 4.3};
    double angles0[] = {0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195,
        210, 225, 240, 255, 270, 285, 300, 315, 330, 345};

    double data1[] = {8.1, 2.5, 5, 5.2, 6.5, 8.5, 9, 7.6, 8.7, 6.4, 5.5, 5.4, 3.0,
        8.7, 7.1, 8.8, 7.9, 2.2, 5.0, 4.0, 1.5, 7.5, 8.3, 9.0};
    double angles1[] = {0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195,
        210, 225, 240, 255, 270, 285, 300, 315, 330, 345};

    // Create a PolarChart object of size 460 x 460 pixels
    PolarChart *c = new PolarChart(460, 460);

    // Add a title to the chart at the top left corner using 15pts Arial Bold Italic
    // font
    c->addTitle(Chart::TopLeft, "<*underline=2*>EM Field Strength", "arialbi.ttf", 15
        );

    // Set center of plot area at (230, 240) with radius 180 pixels
    c->setPlotArea(230, 240, 180);

    // Set the grid style to circular grid
    c->setGridStyle(false);

    // Add a legend box at the top right corner of the chart using 9 pts Arial Bold
    // font
    c->addLegend(459, 0, true, "arialbd.ttf", 9)->setAlignment(Chart::TopRight);

    // Set angular axis as 0 - 360, with a spoke every 30 units
    c->angularAxis()->setLinearScale(0, 360, 30);

    // Add a red (0xff9999) spline area layer to the chart using (data0, angles0)
    c->addSplineAreaLayer(DoubleArray(data0, sizeof(data0)/sizeof(data0[0])),
        0xff9999, "Above 100MHz")->setAngles(DoubleArray(angles0,
        sizeof(angles0)/sizeof(angles0[0])));

    // Add a blue (0xff) spline line layer to the chart using (data1, angle1)
    PolarSplineLineLayer *layer1 = c->addSplineLineLayer(DoubleArray(data1,
        sizeof(data1)/sizeof(data1[0])), 0x0000ff, "Below 100MHz");
    layer1->setAngles(DoubleArray(angles1, sizeof(angles1)/sizeof(angles1[0])));

    // Set the line width to 3 pixels
    layer1->setLineWidth(3);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} EM field at {angle} deg: {value} Watt'");

    return c;
}


BaseChart *polarscatter(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {43, 89, 76, 64, 48, 18, 92, 68, 44, 79, 71, 85};
    double angles0[] = {45, 96, 169, 258, 15, 30, 330, 260, 60, 75, 110, 140};

    double data1[] = {50, 91, 26, 29, 80, 53, 62, 87, 19, 40};
    double angles1[] = {230, 210, 240, 310, 179, 250, 244, 199, 89, 160};

    double data2[] = {88, 65, 76, 49, 80, 53};
    double angles2[] = {340, 310, 340, 210, 30, 300};

    // The labels on the angular axis (spokes)
    const char *labels[] = {"North", "North\nEast", "East", "South\nEast", "South",
        "South\nWest", "West", "North\nWest"};

    // Create a PolarChart object of size 460 x 460 pixels
    PolarChart *c = new PolarChart(460, 460);

    // Add a title to the chart at the top left corner using 15pts Arial Bold Italic
    // font
    c->addTitle(Chart::TopLeft, "<*underline=2*>Plants in Wonderland", "arialbi.ttf",
        15);

    // Set center of plot area at (230, 240) with radius 180 pixels
    c->setPlotArea(230, 240, 180);

    // Use alternative light grey/dark grey sector background color
    c->setPlotAreaBg(0xdddddd, 0xeeeeee, false);

    // Set the grid style to circular grid
    c->setGridStyle(false);

    // Add a legend box at the top right corner of the chart using 9 pts Arial Bold
    // font
    c->addLegend(459, 0, true, "arialbd.ttf", 9)->setAlignment(Chart::TopRight);

    // Set angular axis as 0 - 360, either 8 spokes
    c->angularAxis()->setLinearScale(0, 360, StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    // Set the radial axis label format
    c->radialAxis()->setLabelFormat("{value} km");

    // Add a blue (0xff) polar line layer to the chart using (data0, angle0)
    PolarLineLayer *layer0 = c->addLineLayer(DoubleArray(data0,
        sizeof(data0)/sizeof(data0[0])), 0x0000ff, "Immortal Weed");
    layer0->setAngles(DoubleArray(angles0, sizeof(angles0)/sizeof(angles0[0])));

    layer0->setLineWidth(0);
    layer0->setDataSymbol(Chart::TriangleSymbol, 11);

    // Add a red (0xff0000) polar line layer to the chart using (data1, angles1)
    PolarLineLayer *layer1 = c->addLineLayer(DoubleArray(data1,
        sizeof(data1)/sizeof(data1[0])), 0xff0000, "Precious Flower");
    layer1->setAngles(DoubleArray(angles1, sizeof(angles1)/sizeof(angles1[0])));

    // Disable the line by setting its width to 0, so only the symbols are visible
    layer1->setLineWidth(0);

    // Use a 11 pixel diamond data point symbol
    layer1->setDataSymbol(Chart::DiamondSymbol, 11);

    // Add a green (0x00ff00) polar line layer to the chart using (data2, angles2)
    PolarLineLayer *layer2 = c->addLineLayer(DoubleArray(data2,
        sizeof(data2)/sizeof(data2[0])), 0x00ff00, "Magical Tree");
    layer2->setAngles(DoubleArray(angles2, sizeof(angles2)/sizeof(angles2[0])));

    // Disable the line by setting its width to 0, so only the symbols are visible
    layer2->setLineWidth(0);

    // Use a 9 pixel square data point symbol
    layer2->setDataSymbol(Chart::SquareSymbol, 9);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} at ({value} km, {angle} deg)'");

    return c;
}


BaseChart *polarbubble(int img, const char **imageMap)
{
    // The data for the chart
    double data0[] = {6, 12.5, 18.2, 15};
    double angles0[] = {45, 96, 169, 258};
    double size0[] = {41, 105, 12, 20};

    double data1[] = {18, 16, 11, 14};
    double angles1[] = {30, 210, 240, 310};
    double size1[] = {30, 45, 12, 90};

    // Create a PolarChart object of size 460 x 460 pixels
    PolarChart *c = new PolarChart(460, 460);

    // Add a title to the chart at the top left corner using 15pts Arial Bold Italic
    // font
    c->addTitle(Chart::TopLeft, "<*underline=2*>EM Field Strength", "arialbi.ttf", 15
        );

    // Set center of plot area at (230, 240) with radius 180 pixels
    c->setPlotArea(230, 240, 180);

    // Use alternative light grey/dark grey circular background color
    c->setPlotAreaBg(0xdddddd, 0xeeeeee);

    // Set the grid style to circular grid
    c->setGridStyle(false);

    // Add a legend box at the top right corner of the chart using 9 pts Arial Bold
    // font
    c->addLegend(459, 0, true, "arialbd.ttf", 9)->setAlignment(Chart::TopRight);

    // Set angular axis as 0 - 360, with a spoke every 30 units
    c->angularAxis()->setLinearScale(0, 360, 30);

    // Set the radial axis label format
    c->radialAxis()->setLabelFormat("{value} km");

    // Add a blue (0x9999ff) line layer to the chart using (data0, angle0)
    PolarLineLayer *layer0 = c->addLineLayer(DoubleArray(data0,
        sizeof(data0)/sizeof(data0[0])), 0x9999ff, "Cold Spot");
    layer0->setAngles(DoubleArray(angles0, sizeof(angles0)/sizeof(angles0[0])));

    // Disable the line by setting its width to 0, so only the symbols are visible
    layer0->setLineWidth(0);

    // Use a circular data point symbol
    layer0->setDataSymbol(Chart::CircleSymbol, 11);

    // Modulate the symbol size by size0 to produce a bubble chart effect
    layer0->setSymbolScale(DoubleArray(size0, sizeof(size0)/sizeof(size0[0])));

    // Add a red (0xff9999) line layer to the chart using (data1, angle1)
    PolarLineLayer *layer1 = c->addLineLayer(DoubleArray(data1,
        sizeof(data1)/sizeof(data1[0])), 0xff9999, "Hot Spot");
    layer1->setAngles(DoubleArray(angles1, sizeof(angles1)/sizeof(angles1[0])));

    // Disable the line by setting its width to 0, so only the symbols are visible
    layer1->setLineWidth(0);

    // Use a circular data point symbol
    layer1->setDataSymbol(Chart::CircleSymbol, 11);

    // Modulate the symbol size by size1 to produce a bubble chart effect
    layer1->setSymbolScale(DoubleArray(size1, sizeof(size1)/sizeof(size1[0])));

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} at ({value} km, {angle} deg)\nStrength = {z} Watt'");

    return c;
}


BaseChart *polarvector(int img, const char **imageMap)
{
    // Coordinates of the starting points of the vectors
    double radius[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 25, 25, 25, 25, 25, 25, 25, 25,
        25, 25, 25, 25};
    double angle[] = {0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 0, 30,
        60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 0, 30, 60, 90, 120, 150, 180,
        210, 240, 270, 300, 330, 0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300,
        330, 0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330};

    // Magnitude and direction of the vectors
    double magnitude[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    double direction[] = {60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 0, 30, 60,
        90, 120, 150, 180, 210, 240, 270, 300, 330, 0, 30, 60, 90, 120, 150, 180,
        210, 240, 270, 300, 330, 0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300,
        330, 0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 0, 30};

    // Create a PolarChart object of size 460 x 460 pixels
    PolarChart *c = new PolarChart(460, 460);

    // Add a title to the chart at the top left corner using 15pts Arial Bold Italic
    // font
    c->addTitle("Polar Vector Chart Demonstration", "arialbi.ttf", 15);

    // Set center of plot area at (230, 240) with radius 180 pixels
    c->setPlotArea(230, 240, 180);

    // Set the grid style to circular grid
    c->setGridStyle(false);

    // Set angular axis as 0 - 360, with a spoke every 30 units
    c->angularAxis()->setLinearScale(0, 360, 30);

    // Add a polar vector layer to the chart with blue (0000ff) vectors
    c->addVectorLayer(DoubleArray(radius, sizeof(radius)/sizeof(radius[0])),
        DoubleArray(angle, sizeof(angle)/sizeof(angle[0])), DoubleArray(magnitude,
        sizeof(magnitude)/sizeof(magnitude[0])), DoubleArray(direction,
        sizeof(direction)/sizeof(direction[0])), Chart::RadialAxisScale, 0x0000ff);

    // output the chart
    c->makeChart();

    //include tool tip for the chart
    *imageMap = c->getHTMLImageMap("clickable", "",
        "title='Vector at ({value}, {angle} deg): Length = {len}, Angle = {dir} deg'"
        );

    return c;
}


BaseChart *rose(int img, const char **imageMap)
{
    // Data for the chart
    double data[] = {9.4, 1.8, 2.1, 2.3, 3.5, 7.7, 8.8, 6.1, 5.0, 3.1, 6.0, 4.3, 5.1,
        2.6, 1.5, 2.2, 5.1, 4.3, 4.0, 9.0, 1.7, 8.8, 9.9, 9.5};
    double angles[] = {0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195,
        210, 225, 240, 255, 270, 285, 300, 315, 330, 345};

    // Create a PolarChart object of size 460 x 460 pixels, with a silver background
    // and a 1 pixel 3D border
    PolarChart *c = new PolarChart(460, 460, Chart::silverColor(), 0x000000, 1);

    // Add a title to the chart at the top left corner using 15pts Arial Bold Italic
    // font. Use white text on deep blue background.
    c->addTitle("Polar Vector Chart Demonstration", "arialbi.ttf", 15, 0xffffff
        )->setBackground(0x000080);

    // Set plot area center at (230, 240) with radius 180 pixels and white background
    c->setPlotArea(230, 240, 180, 0xffffff);

    // Set the grid style to circular grid
    c->setGridStyle(false);

    // Set angular axis as 0 - 360, with a spoke every 30 units
    c->angularAxis()->setLinearScale(0, 360, 30);

    // Add sectors to the chart as sector zones
    int i;
    for(i = 0; i < sizeof(data) / sizeof(data[0]); ++i) {
        c->angularAxis()->addZone(angles[i], angles[i] + 15, 0, data[i], 0x33ff33,
            0x008000);
    }

    // Add an Transparent invisible layer to ensure the axis is auto-scaled using the
    // data
    c->addLineLayer(DoubleArray(data, sizeof(data)/sizeof(data[0])),
        Chart::Transparent);

    // output the chart
    c->makeChart();

    return c;
}


BaseChart *stackrose(int img, const char **imageMap)
{
    // Data for the chart
    double data0[] = {5, 3, 10, 4, 3, 5, 2, 5};
    double data1[] = {12, 6, 17, 6, 7, 9, 4, 7};
    double data2[] = {17, 7, 22, 7, 18, 13, 5, 11};

    double angles[] = {0, 45, 90, 135, 180, 225, 270, 315};
    const char *labels[] = {"North", "North\nEast", "East", "South\nEast", "South",
        "South\nWest", "West", "North\nWest"};

    // Create a PolarChart object of size 460 x 500 pixels, with a grey (e0e0e0)
    // background and a 1 pixel 3D border
    PolarChart *c = new PolarChart(460, 500, 0xe0e0e0, 0x000000, 1);

    // Add a title to the chart at the top left corner using 15pts Arial Bold Italic
    // font. Use white text on deep blue background.
    c->addTitle("Wind Direction", "arialbi.ttf", 15, 0xffffff)->setBackground(
        0x000080);

    LegendBox *legendBox = c->addLegend(230, 35, false, "arialbd.ttf", 9);
    legendBox->setAlignment(Chart::TopCenter);
    legendBox->setBackground(Chart::Transparent, Chart::Transparent, 1);

    legendBox->addKey("5 m/s or above", 0xff3333);
    legendBox->addKey("1 - 5 m/s", 0x33ff33);
    legendBox->addKey("less than 1 m/s", 0x3333ff);

    // Set plot area center at (230, 280) with radius 180 pixels and white background
    c->setPlotArea(230, 280, 180, 0xffffff);

    // Set the grid style to circular grid
    c->setGridStyle(false);

    // Set angular axis as 0 - 360, with a spoke every 30 units
    c->angularAxis()->setLinearScale(0, 360, StringArray(labels,
        sizeof(labels)/sizeof(labels[0])));

    int i;
    for(i = 0; i < sizeof(angles) / sizeof(angles[0]); ++i) {
        c->angularAxis()->addZone(angles[i] - 10, angles[i] + 10, 0, data0[i],
            0x3333ff, 0);
        c->angularAxis()->addZone(angles[i] - 10, angles[i] + 10, data0[i], data1[i],
            0x33ff33, 0);
        c->angularAxis()->addZone(angles[i] - 10, angles[i] + 10, data1[i], data2[i],
            0xff3333, 0);
    }

    // Add an Transparent invisible layer to ensure the axis is auto-scaled using the
    // data
    c->addLineLayer(DoubleArray(data2, sizeof(data2)/sizeof(data2[0])),
        Chart::Transparent);

    // output the chart
    c->makeChart();

    return c;
}


BaseChart *polarzones(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {51, 15, 51, 40, 17, 87, 94, 21, 35, 88, 50, 60};

    // The labels for the chart
    const char *labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
        "Sept", "Oct", "Nov", "Dec"};

    // Create a PolarChart object of size 400 x 420 pixels
    PolarChart *c = new PolarChart(400, 420);

    // Set background color to a 2 pixel pattern color, with a black border and 1
    // pixel 3D border effect
    int pattern1[] = {0xffffff, 0xe0e0e0};
    c->setBackground(c->patternColor(IntArray(pattern1,
        sizeof(pattern1)/sizeof(pattern1[0])), 2), 0, 1);

    // Add a title to the chart using 16 pts Arial Bold Italic font. The title text
    // is white (0xffffff) on 2 pixel pattern background
    int pattern2[] = {0x000000, 0x000080};
    c->addTitle("Chemical Concentration", "arialbi.ttf", 16, 0xffffff
        )->setBackground(c->patternColor(IntArray(pattern2,
        sizeof(pattern2)/sizeof(pattern2[0])), 2));

    // Set center of plot area at (200, 240) with radius 145 pixels. Set background
    // color to blue (9999ff)
    c->setPlotArea(200, 240, 145, 0x9999ff);

    // Color the region between radius = 40 to 80 as green (99ff99)
    c->radialAxis()->addZone(40, 80, 0x99ff99);

    // Color the region with radius > 80 as red (ff9999)
    c->radialAxis()->addZone(80, 999, 0xff9999);

    // Set the grid style to circular grid
    c->setGridStyle(false);

    // Set the radial axis label format
    c->radialAxis()->setLabelFormat("{value} ppm");

    // Use semi-transparent (40ffffff) label background so as not to block the data
    c->radialAxis()->setLabelStyle()->setBackground(0x40ffffff, 0x40000000);

    // Add a legend box at (200, 30) top center aligned, using 9 pts Arial Bold font.
    // with a black border.
    LegendBox *legendBox = c->addLegend(200, 30, false, "arialbd.ttf", 9);
    legendBox->setAlignment(Chart::TopCenter);

    // Add legend keys to represent the red/green/blue zones
    legendBox->addKey("Under-Absorp", 0x9999ff);
    legendBox->addKey("Normal", 0x99ff99);
    legendBox->addKey("Over-Absorp", 0xff9999);

    // Add a blue (000080) spline line layer with line width set to 3 pixels and
    // using yellow (ffff00) circles to represent the data points
    PolarSplineLineLayer *layer = c->addSplineLineLayer(DoubleArray(data,
        sizeof(data)/sizeof(data[0])), 0x000080);
    layer->setLineWidth(3);
    layer->setDataSymbol(Chart::CircleShape, 11, 0xffff00);

    // Set the labels to the angular axis as spokes.
    c->angularAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        );

    // output the chart
    c->makeChart();

    // Include tool tip for the chart.
    *imageMap = layer->getHTMLImageMap("clickable", "",
        "title='Concentration on {label}: {value} ppm'");

    return c;
}


BaseChart *polarzones2(int img, const char **imageMap)
{
    // The data for the chart
    double data[] = {5.1, 1.5, 5.1, 4.0, 1.7, 8.7, 9.4, 2.1, 3.5, 8.8, 5.0, 6.0};

    // The labels for the chart
    const char *labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
        "Sept", "Oct", "Nov", "Dec"};

    // Create a PolarChart object of size 400 x 420 pixels. with a metallic blue
    // (9999ff) background color and 1 pixel 3D border
    PolarChart *c = new PolarChart(400, 420, Chart::metalColor(0x9999ff), 0x000000, 1
        );

    // Add a title to the chart using 16 pts Arial Bold Italic font. The title text
    // is white (0xffffff) on deep blue (000080) background
    c->addTitle("Chemical Concentration", "arialbi.ttf", 16, 0xffffff
        )->setBackground(0x000080);

    // Set center of plot area at (200, 240) with radius 145 pixels. Set background
    // color to green (0x33ff33)
    c->setPlotArea(200, 240, 145, 0x33ff33);

    // Set the labels to the angular axis
    c->angularAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0]))
        );

    // Color the sector between label index = 5.5 to 7.5 as red (ff3333) zone
    c->angularAxis()->addZone(5.5, 7.5, 0xff3333);

    // Color the sector between label index = 4.5 to 5.5, and also between 7.5 to
    // 8.5, as yellow (ff3333) zones
    c->angularAxis()->addZone(4.5, 5.5, 0xffff00);
    c->angularAxis()->addZone(7.5, 8.5, 0xffff00);

    // Set the grid style to circular grid
    c->setGridStyle(false);

    // Use semi-transparent (40ffffff) label background so as not to block the data
    c->radialAxis()->setLabelStyle()->setBackground(0x40ffffff, 0x40000000);

    // Add a legend box at (200, 30) top center aligned, using 9 pts Arial Bold font.
    // with a black border.
    LegendBox *legendBox = c->addLegend(200, 30, false, "arialbd.ttf", 9);
    legendBox->setAlignment(Chart::TopCenter);

    // Add legend keys to represent the red/yellow/green zones
    legendBox->addKey("Very Dry", 0xff3333);
    legendBox->addKey("Critical", 0xffff00);
    legendBox->addKey("Moderate", 0x33ff33);

    // Add a blue (0x80) line layer with line width set to 3 pixels and use purple
    // (ff00ff) cross symbols for the data points
    PolarLineLayer *layer = c->addLineLayer(DoubleArray(data,
        sizeof(data)/sizeof(data[0])), 0x000080);
    layer->setLineWidth(3);
    layer->setDataSymbol(Chart::Cross2Shape(), 15, 0xff00ff);

    // output the chart
    c->makeChart();

    // Include tool tip for the chart.
    *imageMap = layer->getHTMLImageMap("clickable", "",
        "title='Concentration on {label}: {value} ppm'");

    return c;
}


BaseChart *semicirclemeter(int img, const char **imageMap)
{
    // The value to display on the meter
    double value = 27.48;

    // Create an AngularMeter object of size 200 x 115 pixels, with silver background
    // color, black border, 2 pixel 3D border border and rounded corners
    AngularMeter *m = new AngularMeter(200, 115, Chart::silverColor(), 0x000000, 2);
    m->setRoundedFrame();

    // Set the meter center at (100, 100), with radius 85 pixels, and span from -90
    // to +90 degress (semi-circle)
    m->setMeter(100, 100, 85, -90, 90);

    // Meter scale is 0 - 100, with major tick every 20 units, minor tick every 10
    // units, and micro tick every 5 units
    m->setScale(0, 100, 20, 10, 5);

    // Set 0 - 60 as green (66FF66) zone
    m->addZone(0, 60, 0, 85, 0x66ff66);

    // Set 60 - 80 as yellow (FFFF33) zone
    m->addZone(60, 80, 0, 85, 0xffff33);

    // Set 80 - 100 as red (FF6666) zone
    m->addZone(80, 100, 0, 85, 0xff6666);

    // Add a text label centered at (100, 60) with 12 pts Arial Bold font
    m->addText(100, 60, "PSI", "arialbd.ttf", 12, Chart::TextColor, Chart::Center);

    // Add a text box at the top right corner of the meter showing the value
    // formatted to 2 decimal places, using white text on a black background, and
    // with 1 pixel 3D depressed border
    m->addText(156, 8, m->formatValue(value, "2"), "arial.ttf", 8, 0xffffff
        )->setBackground(0x000000, 0, -1);

    // Add a semi-transparent blue (40666699) pointer with black border at the
    // specified value
    m->addPointer(value, 0x40666699, 0x000000);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *roundmeter(int img, const char **imageMap)
{
    // The value to display on the meter
    double value = 45.17;

    // Create an AugularMeter object of size 200 x 200 pixels, with silver
    // background, black border, 2 pixel 3D depressed border and rounded corners.
    AngularMeter *m = new AngularMeter(200, 200, Chart::silverColor(), 0x000000, -2);
    m->setRoundedFrame();

    // Set the meter center at (100, 100), with radius 85 pixels, and span from -135
    // to +135 degress
    m->setMeter(100, 100, 85, -135, 135);

    // Meter scale is 0 - 100, with major tick every 10 units, minor tick every 5
    // units, and micro tick every 1 units
    m->setScale(0, 100, 10, 5, 1);

    // Disable default angular arc by setting its width to 0. Set 2 pixels line width
    // for major tick, and 1 pixel line width for minor ticks.
    m->setLineWidth(0, 2, 1);

    // Set the circular meter surface as metallic blue (9999DD)
    m->addRing(0, 90, Chart::metalColor(0x9999dd));

    // Add a blue (6666FF) ring between radii 88 - 90 as decoration
    m->addRing(88, 90, 0x6666ff);

    // Set 0 - 60 as green (99FF99) zone, 60 - 80 as yellow (FFFF00) zone, and 80 -
    // 100 as red (FF3333) zone
    m->addZone(0, 60, 0x99ff99);
    m->addZone(60, 80, 0xffff00);
    m->addZone(80, 100, 0xff3333);

    // Add a text label centered at (100, 135) with 15 pts Arial Bold font
    m->addText(100, 135, "CPU", "arialbd.ttf", 15, Chart::TextColor, Chart::Center);

    // Add a text box centered at (100, 165) showing the value formatted to 2 decimal
    // places, using white text on a black background, and with 1 pixel 3D depressed
    // border
    m->addText(100, 165, m->formatValue(value, "2"), "Arial", 8, 0xffffff,
        Chart::Center)->setBackground(0x000000, 0x000000, -1);

    // Add a semi-transparent blue (40333399) pointer at the specified value
    m->addPointer(value, 0x40333399);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *wideameter(int img, const char **imageMap)
{
    // The value to display on the meter
    double value = 6.5;

    // Create an AugularMeter object of size 200 x 100 pixels with rounded corners
    AngularMeter *m = new AngularMeter(200, 100);
    m->setRoundedFrame();

    // Set meter background according to a parameter
    if (img == 0) {
        // Use gold background color
        m->setBackground(Chart::goldColor(), 0x000000, -2);
    } else if (img == 1) {
        // Use silver background color
        m->setBackground(Chart::silverColor(), 0x000000, -2);
    } else if (img == 2) {
        // Use metallic blue (9898E0) background color
        m->setBackground(Chart::metalColor(0x9898e0), 0x000000, -2);
    } else if (img == 3) {
        // Use a wood pattern as background color
        m->setBackground(m->patternColor("wood.png"), 0x000000, -2);
    } else if (img == 4) {
        // Use a marble pattern as background color
        m->setBackground(m->patternColor("marble.png"), 0x000000, -2);
    } else {
        // Use a solid light purple (EEBBEE) background color
        m->setBackground(0xeebbee, 0x000000, -2);
    }

    // Set the meter center at (100, 235), with radius 210 pixels, and span from -24
    // to +24 degress
    m->setMeter(100, 235, 210, -24, 24);

    // Meter scale is 0 - 100, with a tick every 1 unit
    m->setScale(0, 10, 1);

    // Set 0 - 6 as green (99ff99) zone, 6 - 8 as yellow (ffff00) zone, and 8 - 10 as
    // red (ff3333) zone
    m->addZone(0, 6, 0x99ff99, 0x808080);
    m->addZone(6, 8, 0xffff00, 0x808080);
    m->addZone(8, 10, 0xff3333, 0x808080);

    // Add a title at the bottom of the meter using 10 pts Arial Bold font
    m->addTitle(Chart::Bottom, "OUTPUT POWER LEVEL\n", "arialbd.ttf", 10);

    // Add a semi-transparent black (80000000) pointer at the specified value
    m->addPointer(value, 0x80000000);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *squareameter(int img, const char **imageMap)
{
    // The value to display on the meter
    double value = 4.75;

    // Create an AugularMeter object of size 110 x 110 pixels, using silver
    // background color with a black 2 pixel 3D depressed border.
    AngularMeter *m = new AngularMeter(110, 110, Chart::silverColor(), 0x000000, -2);

    // Set meter appearance according to a parameter
    if (img == 0) {
        // Set the meter center at bottom left corner (15, 95), with radius 85
        // pixels. Meter spans from 90 - 0 degrees.
        m->setMeter(15, 95, 85, 90, 0);
        // Add a label to the meter centered at (35, 75)
        m->addText(35, 75, "VDC", "arialbd.ttf", 12, Chart::TextColor, Chart::Center)
            ;
        // Add a text box to show the value at top right corner (103, 7)
        m->addText(103, 7, m->formatValue(value, "2"), "arial.ttf", 8, 0xffffff,
            Chart::TopRight)->setBackground(0, 0, -1);
    } else if (img == 1) {
        // Set the meter center at top left corner (15, 15), with radius 85 pixels.
        // Meter spans from 90 - 180 degrees.
        m->setMeter(15, 15, 85, 90, 180);
        // Add a label to the meter centered at (35, 35)
        m->addText(35, 35, "AMP", "arialbd.ttf", 12, Chart::TextColor, Chart::Center)
            ;
        // Add a text box to show the value at bottom right corner (103, 103)
        m->addText(103, 103, m->formatValue(value, "2"), "arial.ttf", 8, 0xffffff,
            Chart::BottomRight)->setBackground(0, 0, -1);
    } else if (img == 2) {
        // Set the meter center at top right corner (15, 95), with radius 85 pixels.
        // Meter spans from 270 - 180 degrees.
        m->setMeter(95, 15, 85, 270, 180);
        // Add a label to the meter centered at (75, 35)
        m->addText(75, 35, "KW", "arialbd.ttf", 12, Chart::TextColor, Chart::Center);
        // Add a text box to show the value at bottom left corner (7, 103)
        m->addText(7, 103, m->formatValue(value, "2"), "arial.ttf", 8, 0xffffff,
            Chart::BottomLeft)->setBackground(0, 0, -1);
    } else {
        // Set the meter center at bottom right corner (95, 95), with radius 85
        // pixels. Meter spans from 270 - 360 degrees.
        m->setMeter(95, 95, 85, 270, 360);
        // Add a label to the meter centered at (75, 75)
        m->addText(75, 75, "RPM", "arialbd.ttf", 12, Chart::TextColor, Chart::Center)
            ;
        // Add a text box to show the value at top left corner (7, 7)
        m->addText(7, 7, m->formatValue(value, "2"), "arial.ttf", 8, 0xffffff,
            Chart::TopLeft)->setBackground(0, 0, -1);
    }

    // Meter scale is 0 - 10, with a major tick every 2 units, and minor tick every 1
    // unit
    m->setScale(0, 10, 2, 1);

    // Set 0 - 6 as green (99ff99) zone, 6 - 8 as yellow (ffff00) zone, and 8 - 10 as
    // red (ff3333) zone
    m->addZone(0, 6, 0x99ff99, 0x808080);
    m->addZone(6, 8, 0xffff00, 0x808080);
    m->addZone(8, 10, 0xff3333, 0x808080);

    // Add a semi-transparent black (80000000) pointer at the specified value
    m->addPointer(value, 0x80000000);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *multiameter(int img, const char **imageMap)
{
    // Create an AugularMeter object of size 200 x 200 pixels
    AngularMeter *m = new AngularMeter(200, 200);

    // Use white on black color palette for default text and line colors
    m->setColors(Chart::whiteOnBlackPalette);

    // Set the meter center at (100, 100), with radius 85 pixels, and span from 0 to
    // 360 degress
    m->setMeter(100, 100, 85, 0, 360);

    // Meter scale is 0 - 100, with major tick every 10 units, minor tick every 5
    // units, and micro tick every 1 units
    m->setScale(0, 100, 10, 5, 1);

    // Set angular arc, major tick and minor tick line widths to 2 pixels.
    m->setLineWidth(2, 2, 2);

    // Add a blue (9999ff) ring between radii 88 - 90 as decoration
    m->addRing(88, 90, 0x9999ff);

    // Set 0 - 60 as green (00AA00) zone, 60 - 80 as yellow (CCCC00) zone, and 80 -
    // 100 as red (AA0000) zone
    m->addZone(0, 60, 0x00aa00);
    m->addZone(60, 80, 0xcccc00);
    m->addZone(80, 100, 0xaa0000);

    // Add a text label centered at (100, 70) with 12 pts Arial Bold font
    m->addText(100, 70, "PSI", "arialbd.ttf", 12, Chart::TextColor, Chart::Center);

    // Add a semi-transparent blue (806666FF) pointer    using the default shape
    m->addPointer(25, 0x806666FF, 0x6666ff);

    // Add a semi-transparent red (80FF6666) pointer using the arrow shape
    m->addPointer(9, 0x80FF6666, 0xff6666)->setShape(Chart::ArrowPointer2);

    // Add a semi-transparent yellow (80FFFF66) pointer using another arrow shape
    m->addPointer(51, 0x80FFFF66, 0xffff66)->setShape(Chart::ArrowPointer);

    // Add a semi-transparent green (8066FF66) pointer using the line shape
    m->addPointer(72, 0x8066FF66, 0x66ff66)->setShape(Chart::LinePointer);

    // Add a semi-transparent grey (80CCCCCC) pointer using the pencil shape
    m->addPointer(85, 0x80CCCCCC, 0xcccccc)->setShape(Chart::PencilPointer);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *iconameter(int img, const char **imageMap)
{
    // The value to display on the meter
    double value = 85;

    // Create an AugularMeter object of size 70 x 90 pixels, using black background
    // with a 2 pixel 3D depressed border.
    AngularMeter *m = new AngularMeter(70, 90, 0, 0, -2);

    // Use white on black color palette for default text and line colors
    m->setColors(Chart::whiteOnBlackPalette);

    // Set the meter center at (10, 45), with radius 50 pixels, and span from 135 to
    // 45 degress
    m->setMeter(10, 45, 50, 135, 45);

    // Set meter scale from 0 - 100, with the specified labels
    const char * pattern1[] = {"E", " ", " ", " ", "F"};
    m->setScale(0, 100, StringArray(pattern1, sizeof(pattern1)/sizeof(pattern1[0])));

    // Set the angular arc and major tick width to 2 pixels
    m->setLineWidth(2, 2);

    // Add a red zone at 0 - 15
    m->addZone(0, 15, 0xff3333);

    // Add an icon at (25, 35)
    m->addText(25, 35, "<*img=gas.gif*>");

    // Add a yellow (ffff00) pointer at the specified value
    m->addPointer(value, 0xffff00);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *hlinearmeter(int img, const char **imageMap)
{
    // The value to display on the meter
    double value = 75.35;

    // Create an LinearMeter object of size 250 x 75 pixels, using silver background
    // with a 2 pixel black 3D depressed border.
    LinearMeter *m = new LinearMeter(250, 75, Chart::silverColor(), 0, -2);

    // Set the scale region top-left corner at (15, 25), with size of 200 x 50
    // pixels. The scale labels are located on the top (implies horizontal meter)
    m->setMeter(15, 25, 220, 20, Chart::Top);

    // Set meter scale from 0 - 100, with a tick every 10 units
    m->setScale(0, 100, 10);

    // Set 0 - 50 as green (99ff99) zone, 50 - 80 as yellow (ffff66) zone, and 80 -
    // 100 as red (ffcccc) zone
    m->addZone(0, 50, 0x99ff99);
    m->addZone(50, 80, 0xffff66);
    m->addZone(80, 100, 0xffcccc);

    // Add a blue (0000cc) pointer at the specified value
    m->addPointer(value, 0x0000cc);

    // Add a label at bottom-left (10, 68) using Arial Bold/8 pts/red (c00000)
    m->addText(10, 68, "Temperature C", "arialbd.ttf", 8, 0xc00000, Chart::BottomLeft
        );

    // Add a text box to show the value formatted to 2 decimal places at bottom
    // right. Use white text on black background with a 1 pixel depressed 3D border.
    m->addText(238, 70, m->formatValue(value, "2"), "arial.ttf", 8, 0xffffff,
        Chart::BottomRight)->setBackground(0, 0, -1);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *vlinearmeter(int img, const char **imageMap)
{
    // The value to display on the meter
    double value = 75.35;

    // Create an LinearMeter object of size 60 x 265 pixels, using silver background
    // with a 2 pixel black 3D depressed border.
    LinearMeter *m = new LinearMeter(60, 265, Chart::silverColor(), 0, -2);

    // Set the scale region top-left corner at (25, 30), with size of 20 x 200
    // pixels. The scale labels are located on the left (default - implies vertical
    // meter)
    m->setMeter(25, 30, 20, 200);

    // Set meter scale from 0 - 100, with a tick every 10 units
    m->setScale(0, 100, 10);

    // Set 0 - 50 as green (99ff99) zone, 50 - 80 as yellow (ffff66) zone, and 80 -
    // 100 as red (ffcccc) zone
    m->addZone(0, 50, 0x99ff99);
    m->addZone(50, 80, 0xffff66);
    m->addZone(80, 100, 0xffcccc);

    // Add a deep blue (000080) pointer at the specified value
    m->addPointer(value, 0x000080);

    // Add a text box label at top-center (30, 5) using Arial Bold/8 pts/deep blue
    // (000088), with a light blue (9999ff) background
    m->addText(30, 5, "Temp C", "arialbd.ttf", 8, 0x000088, Chart::TopCenter
        )->setBackground(0x9999ff);

    // Add a text box to show the value formatted to 2 decimal places at bottom
    // center (30, 260). Use white text on black background with a 1 pixel depressed
    // 3D border.
    m->addText(30, 260, m->formatValue(value, "2"), "arial.ttf", 8, 0xffffff,
        Chart::BottomCenter)->setBackground(0, 0, -1);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *multihmeter(int img, const char **imageMap)
{
    // The values to display on the meter
    double value0 = 30.99;
    double value1 = 45.35;
    double value2 = 77.64;

    // Create an LinearMeter object of size 250 x 75 pixels, using silver background
    // with a 2 pixel black 3D depressed border.
    LinearMeter *m = new LinearMeter(250, 75, Chart::silverColor(), 0, -2);

    // Set the scale region top-left corner at (15, 25), with size of 220 x 20
    // pixels. The scale labels are located on the top (implies horizontal meter)
    m->setMeter(15, 25, 220, 20, Chart::Top);

    // Set meter scale from 0 - 100, with a tick every 10 units
    m->setScale(0, 100, 10);

    // Set 0 - 50 as green (99ff99) zone, 50 - 80 as yellow (ffff66) zone, and 80 -
    // 100 as red (ffcccc) zone
    m->addZone(0, 50, 0x99ff99);
    m->addZone(50, 80, 0xffff66);
    m->addZone(80, 100, 0xffcccc);

    // Add deep red (000080), deep green (008000) and deep blue (800000) pointers to
    // reflect the values
    m->addPointer(value0, 0x000080);
    m->addPointer(value1, 0x008000);
    m->addPointer(value2, 0x800000);

    // Add a label at bottom-left (10, 68) using Arial Bold/8 pts/red (c00000)
    m->addText(10, 68, "Temp C", "arialbd.ttf", 8, 0xc00000, Chart::BottomLeft);

    // Add three text boxes to show the values in this meter
    m->addText(148, 70, m->formatValue(value0, "2"), "arial.ttf", 8, 0x6666ff,
        Chart::BottomRight)->setBackground(0, 0, -1);
    m->addText(193, 70, m->formatValue(value1, "2"), "arial.ttf", 8, 0x33ff33,
        Chart::BottomRight)->setBackground(0, 0, -1);
    m->addText(238, 70, m->formatValue(value2, "2"), "arial.ttf", 8, 0xff3333,
        Chart::BottomRight)->setBackground(0, 0, -1);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *multivmeter(int img, const char **imageMap)
{
    // The values to display on the meter
    double value0 = 30.99;
    double value1 = 45.35;
    double value2 = 77.64;

    // Create an LinearMeter object of size 60 x 245 pixels, using silver background
    // with a 2 pixel black 3D depressed border.
    LinearMeter *m = new LinearMeter(60, 245, Chart::silverColor(), 0, -2);

    // Set the scale region top-left corner at (25, 30), with size of 20 x 200
    // pixels. The scale labels are located on the left (default - implies vertical
    // meter)
    m->setMeter(25, 30, 20, 200);

    // Set meter scale from 0 - 100, with a tick every 10 units
    m->setScale(0, 100, 10);

    // Set 0 - 50 as green (99ff99) zone, 50 - 80 as yellow (ffff66) zone, and 80 -
    // 100 as red (ffcccc) zone
    m->addZone(0, 50, 0x99ff99);
    m->addZone(50, 80, 0xffff66);
    m->addZone(80, 100, 0xffcccc);

    // Add deep red (000080), deep green (008000) and deep blue (800000) pointers to
    // reflect the values
    m->addPointer(value0, 0x000080);
    m->addPointer(value1, 0x008000);
    m->addPointer(value2, 0x800000);

    // Add a text box label at top-center (30, 5) using Arial Bold/8 pts/deep blue
    // (000088), with a light blue (9999ff) background
    m->addText(30, 5, "Temp C", "arialbd.ttf", 8, 0x000088, Chart::TopCenter
        )->setBackground(0x9999ff);

    // output the chart
    m->makeChart();

    return m;
}


BaseChart *linearzonemeter(int img, const char **imageMap)
{
    // The value to display on the meter
    double value = 85;

    // Create an LinearMeter object of size 210 x 45 pixels, using silver background
    // with a 2 pixel black 3D depressed border.
    LinearMeter *m = new LinearMeter(210, 45, Chart::silverColor(), 0, -2);

    // Set the scale region top-left corner at (5, 5), with size of 200 x 20 pixels.
    // The scale labels are located on the bottom (implies horizontal meter)
    m->setMeter(5, 5, 200, 20, Chart::Bottom);

    // Set meter scale from 0 - 100
    m->setScale(0, 100);

    // Add a title at the bottom of the meter with a 1 pixel raised 3D border
    m->addTitle(Chart::Bottom, "Battery Level", "arialbd.ttf", 8)->setBackground(
        Chart::Transparent, -1, 1);

    // Set 3 zones of different colors to represent Good/Weak/Bad data ranges
    m->addZone(50, 100, 0x99ff99, "Good");
    m->addZone(20, 50, 0xffff66, "Weak");
    m->addZone(0, 20, 0xffcccc, "Bad");

    // Add empty labels (just need the ticks) at 0/20/50/80 as separators for zones
    m->addLabel(0, " ");
    m->addLabel(20, " ");
    m->addLabel(50, " ");
    m->addLabel(100, " ");

    // Add a semi-transparent blue (800000ff) pointer at the specified value, using
    // triangular pointer shape
    m->addPointer(value, 0x800000FF)->setShape(Chart::TriangularPointer);

    // output the chart
    m->makeChart();

    return m;
}


////////////////////////////////////////////////////////////////////////////////
//
//	An array storing all demo chart functions. An empty entry signals the end 
//	of the array. 
//
////////////////////////////////////////////////////////////////////////////////
DemoChart demoCharts[] = 
{
    {"==============================", 0, 0},
    {">>> Pie Chart Samples", 0, 0},
    {"==============================", 0, 0},
    {"Simple Pie Chart", 1, simplepie},
    {"3D Pie Chart", 1, threedpie},
    {"Multi-Depth Pie Chart", 1, multidepthpie},
    {"Side Label Layout", 1, sidelabelpie},
    {"Pie with Small Sectors", 1, smallsectorpie},
    {"Circular Label Layout", 2, circlelabelpie},
    {"Pie Chart with Legend", 1, legendpie},
    {"Icon Pie Chart", 1, iconpie},
    {"Icon Pie Chart (2)", 1, iconpie2},
    {"Multi-Pie Chart", 3, multipie},
    {"Donut Chart", 1, donut},
    {"Icon Donut Chart", 1, icondonut},
    {"Texture Donut Chart", 1, texturedonut},
    {"Concentric Donut Chart", 1, concentric},
    {"Text Style and Colors", 1, fontpie},
    {"Coloring and Wallpaper", 4, colorpie},
    {"3D Angle", 7, threedanglepie},
    {"3D Depth", 5, threeddepthpie},
    {"3D Shadow Mode", 4, shadowpie},
    {"Start Angle and Direction", 2, anglepie},
    {"Donut Width", 5, donutwidth},
    {"==============================", 0, 0},
    {">>> Bar Chart Samples", 0, 0},
    {"==============================", 0, 0},
    {"Simple Bar Chart", 1, simplebar},
    {"Multi-Color Bar Chart", 1, colorbar},
    {"Soft Bar Shading", 1, softlightbar},
    {"Glass Bar Shading", 1, glasslightbar},
    {"Cylinder Bar Shading", 1, cylinderlightbar},
    {"3D Bar Chart", 1, threedbar},
    {"Cylinder Bar Shape", 1, cylinderbar},
    {"Polygon Bar Shapes", 1, polygonbar},
    {"Stacked Bar Chart", 1, stackedbar},
    {"Percentage Bar Chart", 1, percentbar},
    {"Multi-Bar Chart", 1, multibar},
    {"Soft Multi-Bar Chart", 1, softmultibar},
    {"Glass Multi-Bar Chart", 1, glassmultibar},
    {"Multi-Cylinder Chart", 1, multicylinder},
    {"Multi-Shape Bar Chart", 1, multishapebar},
    {"Overlapping Bar Chart", 1, overlapbar},
    {"Multi-Stacked Bar Chart", 1, multistackbar},
    {"Depth Bar Chart", 1, depthbar},
    {"Positive Negative Bars", 1, posnegbar},
    {"Borderless Bar Chart", 1, hbar},
    {"Dual Horizontal Bar Charts", 1, dualhbar},
    {"Pareto Chart", 1, pareto},
    {"Bar Gap", 6, gapbar},
    {"==============================", 0, 0},
    {">>> Line Chart Samples", 0, 0},
    {"==============================", 0, 0},
    {"Simple Line Chart", 1, simpleline},
    {"Compact Line Chart", 1, compactline},
    {"3D Line Chart", 1, threedline},
    {"Multi-Line Chart", 1, multiline},
    {"Symbol Line Chart", 1, symbolline},
    {"Symbol Line Chart (2)", 1, symbolline2},
    {"Spline Line Chart", 1, splineline},
    {"Step Line Chart", 1, stepline},
    {"Line Comparison", 1, linecompare},
    {"Line with Error Symbols", 1, errline},
    {"Multi-Symbol Line Chart", 1, multisymbolline},
    {"Custom Symbols", 1, customsymbolline},
    {"Rotated Line Chart", 1, rotatedline},
    {"Arbitrary XY Line Chart", 1, xyline},
    {"Discontinuous Lines", 1, discontline},
    {"==============================", 0, 0},
    {">>> Trending and Curve Fitting", 0, 0},
    {"==============================", 0, 0},
    {"Trend Line Chart", 1, trendline},
    {"Scatter Trend Chart", 1, scattertrend},
    {"Confidence Band", 1, confidenceband},
    {"General Curve Fitting", 1, curvefitting},
    {"==============================", 0, 0},
    {">>> Scatter/Bubble/Vector Charts", 0, 0},
    {"==============================", 0, 0},
    {"Scatter Chart", 1, scatter},
    {"Built-in Symbols", 1, builtinsymbols},
    {"Custom Scatter Symbols", 1, scattersymbols},
    {"Custom Scatter Labels", 1, scatterlabels},
    {"Bubble Chart", 1, bubble},
    {"Bubble XY Scaling", 1, bubblescale},
    {"Vector Chart", 1, vector},
    {"==============================", 0, 0},
    {">>> Area Chart Samples", 0, 0},
    {"==============================", 0, 0},
    {"Simple Area Chart", 1, simplearea},
    {"Enhanced Area Chart", 1, enhancedarea},
    {"3D Area Chart", 1, threedarea},
    {"Pattern Area Chart", 1, patternarea},
    {"Stacked Area Chart", 1, stackedarea},
    {"3D Stacked Area Chart", 1, threedstackarea},
    {"Percentage Area Chart", 1, percentarea},
    {"Depth Area Chart", 1, deptharea},
    {"Rotated Area Chart", 1, rotatedarea},
    {"==============================", 0, 0},
    {">>> Floating Box/Waterfall Charts", 0, 0},
    {"==============================", 0, 0},
    {"Box-Whisker Chart", 1, boxwhisker},
    {"Box-Whisker Chart (2)", 1, boxwhisker2},
    {"Floating Box Chart", 1, floatingbox},
    {"Waterfall Chart", 1, waterfall},
    {"Pos/Neg Waterfall Chart", 1, posnegwaterfall},
    {"==============================", 0, 0},
    {">>> Gantt Charts", 0, 0},
    {"==============================", 0, 0},
    {"Simple Gantt Chart", 1, gantt},
    {"Multi-Color Gantt Chart", 1, colorgantt},
    {"Multi-Layer Gantt Chart", 1, layergantt},
    {"==============================", 0, 0},
    {">>> Finance Charts", 0, 0},
    {"==============================", 0, 0},
    {"Simple HLOC Chart", 1, hloc},
    {"Simple Candlestick Chart", 1, candlestick},
    {"Finance Chart (1)", 1, finance},
    {"Finance Chart (2)", 1, finance2},
    {"==============================", 0, 0},
    {">>> Other XY Chart Features", 0, 0},
    {"==============================", 0, 0},
    {"Marks and Zones", 1, markzone},
    {"Marks and Zones (2)", 1, markzone2},
    {"Y Zone Coloring", 1, yzonecolor},
    {"X Zone Coloring", 1, xzonecolor},
    {"Dual Y-Axis", 1, dualyaxis},
    {"Dual X-Axis", 1, dualxaxis},
    {"Multiple Axes", 1, multiaxes},
    {"4 Quadrant Chart", 1, fourq},
    {"Text Style and Colors", 1, fontxy},
    {"Background and Wallpaper", 4, background},
    {"Log Scale Axis", 2, logaxis},
    {"Y-Axis Scaling", 5, axisscale},
    {"Tick Density", 2, ticks},
    {"==============================", 0, 0},
    {">>> Polar Charts", 0, 0},
    {"==============================", 0, 0},
    {"Simple Radar Chart", 1, simpleradar},
    {"Multi Radar Chart", 1, multiradar},
    {"Stacked Radar Chart", 1, stackradar},
    {"Polar Line Chart", 1, polarline},
    {"Polar Area Chart", 1, polararea},
    {"Polar Spline Chart", 1, polarspline},
    {"Polar Scatter Chart", 1, polarscatter},
    {"Polar Bubble Chart", 1, polarbubble},
    {"Polar Vector Chart", 1, polarvector},
    {"Simple Rose Chart", 1, rose},
    {"Stacked Rose Chart", 1, stackrose},
    {"Circular Zones", 1, polarzones},
    {"Sector Zones", 1, polarzones2},
    {"==============================", 0, 0},
    {">>> Meters/Dials/Guages", 0, 0},
    {"==============================", 0, 0},
    {"Semi-Circle Meter", 1, semicirclemeter},
    {"Round Meter", 1, roundmeter},
    {"Wide Angular Meters", 6, wideameter},
    {"Square Angular Meters", 4, squareameter},
    {"Multi-Pointer Angular Meter", 1, multiameter},
    {"Icon Angular Meter", 1, iconameter},
    {"Horizontal Linear Meter", 1, hlinearmeter},
    {"Vertical Linear Meter", 1, vlinearmeter},
    {"Multi-Pointer Horizontal Meter", 1, multihmeter},
    {"Multi-Pointer Vertical Meter", 1, multivmeter},
    {"Linear Zone Meter", 1, linearzonemeter},
    {0, 0, 0}
};
