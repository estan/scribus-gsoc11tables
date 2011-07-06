#include <QLineF>
#include <QColor>

#include "scpainter.h"
#include "tablecell.h"
#include "pageitem_table.h"
#include "tableborder.h"
#include "tableutils.h"

#include "collapsedtablepainter.h"

void CollapsedTablePainter::paintTable(ScPainter* p)
{
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
				qreal left = table()->columnPosition(col);
				qreal right = left + table()->columnWidth(col);
				qreal top = table()->rowPosition(row);
				qreal bottom = top + table()->rowHeight(row);
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
	// The X position of the borders to paint.
	const qreal borderX = table()->columnPosition(firstCol);

	// The start point of the border currently being painted.
	QPointF start(borderX, 0.0);
	// The end point of the border currently being painted.
	QPointF end(borderX, 0.0);
	// The start offset factors for the border currently being painted.
	QPointF startOffsetFactors;
	// The end offset factors for the border currently being painted.
	QPointF endOffsetFactors;
	// Start row of border currently being painted.
	int startRow = 0;
	// End row of border currently being painted.
	int endRow = 0;

	for (int row = cell.row(); row <= lastRow; row += endRow - startRow + 1)
	{
		// Get the neighboring cells.
		TableCell topLeftCell = table()->cellAt(row - 1, firstCol - 1);
		TableCell topRightCell = table()->cellAt(row - 1, firstCol);
		TableCell leftCell = table()->cellAt(row, firstCol - 1);
		TableCell bottomRightCell = table()->cellAt(row + 1, firstCol);
		TableCell bottomLeftCell = table()->cellAt(row + 1, firstCol - 1);

		// Determine the border to paint.
		TableBorder border;
		if (firstCol == 0)
		{
			// In first column, so collapse with table border.
			border = TableUtils::collapseBorders(cell.leftBorder(), table()->leftBorder());
			startRow = cell.row();
			endRow = lastRow;
		}
		else
		{
			// Collapse with right border of cell to the left.
			border = TableUtils::collapseBorders(cell.leftBorder(), leftCell.rightBorder());
			startRow = qMax(cell.row(), leftCell.row());
			endRow = qMin(lastRow, leftCell.row() + leftCell.rowSpan());
		}

		if (border.isNull())
			continue; // Quit early if the border is null.

		// Determine initial coordinates.
		start.setY(table()->rowPosition(startRow));
		end.setY(table()->rowPosition(endRow) + table()->rowHeight(endRow));

		// Determine neighboring borders and adjust coordinates for joining.
		TableBorder topLeft, top, topRight, bottomLeft, bottom, bottomRight;
		if (leftCell.row() != topLeftCell.row())
			topLeft = TableUtils::collapseBorders(leftCell.topBorder(), topLeftCell.bottomBorder());
		if (topRightCell.column() != topLeftCell.column())
			top = TableUtils::collapseBorders(topRightCell.leftBorder(), topLeftCell.rightBorder());
		if (cell.row() != topRightCell.row())
			topRight = TableUtils::collapseBorders(cell.topBorder(), topRightCell.bottomBorder());
		if (cell.row() != bottomRightCell.row() )
			bottomRight = TableUtils::collapseBorders(bottomRightCell.topBorder(), cell.bottomBorder());
		if (bottomLeftCell.column() != bottomRightCell.column())
			bottom = TableUtils::collapseBorders(bottomRightCell.leftBorder(), bottomLeftCell.rightBorder());
		if (bottomLeftCell.row() != leftCell.row())
			bottomLeft = TableUtils::collapseBorders(bottomLeftCell.topBorder(), leftCell.bottomBorder());

		TableUtils::joinVertical(border, topLeft, top, topRight, bottomLeft, bottom, bottomRight,
								 &start, &end, &startOffsetFactors, &endOffsetFactors);

		// Paint the border.
		if (!border.isNull())
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
	// The X position of the borders to paint.
	const qreal borderX = table()->columnPosition(lastCol) + table()->columnWidth(lastCol);

	// The start point of the border currently being painted.
	QPointF start(borderX, 0.0);
	// The end point of the border currently being painted.
	QPointF end(borderX, 0.0);
	// The start offset factors for the border currently being painted.
	QPointF startOffsetFactors;
	// The end offset factors for the border currently being painted.
	QPointF endOffsetFactors;
	// Start row of border currently being painted.
	int startRow = 0;
	// End row of border currently being painted.
	int endRow = 0;

	for (int row = cell.row(); row <= lastRow; row += endRow - startRow + 1)
	{
		// Get the neighboring cells.
		TableCell topLeftCell = table()->cellAt(row - 1, lastCol);
		TableCell topRightCell = table()->cellAt(row - 1, lastCol + 1);
		TableCell rightCell = table()->cellAt(row, lastCol + 1);
		TableCell bottomRightCell = table()->cellAt(row + 1, lastCol + 1);
		TableCell bottomLeftCell = table()->cellAt(row + 1, lastCol);

		// Determine the border to paint.
		TableBorder border;
		if (lastCol == table()->columns() - 1)
		{
			// In last column, so collapse with table border.
			border = TableUtils::collapseBorders(table()->rightBorder(), cell.rightBorder());
			startRow = cell.row();
			endRow = lastRow;
		}
		else
		{
			// Collapse with left border of cell to the right.
			border = TableUtils::collapseBorders(rightCell.leftBorder(), cell.rightBorder());
			startRow = qMax(cell.row(), rightCell.row());
			endRow = qMin(lastRow, rightCell.row() + rightCell.rowSpan());
		}

		if (border.isNull())
			continue; // Quit early if the border is null.

		// Determine initial coordinates.
		start.setY(table()->rowPosition(startRow));
		end.setY(table()->rowPosition(endRow) + table()->rowHeight(endRow));

		// Determine neighboring borders and adjust coordinates for joining.
		TableBorder topLeft, top, topRight, bottomLeft, bottom, bottomRight;
		if (rightCell.row() != topRightCell.row())
			topRight = TableUtils::collapseBorders(rightCell.topBorder(), topRightCell.bottomBorder());
		if (topRightCell.column() != topLeftCell.column())
			top = TableUtils::collapseBorders(topRightCell.leftBorder(), topLeftCell.rightBorder());
		if (cell.row() != topLeftCell.row())
			topLeft = TableUtils::collapseBorders(cell.topBorder(), topLeftCell.bottomBorder());
		if (cell.row() != bottomLeftCell.row() )
			bottomLeft = TableUtils::collapseBorders(bottomLeftCell.topBorder(), cell.bottomBorder());
		if (bottomLeftCell.column() != bottomRightCell.column())
			bottom = TableUtils::collapseBorders(bottomRightCell.leftBorder(), bottomLeftCell.rightBorder());
		if (bottomRightCell.row() != rightCell.row())
			bottomRight = TableUtils::collapseBorders(bottomRightCell.topBorder(), rightCell.bottomBorder());

		TableUtils::joinVertical(border, topLeft, top, topRight, bottomLeft, bottom, bottomRight,
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
	// The Y position of the borders to paint.
	const qreal borderY = table()->rowPosition(firstRow);

	// The start point of the border currently being painted.
	QPointF start(0.0, borderY);
	// The end point of the border currently being painted.
	QPointF end(0.0, borderY);
	// The start offset factors for the border currently being painted.
	QPointF startOffsetFactors;
	// The end offset factors for the border currently being painted.
	QPointF endOffsetFactors;
	// Start column of border currently being painted.
	int startCol = 0;
	// End column of border currently being painted.
	int endCol = 0;

	for (int col = cell.column(); col <= lastCol; col += endCol - startCol + 1)
	{
		// Get the neighboring cells.
		TableCell topLeftCell = table()->cellAt(firstRow - 1, col - 1);
		TableCell topCell = table()->cellAt(firstRow - 1, col);
		TableCell topRightCell = table()->cellAt(firstRow - 1, col + 1);
		TableCell bottomRightCell = table()->cellAt(firstRow, col + 1);
		TableCell bottomLeftCell = table()->cellAt(firstRow, col - 1);

		// Determine the border to paint.
		TableBorder border;
		if (firstRow == 0)
		{
			// In first row, so collapse with table border.
			border = TableUtils::collapseBorders(cell.topBorder(), table()->topBorder());
			startCol = cell.column();
			endCol = lastCol;
		}
		else
		{
			// Collapse with bottom border of cell above.
			border = TableUtils::collapseBorders(cell.topBorder(), topCell.bottomBorder());
			startCol = qMax(cell.column(), topCell.column());
			endCol = qMin(lastCol, topCell.column() + topCell.columnSpan());
		}

		if (border.isNull())
			continue; // Quit early if the border is null.

		// Determine initial coordinates.
		start.setX(table()->columnPosition(startCol));
		end.setX(table()->columnPosition(endCol) + table()->columnWidth(endCol));

		// Determine neighboring borders and adjust coordinates for joining.
		TableBorder topLeft, left, bottomLeft, topRight, right, bottomRight;
		if (topLeftCell.row() != bottomLeftCell.row())
			left = TableUtils::collapseBorders(bottomLeftCell.topBorder(), topLeftCell.bottomBorder());
		if (topRightCell.row() != bottomRightCell.row())
			right = TableUtils::collapseBorders(bottomRightCell.topBorder(), topRightCell.bottomBorder());
		if (topLeftCell.column() != topCell.column())
			topLeft = TableUtils::collapseBorders(topCell.leftBorder(), topLeftCell.rightBorder());
		if (topRightCell.column() != topCell.column() )
			topRight = TableUtils::collapseBorders(topRightCell.leftBorder(), topCell.rightBorder());
		if (bottomLeftCell.column() != cell.column())
			bottomLeft = TableUtils::collapseBorders(cell.leftBorder(), bottomLeftCell.rightBorder());
		if (bottomRightCell.column() != cell.column())
			bottomRight = TableUtils::collapseBorders(bottomRightCell.leftBorder(), cell.rightBorder());

		TableUtils::joinHorizontal(border, topLeft, left, bottomLeft, topRight, right, bottomRight,
								 &start, &end, &startOffsetFactors, &endOffsetFactors);

		// Paint the border.
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
	// The Y position of the borders to paint.
	const qreal borderY = table()->rowPosition(lastRow) + table()->rowHeight(lastRow);

	// The start point of the border currently being painted.
	QPointF start(0.0, borderY);
	// The end point of the border currently being painted.
	QPointF end(0.0, borderY);
	// The start offset factors for the border currently being painted.
	QPointF startOffsetFactors;
	// The end offset factors for the border currently being painted.
	QPointF endOffsetFactors;
	// Start column of border currently being painted.
	int startCol = 0;
	// End column of border currently being painted.
	int endCol = 0;

	for (int col = cell.column(); col <= lastCol; col += endCol - startCol + 1)
	{
		// Get the neighboring cells.
		TableCell topLeftCell = table()->cellAt(lastRow, col - 1);
		TableCell bottomCell = table()->cellAt(lastRow + 1, col);
		TableCell topRightCell = table()->cellAt(lastRow, col + 1);
		TableCell bottomRightCell = table()->cellAt(lastRow + 1, col + 1);
		TableCell bottomLeftCell = table()->cellAt(lastRow + 1, col - 1);

		// Determine the border to paint.
		TableBorder border;
		if (lastRow == table()->rows() - 1)
		{
			// In last row, so collapse with table border.
			border = TableUtils::collapseBorders(table()->bottomBorder(), cell.bottomBorder());
			startCol = cell.column();
			endCol = lastCol;
		}
		else
		{
			// Collapse with bottom border of cell above.
			border = TableUtils::collapseBorders(bottomCell.topBorder(), cell.bottomBorder());
			startCol = qMax(cell.column(), bottomCell.column());
			endCol = qMin(lastCol, bottomCell.column() + bottomCell.columnSpan());
		}

		if (border.isNull())
			continue; // Quit early if the border is null.

		// Determine initial coordinates.
		start.setX(table()->columnPosition(startCol));
		end.setX(table()->columnPosition(endCol) + table()->columnWidth(endCol));

		// Determine neighboring borders and adjust coordinates for joining.
		TableBorder topLeft, left, bottomLeft, topRight, right, bottomRight;
		if (topLeftCell.row() != bottomLeftCell.row())
			left = TableUtils::collapseBorders(bottomLeftCell.topBorder(), topLeftCell.bottomBorder());
		if (topRightCell.row() != bottomRightCell.row())
			right = TableUtils::collapseBorders(bottomRightCell.topBorder(), topRightCell.bottomBorder());
		if (topLeftCell.column() != cell.column())
			topLeft = TableUtils::collapseBorders(cell.leftBorder(), topLeftCell.rightBorder());
		if (topRightCell.column() != cell.column() )
			topRight = TableUtils::collapseBorders(topRightCell.leftBorder(), cell.rightBorder());
		if (bottomLeftCell.column() != bottomCell.column())
			bottomLeft = TableUtils::collapseBorders(bottomCell.leftBorder(), bottomLeftCell.rightBorder());
		if (bottomRightCell.column() != bottomCell.column())
			bottomRight = TableUtils::collapseBorders(bottomRightCell.leftBorder(), bottomCell.rightBorder());

		TableUtils::joinHorizontal(border, topLeft, left, bottomLeft, topRight, right, bottomRight,
								 &start, &end, &startOffsetFactors, &endOffsetFactors);

		// Paint the border.
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
	p->drawRect(table()->columnPosition(cell.column()), table()->rowPosition(cell.row()),
				table()->columnWidth(cell.column()), table()->rowHeight(cell.row()));

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
	p->setPen(QColor(100, 200, 255), 1.0 / qMax(p->zoomFactor(), 1.0), Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
	p->setStrokeMode(ScPainter::Solid);
	p->drawLine(start, end);
	p->restore();
}
