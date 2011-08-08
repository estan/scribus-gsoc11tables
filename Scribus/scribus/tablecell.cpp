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
#include "tableutils.h"

#include "tablecell.h"

using namespace TableUtils;

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

// TODO: We should cache this rectangle.
QRectF TableCell::contentRect() const
{
	if (!isValid())
		return QRectF();

	QRectF rect = boundingRect();

	// Insets are paddings plus half border width.
	const qreal leftInset = leftPadding() + maxLeftBorderWidth()/2;
	const qreal rightInset = rightPadding() + maxRightBorderWidth()/2;
	const qreal topInset = topPadding() + maxTopBorderWidth()/2;
	const qreal bottomInset = bottomPadding() + maxBottomBorderWidth()/2;

	// Adjust rectangle. (We don't allow width or height < 1.0)
	rect.setLeft(rect.left() + leftInset);
	rect.setTop(rect.top() + topInset);
	rect.setWidth(qMax(1.0, rect.width() - rightInset));
	rect.setHeight(qMax(1.0, rect.height() - bottomInset));

	return rect;
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

qreal TableCell::maxLeftBorderWidth() const
{
	const int endRow = row() + rowSpan() - 1;

	qreal maxWidth = 0.0;
	qreal currentWidth = 0.0;
	for (int currentRow = row(); currentRow <= endRow; ++currentRow)
	{
		TableCell cellLeft = d->table->cellAt(currentRow, column() - 1);
		if (cellLeft.isValid())
			currentWidth = collapseBorders(leftBorder(), cellLeft.rightBorder()).width();
		else
			currentWidth = collapseBorders(leftBorder(), d->table->leftBorder()).width();
		maxWidth = qMax(maxWidth, currentWidth);
	}

	return maxWidth;
}

qreal TableCell::maxRightBorderWidth() const
{
	const int endRow = row() + rowSpan() - 1;
	const int endCol = column() + columnSpan() - 1;

	qreal maxWidth = 0.0;
	qreal currentWidth = 0.0;
	for (int currentRow = row(); currentRow <= endRow; ++currentRow)
	{
		TableCell cellRight = d->table->cellAt(currentRow, endCol + 1);
		if (cellRight.isValid())
			currentWidth = collapseBorders(cellRight.leftBorder(), rightBorder()).width();
		else
			currentWidth = collapseBorders(d->table->rightBorder(), rightBorder()).width();
		maxWidth = qMax(maxWidth, currentWidth);
	}

	return maxWidth;
}

qreal TableCell::maxTopBorderWidth() const
{
	const int endCol = column() + columnSpan() - 1;

	qreal maxWidth = 0.0;
	qreal currentWidth = 0.0;
	for (int currentCol = column(); currentCol <= endCol; ++currentCol)
	{
		TableCell cellAbove = d->table->cellAt(row() - 1, currentCol);
		if (cellAbove.isValid())
			currentWidth = collapseBorders(cellAbove.bottomBorder(), topBorder()).width();
		else
			currentWidth = collapseBorders(topBorder(), d->table->topBorder()).width();
		maxWidth = qMax(maxWidth, currentWidth);
	}

	return maxWidth;
}

qreal TableCell::maxBottomBorderWidth() const
{
	const int endRow = row() + rowSpan() - 1;
	const int endCol = column() + columnSpan() - 1;

	qreal maxWidth = 0.0;
	qreal currentWidth = 0.0;
	for (int currentCol = column(); currentCol <= endCol; ++currentCol)
	{
		TableCell cellBelow = d->table->cellAt(endRow + 1, currentCol);
		if (cellBelow.isValid())
			currentWidth = collapseBorders(bottomBorder(), cellBelow.topBorder()).width();
		else
			currentWidth = collapseBorders(d->table->bottomBorder(), bottomBorder()).width();
		maxWidth = qMax(maxWidth, currentWidth);
	}

	return maxWidth;
}

QDebug operator<<(QDebug debug, const TableCell& cell)
{
	debug.nospace() << cell.asString();
	return debug.space();
}
