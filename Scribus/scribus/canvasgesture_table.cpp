/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QRectF>
#include <QTransform>

#include "canvas.h"
#include "fpoint.h"
#include "pageitem_table.h"
#include "scribusdoc.h"
#include "tablecell.h"

#include "canvasgesture_table.h"

QPointF TableGesture::globalToTableGrid(const QPoint point, bool snap)
{
	FPoint canvasPoint = m_canvas->globalToCanvas(point);

	if (snap)
	{
		// Snap to grid and guides.
		canvasPoint = m_doc->ApplyGridF(canvasPoint);
		m_doc->ApplyGuides(&canvasPoint);
	}

	// Return point mapped to table grid.
	return m_table->getTransform().inverted().map(canvasPoint.toQPointF()) - m_table->gridOffset();
}

void TableGesture::paintTableOutline(
	const QList<qreal>& rowHeights, const QList<qreal>& rowPositions,
	const QList<qreal>& columnWidths, const QList<qreal>& columnPositions, QPainter* p)
{
	if (!m_table || !m_canvas || !p)
		return;

	p->save();
	p->scale(m_canvas->scale(), m_canvas->scale());
	p->translate(-m_doc->minCanvasCoordinate.x(), -m_doc->minCanvasCoordinate.y());
	p->setTransform(m_table->getTransform(), true);
	p->setRenderHint(QPainter::Antialiasing);
	p->setPen(QPen(QColor(100, 200, 255), 3.0 / m_canvas->scale(), Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));

	QPointF offset = m_table->gridOffset();

	p->drawRect(QRectF(offset.x(), offset.y(), columnPositions.last() + columnWidths.last(),
		rowPositions.last() + rowHeights.last()));

	for (int row = 0; row < m_table->rows(); ++row)
	{
		int colSpan = 0;
		for (int col = 0; col < m_table->columns(); col += colSpan)
		{
			TableCell cell = m_table->cellAt(row, col);
			if (row == cell.row())
			{
				int endCol = col + cell.columnSpan() - 1;
				int endRow = row + cell.rowSpan() - 1;
				qreal left = columnPositions[col] + offset.x();
				qreal right = columnPositions[endCol] + columnWidths[endCol] + offset.x();
				qreal top = rowPositions[row] + offset.y();
				qreal bottom = rowPositions[endRow] + rowHeights[endRow] + offset.y();
				// Paint left and top edge of cell.
				if (col != 0)
					p->drawLine(QPointF(left, top), QPointF(left, bottom));
				if (row != 0)
					p->drawLine(QPointF(left, top), QPointF(right, top));
			}
			colSpan = cell.columnSpan();
		}
	}

	p->restore();
}

void TableGesture::paintCellSelection(QPainter* p)
{
	if (!m_table || !m_canvas || !p)
		return;

	p->save();
	p->scale(m_canvas->scale(), m_canvas->scale());
	p->translate(-m_doc->minCanvasCoordinate.x(), -m_doc->minCanvasCoordinate.y());
	p->setTransform(m_table->getTransform(), true);
	p->setRenderHint(QPainter::Antialiasing);
	p->setPen(QPen(QColor(100, 200, 255), 3.0 / m_canvas->scale(), Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
	p->setBrush(QColor(100, 200, 255, 50));

	const QPointF offset = m_table->gridOffset();
	const qreal rectExpand = 1.0;
	QPainterPath selectionPath;

	/*
	 * The code below makes selectionPath a union of the cell rectangles of the selected cells.
	 * Since the cell rectangles are adjacent, they must be expanded slightly (1.0) for the
	 * uniting to work. This may not be the fastest way to compose the path of the selection,
	 * but it makes for some very simple code, and the result looks good.
	 */
	foreach (const TableCell& cell, m_table->selection())
	{
		QRectF cellRect = m_table->cellRect(cell);
		cellRect.translate(offset);
		cellRect.adjust(-rectExpand, -rectExpand, rectExpand, rectExpand);
		QPainterPath cellPath;
		cellPath.addRect(cellRect);
		selectionPath = selectionPath.united(cellPath);
	}

	p->drawPath(selectionPath);
	p->restore();
}
