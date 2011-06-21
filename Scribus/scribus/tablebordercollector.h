/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef TABLEBORDERCOLLECTOR_H
#define TABLEBORDERCOLLECTOR_H

/**
 * Struct for table borders. Used temporarily during table painting.
 */
struct TableBorder
{
	qreal width;
	QColor color;
};

/**
 * The TableBorderCollector class is an abstract base class for table border collectors.
 * <p>
 * Table border collectors are responsible for collecting the horizontal and vertical
 * borders that needs to be drawn on a table.
 * <p>
 * This base class has a partial implementation of the border collection algorithm in its
 * collectBorders() function. The two concrete subclasses CollapsedBorderCollector and
 * SeparatedBorderCollector specialize the behavior of this method by implementing
 * the pure virtual functions collectLeftBorder(), collectRightBorder(), collectTopBorder()
 * and collectBottomBorder().
 */
class TableBorderCollector
{
public:
	/**
	 * Constructs a new border collector configured to collect borders from @a table.
	 */
	explicit TableBorderCollector(PageItem_Table *table) : m_table(table) {}
	/// Destructor.
	virtual ~TableBorderCollector() {}

	/**
	 * Collects the horizontal and vertical borders of the table in @a horizontalBorders and
	 * @a verticalBorders, respectively.
	 */
	void collectBorders(QList<TableBorder>& horizontalBorders, QList<TableBorder>& verticalBorders);

	/// Collects the left border of @a cell in @a verticalBorders.
	virtual void collectLeftBorder(QList<TableBorder>& verticalBorders, TableCell& cell) = 0;
	/// Collects the right border of @a cell in @a verticalBorders.
	virtual void collectRightBorder(QList<TableBorder>& verticalBorders, TableCell& cell) = 0;
	/// Collects the top border of @a cell in @a horizontalBorders.
	virtual void collectTopBorder(QList<TableBorder>& horizontalBorders, TableCell& cell) = 0;
	/// Collects the bottom border of @a cell in @a horizontalBorders.
	virtual void collectBottomBorder(QList<TableBorder>& horizontalBorders, TableCell& cell) = 0;

protected:
	PageItem_Table* table();

private:
	// The table the collector is set up to collect from.
	PageItem_Table* m_table;
};

inline bool operator==(const TableBorder& lhs, const TableBorder& rhs)
{
	return lhs.width == rhs.width && rhs.color == rhs.color;
};

#endif // TABLEBORDERCOLLECTOR_H
