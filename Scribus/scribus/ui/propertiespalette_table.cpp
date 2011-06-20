#include <QWidget>

#include "propertiespalette_table.h"

PropertiesPalette_Table::PropertiesPalette_Table(QWidget* parent) : QWidget(parent),
	m_mainWindow(0), m_doc(0), m_item(0)
{
	setupUi(this);
}

void PropertiesPalette_Table::updateColorList()
{
}

void PropertiesPalette_Table::setMainWindow(ScribusMainWindow* mainWindow)
{
	m_mainWindow = mainWindow;
}

void PropertiesPalette_Table::setDocument(ScribusDoc *doc)
{
	m_doc = doc;
}

void PropertiesPalette_Table::unsetDocument()
{
	m_doc = 0;
}

void PropertiesPalette_Table::setItem(PageItem* item)
{
	m_item = item;
}

void PropertiesPalette_Table::unsetItem()
{
	m_item = 0;
}

void PropertiesPalette_Table::handleSelectionChanged()
{
}

void PropertiesPalette_Table::languageChange()
{
}

void PropertiesPalette_Table::unitChange()
{
}
