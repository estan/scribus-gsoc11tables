#ifndef PROPERTIESPALETTE_TABLE_H
#define PROPERTIESPALETTE_TABLE_H

#include <QWidget>

#include "scribusapi.h"
#include "scguardedptr.h"

#include "ui_propertiespalette_tablebase.h"

class PageItem;
class ScribusMainWindow;
class ScribusDoc;

/**
 * Widget for table properties in the Properties Palette.
 */
class SCRIBUS_API PropertiesPalette_Table : public QWidget, Ui::PropertiesPalette_TableBase
{
	Q_OBJECT

public:
	explicit PropertiesPalette_Table(QWidget *parent = 0);
	~PropertiesPalette_Table() {}

	/// Update the list of colors.
	void updateColorList();

public slots:
	/// Set the main window to @a mainWindow.
	void setMainWindow(ScribusMainWindow* mainWindow);
	/// Set the current document to @a doc.
	void setDocument(ScribusDoc* doc);
	/// Unset the current document.
	void unsetDocument();
	/// Set the current item to @a item.
	void setItem(PageItem* item);
	/// Unset the current item.
	void unsetItem();

	/// Called when the selection changes.
	void handleSelectionChanged();
	/// Called when a language change occured.
	void languageChange();
	/// Called when a unit change occurred.
	void unitChange();

private:
	ScribusMainWindow* m_mainWindow;
	ScGuardedPtr<ScribusDoc> m_doc;
	PageItem *m_item;
};

#endif // PROPERTIESPALETTE_TABLE_H
