/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "smcellstylewidget.h"
#include "util_icon.h"

SMCellStyleWidget::SMCellStyleWidget(QWidget *parent) : QWidget()
{
	setupUi(this);

	backgroundColorIcon->setPixmap(loadIcon("16/color-fill.png"));
	backgroundColor->addItem(CommonStrings::tr_NoneColor);
}

SMCellStyleWidget::~SMCellStyleWidget()
{
}


void SMCellStyleWidget::show(CellStyle *cellStyle, QList<CellStyle> &cellStyles, const QString &defLang, int unitIndex)
{
	Q_ASSERT(cellStyle);
	if (!cellStyle)
		return;

	// TODO: Handle parent styles (and language?)

	backgroundColor->setCurrentText(cellStyle->backgroundColor());
}

void SMCellStyleWidget::show(QList<CellStyle*> &cellStyles, QList<CellStyle> &cellStylesAll, const QString &defaultLanguage, int unitIndex)
{
	if (cellStyles.count() == 1) {
		show(cellStyles[0], cellStylesAll, defaultLanguage, unitIndex);
	}

	// TODO: Implement actual support multiple styles.
}

void SMCellStyleWidget::languageChange()
{
	backgroundColor->setToolTip(tr("Background Color"));
}

void SMCellStyleWidget::fillBackgroundColorCombo(ColorList &colors)
{
	backgroundColor->clear();

	backgroundColor->addItem(CommonStrings::tr_NoneColor);
	ColorList::Iterator itEnd = colors.end();
	ScribusDoc* doc = colors.document();
	for (ColorList::Iterator it = colors.begin(); it != itEnd; ++it)
	{
		backgroundColor->insertFancyItem(it.value(), doc, it.key());
	}
	backgroundColor->view()->setMinimumWidth(backgroundColor->view()->maximumViewportSize().width()+24);
}
