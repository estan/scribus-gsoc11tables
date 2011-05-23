/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <QRectF>

#include "pageitem.h"
#include "pageitem_table.h"
#include "scribusdoc.h"
#include "scpainter.h"

PageItem_Table::PageItem_Table(ScribusDoc *pa, double x, double y, double w, double h, double w2, QString fill, QString outline)
	: PageItem(pa, PageItem::Line, x, y, w, h, w2, fill, outline), m_rows(0), m_columns(0)
{
	insertRows(0, 1);
	insertColumns(0, 1);
}

void PageItem_Table::insertRows(int index, int numRows)
{
	qreal rowHeight = 10.0; // Hardcoded for now.

	if (index < 0 || index > rows() || numRows < 1)
		return;

	// Insert row heights and positions.
	qreal rowPosition = index == 0 ? 0.0 : m_rowPositions.at(index - 1) + m_rowHeights.at(index - 1);
	for (int i = 0; i < numRows; ++i)
	{
		m_rowHeights.insert(index + i, rowHeight);
		m_rowPositions.insert(index + i, rowPosition);
		rowPosition += rowHeight;
	}

	// Adjust positions of following rows.
	for (int row = index + numRows; row < rows(); ++row)
		m_rowPositions[row] += rowHeight;

	// Adjust cell spans.
	QMutableListIterator<QRect> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		QRect span = spanIt.next();
		if (index <= span.top())
		{
			// Rows were inserted before this span, so move it down.
			spanIt.setValue(span.translated(0, numRows));
		}
		if (index > span.top() && index < span.bottom())
		{
			// Rows were inserted inside this span, so extend it.
			spanIt.setValue(span.adjusted(0, 0, 0, numRows));
		}
	}

	m_rows += numRows;
}

void PageItem_Table::removeRows(int index, int numRows)
{
	if (!validRow(index) || numRows < 1 || index + numRows > rows())
		return;

	// Remove row heights and positions.
	for (int i = 0; i < numRows; ++i)
	{
		m_rowHeights.removeAt(index);
		m_rowPositions.removeAt(index);
	}

	// Adjust cell spans.
	QRect removedRect(0, index, columns(), numRows);
	QMutableListIterator<QRect> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		QRect span = spanIt.next();
		if (index + numRows <= span.top())
		{
			// All removed rows are before the span, so move it up.
			spanIt.setValue(span.translated(0, -numRows));
		}
		else
		{
			QRect removedInSpan = removedRect & span;
			if (removedInSpan.height() == span.height())
			{
				// All rows in span removed, so remove span completely.
				spanIt.remove();
			} else {
				// Decrease span by removed rows.
				spanIt.setValue(span.adjusted(0, 0, 0, -removedInSpan.height()));
			}
		}
	}

	m_rows -= numRows;
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
	m_rowHeights.replace(row, height);
}

void PageItem_Table::insertColumns(int index, int numColumns)
{
	qreal columnWidth = 10.0; // Hardcoded for now.

	if (index < 0 || index > columns() || numColumns < 1)
		return;

	// Insert columns widths and positions.
	qreal columnPosition = index == 0 ? 0.0 : m_columnPositions.at(index - 1) + m_columnWidths.at(index - 1);
	for (int i = 0; i < numColumns; ++i)
	{
		m_columnWidths.insert(index + i, columnWidth);
		m_columnPositions.insert(index + i, columnPosition);
		columnPosition += columnWidth;
	}

	// Adjust positions of following columns.
	for (int column = index + numColumns; column < columns(); ++column)
		m_columnPositions[column] += columnWidth;

	// Adjust cell spans.
	QMutableListIterator<QRect> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		QRect span = spanIt.next();
		if (index <= span.left())
		{
			// Columns were inserted before this span, so move it to the right.
			spanIt.setValue(span.translated(numColumns, 0));
		}
		if (index > span.left() && index < span.right())
		{
			// Columns were inserted inside this span, so extend it.
			spanIt.setValue(span.adjusted(0, 0, numColumns, 0));
		}
	}

	m_columns += numColumns;
}

void PageItem_Table::removeColumns(int index, int numColumns)
{
	if (!validColumn(index) || numColumns < 1 || index + numColumns > columns())
		return;

	// Remove column widths and positions.
	for (int i = 0; i < numColumns; ++i)
	{
		m_columnWidths.removeAt(index);
		m_columnPositions.removeAt(index);
	}

	// Adjust cell spans.
	QRect removedRect(index, 0, numColumns, rows());
	QMutableListIterator<QRect> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		QRect span = spanIt.next();
		if (index + numColumns <= span.left())
		{
			// All removed columns are before the span, so move it left.
			spanIt.setValue(span.translated(-numColumns, 0));
		}
		else
		{
			QRect removedInSpan = removedRect & span;
			if (removedInSpan.width() == span.width())
			{
				// All columns in span removed, so remove span completely.
				spanIt.remove();
			} else {
				// Decrease span by removed columns.
				spanIt.setValue(span.adjusted(0, 0, -removedInSpan.width(), 0));
			}
		}
	}

	m_columns -= numColumns;
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
	m_columnWidths.replace(column, width);
}

void PageItem_Table::mergeCells(int row, int column, int numRows, int numCols)
{
	if (!validCell(row, column) || !validCell(row + numRows, column + numCols))
		return;

	QRect newSpan(row, column, numRows, numCols);
	QMutableListIterator<QRect> spanIt(m_cellSpans);
	while (spanIt.hasNext())
	{
		QRect span = spanIt.next();
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

	QList<QRect>::const_iterator spanIt;
	for (spanIt = m_cellSpans.begin(); spanIt != m_cellSpans.end(); ++spanIt)
	{
		QRect span = (*spanIt);
		if (span.contains(row, column) && !(span.x() == column && span.y() == row))
			return true;
	}
	return false;
}

void PageItem_Table::DrawObj_Item(ScPainter *p, QRectF /*e*/)
{
	if (!m_Doc->RePos)
	{
		p->drawText(QRectF(0, 0, Width, Height), "I am a table");
	}
}
