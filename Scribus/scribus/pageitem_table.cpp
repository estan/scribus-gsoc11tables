/*
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
	: PageItem(pa, PageItem::Line, x, y, w, h, w2, fill, outline), m_rows(0), m_columns(0)
{
	insertRows(0, numRows);
	insertColumns(0, numColumns);

	// Just distribute all available space evenly for now.
	qreal initialRowHeight = h / rows();
	qreal initialColumnWidth = w / columns();
	for (int row = 0; row < rows(); ++row)
		setRowHeight(row, initialRowHeight);
	for (int col = 0; col < columns(); ++col)
		setColumnWidth(col, initialColumnWidth);

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
	m_rowHeights.replace(row, height);

	// Adjust positions of following rows.
	for (int r = row + 1; r < rows(); ++r)
		m_rowPositions[r] += deltaHeight;
}

void PageItem_Table::insertColumns(int index, int numColumns)
{
	qreal columnWidth = 20.0; // Hardcoded for now.

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
	m_columnWidths.replace(column, width);

	// Adjust positions of following columns.
	for (int col = column + 1; col < columns(); ++col)
		m_columnPositions[col] += deltaWidth;
}

void PageItem_Table::mergeCells(int row, int column, int numRows, int numCols)
{
	if (!validCell(row, column) || !validCell(row + numRows - 1, column + numCols - 1))
		return;

	CellArea newSpan(row, column, numCols, numRows);
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

void PageItem_Table::DrawObj_Item(ScPainter *p, QRectF /*e*/)
{
	if (m_Doc->RePos)
		return;

	/*
	 * Below is just some temporary painting to see what things looks like.
	 * It is not proper painting of tables.
	 */

	// A couple of hardcoded values for now.
	QColor cellBorderColor(Qt::red);
	QColor tableBorderColor(Qt::black);

	// TODO: How do I set up a clip rect here?

	// Draw table border.
	p->setPen(tableBorderColor);
	p->drawRect(0, 0, m_columnPositions.last() + m_columnWidths.last(),
				m_rowPositions.last() + m_rowHeights.last());

	// Draw table cells.
	p->setPen(cellBorderColor);
	QVector<double> dashValues;
	dashValues.append(1);
	p->setDash(dashValues, 1);
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
					p->drawLine(rect.topRight(), rect.bottomRight());
				if (row != rows() - 1)
					p->drawLine(rect.bottomRight(), rect.bottomLeft());
			}
		}
	}
}
