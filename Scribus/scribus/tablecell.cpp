/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QDebug>
#include <QString>

#include "tablecell.h"

#include "commonstrings.h"
#include "pageitem_table.h"

TableCell::TableCell(int row, int column, PageItem_Table *table) : d(new TableCellData)
{
	setValid(true);
	setRow(row);
	setColumn(column);
	setRowSpan(1);
	setColumnSpan(1);
	setLeftBorderWidth(1.0);
	setRightBorderWidth(1.0);
	setTopBorderWidth(1.0);
	setBottomBorderWidth(1.0);
	setLeftBorderColor(CommonStrings::None);
	setRightBorderColor(CommonStrings::None);
	setTopBorderColor(CommonStrings::None);
	setBottomBorderColor(CommonStrings::None);

	d->table = table;
}

QString TableCell::asString() const
{
	QString str("cell(");
	str += QString("row=%1, column=%2, ").arg(row()).arg(column());
	str += QString("rowSpan=%1, columnSpan=%2, ").arg(rowSpan()).arg(columnSpan());
	str += QString("leftBorderWidth=%1, rightBorderWidth=%2, ").arg(leftBorderWidth()).arg(rightBorderWidth());
	str += QString("topBorderWidth=%1, bottomBorderWidth=%2, ").arg(topBorderWidth()).arg(bottomBorderWidth());
	str += QString("leftBorderColor=%1, rightBorderColor=%2, ").arg(leftBorderColor()).arg(rightBorderColor());
	str += QString("topBorderColor=%1, bottomBorderColor=%2").arg(topBorderColor()).arg(bottomBorderColor());
	str += QString(")");

	return str;
}

QDebug operator<<(QDebug debug, const TableCell& cell)
{
	debug.nospace() << cell.asString();
	return debug.space();
}
