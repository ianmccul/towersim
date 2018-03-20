// -*- C++ -*-

// to inhibit screensaver: qdbus org.freedesktop.ScreenSaver /ScreenSaver SimulateUserActivity

#include "chart.h"
#include "chartview.h"
#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

QT_CHARTS_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow window;
    Chart *chart = new Chart;
    chart->legend()->hide();
    chart->EnableAnimations();
    chart->ConnectTo("192.168.0.20", 5701);
    //chart->ConnectTo("localhost", 5701);
    ChartView chartView(chart);
    chartView.setRenderHint(QPainter::Antialiasing);
    window.setCentralWidget(&chartView);
    window.resize(400, 300);
    window.show();
    return a.exec();
}

