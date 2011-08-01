/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPointF>

#include "canvas.h"
#include "fpoint.h"
#include "pageitem.h"
#include "pageitem_table.h"
#include "selection.h"
#include "tableutils.h"

#include "canvasgesture_rowresize.h"

void RowResize::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		// Cancel the resize.
		event->accept();
		m_view->stopGesture();
	}
}

void RowResize::mouseReleaseEvent(QMouseEvent* event)
{
	event->accept();

	// TODO: There must be a bug in ApplyGuides() as there's some "wiggle room". Fix it.

	// Snap to grid and guides.
	FPoint canvasPoint = m_doc->ApplyGridF(m_canvas->globalToCanvas(event->globalPos()));
	m_doc->ApplyGuides(&canvasPoint);

	// Convert to table grid coordinates.
	QPointF gridPoint = m_table->getTransform().inverted().map(canvasPoint.toQPointF()) -
		QPointF(m_table->maxLeftBorderWidth()/2, m_table->maxTopBorderWidth()/2);

	// Perform the actual resize of the row.
	qreal requestedHeight = gridPoint.y() - m_table->rowPosition(m_row);
	qreal actualHeight = qMax(PageItem_Table::MinimumRowHeight, requestedHeight);
	m_table->setRowHeight(m_row, actualHeight);
	m_table->update();

	m_view->stopGesture();
}

void RowResize::mouseMoveEvent(QMouseEvent* event)
{
	event->accept();

	// TODO: There must be a bug in ApplyGuides() as there's some "wiggle room". Fix it.

	// Snap to grid and guides.
	FPoint canvasPoint = m_doc->ApplyGridF(m_canvas->globalToCanvas(event->globalPos()));
	m_doc->ApplyGuides(&canvasPoint);

	// Convert to table grid coordinates.
	QPointF gridPoint = m_table->getTransform().inverted().map(canvasPoint.toQPointF()) -
		QPointF(m_table->maxLeftBorderWidth()/2, m_table->maxTopBorderWidth()/2);

	// Set height of row for the table outline.
	qreal rowPosition = m_rowPositions[m_row];
	qreal requestedHeight = gridPoint.y() - rowPosition;
	qreal actualHeight = qMax(PageItem_Table::MinimumRowHeight, requestedHeight);
	m_rowHeights[m_row] = actualHeight;

	// Set positions of following rows.
	for (int row = m_row; row < m_rowPositions.size(); ++row)
	{
		m_rowPositions[row] = rowPosition;
		rowPosition += m_rowHeights[row];
	}

	m_canvas->update();
}

void RowResize::drawControls(QPainter* p)
{
	p->save();
	p->scale(m_canvas->scale(), m_canvas->scale());
	p->translate(-m_doc->minCanvasCoordinate.x(), -m_doc->minCanvasCoordinate.y());
	p->setTransform(m_table->getTransform(), true);

	// Paint the table outline using the changed row geometries.
	TableUtils::paintOutline(m_table, m_rowHeights, m_rowPositions,
		m_table->columnWidths(), m_table->columnPositions(), m_canvas, p);

	p->restore();
}

void RowResize::setup(PageItem_Table* table, int row)
{
	Q_ASSERT(table);
	Q_ASSERT(row >= 0 && row < table->rows());

	m_table = table;
	m_row = row;

	// Make copies of the row geometries to be used during resize.
	m_rowHeights = m_table->rowHeights();
	m_rowPositions = m_table->rowPositions();
}
