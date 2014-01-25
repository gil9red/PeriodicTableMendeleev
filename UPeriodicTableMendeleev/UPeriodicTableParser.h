#ifndef UPERIODICTABLEPARSER_H
#define UPERIODICTABLEPARSER_H

#include <QDomElement>

#include <QTableWidget>
#include <QHeaderView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

#include <QFile>
#include <QDebug>

struct UFamilyElement
{
    UFamilyElement() { }
    UFamilyElement(QString _name, QString _color)
        : name(_name), color(_color) { }

    QString name;
    QString color;
};

struct UElement
{
    QString name;
    QString number;
    QString mass;
    QString longName;
    QString row;
    QString column;

    UFamilyElement family;
};


class UPeriodicTableParser
{
public:
    UPeriodicTableParser();
    UPeriodicTableParser * fromXml(const QString path, bool *ok = 0);
    void toTableWidget(QTableWidget *tw, const bool directAsNumber = false);
    void toGraphicsScene(QGraphicsScene *gs, const bool directAsNumber = false);
    QList <UElement> getSortToNumberListElements();

public:
    int d_row;
    int d_column;
    QList <UElement> d_elements;
    QMap <QString, UFamilyElement> d_families;

private:
    void init();
    UElement getElementFromDom(const QDomElement &de);
};

#endif // UPERIODICTABLEPARSER_H
