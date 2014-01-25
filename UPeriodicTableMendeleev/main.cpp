#include <QApplication>
#include <QTextCodec>
#include <QDateTime>

#include "UPeriodicTableParser.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>

#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include <QGraphicsBlurEffect>

#include <QTabWidget>

int main(int argc, char *argv[])
{
    qsrand(QDateTime::currentMSecsSinceEpoch());

    QApplication app(argc, argv);
    app.setApplicationName("PeriodicTableMendeleev");
    app.setApplicationVersion("0.0.1");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

    QString path = qApp->applicationDirPath()
            + "\\" + "periodic_table_mendeleev.xml";

    QTableWidget *tWidget = new QTableWidget();
    QTableWidget *tWidget1 = new QTableWidget();

    tWidget->horizontalHeader()->hide();
    tWidget->verticalHeader()->hide();

    tWidget->setWindowTitle("Свободно");
    tWidget1->setWindowTitle("По порядковому номеру");

    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsScene *scene1 = new QGraphicsScene();

    bool ok;

    UPeriodicTableParser parser;
    parser.fromXml(path, &ok);

    parser.toTableWidget(tWidget, false);
    parser.toTableWidget(tWidget1, true);

    parser.toGraphicsScene(scene, false);
    parser.toGraphicsScene(scene1, true);

    QGraphicsView *view = new QGraphicsView();
    view->setRenderHint(QPainter::Antialiasing);
    view->setWindowTitle("Свободно");
    view->setScene(scene);

    QGraphicsView *view1 = new QGraphicsView();
    view1->setRenderHint(QPainter::Antialiasing);
    view1->setWindowTitle("По порядковому номеру");
    view1->setScene(scene1);

    if(!ok)
    {
        qDebug() << "Произошла ошибка при парсинге";
        return 1;
    }

    QTabWidget *tabWidgetTable = new QTabWidget();
    tabWidgetTable->addTab(tWidget, tWidget->windowTitle());
    tabWidgetTable->addTab(tWidget1, tWidget1->windowTitle());

    QTabWidget *tabWidgetScene = new QTabWidget();
    tabWidgetScene->addTab(view, view->windowTitle());
    tabWidgetScene->addTab(view1, view1->windowTitle());

    tabWidgetTable->setTabPosition(QTabWidget::South);
    tabWidgetScene->setTabPosition(QTabWidget::South);

    tabWidgetTable->setWindowTitle("QTableWidget - Периодическая система химических элементов");
    tabWidgetScene->setWindowTitle("QGraphicsScene - Периодическая система химических элементов");

    tabWidgetTable->show();
    tabWidgetScene->show();

    return app.exec();
}
