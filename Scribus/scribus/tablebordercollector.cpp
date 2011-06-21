/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <QList>

#include "pageitem_table.h"
#include "tablecell.h"

#include "tablebordercollector.h"

void TableBorderCollector::collectBorders(QList<TableBorder>& horizontalBorders, QList<TableBorder>& verticalBorders)
{
	Q_ASSERT(m_table);

	for (int row = 0; row < m_table->rows(); ++row)
	{
		for (int column = 0; column < m_table->columns(); ++column)
		{
			TableCell cell = m_table->cellAt(row, column);
			collectLeftBorder(verticalBorders, cell);
			collectRightBorder(verticalBorders, cell);
			collectTopBorder(horizontalBorders, cell);
			collectBottomBorder(horizontalBorders, cell);
		}
	}
}
