/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <QList>
#include <QDebug>

#include "tablecell.h"

#include "collapsedbordercollector.h"

void CollapsedBorderCollector::collectLeftBorder(QList<TableBorder>& verticalBorders, TableCell& cell)
{
	qDebug() << "Collecting left border";
}

void CollapsedBorderCollector::collectRightBorder(QList<TableBorder>& verticalBorders, TableCell& cell)
{
	qDebug() << "Collecting right border";
}

void CollapsedBorderCollector::collectTopBorder(QList<TableBorder>& horizontalBorders, TableCell& cell)
{
	qDebug() << "Collecting top border";
}

void CollapsedBorderCollector::collectBottomBorder(QList<TableBorder>& horizontalBorders, TableCell& cell)
{
	qDebug() << "Collecting bottom border";
}
