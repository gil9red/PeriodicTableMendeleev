#include "PeriodicTableParser.h"
#include "utils.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QFile>

bool compareElementsLessThan( Element * el1, Element * el2 )
{
    return el1->number.toInt() < el2->number.toInt();
}

PeriodicTableParser::PeriodicTableParser()
    : row(-1), column(-1)
{

}

PeriodicTableParser * PeriodicTableParser::fromXml(const QString & path, bool *ok)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        WARNING( qPrintable( file.errorString() ) );
        *ok = false;
        return this;
    }

    QString error;
    int errorLine;
    int errorColumn;

    QDomDocument doc("PeriodicTable");
    if (!doc.setContent(file.readAll(), &error, &errorLine, &errorColumn))
    {
        WARNING( qPrintable( QString( error + QString(" %1; %2").arg(errorLine).arg(errorColumn) ) ) );
        file.close();
        *ok = false;
        return this;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();

    while(!n.isNull())
    {
        QDomElement de = n.toElement();
        if(!de.isNull())
        {
            if(de.tagName() == "family")
            {
                QDomNodeList nodeFamilyList = de.childNodes();
                for(int i = 0; i < nodeFamilyList.size(); i++)
                {
                    QDomElement familyElement = nodeFamilyList.at(i).toElement();

                    QString tagName = familyElement.tagName();
                    QString name = familyElement.attribute("name");
                    QString color = familyElement.attribute("color");

                    FamilyElement * family = new FamilyElement(name, color);
                    allFamilies << family;
                    hash_Name_Family.insert(tagName, family);
                }

            }else if(de.tagName() == "element")
            {
                Element * el = getElementFromDom(de);
                allElements << el;
                hash_Name_Element.insert( el->name, el );

                if(row < el->row.toInt())
                    row = el->row.toInt();

                if(column < el->column.toInt())
                    column = el->column.toInt();
            }
        }

        n = n.nextSibling();
    }

    *ok = true;
    return this;
}
void PeriodicTableParser::toTableWidget(QTableWidget *tw, const bool directAsNumber)
{
    tw->clear();

    if(directAsNumber)
    {
        tw->setColumnCount(9);
        tw->setRowCount(1);
        tw->horizontalHeader()->hide();
        tw->verticalHeader()->hide();
    }else
    {
        tw->setColumnCount(column);
        tw->setRowCount(row);
    }

    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tw->setSelectionMode(QAbstractItemView::NoSelection);
    tw->setGridStyle(Qt::NoPen);
    tw->setShowGrid(true);

    if(directAsNumber)
    {
        int row = 0;
        int column = 0;
        foreach(Element * el, getSortToNumberListElements())
        {
            tw->setRowCount(row+1);

            QTableWidgetItem *item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString("%1").arg(el->name));
            item->setToolTip(QString("<font size=2 align=left>%1</font>"
                                     "<p align=right>"
                                     "<b>%2</b><br>"
                                     "%3<br>"
                                     "%4<br>"
                                     "<font size=2>%5</font></p>")
                             .arg(el->number)
                             .arg(el->name)
                             .arg(el->longName)
                             .arg(el->family->name)
                             .arg(el->mass));

            item->setBackgroundColor(QColor(el->family->color));
            tw->setItem(row, column, item);

            if(++column % tw->columnCount() == 0)
            {
                column = 0;
                row++;
            }
        }

    }else
    {
        foreach(Element * el, allElements)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString("%1").arg(el->name));
            item->setToolTip(QString("<font size=2 align=left>%1</font>"
                                     "<p align=right>"
                                     "<b>%2</b><br>"
                                     "%3<br>"
                                     "%4<br>"
                                     "<font size=2>%5</font></p>")
                             .arg(el->number)
                             .arg(el->name)
                             .arg(el->longName)
                             .arg(el->family->name)
                             .arg(el->mass));
            item->setBackgroundColor(QColor(el->family->color));
            int row = el->row.toInt() - 1;
            int column = el->column.toInt() - 1 ;
            tw->setItem(row, column, item);
        }
    }

    tw->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
    tw->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
void PeriodicTableParser::toGraphicsScene(QGraphicsScene *gs, const bool directAsNumber)
{
    gs->clear();

    int size = 28;
    int row = 0;
    int column = 0;
    int indent = 4;

    int lastX = 0;
    int lastY = 0;

    const QList <Element * > & elements = directAsNumber ? getSortToNumberListElements() : allElements;

    foreach(Element * el, elements)
    {
        QGraphicsRectItem *item = new QGraphicsRectItem();
        item->setRect(0, 0, size, size);

        int x = directAsNumber ? lastX : (el->column.toInt() - 1) * size + indent * size;
        int y = directAsNumber ? lastY : (el->row.toInt() - 1) * size + indent * size;

        item->moveBy(x, y);

        if(directAsNumber)
            x += size + indent;

        if(++column % 9 == 0)
        {
            if(directAsNumber)
            {
                y += size + indent;
                x = 0;
            }
            column = 0;
            row++;
        }

        if(directAsNumber)
        {
            lastX = x;
            lastY = y;
        }

//        item->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable);

        item->setBrush(QBrush(QColor(el->family->color)));

        QGraphicsTextItem *textItem = new QGraphicsTextItem(item);

//        textItem->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable);
        textItem->setTextWidth(size);
        textItem->setHtml(QString("<center>%2</center>").arg(el->name));
        textItem->setToolTip(QString("<font size=2 align=left>%1</font>"
                                 "<p align=right>"
                                 "<b>%2</b><br>"
                                 "%3<br>"
                                 "%4<br>"
                                 "<font size=2>%5</font></p>")
                         .arg(el->number)
                         .arg(el->name)
                         .arg(el->longName)
                         .arg(el->family->name)
                         .arg(el->mass));

        gs->addItem(item);
    }
}
void PeriodicTableParser::toTreeWidget(QTreeWidget *tw)
{
    tw->setAnimated( true );
    tw->setColumnCount( 3 );
    tw->setHeaderLabels( QStringList() << "Название" << "Атомная масса (г/моль)"<< "Атомный номер" );

    foreach ( FamilyElement * family, allFamilies )
    {
        QTreeWidgetItem * familyItem = new QTreeWidgetItem();
        QFont font = familyItem->font(0);
        font.setPointSize( font.pointSize() + 1 );

        familyItem->setFont( 0, font );
        familyItem->setText( 0, family->name );

        for ( int i = 0; i < tw->columnCount(); i++ )
            familyItem->setBackground( i, QColor( family->color ) );

        tw->addTopLevelItem( familyItem );

        foreach ( Element * element, family->elements )
        {
            QTreeWidgetItem * elementItem = new QTreeWidgetItem();
            elementItem->setText( 0, QString("%1 (%2)").arg( element->name ).arg( element->longName ) );
            elementItem->setText( 1, element->mass );
            elementItem->setText( 2, element->number );

            const QString & toolTip = QString( "<font size=2 align=left>%1</font>"
                                               "<p align=right>"
                                               "<b>%2</b><br>"
                                               "%3<br>"
                                               "%4<br>"
                                               "<font size=2>%5</font></p>" )
                                   .arg( element->number )
                                   .arg( element->name )
                                   .arg( element->longName )
                                   .arg( family->name )
                                   .arg( element->mass );

            for ( int i = 0; i < tw->columnCount(); i++ )
                elementItem->setToolTip( i, toolTip );

            familyItem->addChild( elementItem );
        }
    }

    tw->header()->resizeSections(QHeaderView::ResizeToContents);
}
QList<Element *> PeriodicTableParser::getSortToNumberListElements()
{
   QList <Element *> copyList = allElements;
   qSort(copyList.begin(), copyList.end(), compareElementsLessThan);
   return copyList;
}

Element * PeriodicTableParser::getElementFromDom(const QDomElement &de)
{
    Element * element = new Element;
    element->name = de.attribute("name");
    element->number = de.attribute("number");
    element->mass = de.attribute("mass");
    element->longName = de.attribute("longName");
    element->row = de.attribute("row");
    element->column = de.attribute("column");

    const QString & nameFamily = de.attribute("family");
    FamilyElement * familyElement = hash_Name_Family.value( nameFamily );
    familyElement->elements << element;
    element->family = familyElement;


    return element;
}
