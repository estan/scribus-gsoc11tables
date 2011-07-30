/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QMouseEvent>
#include <QKeyEvent>

#include "canvas.h"
#include "pageitem.h"
#include "pageitem_table.h"
#include "selection.h"

#include "canvasgesture_rowresize.h"

void RowResize::activate(bool fromGesture)
{
	PageItem *item = m_doc->m_Selection->itemAt(0);
	Q_ASSERT(item && item->isTable());
	m_table = item->asTable();
}

void RowResize::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		// Cancel the resize.
		m_table->setRowHeight(m_row, m_oldRowHeight);
		m_table->update();
		m_view->stopGesture();
	}
}

void RowResize::mousePressEvent(QMouseEvent* event)
{
	m_oldRowHeight = m_table->rowHeight(m_row);
}

void RowResize::mouseReleaseEvent(QMouseEvent* event)
{
	m_view->stopGesture();
}

void RowResize::mouseMoveEvent(QMouseEvent* event)
{
	/*
	 * FIXME1: Doing an actual resize here in the move event is pretty
	 *         slow. We probably want to draw a "ghost" table instead.
	 * FIXME2: Snap to guides.
	 */
	QPointF gridPoint = m_table->getTransform().inverted().map(
		m_canvas->globalToCanvas(event->globalPos()).toQPointF());
	qreal height = gridPoint.y() - m_table->rowPosition(m_row);

	m_table->setRowHeight(m_row, qMax(PageItem_Table::MinimumRowHeight, height));
	m_table->update();
}
