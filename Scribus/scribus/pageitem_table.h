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

	/// Returns the number of rows in the table.
	int	rows () const { return m_rows; }

	/// Returns the number of columns in the table.
	int	columns () const { return m_columns; }

	/**
	 * Inserts @a rows rows before the row with @a index.
	 *
	 * If the row with the given index doesn't exist or the number of rows are
	 * less than 1 then this method does nothing.
	 */
	void insertRows(int index, int rows);

	/**
	 * Removes @a rows rows from the table, starting with the row at @a index.
	 *
	 * If the specified range falls outside the table or the number of rows is
	 * less than 1 then this method does nothing.
	 */
	void removeRows(int index, int rows);

	/**
	 * Inserts @a columns columns before the column with @a index.
	 *
	 * If the column with the given index doesn't exist or the number of columns are
	 * less than 1 then this method does nothing.
	 */
	void insertColumns(int index, int columns);

	/**
	 * Removes @a columns columns from the table, starting with the column at @a index.
	 *
	 * If the specified range falls outside the table or the number of columns is
	 * less than 1 then this method does nothing.
	 */
	void removeColumns(int index, int columns);

	/**
	 * Merges the cell at the specified @a row and @a column with the adjacent cells into
	 * one cell.
	 *
	 * The new cell will span @a numRows rows and @a numCols columns. If @a numRows or
	 * @a numCols is less than the current number of rows or columns the cell spans then
	 * this method does nothing.
	 */
	void mergeCells(int row, int column, int numRows, int numCols);

	/**
	 * Splits the specified cell at @a row and @a column into an array of multiple cells with
	 * dimensions specified by @a numRows and @a numCols.
	 */
	void splitCell(int row, int column, int numRows, int numCols);

	virtual PageItem_Table* asTable() { return this; }
	virtual bool isTable() const { return true; }
	PageItem::ItemType realItemType() const { return PageItem::Table; }

	virtual void applicableActions(QStringList &actionList) {};
	virtual QString infoDescription() { return QString(); }

protected:
	virtual void DrawObj_Item(ScPainter *painter, QRectF clipRect);

private:
	int m_rows;
	int m_columns;
};

#endif // PAGEITEM_TABLE_H
