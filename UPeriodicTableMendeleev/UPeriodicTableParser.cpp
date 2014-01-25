#include "UPeriodicTableParser.h"

bool compareElementsLessThan(const UElement &el1, const UElement &el2)
{
    return el1.number.toInt() < el2.number.toInt();
}

/// PUBLIC
UPeriodicTableParser::UPeriodicTableParser()
{
    init();
}

UPeriodicTableParser * UPeriodicTableParser::fromXml(const QString path, bool *ok)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << file.errorString();
        *ok = false;
        return this;
    }

    QString error;
    int row;
    int col;

    QDomDocument doc("PeriodicTable");
    if (!doc.setContent(file.readAll(), &error, &row, &col))
    {
        qDebug() << error << QString("%1;%2").arg(row).arg(col);
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
                    d_families.insert(tagName, UFamilyElement(name, color));
                }

            }else if(de.tagName() == "element")
            {
                UElement el = getElementFromDom(de);
                d_elements << el;

                if(d_row < el.row.toInt())
                    d_row = el.row.toInt();

                if(d_column < el.column.toInt())
                    d_column = el.column.toInt();
            }
        }

        n = n.nextSibling();
    }

    *ok = true;
    return this;
}
void UPeriodicTableParser::toTableWidget(QTableWidget *tw, const bool directAsNumber)
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
        tw->setColumnCount(d_column);
        tw->setRowCount(d_row);
    }

    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tw->setSelectionMode(QAbstractItemView::NoSelection);
    tw->setGridStyle(Qt::NoPen);
    tw->setShowGrid(true);

    if(directAsNumber)
    {
        QList <UElement> elements = getSortToNumberListElements();
        int row = 0;
        int column = 0;
        foreach(UElement el, elements)
        {
            tw->setRowCount(row+1);

            QTableWidgetItem *item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString("%1").arg(el.name));
            item->setToolTip(QString("<font size=2 align=left>%1</font>"
                                     "<p align=right>"
                                     "<b>%2</b><br>"
                                     "%3<br>"
                                     "%4<br>"
                                     "<font size=2>%5</font></p>")
                             .arg(el.number)
                             .arg(el.name)
                             .arg(el.longName)
                             .arg(el.family.name)
                             .arg(el.mass));

            item->setBackgroundColor(QColor(el.family.color));
            tw->setItem(row, column, item);

            if(++column % tw->columnCount() == 0)
            {
                column = 0;
                row++;
            }
        }

    }else
    {
        foreach(UElement el, d_elements)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString("%1").arg(el.name));
            item->setToolTip(QString("<font size=2 align=left>%1</font>"
                                     "<p align=right>"
                                     "<b>%2</b><br>"
                                     "%3<br>"
                                     "%4<br>"
                                     "<font size=2>%5</font></p>")
                             .arg(el.number)
                             .arg(el.name)
                             .arg(el.longName)
                             .arg(el.family.name)
                             .arg(el.mass));
            item->setBackgroundColor(QColor(el.family.color));
            int row = el.row.toInt() - 1;
            int column = el.column.toInt() - 1 ;
            tw->setItem(row, column, item);
        }
    }

    tw->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
    tw->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
void UPeriodicTableParser::toGraphicsScene(QGraphicsScene *gs, const bool directAsNumber /*= false*/)
{
    gs->clear();

    int size = 28;
    int row = 0;
    int column = 0;
    int indent = 4;

    int lastX = 0;
    int lastY = 0;

    QList <UElement> elements;

    if(directAsNumber)
        elements = getSortToNumberListElements();
    else
        elements = d_elements;

    foreach(UElement el, elements)
    {
        QGraphicsRectItem *item = new QGraphicsRectItem();
        item->setRect(0, 0, size, size);

        int x;
        int y;

        if(directAsNumber)
        {
            x = lastX;
            y = lastY;
        }else
        {
            x = (el.column.toInt() - 1) * size + indent * size;
            y = (el.row.toInt() - 1) * size + indent * size;
        }

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

        item->setBrush(QBrush(QColor(el.family.color)));

        QGraphicsTextItem *textItem = new QGraphicsTextItem(item);

//        textItem->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable);
        textItem->setTextWidth(size);
        textItem->setHtml(QString("<center>%2</center>").arg(el.name));
        textItem->setToolTip(QString("<font size=2 align=left>%1</font>"
                                 "<p align=right>"
                                 "<b>%2</b><br>"
                                 "%3<br>"
                                 "%4<br>"
                                 "<font size=2>%5</font></p>")
                         .arg(el.number)
                         .arg(el.name)
                         .arg(el.longName)
                         .arg(el.family.name)
                         .arg(el.mass));

        gs->addItem(item);
    }
}
QList <UElement> UPeriodicTableParser::getSortToNumberListElements()
{
   QList <UElement> copyList = d_elements;
   qSort(copyList.begin(), copyList.end(), compareElementsLessThan);
   return copyList;
}

/// PRIVATE
void UPeriodicTableParser::init()
{
    d_row = -1;
    d_column = -1;
}
UElement UPeriodicTableParser::getElementFromDom(const QDomElement &de)
{
    UElement element;
    element.name = de.attribute("name");
    element.number = de.attribute("number");
    element.mass = de.attribute("mass");
    element.longName = de.attribute("longName");
    element.row = de.attribute("row");
    element.column = de.attribute("column");
    element.family = d_families.value(de.attribute("family"));

    return element;
}
