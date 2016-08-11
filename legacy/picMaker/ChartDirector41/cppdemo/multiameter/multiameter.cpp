#include "chartdir.h"

int main(int argc, char *argv[])
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
    m->makeChart("multiameter.png");

    //free up resources
    delete m;
    return 0;
}

