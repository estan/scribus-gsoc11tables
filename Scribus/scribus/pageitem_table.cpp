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
	: PageItem(pa, PageItem::Line, x, y, w, h, w2, fill, outline)
{
}

void PageItem_Table::DrawObj_Item(ScPainter *p, QRectF /*e*/)
{
	if (!m_Doc->RePos)
	{
		p->drawText(QRectF(0, 0, Width, Height), "I am a table");
	}
}
