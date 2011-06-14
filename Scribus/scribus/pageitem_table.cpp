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
#include "cellarea.h"
#include "text/frect.h"
#include "scribusdoc.h"
#include "scpainter.h"

PageItem_Table::PageItem_Table(ScribusDoc *pa, double x, double y, double w, double h, double w2, QString fill, QString outline, int numRows, int numColumns)
	: PageItem(pa, PageItem::Table, x, y, w, h, w2, fill, outline), m_rows(0), m_columns(0)
{
	// NOTE: The order here is important as insertRows() assumes that columns() > 0.
	insertColumns(0, qMax(1, numColumns));
	insertRows(0, qMax(1, numRows));

	adjustToFrame();
}

void PageItem_Table::insertRows(int index, int numRows)
{
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
		if (columns() > 0 || index > 0)
		{
			// NOTE: We assume here that columns() > 0.
			QList<TableCell> cellRow;
			for (int col = 0; col < columns(); ++col)
				cellRow.append(TableCell(row, col, this));
			m_cellRows.insert(row, cellRow);
		}
	}

	// Increase number of rows.
	m_rows += numRows;

	// Update row spans.
	updateSpans(index, numRows, RowsInserted);

	// Adjust following rows.
	qreal insertedHeight = rowHeight * numRows;
	for (int nextRow = index + numRows; nextRow < rows(); ++nextRow)
	{
		// Adjust position of following row.
		m_rowPositions[nextRow] += insertedHeight;

		// "Move" cells in following row down.
		foreach (TableCell cell, m_cellRows[nextRow])
			cell.moveDown(numRows);
	}
}

void PageItem_Table::removeRows(int index, int numRows)
{
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

	// Decrease number of rows.
	m_rows -= numRows;

	// Update row spans.
	updateSpans(index, numRows, RowsRemoved);

	// Adjust following rows.
	for (int nextRow = index; nextRow < rows(); ++nextRow)
	{
		// Adjust position of following row.
		m_rowPositions[nextRow] -= removedHeight;

		// "Move" cells in following row up.
		foreach (TableCell cell, m_cellRows[nextRow])
			cell.moveUp(numRows);
	}
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

	// Increase number of columns.
	m_columns += numColumns;

	// Update column spans.
	updateSpans(index, numColumns, ColumnsInserted);

	// Adjust following columns.
	qreal insertedWidth = columnWidth * numColumns;
	for (int nextColumn = index + numColumns; nextColumn < columns(); ++nextColumn)
	{
		// Adjust position of following column.
		m_columnPositions[nextColumn] += insertedWidth;

		// "Move" cells in following column right.
		foreach (QList<TableCell> cellRow, m_cellRows)
			cellRow[nextColumn].moveRight(numColumns);
	}
}

void PageItem_Table::removeColumns(int index, int numColumns)
{
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

	// Decrease number of columns.
	m_columns -= numColumns;

	// Update column spans.
	updateSpans(index, numColumns, ColumnsRemoved);

	// Adjust following columns.
	for (int nextColumn = index; nextColumn < columns(); ++nextColumn)
	{
		// Adjust position of following column.
		m_columnPositions[nextColumn] -= removedWidth;

		// "Move" cells in following column left.
		foreach (QList<TableCell> cellRow, m_cellRows)
			cellRow[nextColumn].moveLeft(numColumns);
	}
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
	if (!validCell(row, column) || !validCell(row + numRows - 1, column + numCols - 1))
		return;

	CellArea newArea(row, column, numCols, numRows);

	// Unite intersecting areas.
	QMutableListIterator<CellArea> areaIt(m_cellAreas);
	while (areaIt.hasNext())
	{
		CellArea area = areaIt.next();
		if (newArea.intersects(area))
		{
			// The two areas intersect, so unite them.
			newArea = newArea.united(area);
			areaIt.remove();
		}
	}

	// TODO: Should covered cells be invalidated here or not?

	m_cellAreas.append(newArea);
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

	QList<CellArea>::const_iterator areaIt;
	for (areaIt = m_cellAreas.begin(); areaIt != m_cellAreas.end(); ++areaIt)
	{
		CellArea area = (*areaIt);
		if (area.contains(row, column) && !(area.row() == row && area.column() == column))
			return true;
	}
	return false;
}

FRect PageItem_Table::cellRect(int row, int column) const
{
	if (!validCell(row, column))
		return FRect();

	FRect rect(m_columnPositions.at(column), m_rowPositions.at(row),
				m_columnWidths.at(column), m_rowHeights.at(row));

	// Adjust the rectangle if it's part of a area.
	QList<CellArea>::const_iterator areaIt;
	for (areaIt = m_cellAreas.begin(); areaIt != m_cellAreas.end(); ++areaIt)
	{
		CellArea area = (*areaIt);
		if (area.contains(row, column))
		{
			// Cell is contained in area, so use rectangle of the area.
			rect.setCoords(m_columnPositions.at(area.column()), m_rowPositions.at(area.row()),
						   m_columnPositions.at(area.right()) + m_columnWidths.at(area.right()) - 1,
						   m_rowPositions.at(area.bottom()) + m_rowHeights.at(area.bottom()) - 1);
			break;
		}
	}

	return rect;
}

void PageItem_Table::updateSpans(int index, int number, ChangeType changeType)
{
	// Loop through areas of merged cells.
	QMutableListIterator<CellArea> areaIt(m_cellAreas);
	while (areaIt.hasNext())
	{
		CellArea area = areaIt.next();

		// Get a copy of the area adjusted to the change.
		CellArea newArea;
		switch (changeType)
		{
			case RowsInserted:
				newArea = area.adjustedForRowInsertion(index, number);
				break;
			case RowsRemoved:
				newArea = area.adjustedForRowRemoval(index, number);
				break;
			case ColumnsInserted:
				newArea = area.adjustedForColumnInsertion(index, number);
				break;
			case ColumnsRemoved:
				newArea = area.adjustedForColumnRemoval(index, number);
				break;
			default:
				break;
		}

		if (newArea != area)
		{
			// If the area was affected by the change we either..
			if (newArea.height() < 2)
			{
				// ..remove areas that have become less than 2 in height or..
				areaIt.remove();
			}
			else
			{
				// ..replace the area with the adjusted copy.
				areaIt.setValue(newArea);
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
	qDebug() << "m_cellSpans: " <<  m_cellAreas;
	qDebug() << "m_cells: ";
	for (int row = 0; row < m_cellRows.size(); ++row)
	{
		QString rowStr = QString("row %1: ").arg(row);
		QList<TableCell> cellRow = m_cellRows[row];
		for (int col = 0; col < cellRow.size(); ++col)
			rowStr += cellAt(row, col); // Use cellAt(..) to exercise the API a little.
		qDebug().nospace() << rowStr;
	}
	qDebug() << "-------------------------------------------------";
}

void PageItem_Table::drawGridLine(const FPoint& start, const FPoint& end, ScPainter *p) const
{
	p->save();
	// TODO: Color should be configurable.
	double width = 0.5 / qMax(p->zoomFactor(), 1.0);
	p->setPen(Qt::red, width, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
	p->drawLine(start, end);
	p->restore();
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

	// Just draw the grid with decorative lines for now.
	p->save();
	FPoint topLeft(0.0, 0.0);
	FPoint topRight(m_columnPositions.last() + m_columnWidths.last(), 0.0);
	FPoint bottomRight(m_columnPositions.last() + m_columnWidths.last(), m_rowPositions.last() + m_rowHeights.last());
	FPoint bottomLeft(0.0, m_rowPositions.last() + m_rowHeights.last());
	drawGridLine(topLeft, topRight, p);
	drawGridLine(topRight, bottomRight, p);
	drawGridLine(bottomRight, bottomLeft, p);
	drawGridLine(bottomLeft, topLeft, p);

	for (int row = 0; row < rows(); ++row)
	{
		for (int col = 0; col < columns(); ++col)
		{
			if (!isCovered(row, col))
			{
				FRect rect = cellRect(row, col);
				rect.setRight(rect.right() + 1);
				rect.setBottom(rect.bottom() + 1);
				if (col != columns() - 1)
					drawGridLine(rect.topRight(), rect.bottomRight(), p);
				if (row != rows() - 1)
					drawGridLine(rect.bottomRight(), rect.bottomLeft(), p);
			}
		}
	}
	p->restore();
}
