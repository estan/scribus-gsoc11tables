/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PAGEITEM_TABLE_H
#define PAGEITEM_TABLE_H

#include <QString>
#include <QRectF>

#include "scribusapi.h"
#include "pageitem.h"

class ScPainter;
class ScribusDoc;

/**
 * The PageItem_Table class represents a table item.
 *
 * It is just a dummy class for now.
 */
class SCRIBUS_API PageItem_Table : public PageItem
{
	Q_OBJECT

public:
	PageItem_Table(ScribusDoc *pa, double x, double y, double w, double h, double w2, QString fill, QString outline);
	~PageItem_Table() {};

	virtual PageItem_Table* asTable() { return this; }
	virtual bool isTable() const { return true; }
	PageItem::ItemType realItemType() const { return PageItem::Table; }

	virtual void applicableActions(QStringList &actionList) {};
	virtual QString infoDescription() { return QString(); }

protected:
	virtual void DrawObj_Item(ScPainter *painter, QRectF clipRect);

private:
	int nRows;
	int nColumn;
};

#endif // PAGEITEM_TABLE_H
