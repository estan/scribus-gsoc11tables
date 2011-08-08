/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <QLineF>
#include <QColor>

#include "scpainter.h"
#include "tablecell.h"
#include "pageitem_table.h"
#include "tableborder.h"
#include "tableutils.h"

#include "collapsedtablepainter.h"

using namespace TableUtils;

void CollapsedTablePainter::paintTable(ScPainter* p)
{
	p->save();
	p->translate(table()->gridOffset());

	// Paint table fill.
	paintTableFill(p);

	/*
	 * We paint the table in four passes, first the cell fills, then the vertical
	 * borders, then the horizontal borders and finally the decorative grid lines.
	 */

	// Pass 1: Paint cell fills.
	for (int row = 0; row < table()->rows(); ++row)
	{
		int colSpan = 0;
		for (int col = 0; col < table()->columns(); col += colSpan)
		{
			TableCell cell = table()->cellAt(row, col);
			if (row == cell.row())
				paintCellFill(cell, p);
			colSpan = cell.columnSpan();
		}
	}

	// Pass 2: Paint vertical borders.
	for (int row = 0; row < table()->rows(); ++row)
	{
		int colSpan = 0;
		for (int col = 0; col < table()->columns(); col += colSpan)
		{
			TableCell cell = table()->cellAt(row, col);
			if (row == cell.row())
			{
				paintCellRightBorders(cell, p);
				if (col == 0)
					paintCellLeftBorders(cell, p);
			}
			colSpan = cell.columnSpan();
		}
	}

	// Pass 3: Paint horizontal borders.
	for (int row = 0; row < table()->rows(); ++row)
	{
		int colSpan = 0;
		for (int col = 0; col < table()->columns(); col += colSpan)
		{
			TableCell cell = table()->cellAt(row, col);
			if (row == cell.row())
			{
				paintCellBottomBorders(cell, p);
				if (row == 0)
					paintCellTopBorders(cell, p);
			}
			colSpan = cell.columnSpan();
		}
	}

	// Pass 4: Paint grid lines.
	// TODO: This should be configurable.
	for (int row = 0; row < table()->rows(); ++row)
	{
		int colSpan = 0;
		for (int col = 0; col < table()->columns(); col += colSpan)
		{
			TableCell cell = table()->cellAt(row, col);
			if (row == cell.row())
			{
				int endCol = col + cell.columnSpan() - 1;
				int endRow = row + cell.rowSpan() - 1;
				qreal left = table()->columnPosition(col);
				qreal right = table()->columnPosition(endCol) + table()->columnWidth(endCol);
				qreal top = table()->rowPosition(row);
				qreal bottom = table()->rowPosition(endRow) + table()->rowHeight(endRow);
				// Paint right and bottom grid line.
				paintGridLine(QPointF(right, top), QPointF(right, bottom), p);
				paintGridLine(QPointF(left, bottom), QPointF(right, bottom), p);
				// Paint left and top grid line.
				if (col == 0)
					paintGridLine(QPointF(left, top), QPointF(left, bottom), p);
				if (row == 0)
					paintGridLine(QPointF(left, top), QPointF(right, top), p);
			}
			colSpan = cell.columnSpan();
		}
	}

	p->restore();
}

void CollapsedTablePainter::paintTableFill(ScPainter* p) const
{
	QString colorName = table()->fillColor();

	if (colorName == CommonStrings::None)
		return;

	p->save();

	int lastCol = table()->columns() - 1;
	int lastRow = table()->rows() - 1;

	qreal x = table()->columnPosition(0);
	qreal y = table()->rowPosition(0);
	qreal width = table()->columnPosition(lastCol) + table()->columnWidth(lastCol) - x;
	qreal height = table()->rowPosition(lastRow) + table()->rowHeight(lastRow) - y;

	QColor color;
	table()->SetQColor(&color, colorName, 100.0);
	p->setBrush(color);
	p->setFillMode(ScPainter::Solid);
	p->setStrokeMode(ScPainter::None);
	p->drawRect(x, y, width, height);

	p->restore();
}

void CollapsedTablePainter::paintCellLeftBorders(const TableCell& cell, ScPainter* p) const
{
	/*
	 * We are going to paint the border marked # in the following setup.
	 *
	 *       +----------------------+----------------------+
	 *       |                      |                      |
	 *       |                      |                      |
	 *       |    topLeftCell      top     topRightCell    |
	 *       |                      |                      |
	 *       |                      |                      |
	 *       +--------topLeft-------+-------topRight-------+
	 *       |                      #                      |
	 *       |                      #                      |
	 *       |      leftCell     border        cell        |
	 *       |                      #                      |
	 *       |                      #                      |
	 *       +-------bottomLeft-----+------bottomRight-----+
	 *       |                      |                      |
	 *       |                      |                      |
	 *       |  bottomLeftCell   bottom   bottomRightCell  |
	 *       |                      |                      |
	 *       |                      |                      |
	 *       +----------------------+----------------------+
	 */

	// The cell ends in this row.
	const int lastRow = cell.row() + cell.rowSpan() - 1;
	// The cell starts in this column.
	const int firstCol = cell.column();
	// The X position of the border segments to paint.
	const qreal borderX = table()->columnPosition(firstCol);

	// The start point of the border segment currently being painted.
	QPointF start(borderX, 0.0);
	// The end point of the border segment currently being painted.
	QPointF end(borderX, 0.0);
	// The start and end offset factors for the border segment currently being painted.
	QPointF startOffsetFactors, endOffsetFactors;
	// Start and end row of border segment currently being painted.
	int startRow, endRow;

	for (int row = cell.row(); row <= lastRow; row += endRow - startRow + 1)
	{
		// Get the neighboring cell to the left and determine border segment row interval.
		TableCell leftCell = table()->cellAt(row, firstCol - 1);
		startRow = qMax(cell.row(), leftCell.row());
		endRow = qMin(lastRow, leftCell.isValid() ? leftCell.row() + leftCell.rowSpan() - 1 : lastRow);

		// Get the remaining neighboring cells.
		TableCell topLeftCell = table()->cellAt(startRow - 1, firstCol - 1);
		TableCell topRightCell = table()->cellAt(startRow - 1, firstCol);
		TableCell bottomRightCell = table()->cellAt(lastRow + 1, firstCol);
		TableCell bottomLeftCell = table()->cellAt(lastRow + 1, firstCol - 1);

		// Resolve borders between neighboring cells.
		TableBorder topLeft, top, topRight, border, bottomLeft, bottom, bottomRight;
		resolveBordersVertical(topLeftCell, topRightCell, leftCell, cell, bottomLeftCell, bottomRightCell,
			&topLeft, &top, &topRight, &border, &bottomLeft, &bottom, &bottomRight, table());

		if (border.isNull())
			continue; // Quit early if the border to paint is null.

		// Set initial coordinates.
		start.setY(table()->rowPosition(startRow));
		end.setY(table()->rowPosition(endRow) + table()->rowHeight(endRow));

		// Adjust coordinates for joining.
		joinVertical(border, topLeft, top, topRight, bottomLeft, bottom, bottomRight,
			 &start, &end, &startOffsetFactors, &endOffsetFactors);

		// Paint the border segment.
		paintBorder(border, start, end, startOffsetFactors, endOffsetFactors, p);
	}
}

void CollapsedTablePainter::paintCellRightBorders(const TableCell& cell, ScPainter* p) const
{
	/*
	 * We are going to paint the border marked # in the following setup.
	 *
	 *       +----------------------+----------------------+
	 *       |                      |                      |
	 *       |                      |                      |
	 *       |    topLeftCell      top     topRightCell    |
	 *       |                      |                      |
	 *       |                      |                      |
	 *       +--------topLeft-------+-------topRight-------+
	 *       |                      #                      |
	 *       |                      #                      |
	 *       |         cell      border      rightCell     |
	 *       |                      #                      |
	 *       |                      #                      |
	 *       +-------bottomLeft-----+------bottomRight-----+
	 *       |                      |                      |
	 *       |                      |                      |
	 *       |  bottomLeftCell   bottom   bottomRightCell  |
	 *       |                      |                      |
	 *       |                      |                      |
	 *       +----------------------+----------------------+
	 */

	// The cell ends in this row.
	const int lastRow = cell.row() + cell.rowSpan() - 1;
	// The cell ends in this column.
	const int lastCol = cell.column() + cell.columnSpan() - 1;
	// The X position of the border segments to paint.
	const qreal borderX = table()->columnPosition(lastCol) + table()->columnWidth(lastCol);

	// The start point of the border segment currently being painted.
	QPointF start(borderX, 0.0);
	// The end point of the border segment currently being painted.
	QPointF end(borderX, 0.0);
	// The start and end offset factors for the border segment currently being painted.
	QPointF startOffsetFactors, endOffsetFactors;
	// The start and end row of border segment currently being painted.
	int startRow, endRow;

	for (int row = cell.row(); row <= lastRow; row += endRow - startRow + 1)
	{
		// Get the neighboring cell to the right and determine border segment row interval.
		TableCell rightCell = table()->cellAt(row, lastCol + 1);
		startRow = qMax(cell.row(), rightCell.row());
		endRow = qMin(lastRow, rightCell.isValid() ? rightCell.row() + rightCell.rowSpan() - 1 : lastRow);

		// Get the remaining neighboring cells surrounding the segment.
		TableCell topLeftCell = table()->cellAt(startRow - 1, lastCol);
		TableCell topRightCell = table()->cellAt(startRow - 1, lastCol + 1);
		TableCell bottomRightCell = table()->cellAt(endRow + 1, lastCol + 1);
		TableCell bottomLeftCell = table()->cellAt(endRow + 1, lastCol);

		// Resolve borders between neighboring cells.
		TableBorder topLeft, top, topRight, border, bottomLeft, bottom, bottomRight;
		resolveBordersVertical(topLeftCell, topRightCell, cell, rightCell, bottomLeftCell, bottomRightCell,
			&topLeft, &top, &topRight, &border, &bottomLeft, &bottom, &bottomRight, table());

		if (border.isNull())
			continue; // Quit early if the border to paint is null.

		// Set initial coordinates.
		start.setY(table()->rowPosition(startRow));
		end.setY(table()->rowPosition(endRow) + table()->rowHeight(endRow));

		// Adjust coordinates for joining.
		joinVertical(border, topLeft, top, topRight, bottomLeft, bottom, bottomRight,
			 &start, &end, &startOffsetFactors, &endOffsetFactors);

		// Paint the border.
		paintBorder(border, start, end, startOffsetFactors, endOffsetFactors, p);
	}
}

void CollapsedTablePainter::paintCellTopBorders(const TableCell& cell, ScPainter* p) const
{
	/*
	 * We are going to paint the border marked # in the following setup.
	 *
	 *  +--------------------------+--------------------------+--------------------------+
	 *  |                          |                          |                          |
	 *  |                          |                          |                          |
	 *  |        topLeftCell   topLeft        topCell      topRight   topRightCell       |
	 *  |                          |                          |                          |
	 *  |                          |                          |                          |
	 *  +----------left------------+######### border-#########+----------right-----------+
	 *  |                          |                          |                          |
	 *  |                          |                          |                          |
	 *  |     bottomLeftCell   bottomLeft      cell      bottomRight  bottomRightCell    |
	 *  |                          |                          |                          |
	 *  |                          |                          |                          |
	 *  +--------------------------+--------------------------+--------------------------+
	 */
	// The cell starts in this row.
	const int firstRow = cell.row();
	// The cell ends in this column.
	const int lastCol = cell.column() + cell.columnSpan() - 1;
	// The Y position of the border segments to paint.
	const qreal borderY = table()->rowPosition(firstRow);

	// The start point of the border segment currently being painted.
	QPointF start(0.0, borderY);
	// The end point of the border segment currently being painted.
	QPointF end(0.0, borderY);
	// The start and end offset factors for the border segment currently being painted.
	QPointF startOffsetFactors, endOffsetFactors;
	// Start and end column of border segment currently being painted.
	int startCol, endCol;

	for (int col = cell.column(); col <= lastCol; col += endCol - startCol + 1)
	{
		// Get the neighboring cell above and determine border segment column interval.
		TableCell topCell = table()->cellAt(firstRow - 1, col);
		startCol = qMax(cell.column(), topCell.column());
		endCol = qMin(lastCol, topCell.isValid() ? topCell.column() + topCell.columnSpan() - 1 : lastCol);

		// Get the remaining neighboring cells.
		TableCell topLeftCell = table()->cellAt(firstRow - 1, startCol - 1);
		TableCell topRightCell = table()->cellAt(firstRow - 1, endCol + 1);
		TableCell bottomRightCell = table()->cellAt(firstRow, endCol + 1);
		TableCell bottomLeftCell = table()->cellAt(firstRow, startCol - 1);

		// Resolve borders between neighboring cells.
		TableBorder topLeft, left, bottomLeft, border, topRight, right, bottomRight;
		resolveBordersHorizontal(topLeftCell, topCell, topRightCell, bottomLeftCell, cell,
			bottomRightCell, &topLeft, &left, &bottomLeft, &border, &topRight, &right, &bottomRight, table());

		if (border.isNull())
			continue; // Quit early if the border is null.

		// Set initial coordinates.
		start.setX(table()->columnPosition(startCol));
		end.setX(table()->columnPosition(endCol) + table()->columnWidth(endCol));

		// Adjust coordinates for joining.
		joinHorizontal(border, topLeft, left, bottomLeft, topRight, right, bottomRight,
			 &start, &end, &startOffsetFactors, &endOffsetFactors);

		// Paint the border segment.
		paintBorder(border, start, end, startOffsetFactors, endOffsetFactors, p);
	}
}

void CollapsedTablePainter::paintCellBottomBorders(const TableCell& cell, ScPainter* p) const
{
	/*
	 * We are going to paint the border marked # in the following setup.
	 *
	 *  +--------------------------+--------------------------+--------------------------+
	 *  |                          |                          |                          |
	 *  |                          |                          |                          |
	 *  |        topLeftCell   topLeft         cell        topRight   topRightCell       |
	 *  |                          |                          |                          |
	 *  |                          |                          |                          |
	 *  +----------left------------+######### border-#########+----------right-----------+
	 *  |                          |                          |                          |
	 *  |                          |                          |                          |
	 *  |     bottomLeftCell   bottomLeft   bottomCell   bottomRight  bottomRightCell    |
	 *  |                          |                          |                          |
	 *  |                          |                          |                          |
	 *  +--------------------------+--------------------------+--------------------------+
	 */
	// The cell ends in this row.
	const int lastRow = cell.row() + cell.rowSpan() - 1;
	// The cell ends in this column.
	const int lastCol = cell.column() + cell.columnSpan() - 1;
	// The Y position of the border segments to paint.
	const qreal borderY = table()->rowPosition(lastRow) + table()->rowHeight(lastRow);

	// The start point of the border segment currently being painted.
	QPointF start(0.0, borderY);
	// The end point of the border segment currently being painted.
	QPointF end(0.0, borderY);
	// The start and end offset factors for the border segment currently being painted.
	QPointF startOffsetFactors, endOffsetFactors;
	// Start and end column of border segment currently being painted.
	int startCol, endCol;

	for (int col = cell.column(); col <= lastCol; col += endCol - startCol + 1)
	{
		// Get the neighboring cell below and determine border segment column interval.
		TableCell bottomCell = table()->cellAt(lastRow + 1, col);
		startCol = qMax(cell.column(), bottomCell.column());
		endCol = qMin(lastCol, bottomCell.isValid() ? bottomCell.column() + bottomCell.columnSpan() - 1 : lastCol);

		// Get the remaining neighboring cells.
		TableCell topLeftCell = table()->cellAt(lastRow, startCol - 1);
		TableCell topRightCell = table()->cellAt(lastRow, endCol + 1);
		TableCell bottomRightCell = table()->cellAt(lastRow + 1, endCol + 1);
		TableCell bottomLeftCell = table()->cellAt(lastRow + 1, startCol - 1);

		// Resolve borders between neighboring cells.
		TableBorder topLeft, left, bottomLeft, border, topRight, right, bottomRight;
		resolveBordersHorizontal(topLeftCell, cell, topRightCell, bottomLeftCell, bottomCell,
			bottomRightCell, &topLeft, &left, &bottomLeft, &border, &topRight, &right, &bottomRight, table());

		if (border.isNull())
			continue; // Quit early if the border is null.

		// Set initial coordinates.
		start.setX(table()->columnPosition(startCol));
		end.setX(table()->columnPosition(endCol) + table()->columnWidth(endCol));

		// Adjust coordinates for joining.
		joinHorizontal(border, topLeft, left, bottomLeft, topRight, right, bottomRight,
			 &start, &end, &startOffsetFactors, &endOffsetFactors);

		// Paint the border segment.
		paintBorder(border, start, end, startOffsetFactors, endOffsetFactors, p);
	}
}

void CollapsedTablePainter::paintCellFill(const TableCell& cell, ScPainter* p) const
{
	QString colorName = cell.fillColor();

	if (colorName == CommonStrings::None)
		return;

	p->save();

	QColor color;
	table()->SetQColor(&color, colorName, 100.0); // TODO: Support shade.
	p->setBrush(color);
	p->setFillMode(ScPainter::Solid);
	p->setStrokeMode(ScPainter::None);

	int row = cell.row();
	int col = cell.column();
	int lastRow = row + cell.rowSpan() - 1;
	int lastCol = col + cell.columnSpan() - 1;

	qreal x = table()->columnPosition(col);
	qreal y = table()->rowPosition(row);
	qreal width = table()->columnPosition(lastCol) + table()->columnWidth(lastCol) - x;
	qreal height = table()->rowPosition(lastRow) + table()->rowHeight(lastRow) - y;
	p->drawRect(x, y, width, height);

	p->restore();
}

void CollapsedTablePainter::paintBorder(const TableBorder& border, const QPointF& start, const QPointF& end,
										const QPointF& startOffsetFactors, const QPointF& endOffsetFactors,
										ScPainter *p) const
{
	p->save();
	p->setStrokeMode(ScPainter::Solid);
	p->setFillMode(ScPainter::None);

	QColor lineColor;
	QPointF lineStart, lineEnd;
	foreach (const TableBorderLine& line, border.borderLines())
	{
		// Adjust line start and ends by line width multipled by offset factors.
		lineStart.setX(start.x() + line.width() * startOffsetFactors.x());
		lineStart.setY(start.y() + line.width() * startOffsetFactors.y());
		lineEnd.setX(end.x() + line.width() * endOffsetFactors.x());
		lineEnd.setY(end.y() + line.width() * endOffsetFactors.y());
		// Set line color.
		table()->SetQColor(&lineColor, line.color(), 100.0); // TODO: Support shade.
		// Draw line.
		p->setPen(lineColor, line.width(), line.style(), Qt::FlatCap, Qt::MiterJoin);
		p->drawLine(lineStart, lineEnd);
	}

	p->restore();
}

void CollapsedTablePainter::paintGridLine(const QPointF& start, const QPointF& end, ScPainter *p) const
{
	p->save();
	p->setPen(QColor(100, 200, 255), 1.0 / qMax(p->zoomFactor(), 1.0), Qt::DotLine, Qt::FlatCap, Qt::MiterJoin);
	p->setStrokeMode(ScPainter::Solid);
	p->drawLine(start, end);
	p->restore();
}
