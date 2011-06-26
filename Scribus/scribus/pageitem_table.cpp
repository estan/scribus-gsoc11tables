/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <QRectF>

#include "pageitem.h"
#include "pageitem_table.h"
#include "styles/tablestyle.h"
#include "cellarea.h"
#include "scribusdoc.h"
#include "scpainter.h"
#include "util_color.h"

#ifdef WANT_DEBUG
	#define ASSERT_VALID() qt_noop()
#else
	#define ASSERT_VALID() assertValid(); qt_noop()
#endif

PageItem_Table::PageItem_Table(ScribusDoc *pa, double x, double y, double w, double h, double w2, QString fill, QString outline, int numRows, int numColumns)
	: PageItem(pa, PageItem::Table, x, y, w, h, w2, fill, outline), m_rows(0), m_columns(0)
{
	Q_ASSERT(m_Doc);
	m_style.setContext(&m_Doc->tableStyles());

	// NOTE: The order here is important as insertRows() assumes that columns() > 0.
	insertColumns(0, qMax(1, numColumns));
	insertRows(0, qMax(1, numRows));

	setStyle("");
	adjustToFrame();
}

void PageItem_Table::insertRows(int index, int numRows)
{
	ASSERT_VALID();

	if (index < 0 || index > rows() || numRows < 1)
		return;

	// Height and position of first inserted row.
	qreal rowHeight = rows() == 0 ? height() : m_rowHeights.at(qMax(index - 1, 0));
	qreal rowPosition = index == 0 ? 0.0 : m_rowPositions.at(qMax(index - 1, 0)) + rowHeight;

	// Insert row heights, row positions and a new row of cells.
	for (int row = index; row < index + numRows; ++row)
	{
		// Insert row height and position.
		m_rowHeights.insert(row, rowHeight);
		m_rowPositions.insert(row, rowPosition);
		rowPosition += rowHeight;

		// Insert a new row of cells if table is non-empty.
		// NOTE: We assume here that columns() > 0.
		QList<TableCell> cellRow;
		for (int col = 0; col < columns(); ++col)
			cellRow.append(TableCell(row, col, this));
		m_cellRows.insert(row, cellRow);
	}

	// Adjust following rows.
	qreal insertedHeight = rowHeight * numRows;
	for (int nextRow = index + numRows; nextRow < rows() + numRows; ++nextRow)
	{
		// Adjust position of following row.
		m_rowPositions[nextRow] += insertedHeight;

		// "Move" cells in following row down.
		foreach (TableCell cell, m_cellRows[nextRow])
			cell.moveDown(numRows);
	}

	// Update row spans.
	updateSpans(index, numRows, RowsInserted);

	// Increase number of rows.
	m_rows += numRows;

	ASSERT_VALID();
}

void PageItem_Table::removeRows(int index, int numRows)
{
	ASSERT_VALID();

	if (!validRow(index) || numRows < 1 || numRows >= rows() || index + numRows > rows())
		return;

	// Remove row heights, row positions and rows of cells.
	qreal removedHeight = 0.0;
	for (int i = 0; i < numRows; ++i)
	{
		// Remove row height and position.
		removedHeight += m_rowHeights.takeAt(index);
		m_rowPositions.removeAt(index);

		// Invalidate removed cells.
		foreach (TableCell removedCell, m_cellRows[index])
			removedCell.setValid(false);

		// Remove row of cells.
		m_cellRows.removeAt(index);
	}

	// Adjust following rows.
	for (int nextRow = index; nextRow < rows() - numRows; ++nextRow)
	{
		// Adjust position of following row.
		m_rowPositions[nextRow] -= removedHeight;

		// "Move" cells in following row up.
		foreach (TableCell cell, m_cellRows[nextRow])
			cell.moveUp(numRows);
	}

	// Update row spans.
	updateSpans(index, numRows, RowsRemoved);

	// Decrease number of rows.
	m_rows -= numRows;

	ASSERT_VALID();
}

qreal PageItem_Table::rowHeight(int row) const
{
	if (!validRow(row))
		return 0.0;

	return m_rowHeights.at(row);
}

void PageItem_Table::setRowHeight(int row, qreal height)
{
	if (!validRow(row) || height <= 0.0)
		return;

	// Set the height and save the change.
	qreal deltaHeight = height - m_rowHeights.at(row);
	m_rowHeights[row] = height;

	// Adjust positions of following rows.
	for (int nextRow = row + 1; nextRow < rows(); ++nextRow)
		m_rowPositions[nextRow] += deltaHeight;
}

void PageItem_Table::insertColumns(int index, int numColumns)
{
	ASSERT_VALID();

	if (index < 0 || index > columns() || numColumns < 1)
		return;

	// Width and position of first inserted column.
	qreal columnWidth = columns() == 0 ? width() : m_columnWidths.at(qMax(index - 1, 0));
	qreal columnPosition = index == 0 ? 0.0 : m_columnPositions.at(qMax(index - 1, 0)) + columnWidth;

	// Insert column widths, column positions and a new column of cells.
	for (int col = index; col < index + numColumns; ++col)
	{
		// Insert column widths and positions.
		m_columnWidths.insert(col, columnWidth);
		m_columnPositions.insert(col, columnPosition);
		columnPosition += columnWidth;

		// Insert a new column of cells.
		for (int row = 0; row < rows(); ++row)
			m_cellRows[row].insert(col, TableCell(row, col, this));
	}

	// Adjust following columns.
	qreal insertedWidth = columnWidth * numColumns;
	for (int nextColumn = index + numColumns; nextColumn < columns() + numColumns; ++nextColumn)
	{
		// Adjust position of following column.
		m_columnPositions[nextColumn] += insertedWidth;

		// "Move" cells in following column right.
		foreach (QList<TableCell> cellRow, m_cellRows)
			cellRow[nextColumn].moveRight(numColumns);
	}

	// Update column spans.
	updateSpans(index, numColumns, ColumnsInserted);

	// Increase number of columns.
	m_columns += numColumns;

	ASSERT_VALID();
}

void PageItem_Table::removeColumns(int index, int numColumns)
{
	ASSERT_VALID();

	if (!validColumn(index) || numColumns < 1 || numColumns >= columns() || index + numColumns > columns())
		return;

	// Remove column widths, column positions and columns of cells.
	qreal removedWidth = 0.0;
	for (int i = 0; i < numColumns; ++i)
	{
		// Remove columns widths and positions.
		removedWidth += m_columnWidths.takeAt(index);
		m_columnPositions.removeAt(index);

		// Remove and invalidate cells.
		QMutableListIterator<QList<TableCell> > rowIt(m_cellRows);
		while (rowIt.hasNext())
			rowIt.next().takeAt(index).setValid(false);
	}

	// Adjust following columns.
	for (int nextColumn = index; nextColumn < columns() - numColumns; ++nextColumn)
	{
		// Adjust position of following column.
		m_columnPositions[nextColumn] -= removedWidth;

		// "Move" cells in following column left.
		foreach (QList<TableCell> cellRow, m_cellRows)
			cellRow[nextColumn].moveLeft(numColumns);
	}

	// Update column spans.
	updateSpans(index, numColumns, ColumnsRemoved);

	// Decrease number of columns.
	m_columns -= numColumns;

	ASSERT_VALID();
}

qreal PageItem_Table::columnWidth(int column) const
{
	if (!validColumn(column))
		return 0.0;

	return m_columnWidths.at(column);
}

void PageItem_Table::setColumnWidth(int column, qreal width)
{
	if (!validColumn(column) || width <= 0.0)
		return;

	// Set the width and save the change.
	qreal deltaWidth = width - m_columnWidths.at(column);
	m_columnWidths[column] = width;

	// Adjust positions of following columns.
	for (int nextColumn = column + 1; nextColumn < columns(); ++nextColumn)
		m_columnPositions[nextColumn] += deltaWidth;
}

void PageItem_Table::mergeCells(int row, int column, int numRows, int numCols)
{
	ASSERT_VALID();

	if (!validCell(row, column) || !validCell(row + numRows - 1, column + numCols - 1))
		return;

	CellArea newArea(row, column, numCols, numRows);

	// Unite intersecting areas.
	QMutableListIterator<CellArea> areaIt(m_cellAreas);
	while (areaIt.hasNext())
	{
		CellArea oldArea = areaIt.next();
		if (newArea.intersects(oldArea))
		{
			// The two areas intersect, so unite them.
			newArea = newArea.united(oldArea);

			// Reset row/column span of old spanning cell, then remove old area.
			TableCell oldSpanningCell = cellAt(oldArea.row(), oldArea.column());
			oldSpanningCell.setRowSpan(1);
			oldSpanningCell.setColumnSpan(1);
			areaIt.remove();
		}
	}

	// Set row/column span of new spanning cell, and finally add new area.
	TableCell newSpanningCell = cellAt(newArea.row(), newArea.column());
	newSpanningCell.setRowSpan(newArea.height());
	newSpanningCell.setColumnSpan(newArea.width());
	m_cellAreas.append(newArea);

	ASSERT_VALID();
}

void PageItem_Table::splitCell(int row, int column, int numRows, int numCols)
{
	// Not implemented.
}

TableCell PageItem_Table::cellAt(int row, int column) const
{
	if (!validCell(row, column))
		return TableCell();

	TableCell cell = m_cellRows[row][column];

	QList<CellArea>::const_iterator areaIt;
	for (areaIt = m_cellAreas.begin(); areaIt != m_cellAreas.end(); ++areaIt)
	{
		CellArea area = (*areaIt);
		if (area.contains(row, column))
		{
			// Cell was contained in merged area, so use spanning cell.
			cell = m_cellRows[area.row()][area.column()];
			break;
		}
	}

	return cell;
}

bool PageItem_Table::isCovered(int row, int column) const
{
	if (!validCell(row, column))
		return false;

	TableCell cell = cellAt(row, column);

	return row != cell.row() || column != cell.column();
}

QRectF PageItem_Table::cellRect(int row, int column) const
{
	if (!validCell(row, column))
		return QRectF();

	TableCell cell = cellAt(row, column);

	// Determine where the cell starts and ends.
	int startRow = cell.row();
	int startColumn = cell.column();
	int endRow = startRow + cell.rowSpan() - 1;
	int endColumn = startColumn + cell.columnSpan() - 1;

	// Calculate cell geometry.
	qreal left = m_columnPositions[startColumn];
	qreal top = m_rowPositions[startRow];
	qreal width = m_columnPositions[endColumn] + m_columnWidths[endColumn] - left;
	qreal height = m_rowPositions[endRow] + m_rowHeights[endRow] - top;

	return QRectF(left, top, width, height);
}

void PageItem_Table::updateSpans(int index, int number, ChangeType changeType)
{
	// Loop through areas of merged cells.
	QMutableListIterator<CellArea> areaIt(m_cellAreas);
	while (areaIt.hasNext())
	{
		CellArea oldArea = areaIt.next();

		// Get a copy of the area adjusted to the change.
		CellArea newArea;
		switch (changeType)
		{
			case RowsInserted:
				newArea = oldArea.adjustedForRowInsertion(index, number);
				break;
			case RowsRemoved:
				newArea = oldArea.adjustedForRowRemoval(index, number);
				break;
			case ColumnsInserted:
				newArea = oldArea.adjustedForColumnInsertion(index, number);
				break;
			case ColumnsRemoved:
				newArea = oldArea.adjustedForColumnRemoval(index, number);
				break;
			default:
				break;
		}

		// Check if the area was affected by the change.
		if (newArea != oldArea)
		{
			if (newArea.height() < 2 && newArea.width() < 2)
			{
				// Adjusted area is 1x1 or less, so remove it.
				areaIt.remove();

				// And reset row/column span of spanning cell to 1.
				TableCell oldSpanningCell = cellAt(oldArea.row(), oldArea.column());
				oldSpanningCell.setRowSpan(1);
				oldSpanningCell.setColumnSpan(1);
			}
			else
			{
				// Replace the area with the adjusted copy.
				areaIt.setValue(newArea);

				// And set row/column spanning of spanning cell.
				TableCell newSpanningCell = cellAt(newArea.row(), newArea.column());
				newSpanningCell.setRowSpan(newArea.height());
				newSpanningCell.setColumnSpan(newArea.width());
			}
		}
	}
}

TableBorder PageItem_Table::collapsedLeftBorder(const TableCell& cell) const
{
	// Construct the two borders that should be collapsed.
	TableBorder leftBorder(cell.leftBorderWidth(), cell.leftBorderColor());
	TableBorder neighborBorder;
	if (cell.column() == 0)
	{
		// Neighboring border is left border of table.
		neighborBorder.width = leftBorderWidth();
		neighborBorder.color = leftBorderColor();
	}
	else
	{
		// Neighboring border is right border of neighboring cell.
		TableCell neighborCell = cellAt(cell.column() - 1, cell.row());
		neighborBorder.width = neighborCell.rightBorderWidth();
		neighborBorder.color = neighborCell.rightBorderColor();
	}

	// Collapse the borders.
	TableBorder collapsedBorder = collapseBorders(neighborBorder, leftBorder);

	// Set border coordinates.
	QRectF rect = cellRect(cell.row(), cell.column());
	collapsedBorder.start = rect.topLeft();
	collapsedBorder.end = rect.bottomLeft();

	return collapsedBorder;
}

TableBorder PageItem_Table::collapsedRightBorder(const TableCell& cell) const
{
	// Construct the two borders that should be collapsed.
	TableBorder rightBorder(cell.rightBorderWidth(), cell.rightBorderColor());
	TableBorder neighborBorder;
	if (cell.column() == columns() - 1)
	{
		// Neighboring border is right border of table.
		neighborBorder.width = rightBorderWidth();
		neighborBorder.color = rightBorderColor();
	}
	else
	{
		// Neighboring border is left border of neighboring cell.
		TableCell neighborCell = cellAt(cell.column() + 1, cell.row());
		neighborBorder.width = neighborCell.leftBorderWidth();
		neighborBorder.color = neighborCell.leftBorderColor();
	}

	// Collapse the borders.
	TableBorder collapsedBorder = collapseBorders(rightBorder, neighborBorder);

	// Set border coordinates.
	QRectF rect = cellRect(cell.row(), cell.column());
	collapsedBorder.start = rect.topRight();
	collapsedBorder.end = rect.bottomRight();

	return collapsedBorder;
}

TableBorder PageItem_Table::collapsedTopBorder(const TableCell& cell) const
{
	// Construct the two borders that should be collapsed.
	TableBorder topBorder(cell.topBorderWidth(), cell.topBorderColor());
	TableBorder neighborBorder;
	if (cell.row() == 0)
	{
		// Neighboring border is top border of table.
		neighborBorder.width = topBorderWidth();
		neighborBorder.color = topBorderColor();
	}
	else
	{
		// Neighboring border is bottom border of neighboring cell.
		TableCell neighborCell = cellAt(cell.column(), cell.row() - 1);
		neighborBorder.width = neighborCell.bottomBorderWidth();
		neighborBorder.color = neighborCell.bottomBorderColor();
	}

	// Collapse the borders.
	TableBorder collapsedBorder = collapseBorders(topBorder, neighborBorder);

	// Set initial coordinates of the border.
	QRectF rect = cellRect(cell.row(), cell.column());
	collapsedBorder.start = rect.topLeft();
	collapsedBorder.end = rect.topRight();

	return collapsedBorder;
}

TableBorder PageItem_Table::collapsedBottomBorder(const TableCell& cell) const
{
	// Construct the two borders that should be collapsed.
	TableBorder bottomBorder(cell.bottomBorderWidth(), cell.bottomBorderColor());
	TableBorder neighborBorder;
	if (cell.row() == rows() - 1)
	{
		// Neighboring border is bottom border of table.
		neighborBorder.width = bottomBorderWidth();
		neighborBorder.color = bottomBorderColor();
	}
	else
	{
		// Neighboring border is top border of neighboring cell.
		TableCell neighborCell = cellAt(cell.column(), cell.row() + 1);
		neighborBorder.width = neighborCell.topBorderWidth();
		neighborBorder.color = neighborCell.topBorderColor();
	}

	// Collapse the borders.
	TableBorder collapsedBorder = collapseBorders(neighborBorder, bottomBorder);

	// Set initial coordinates of the border.
	QRectF rect = cellRect(cell.row(), cell.column());
	collapsedBorder.start = rect.bottomLeft();
	collapsedBorder.end = rect.bottomRight();

	return collapsedBorder;
}

TableBorder PageItem_Table::collapseBorders(const TableBorder& firstBorder, const TableBorder& secondBorder) const
{
	/*
	 * Collapse firstBorder and secondBorder according to the following rules.
	 *
	 * (1) If neither firstBorder nor secondBorder has a color, collapsed
	 *     border has no color and 0 width.
	 * (2) If just secondBorder has a color, collapsed border is secondBorder.
	 * (3) If just firstBorder has a color, collapsed border is firstBorder.
	 * (4) If both firstBorder and secondBorder have a color but secondBorder
	 *     is wider than firstBorder, collapsed border is secondBorder.
	 * (5) If both firstBorder and secondBorder have a color but firstBorder
	 *     is wider than, or equally wide to, secondBorder, collapsed border
	 *     is firstBorder.
	 */

	TableBorder collapsedBorder;

	if (firstBorder.color == CommonStrings::None && secondBorder.color == CommonStrings::None)
	{
		// (1)
		collapsedBorder.width = 0.0;
		collapsedBorder.color = CommonStrings::None;
	}
	else if (firstBorder.color == CommonStrings::None)
		collapsedBorder = secondBorder; // (2)
	else if (secondBorder.color == CommonStrings::None)
		collapsedBorder = firstBorder; // (3)
	else
		if (secondBorder.width > firstBorder.width)
			collapsedBorder = secondBorder; // (4)
		else
			collapsedBorder = firstBorder; // (5)

	return collapsedBorder;
}

void PageItem_Table::debug() const
{
	qDebug() << "-------------------------------------------------";
	qDebug() << "Table Debug";
	qDebug() << "-------------------------------------------------";
	qDebug() << "m_rows: " <<  m_rows;
	qDebug() << "m_columns: " <<  m_columns;
	qDebug() << "m_columnPositions: " <<  m_columnPositions;
	qDebug() << "m_columnWidths: " <<  m_columnWidths;
	qDebug() << "m_rowPositions: " <<  m_rowPositions;
	qDebug() << "m_rowHeights: " <<  m_rowHeights;
	qDebug() << "m_cellSpans: " <<  m_cellAreas;
	qDebug() << "m_cellRows contains: ";
	foreach(QList<TableCell> cellRow, m_cellRows)
		foreach(TableCell cell, cellRow)
			qDebug() << cell.asString();
	qDebug() << "-------------------------------------------------";
}

void PageItem_Table::assertValid() const
{
	// Check list sizes.
	Q_ASSERT_X(rows() == m_rowPositions.size(), "isValid", "rows() != m_rowPositions.size()");
	Q_ASSERT_X(rows() == m_rowHeights.size(), "isValid", "rows() != m_rowHeights.size()");
	Q_ASSERT_X(columns() == m_columnPositions.size(), "isValid", "columns() != m_columnPositions.size()");
	Q_ASSERT_X(columns() == m_columnWidths.size(), "isValid", "columns() != m_columnWidths.size()");
	Q_ASSERT_X(rows() == m_cellRows.size(), "isValid", "rows() != m_cellRows.size()");
	foreach (QList<TableCell> cellRow, m_cellRows)
		Q_ASSERT_X(columns() == cellRow.size(), "isValid", "columns() != cellRow.size()");

	// Check that cells report correct row, column, row span and column span.
	for (int row = 0; row < rows(); ++row)
	{
		for (int col = 0; col < columns(); ++col)
		{
			TableCell cell = m_cellRows[row][col];
			Q_ASSERT_X(cell.row() == row, "isValid", "cell.row() != row");
			Q_ASSERT_X(cell.column() == col, "isValid", "cell.column() != col");
			if (cell.rowSpan() > 1 || cell.columnSpan() > 1)
			{
				// Cell has row or column span, so check that there's exactly one matching area.
				CellArea expectedArea(cell.row(), cell.column(), cell.columnSpan(), cell.rowSpan());
				Q_ASSERT_X(m_cellAreas.count(expectedArea) == 1, "isValid", "Unexpected number of cell areas");
			}
		}
	}

	// Check that cellAt(int, int) has correct behavior on covered cells.
	for (int row = 0; row < rows(); ++row)
	{
		for (int col = 0; col < columns(); ++col)
		{
			TableCell cell = cellAt(row, col);
			foreach (CellArea area, m_cellAreas)
			{
				if (area.contains(row, col))
				{
					// Area contains the requested row and column, so cellAt should report spanning cell.
					Q_ASSERT_X(area.width() > 0 && area.height() > 0 && (area.width() > 1 || area.height() > 1),
							   "isValid", "Invalid cell area size");
					Q_ASSERT_X(cell.row() == area.row() && cell.column() == area.column() &&
							   cell.rowSpan() == area.height() && cell.columnSpan() == area.width(),
							   "isValid", "cellAt(int, int) reports wrong cell");
				}
			}
		}
	}
}

void PageItem_Table::drawTableBackground(ScPainter* p)
{
	QString colorName = m_style.backgroundColor();

	if (colorName == CommonStrings::None)
		return;

	p->save();

	qreal x = m_columnPositions.first();
	qreal y = m_rowPositions.first();
	qreal width = m_columnPositions.last() + m_columnWidths.last() - x;
	qreal height = m_rowPositions.last() + m_rowHeights.last() - y;

	QColor color;
	SetQColor(&color, colorName, 100.0);
	p->setBrush(color);
	p->setFillMode(ScPainter::Solid);
	p->setStrokeMode(ScPainter::None);
	p->drawRect(x, y, width, height);

	p->restore();
}

void PageItem_Table::drawCellBackground(const TableCell& cell, ScPainter* p)
{
	QString colorName = cell.backgroundColor();

	if (colorName == CommonStrings::None)
		return;

	p->save();

	QColor color;
	SetQColor(&color, colorName, 100.0); // TODO: Support shade.
	p->setBrush(color);
	p->setFillMode(ScPainter::Solid);
	p->setStrokeMode(ScPainter::None);
	QRectF rect = cellRect(cell.row(), cell.column());
	p->drawRect(rect.x(), rect.y(), rect.width(), rect.height());

	p->restore();
}

void PageItem_Table::drawBorders(const QList<TableBorder>& borders, ScPainter* p)
{
	p->save();

	p->setStrokeMode(ScPainter::Solid);
	p->setFillMode(ScPainter::None);

	QColor color;
	QList<TableBorder>::const_iterator borderIt;
	for (borderIt = borders.begin(); borderIt != borders.end(); ++borderIt)
	{
		TableBorder border = (*borderIt);
		SetQColor(&color, border.color, 100.0); // TODO: Support shade.
		p->setPen(color, border.width, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
		p->drawLine(border.start, border.end);
	}

	p->restore();
}

void PageItem_Table::drawTableCollapsed(ScPainter* p)
{
	// Draw table background.
	drawTableBackground(p);

	// Draw table cell backgrounds and collect borders.
	TableCell cell;
	QList<TableBorder> verticalBorders;
	QList<TableBorder> horizontalBorders;
	for (int row = 0; row < rows(); ++row)
	{
		for (int col = 0; col < columns(); col += cell.columnSpan())
		{
			cell = cellAt(row, col);

			// Draw cell background.
			drawCellBackground(cell, p);

			// Collect borders.
			// TODO: Border line ends should be adjusted according to
			//       drawing order before being collected.
			verticalBorders.append(collapsedRightBorder(cell));
			horizontalBorders.append(collapsedBottomBorder(cell));
			if (row == 0)
				horizontalBorders.append(collapsedTopBorder(cell));
			if (col == 0)
				verticalBorders.append(collapsedLeftBorder(cell));
		}
	}

	// Draw borders.
	TableStyle::BorderDrawingOptions options = borderDrawingOptions();
	if (options & TableStyle::HorizontalFirst)
	{
		drawBorders(horizontalBorders, p);
		drawBorders(verticalBorders, p);
	}
	else if (options & TableStyle::VerticalFirst)
	{
		drawBorders(verticalBorders, p);
		drawBorders(horizontalBorders, p);
	}
}

void PageItem_Table::adjustToFrame()
{
	// Distribute width equally across columns.
	qreal columnWidth = width() / columns();
	qreal columnPosition = 0.0;
	for (int col = 0; col < columns(); ++col)
	{
		m_columnWidths[col] = columnWidth;
		m_columnPositions[col] = columnPosition;
		columnPosition += columnWidth;
	}

	// Distribute height equally across rows.
	qreal rowHeight = height() / rows();
	qreal rowPosition = 0.0;
	for (int row = 0; row < rows(); ++row)
	{
		m_rowHeights[row] = rowHeight;
		m_rowPositions[row] = rowPosition;
		rowPosition += rowHeight;
	}
}

void PageItem_Table::DrawObj_Item(ScPainter *p, QRectF /*e*/)
{
	if (m_Doc->RePos)
		return;

	if (borderModel() == TableStyle::Collapsed)
		drawTableCollapsed(p);

	// TODO: Implement TableStyle::Separated.
}
