/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QCursor>
#include <QDebug>
#include <QPainter>
#include <QPointF>
#include <QTimer>

#include "canvas.h"
#include "canvasgesture_cellselect.h"
#include "canvasgesture_columnresize.h"
#include "canvasgesture_rowresize.h"
#include "canvasgesture_tableresize.h"
#include "cellarea.h"
#include "fpoint.h"
#include "pageitem_table.h"
#include "scribusdoc.h"
#include "scribusview.h"
#include "selection.h"
#include "tablehandle.h"
#include "util_icon.h"

#include "canvasmode_edittable.h"

CanvasMode_EditTable::CanvasMode_EditTable(ScribusView* view) : CanvasMode(view),
	m_table(0),
	m_canvasUpdateTimer(new QTimer(view)),
	m_longBlink(false),
	m_cursorVisible(false),
	m_selectRowCursor(loadIcon("select_row.png")),
	m_selectColumnCursor(loadIcon("select_column.png")),
	m_tableResizeGesture(new TableResize(this)),
	m_rowResizeGesture(new RowResize(this)),
	m_columnResizeGesture(new ColumnResize(this)),
	m_cellSelectGesture(new CellSelect(this))
{
	connect(m_canvasUpdateTimer, SIGNAL(timeout()), this, SLOT(updateCanvas()));
}

CanvasMode_EditTable::~CanvasMode_EditTable()
{
	delete m_tableResizeGesture;
	delete m_rowResizeGesture;
	delete m_columnResizeGesture;
	delete m_cellSelectGesture;
}

void CanvasMode_EditTable::activate(bool fromGesture)
{
	PageItem *item = m_doc->m_Selection->itemAt(0);
	Q_ASSERT(item && item->isTable());
	m_table = item->asTable();

	m_blinkTime.start();
	m_canvasUpdateTimer->start(200);

	if (fromGesture)
		qApp->changeOverrideCursor(Qt::ArrowCursor);
}

void CanvasMode_EditTable::deactivate(bool forGesture)
{
	if (!forGesture)
		m_canvasUpdateTimer->stop();
}

void CanvasMode_EditTable::keyPressEvent(QKeyEvent* event)
{
	event->accept();

	if (event->key() == Qt::Key_Escape)
	{
		// Go back to normal mode.
		m_view->requestMode(modeNormal);
		return;
	}

	// Determine if we want a long blink.
	switch (event->key())
	{
		case Qt::Key_PageUp:
		case Qt::Key_PageDown:
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_Home:
		case Qt::Key_End:
			m_longBlink = true;
			break;
		default:
			m_longBlink = false;
			break;
	}

	// Pass event to text frame of active cell.
	bool repeat;
	m_table->activeCell().textFrame()->handleModeEditKey(event, repeat);
	m_canvas->m_viewMode.forceRedraw = true; // Hack?

	updateCanvas();
}

void CanvasMode_EditTable::mouseMoveEvent(QMouseEvent* event)
{
	event->accept();

	QPointF canvasPoint = m_canvas->globalToCanvas(event->globalPos()).toQPointF();
	qreal threshold = m_doc->guidesPrefs().grabRadius / m_canvas->scale();
	TableHandle handle = m_table->hitTest(canvasPoint, threshold);

	// Set an appropriate cursor.
	QCursor cursor(Qt::ArrowCursor);
	switch (handle.type())
	{
		case TableHandle::RowSelect:
			cursor = m_selectRowCursor;
			break;
		case TableHandle::RowResize:
			cursor = Qt::SizeVerCursor;
			break;
		case TableHandle::ColumnSelect:
			cursor = m_selectColumnCursor;
			break;
		case TableHandle::ColumnResize:
			cursor = Qt::SizeHorCursor;
			break;
		case TableHandle::TableResize:
			cursor = Qt::SizeFDiagCursor;
			break;
		case TableHandle::CellSelect:
			cursor = Qt::IBeamCursor;
			break;
		case TableHandle::None:
			break;
		default:
			qWarning("Unknown hit target");
			break;
	}
	qApp->changeOverrideCursor(cursor);
}

void CanvasMode_EditTable::mousePressEvent(QMouseEvent* event)
{
	event->accept();
	QPointF canvasPoint = m_canvas->globalToCanvas(event->globalPos()).toQPointF();
	qreal threshold = m_doc->guidesPrefs().grabRadius / m_canvas->scale();
	TableHandle handle = m_table->hitTest(canvasPoint, threshold);

	if (event->button() == Qt::LeftButton)
	{
		switch (handle.type())
		{
			case TableHandle::RowSelect:
				// Not implemented.
				break;
			case TableHandle::RowResize:
				m_rowResizeGesture->setup(m_table, handle.index());
				m_view->startGesture(m_rowResizeGesture);
				break;
			case TableHandle::ColumnSelect:
				// Not implemented.
				break;
			case TableHandle::ColumnResize:
				m_columnResizeGesture->setup(m_table, handle.index());
				m_view->startGesture(m_columnResizeGesture);
				break;
			case TableHandle::TableResize:
				m_tableResizeGesture->setup(m_table);
				m_view->startGesture(m_tableResizeGesture);
				break;
			case TableHandle::CellSelect:
				//m_cellSelectGesture->setup(m_table, m_table->cellAt(canvasPoint));
				//m_view->startGesture(m_cellSelectGesture);
				m_table->setActiveCell(m_table->cellAt(canvasPoint));
				break;
			case TableHandle::None:
				// Not implemented.
				break;
			default:
				qWarning("Unknown hit target");
				break;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		qDebug() << "Show table context menu";
	}
}

void CanvasMode_EditTable::drawControls(QPainter* p)
{
	p->save();
	commonDrawControls(p, false);
	drawTextCursor(p);
	p->restore();
}

void CanvasMode_EditTable::updateCanvas()
{
	m_canvas->update(m_canvas->canvasToLocal(m_table->getBoundingRect()));
}

void CanvasMode_EditTable::drawTextCursor(QPainter* p)
{
	if ((!m_longBlink && m_blinkTime.elapsed() > qApp->cursorFlashTime() / 2)
		|| (m_longBlink && m_blinkTime.elapsed() > qApp->cursorFlashTime()))
	{
		// Reset blink timer
		m_blinkTime.restart();
		m_longBlink = false;
		m_cursorVisible = !m_cursorVisible;
	}

	if (m_cursorVisible)
	{
		// Paint text cursor.
		p->save();
		p->setTransform(m_table->getTransform(), true);
		commonDrawTextCursor(p, m_table->activeCell().textFrame(), m_table->gridOffset());
		p->restore();
	}
}
