/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QDebug>
#include <QRectF>
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
}

QRectF TableCell::boundingRect() const
{
	if (!isValid())
		return QRectF();

	const int endRow = row() + rowSpan() - 1;
	const int endCol = column() + columnSpan() - 1;

	const qreal x = d->table->columnPosition(column());
	const qreal y = d->table->rowPosition(row());
	const qreal width = d->table->columnPosition(endCol) + d->table->columnWidth(endCol) - x;
	const qreal height = d->table->rowPosition(endRow) + d->table->rowHeight(endRow) - y;

	return QRectF(x, y, width, height);
}

QRectF TableCell::contentRect() const
{
	if (!isValid())
		return QRectF();

	const qreal leftInset = leftPadding();
	const qreal rightInset = rightPadding();
	const qreal topInset = topPadding();
	const qreal bottomInset = bottomPadding();

	return boundingRect().adjusted(leftInset, topInset, -rightInset, -bottomInset);
}

QString TableCell::asString() const
{
	QString str("cell(");
	str += QString("row=%1, column=%2, ").arg(row()).arg(column());
	str += QString("rowSpan=%1, columnSpan=%2, ").arg(rowSpan()).arg(columnSpan());
	str += QString("leftBorder=%1, rightBorder=%2, ").arg(leftBorder().asString()).arg(rightBorder().asString());
	str += QString("topBorder=%1, bottomBorder=%2, ").arg(topBorder().asString()).arg(bottomBorder().asString());
	str += QString("leftPadding=%1, rightPadding=%2, ").arg(leftPadding()).arg(rightPadding());
	str += QString("topPadding=%1, bottomPadding=%2, ").arg(topPadding()).arg(bottomPadding());
	str += QString("style=%1").arg(d->style.asString());
	str += QString(")");

	return str;
}

QDebug operator<<(QDebug debug, const TableCell& cell)
{
	debug.nospace() << cell.asString();
	return debug.space();
}
