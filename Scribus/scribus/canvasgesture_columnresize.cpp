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

#include "canvasgesture_columnresize.h"

void ColumnResize::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		// Cancel the resize.
		event->accept();
		m_view->stopGesture();
	}
}

void ColumnResize::mouseReleaseEvent(QMouseEvent* event)
{
	event->accept();

	// TODO: There must be a bug in ApplyGuides() as there's some "wiggle room". Fix it.

	// Snap to grid and guides.
	FPoint canvasPoint = m_doc->ApplyGridF(m_canvas->globalToCanvas(event->globalPos()));
	m_doc->ApplyGuides(&canvasPoint);

	// Convert to table grid coordinates.
	QPointF gridPoint = m_table->getTransform().inverted().map(canvasPoint.toQPointF()) - m_table->gridOffset();

	// Perform the actual resize of the column.
	m_table->setColumnWidth(m_column, gridPoint.x() - m_table->columnPosition(m_column));
	m_table->update();

	m_view->stopGesture();
}

void ColumnResize::mouseMoveEvent(QMouseEvent* event)
{
	event->accept();

	// Snap to grid and guides.
	FPoint canvasPoint = m_doc->ApplyGridF(m_canvas->globalToCanvas(event->globalPos()));
	m_doc->ApplyGuides(&canvasPoint);

	// Convert to table grid coordinates.
	QPointF gridPoint = m_table->getTransform().inverted().map(canvasPoint.toQPointF()) - m_table->gridOffset();

	// Set width of column for the table outline.
	qreal columnPosition = m_columnPositions[m_column];
	qreal requestedWidth = gridPoint.x() - columnPosition;
	qreal actualWidth = qMax(PageItem_Table::MinimumColumnWidth, requestedWidth);
	m_columnWidths[m_column] = actualWidth;

	// Set positions of following columns.
	for (int column = m_column; column < m_columnPositions.size(); ++column)
	{
		m_columnPositions[column] = columnPosition;
		columnPosition += m_columnWidths[column];
	}

	// Display width tooltip.
	m_canvas->displayDoubleHUD(event->globalPos(), tr("Width"), actualWidth);

	m_canvas->update();
}

void ColumnResize::drawControls(QPainter* p)
{
	p->save();
	commonDrawControls(p, false);
	p->restore();

	p->save();
	p->scale(m_canvas->scale(), m_canvas->scale());
	p->translate(-m_doc->minCanvasCoordinate.x(), -m_doc->minCanvasCoordinate.y());
	p->setTransform(m_table->getTransform(), true);

	// Paint the table outline using the changed column geometries.
	TableUtils::paintOutline(m_table, m_table->rowHeights(), m_table->rowPositions(),
		m_columnWidths, m_columnPositions, m_canvas, p);

	p->restore();
}

void ColumnResize::setup(PageItem_Table* table, int column)
{
	Q_ASSERT(table);
	Q_ASSERT(column >= 0 && column < table->columns());

	m_table = table;
	m_column = column;

	// Make copies of the column geometries to be used during resize.
	m_columnWidths = m_table->columnWidths();
	m_columnPositions = m_table->columnPositions();
}
