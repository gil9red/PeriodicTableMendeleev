#ifndef PERIODICTABLEPARSER_H
#define PERIODICTABLEPARSER_H

#include <QDomElement>

class FamilyElement;

class Element
{
public:
    QString name;
    QString number;
    QString mass;
    QString longName;
    QString row;
    QString column;

    FamilyElement * family;
};

#include <QList>
class FamilyElement
{
public:
    FamilyElement() { }
    FamilyElement( const QString & n, const QString & c) : name(n), color(c) { }

    QString name;
    QString color;

    QList < Element * > elements;
};

#include <QHash>


#include <QTableWidget>
#include <QGraphicsScene>
#include <QTreeWidget>
class PeriodicTableParser
{
public:
    PeriodicTableParser();
    PeriodicTableParser * fromXml(const QString & path, bool *ok = 0);
    void toTableWidget(QTableWidget *tw, const bool directAsNumber = false);
    void toGraphicsScene(QGraphicsScene *gs, const bool directAsNumber = false);
    void toTreeWidget(QTreeWidget *tw);
    QList < Element * > getSortToNumberListElements();

public:
    int row;
    int column;

    QList < FamilyElement * > allFamilies;
    QHash < QString, FamilyElement * > hash_Name_Family;

    QList < Element * > allElements;
    QHash < QString, Element * > hash_Name_Element;

private:
    Element * getElementFromDom(const QDomElement &de);
};

#endif // PERIODICTABLEPARSER_H
