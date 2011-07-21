/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <algorithm>

#include <QMutableListIterator>
#include <QRectF>
#include <QString>
#include <QtAlgorithms>

#include "cellarea.h"
#include "collapsedtablepainter.h"
#include "pageitem.h"
#include "scpainter.h"
#include "scribusdoc.h"
#include "styles/tablestyle.h"
#include "tableutils.h"

#include "pageitem_table.h"

#ifdef WANT_DEBUG
	#define ASSERT_VALID() qt_noop()
#else
	#define ASSERT_VALID() assertValid(); qt_noop()
#endif

// The minimum row height.
const qreal PageItem_Table::MinimumRowHeight = 3.0;

// The minimum column width.
const qreal PageItem_Table::MinimumColumnWidth = 3.0;

PageItem_Table::PageItem_Table(ScribusDoc *pa, double x, double y, double w, double h,
	double w2, QString fill, QString outline, int numRows, int numColumns) :
	PageItem(pa, PageItem::Table, x, y, w, h, w2, fill, outline),
	m_rows(0), m_columns(0), m_tablePainter(new CollapsedTablePainter(this))
{
	Q_ASSERT(m_Doc);
	m_style.setContext(&m_Doc->tableStyles());

	// NOTE: The order here is important as insertRows() assumes that columns() > 0.
	insertColumns(0, qMax(1, numColumns));
	insertRows(0, qMax(1, numRows));

	adjustTableToFrame();
	adjustFrameToTable();
}

PageItem_Table::~PageItem_Table()
{
	delete m_tablePainter;
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

	emit changed();

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

	emit changed();

	ASSERT_VALID();
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

	emit changed();

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

	emit changed();

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
	qreal newRowHeight = qMax(height, MinimumRowHeight);
	qreal deltaHeight = newRowHeight - m_rowHeights.at(row);
	m_rowHeights[row] = newRowHeight;

	// Adjust positions of following rows.
	for (int nextRow = row + 1; nextRow < rows(); ++nextRow)
		m_rowPositions[nextRow] += deltaHeight;

	emit changed();
}

qreal PageItem_Table::rowPosition(int row) const
{
	if (!validRow(row))
		return 0.0;

	return m_rowPositions.at(row);
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
	qreal newColumnWidth = qMax(width, MinimumColumnWidth);
	qreal deltaWidth = newColumnWidth - m_columnWidths.at(column);
	m_columnWidths[column] = newColumnWidth;

	// Adjust positions of following columns.
	for (int nextColumn = column + 1; nextColumn < columns(); ++nextColumn)
		m_columnPositions[nextColumn] += deltaWidth;

	emit changed();
}

qreal PageItem_Table::columnPosition(int column) const
{
	if (!validColumn(column))
		return 0.0;

	return m_columnPositions.at(column);
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

	emit changed();

	ASSERT_VALID();
}

void PageItem_Table::splitCell(int row, int column, int numRows, int numCols)
{
	// Not implemented.
	emit changed();
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

TableCell PageItem_Table::cellAtPoint(qreal x, qreal y) const
{
	qreal gridOriginX = maxLeftBorderWidth() / 2;
	qreal gridOriginY = maxTopBorderWidth() / 2;

	if (x < gridOriginX || y < gridOriginY)
		return TableCell(); // Outside table grid at top/left.

	// Rebase the point against the table grid origin.
	x -= gridOriginX;
	y -= gridOriginY;

	if (x > tableWidth() || y > tableHeight())
		return TableCell(); // Outside table grid at bottom/right.

	return cellAt(
		qUpperBound(m_rowPositions, y) - m_rowPositions.begin() - 1,
		qUpperBound(m_columnPositions, x) - m_columnPositions.begin() - 1);
}

void PageItem_Table::resize(qreal width, qreal height, ResizeStrategy strategy)
{
	if (width < 0 || height < 0)
		return;

	if (strategy == Equal)
		resizeEqual(width, height);
	else if (strategy == Proportional)
		resizeProportional(width, height);
	else
		qWarning("Unknown table resize strategy");

	emit changed();
}

void PageItem_Table::adjustTableToFrame()
{
	resize(width() - (maxLeftBorderWidth() + maxRightBorderWidth()) / 2,
		height() - (maxTopBorderWidth() + maxBottomBorderWidth()) / 2);
}

void PageItem_Table::adjustFrameToTable()
{
	if (!m_Doc)
		return;

	m_Doc->SizeItem(effectiveWidth(), effectiveHeight(), this);
}

void PageItem_Table::setFillColor(const QString& color)
{
	m_style.setFillColor(color);
	emit changed();
}

void PageItem_Table::unsetFillColor()
{
	m_style.resetFillColor();
	emit changed();
}

QString PageItem_Table::fillColor() const
{
	return m_style.fillColor();
}

void PageItem_Table::setLeftBorder(const TableBorder& border)
{
	m_style.setLeftBorder(border);
	emit changed();
}

void PageItem_Table::unsetLeftBorder()
{
	m_style.resetLeftBorder();
	emit changed();
}

TableBorder PageItem_Table::leftBorder() const
{
	return m_style.leftBorder();
}

void PageItem_Table::setRightBorder(const TableBorder& border)
{
	m_style.setRightBorder(border);
	emit changed();
}

void PageItem_Table::unsetRightBorder()
{
	m_style.resetRightBorder();
	emit changed();
}

TableBorder PageItem_Table::rightBorder() const
{
	return m_style.rightBorder();
}

void PageItem_Table::setTopBorder(const TableBorder& border)
{
	m_style.setTopBorder(border);
	emit changed();
}

void PageItem_Table::unsetTopBorder()
{
	m_style.resetTopBorder();
	emit changed();
}

TableBorder PageItem_Table::topBorder() const
{
	return m_style.topBorder();
}

void PageItem_Table::setBottomBorder(const TableBorder& border)
{
	m_style.setBottomBorder(border);
	emit changed();
}

void PageItem_Table::unsetBottomBorder()
{
	m_style.resetBottomBorder();
	emit changed();
}

TableBorder PageItem_Table::bottomBorder() const
{
	return m_style.bottomBorder();
}

void PageItem_Table::setStyle(const QString& style)
{
	m_style.setParent(style);
	emit changed();
}

void PageItem_Table::unsetStyle()
{
	m_style.setParent("");
	emit changed();
}

QString PageItem_Table::style() const
{
	return m_style.parent();
}

qreal PageItem_Table::maxLeftBorderWidth() const
{
	qreal maxWidth = 0.0;
	TableCell cell;
	for (int row = 0; row < rows(); row += cell.rowSpan())
	{
		cell = cellAt(row, 0);
		maxWidth = qMax(maxWidth, TableUtils::collapseBorders(cell.leftBorder(), leftBorder()).width());
	}
	return maxWidth;
}

qreal PageItem_Table::maxRightBorderWidth() const
{
	qreal maxWidth = 0.0;
	TableCell cell;
	for (int row = 0; row < rows(); row += cell.rowSpan())
	{
		cell = cellAt(row, columns() - 1);
		maxWidth = qMax(maxWidth, TableUtils::collapseBorders(leftBorder(), cell.rightBorder()).width());
	}
	return maxWidth;
}

qreal PageItem_Table::maxTopBorderWidth() const
{
	qreal maxWidth = 0.0;
	TableCell cell;
	for (int col = 0; col < columns(); col += cell.columnSpan())
	{
		cell = cellAt(0, col);
		maxWidth = qMax(maxWidth, TableUtils::collapseBorders(cell.topBorder(), topBorder()).width());
	}
	return maxWidth;
}

qreal PageItem_Table::maxBottomBorderWidth() const
{
	qreal maxWidth = 0.0;
	TableCell cell;
	for (int col = 0; col < columns(); col += cell.columnSpan())
	{
		cell = cellAt(rows() - 1, col);
		maxWidth = qMax(maxWidth, TableUtils::collapseBorders(bottomBorder(), cell.bottomBorder()).width());
	}
	return maxWidth;
}

void PageItem_Table::applicableActions(QStringList& actionList)
{
	actionList << "itemAdjustFrameToTable";
	actionList << "itemAdjustTableToFrame";
}

void PageItem_Table::DrawObj_Item(ScPainter *p, QRectF /*e*/)
{
	if (m_Doc->RePos)
		return;

	p->save();

	// Set the clip path.
	p->setupPolygon(&PoLine);
	p->setClipPath();

	// Paint the table.
	m_tablePainter->paintTable(p);

	p->restore();

	// Paint the overflow marker.
	if (isOverflowing())
		drawOverflowMarker(p);
}

void PageItem_Table::resizeEqual(qreal width, qreal height)
{
	/*
	 * Distribute width equally to columns, but don't let any column width below
	 * MinimumColumnWidth.
	 */
	qreal requestedWidthChange = (width - tableWidth()) / columns();
	qreal oldMinWidth = *std::min_element(m_columnWidths.begin(), m_columnWidths.end());
	qreal newMinWidth = qMax(oldMinWidth + requestedWidthChange, MinimumColumnWidth);
	qreal actualWidthChange = newMinWidth - oldMinWidth;
	for (int col = 0; col < columns(); ++col)
	{
		m_columnWidths[col] += actualWidthChange;
		m_columnPositions[col] += col * actualWidthChange;
	}

	/*
	 * Distribute height equally to rows, but don't let any row height below
	 * MinimumRowHeight.
	 */
	qreal requestedHeightChange = (height - tableHeight()) / rows();
	qreal oldMinHeight = *std::min_element(m_rowHeights.begin(), m_rowHeights.end());
	qreal newMinHeight = qMax(oldMinHeight + requestedHeightChange, MinimumRowHeight);
	qreal actualHeightChange = newMinHeight - oldMinHeight;
	for (int row = 0; row < rows(); ++row)
	{
		m_rowHeights[row] += actualHeightChange;
		m_rowPositions[row] += row * actualHeightChange;
	}
}

void PageItem_Table::resizeProportional(qreal width, qreal height)
{
	/*
	 * Distribute width proportionally to columns, but don't let any column width below
	 * MinimumColumnWidth.
	 */
	qreal requestedWidthFactor = width / tableWidth();
	qreal oldMinWidth = *std::min_element(m_columnWidths.begin(), m_columnWidths.end());
	qreal newMinWidth = qMax(oldMinWidth * requestedWidthFactor, MinimumColumnWidth);
	qreal actualWidthFactor = newMinWidth / oldMinWidth;
	for (int col = 0; col < columns(); ++col)
	{
		m_columnWidths[col] *= actualWidthFactor;
		m_columnPositions[col] *= actualWidthFactor;
	}

	/*
	 * Distribute height proportionally to rows, but don't let any row height below
	 * MinimumRowHeight.
	 */
	qreal requestedHeightFactor = height / tableHeight();
	qreal oldMinHeight = *std::min_element(m_rowHeights.begin(), m_rowHeights.end());
	qreal newMinHeight = qMax(oldMinHeight * requestedHeightFactor, MinimumRowHeight);
	qreal actualHeightFactor = newMinHeight / oldMinHeight;
	for (int row = 0; row < rows(); ++row)
	{
		m_rowHeights[row] *= actualHeightFactor;
		m_rowPositions[row] *= actualHeightFactor;
	}
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
	qDebug() << "m_cellAreas: " <<  m_cellAreas;
	qDebug() << "m_cellRows: ";
	foreach(QList<TableCell> cellRow, m_cellRows)
		foreach(TableCell cell, cellRow)
			qDebug() << cell.asString();
	qDebug() << "-------------------------------------------------";
}

void PageItem_Table::assertValid() const
{
	// Check list sizes.
	Q_ASSERT(rows() == m_rowPositions.size());
	Q_ASSERT(rows() == m_rowHeights.size());
	Q_ASSERT(columns() == m_columnPositions.size());
	Q_ASSERT(columns() == m_columnWidths.size());
	Q_ASSERT(rows() == m_cellRows.size());
	foreach (QList<TableCell> cellRow, m_cellRows)
		Q_ASSERT(columns() == cellRow.size());

	for (int row = 0; row < rows(); ++row)
	{
		for (int col = 0; col < columns(); ++col)
		{
			TableCell cell = m_cellRows[row][col];

			// Check that the cell reports correct row and column.
			Q_ASSERT(cell.row() == row);
			Q_ASSERT(cell.column() == col);

			// Check that the row and column span is sane.
			Q_ASSERT(cell.rowSpan() >= 1 && cell.columnSpan() >= 1);

			if (cell.rowSpan() > 1 || cell.columnSpan() > 1)
			{
				// Check that there's exactly one matching cell area.
				CellArea expectedArea(cell.row(), cell.column(), cell.columnSpan(), cell.rowSpan());
				Q_ASSERT(m_cellAreas.count(expectedArea) == 1);
			}
		}
	}
}
