/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QDebug>
#include <QString>

#include "commonstrings.h"
#include "pageitem_table.h"
#include "scribusdoc.h"
#include "scpainter.h"

#include "tablecell.h"

TableCell::TableCell(int row, int column, PageItem_Table *table) : d(new TableCellData)
{
	Q_ASSERT(table);
	d->table = table;

	d->style.setContext(&d->table->doc()->cellStyles());

	setValid(true);
	setRow(row);
	setColumn(column);
	setRowSpan(1);
	setColumnSpan(1);
	setStyle("");
}

QString TableCell::asString() const
{
	QString str("cell(");
	str += QString("row=%1, column=%2, ").arg(row()).arg(column());
	str += QString("rowSpan=%1, columnSpan=%2, ").arg(rowSpan()).arg(columnSpan());
	str += QString("leftBorder=%1, rightBorder=%2, ").arg(leftBorder().asString()).arg(rightBorder().asString());
	str += QString("topBorder=%1, bottomBorder=%2, ").arg(topBorder().asString()).arg(bottomBorder().asString());
	str += QString("style=%1").arg(d->style.asString());
	str += QString(")");

	return str;
}

QDebug operator<<(QDebug debug, const TableCell& cell)
{
	debug.nospace() << cell.asString();
	return debug.space();
}
