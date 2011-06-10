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
	insertRows(0, qMax(1, numRows));
	insertColumns(0, qMax(1, numColumns));
	adjustToFrame();
}

void PageItem_Table::insertRows(int index, int numRows)
{
	if (index < 0 || index > rows() || numRows < 1)
		return;

	// Insert row heights and positions.
	qreal rowHeight = rows() == 0 ? height() : m_rowHeights.at(qMax(index - 1, 0));
	qreal rowPosition = rows() == 0 ? 0.0 : m_rowPositions.at(qMax(index - 1, 0)) + rowHeight;
	for (int i = 0; i < numRows; ++i)
	{
		m_rowHeights.insert(index + i, rowHeight);
		m_rowPositions.insert(index + i, rowPosition);
		rowPosition += rowHeight;
	}

	// Adjust cell spans.
	QMutableListIterator<CellArea> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		CellArea span = spanIt.next();
		if (index < span.row())
		{
			// Rows were inserted before this span, so move it down.
			spanIt.setValue(span.translated(numRows, 0));
		}
		if (index >= span.row() && index <= span.bottom())
		{
			// Rows were inserted inside this span, so increase its height.
			spanIt.setValue(span.adjusted(0, 0, 0, numRows));
		}
	}

	m_rows += numRows;

	// Adjust positions of following rows.
	qreal insertedHeight = rowHeight * numRows;
	for (int nextRow = index + numRows; nextRow < rows(); ++nextRow)
		m_rowPositions[nextRow] += insertedHeight;
}

void PageItem_Table::removeRows(int index, int numRows)
{
	if (!validRow(index) || numRows < 1 || numRows >= rows() || index + numRows > rows())
		return;

	// Remove row heights and positions.
	qreal removedHeight = 0.0;
	for (int i = 0; i < numRows; ++i)
	{
		removedHeight += m_rowHeights.takeAt(index);
		m_rowPositions.removeAt(index);
	}

	// Adjust cell spans.
	QMutableListIterator<CellArea> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		CellArea span = spanIt.next();
		int removedInSpan = qMin(span.bottom(), index + numRows - 1) - qMax(span.row(), index);
		if (removedInSpan == span.height())
		{
			// All rows in span removed, so remove span completely.
			spanIt.remove();
		}
		else if (removedInSpan > 0)
		{
			// Some rows in span removed, so decrease its height.
			spanIt.setValue(span.adjusted(0, 0, 0, -removedInSpan));
		}
	}

	m_rows -= numRows;

	// Adjust positions of following rows.
	for (int nextRow = index; nextRow < rows(); ++nextRow)
		m_rowPositions[nextRow] -= removedHeight;
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

	// Insert column widths and positions.
	qreal columnWidth = columns() == 0 ? width() : m_columnWidths.at(qMax(index - 1, 0));
	qreal columnPosition = columns() == 0 ? 0.0 : m_columnPositions.at(qMax(index - 1, 0)) + columnWidth;
	for (int i = 0; i < numColumns; ++i)
	{
		m_columnWidths.insert(index + i, columnWidth);
		m_columnPositions.insert(index + i, columnPosition);
		columnPosition += columnWidth;
	}

	// Adjust cell spans.
	QMutableListIterator<CellArea> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		CellArea span = spanIt.next();
		if (index < span.column())
		{
			// Columns were inserted before this span, so move it to the right.
			spanIt.setValue(span.translated(0, numColumns));
		}
		if (index >= span.column() && index <= span.right())
		{
			// Columns were inserted inside this span, so increase its width.
			spanIt.setValue(span.adjusted(0, 0, numColumns, 0));
		}
	}

	m_columns += numColumns;

	// Adjust positions of following columns.
	qreal insertedWidth = columnWidth * numColumns;
	for (int nextColumn = index + numColumns; nextColumn < columns(); ++nextColumn)
		m_columnPositions[nextColumn] += insertedWidth;
}

void PageItem_Table::removeColumns(int index, int numColumns)
{
	if (!validColumn(index) || numColumns < 1 || numColumns >= columns() || index + numColumns > columns())
		return;

	// Remove column widths and positions.
	qreal removedWidth = 0.0;
	for (int i = 0; i < numColumns; ++i)
	{
		removedWidth += m_columnWidths.takeAt(index);
		m_columnPositions.removeAt(index);
	}

	// Adjust cell spans.
	QMutableListIterator<CellArea> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		CellArea span = spanIt.next();
		int removedInSpan = qMin(span.right(), index + numColumns - 1) - qMax(span.column(), index);
		if (removedInSpan == span.width())
		{
			// All columns in span removed, so remove span completely.
			spanIt.remove();
		}
		else if (removedInSpan > 0)
		{
			// Some columns in span removed, so decrease its width.
			spanIt.setValue(span.adjusted(0, 0, -removedInSpan, 0));
		}
	}

	m_columns -= numColumns;

	// Adjust positions of following columns.
	for (int nextColumn = index; nextColumn < columns(); ++nextColumn)
		m_columnPositions[nextColumn] -= removedWidth;
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

	CellArea newSpan(row, column, numCols, numRows);

	// Unite intersecting spans.
	QMutableListIterator<CellArea> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		CellArea span = spanIt.next();
		if (newSpan.intersects(span))
		{
			// The two spans intersect, so unite them.
			newSpan = newSpan.united(span);
			spanIt.remove();
		}
	}

	m_cellSpans.append(newSpan);
}

void PageItem_Table::splitCell(int row, int column, int numRows, int numCols)
{
	// Not implemented.
}

bool PageItem_Table::isCovered(int row, int column) const
{
	if (!validCell(row, column))
		return false;

	QList<CellArea>::const_iterator spanIt;
	for (spanIt = m_cellSpans.begin(); spanIt != m_cellSpans.end(); ++spanIt)
	{
		CellArea span = (*spanIt);
		if (span.contains(row, column) && !(span.row() == row && span.column() == column))
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

	// Adjust the rectangle if it's part of a span.
	QList<CellArea>::const_iterator spanIt;
	for (spanIt = m_cellSpans.begin(); spanIt != m_cellSpans.end(); ++spanIt)
	{
		CellArea span = (*spanIt);
		if (span.contains(row, column))
		{
			// Cell is contained in span, so use rectangle of the span.
			rect.setCoords(m_columnPositions.at(span.column()), m_rowPositions.at(span.row()),
						   m_columnPositions.at(span.right()) + m_columnWidths.at(span.right()) - 1,
						   m_rowPositions.at(span.bottom()) + m_rowHeights.at(span.bottom()) - 1);
			break;
		}
	}

	return rect;
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
	qDebug() << "m_cellSpans: " <<  m_cellSpans;
	qDebug() << "-------------------------------------------------";
}

void PageItem_Table::drawGridLine(const FPoint& start, const FPoint& end, ScPainter *p) const
{
	p->save();
	// TODO: Color should be configurable.
	double width = 1.0 / (qMax(p->zoomFactor(), 1.0) * 5.0);
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
