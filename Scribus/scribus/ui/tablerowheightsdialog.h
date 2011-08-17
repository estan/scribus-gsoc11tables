/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef TABLEROWHEIGHTSDIALOG_H
#define TABLEROWHEIGHTSDIALOG_H

#include "ui_tablerowheightsdialog.h"

/**
 * This is the dialog used when setting row heights on a table.
 */
class TableRowHeightsDialog : public QDialog, private Ui::TableRowHeightsDialog
{
	Q_OBJECT
public:
	/// Constructs a new dialog. The dialog will use the unit with index @a unitIndex.
	explicit TableRowHeightsDialog(int unitIndex, QWidget *parent = 0);

	/// Returns the number of rows the user entered.
	qreal rowHeight() const { return tableRowHeight->getValue(); }

private:
	int m_unitIndex;
};

#endif // INSERTTABLEROWSDIALOG_H
