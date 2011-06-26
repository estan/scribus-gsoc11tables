/*
 * Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>
 *
 * For general Scribus (>=1.3.2) copyright and licensing information please refer
 * to the COPYING file provided with the program. Following this notice may exist
 * a copyright and/or license notice that predates the release of Scribus 1.3.2
 * for which a new license (GPL+exception) is in place.
 */

// Pulls in <Python.h> first
#include "cmdvar.h"

/*! Table handling */

#ifndef CMDTABLE_H
#define CMDTABLE_H

/*! docstring */
PyDoc_STRVAR(scribus_gettablerows__doc__,
QT_TR_NOOP("getTableRows([\"name\"]) -> integer\n\
\n\
Gets the number of rows in the table \"name\". If \"name\" is not\n\
given the currently selected item is used.\n\
"));
/*! Get number of table rows */
PyObject *scribus_gettablerows(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettablecolumns__doc__,
QT_TR_NOOP("getTableColumns([\"name\"]) -> integer\n\
\n\
Gets the number of columns in the table \"name\". If \"name\" is not\n\
given the currently selected item is used.\n\
"));
/*! Get number of table columns */
PyObject *scribus_gettablecolumns(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_inserttablerows__doc__,
QT_TR_NOOP("insertTableRows(index, numRows, [\"name\"])\n\
\n\
Inserts \"numRows\" rows before the row at \"index\" in the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if number of rows is not at least one or index is out of bounds.\n\
"));
/*! Insert table rows */
PyObject *scribus_inserttablerows(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_removetablerows__doc__,
QT_TR_NOOP("removeTableRows(index, numRows, [\"name\"])\n\
\n\
Removes \"numRows\" rows from the table \"name\" starting with the row at \"index\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if number of rows is not at least one or the range to be deleted is out of bounds.\n\
"));
/*! Remove table rows */
PyObject *scribus_removetablerows(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettablerowheight__doc__,
QT_TR_NOOP("getTableRowHeight(row, [\"name\"]) -> float\n\
\n\
Returns the row height of \"row\" in the table \"name\" expressed in\n\
points, or 0.0 if the row does not exist. If \"name\" is not given the currently\n\
selected item is used.\n\
"));
/*! Get table row height */
PyObject *scribus_gettablerowheight(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settablerowheight__doc__,
QT_TR_NOOP("setTableRowHeight(row, height, [\"name\"])\n\
\n\
Sets the height of \"row\" to \"height\" in the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the height is less than 0 or the row does not exist.\n\
"));
/*! Set table row height */
PyObject *scribus_settablerowheight(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_inserttablecolumns__doc__,
QT_TR_NOOP("insertTableColumns(index, numColumns, [\"name\"])\n\
\n\
Inserts \"numColumns\" columns before the column at \"index\" in the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if number of columns is not at least one or index is out of bounds.\n\
"));
/*! Insert table columns */
PyObject *scribus_inserttablecolumns(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_removetablecolumns__doc__,
QT_TR_NOOP("removeTableColumns(index, numColumns, [\"name\"])\n\
\n\
Removes \"numColumns\" columns from the table \"name\" starting with the column at \"index\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if number of columns is not at least one or the range to be deleted is out of bounds.\n\
"));
/*! Remove table columns */
PyObject *scribus_removetablecolumns(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettablecolumnwidth__doc__,
QT_TR_NOOP("getTableColumnWidth(column, [\"name\"]) -> float\n\
\n\
Returns the column width of \"column\" in the table \"name\" expressed in\n\
points, or 0.0 if the column does not exist. If \"name\" is not given the currently\n\
selected item is used.\n\
"));
/*! Get table column width */
PyObject *scribus_gettablecolumnwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settablecolumnwidth__doc__,
QT_TR_NOOP("setTableColumnWidth(column, width, [\"name\"])\n\
\n\
Sets the width of \"column\" to \"width\" in the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the width is less than 0 or the column does not exist.\n\
"));
/*! Set table column width */
PyObject *scribus_settablecolumnwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_mergetablecells__doc__,
QT_TR_NOOP("mergeTableCells(row, column, numRows, numColumns, [\"name\"])\n\
\n\
Merges the cell at the specified \"row\" and \"column\" with the adjacent cells into one cell.\n\
\n\
May throw ValueError if number if numRows or numColumns is less than 1 or the specified area is out of bounds.\n\
"));
/*! Merge table cells */
PyObject *scribus_mergetablecells(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettablestyle__doc__,
QT_TR_NOOP("getTableStyle([\"name\"]) -> string\n\
\n\
Returns the named style of the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table style */
PyObject *scribus_gettablestyle(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settablestyle__doc__,
QT_TR_NOOP("setTableStyle(style, [\"name\"])\n\
\n\
Sets the named style of the table \"name\" to \"style\".\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Set table style */
PyObject *scribus_settablestyle(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettablebackgroundcolor__doc__,
QT_TR_NOOP("getTableBackgroundColor([\"name\"]) -> string\n\
\n\
Returns the color of the background of the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table background color */
PyObject *scribus_gettablebackgroundcolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settablebackgroundcolor__doc__,
QT_TR_NOOP("setTableBackgroundColor(color, [\"name\"])\n\
\n\
Sets the color of the background of the table \"name\" to \"color\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError the table does not exist.\n\
"));
/*! Set table background color */
PyObject *scribus_settablebackgroundcolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettableleftborderwidth__doc__,
QT_TR_NOOP("getTableLeftBorderWidth([\"name\"]) -> float\n\
\n\
Returns the width of the left border of the table \"name\" expressed in points.\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table left border width */
PyObject *scribus_gettableleftborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settableleftborderwidth__doc__,
QT_TR_NOOP("setTableLeftBorderWidth(width, [\"name\"])\n\
\n\
Sets the width of the left border of the table \"name\" to \"width\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the width is less than 0.\n\
"));
/*! Set table left border width */
PyObject *scribus_settableleftborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettablerightborderwidth__doc__,
QT_TR_NOOP("getTableRightBorderWidth([\"name\"]) -> float\n\
\n\
Returns the width of the right border of the table \"name\" expressed in points.\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table right border width */
PyObject *scribus_gettablerightborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settablerightborderwidth__doc__,
QT_TR_NOOP("setTableRightBorderWidth(width, [\"name\"])\n\
\n\
Sets the width of the right border of the table \"name\" to \"width\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the width is less than 0.\n\
"));
/*! Set table right border width */
PyObject *scribus_settablerightborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettabletopborderwidth__doc__,
QT_TR_NOOP("getTableTopBorderWidth([\"name\"]) -> float\n\
\n\
Returns the width of the top border of the table \"name\" expressed in points\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table top border width */
PyObject *scribus_gettabletopborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settabletopborderwidth__doc__,
QT_TR_NOOP("setTableTopBorderWidth(width, [\"name\"])\n\
\n\
Sets the width of the top border of the table \"name\" to \"width\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the width is less than 0.\n\
"));
/*! Set table top border width */
PyObject *scribus_settabletopborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettablebottomborderwidth__doc__,
QT_TR_NOOP("getTableBottomBorderWidth([\"name\"]) -> float\n\
\n\
Returns the width of the bottom border of the table \"name\" expressed in points\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table bottom border width */
PyObject *scribus_gettablebottomborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settablebottomborderwidth__doc__,
QT_TR_NOOP("setTableBottomBorderWidth(width, [\"name\"])\n\
\n\
Sets the width of the bottom border of the table \"name\" to \"width\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the width is less than 0.\n\
"));
/*! Set table bottom border width */
PyObject *scribus_settablebottomborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettableleftbordercolor__doc__,
QT_TR_NOOP("getTableLeftBorderColor([\"name\"]) -> string\n\
\n\
Returns the color of the left border of the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table left border color */
PyObject *scribus_gettableleftbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settableleftbordercolor__doc__,
QT_TR_NOOP("setTableLeftBorderColor(color, [\"name\"])\n\
\n\
Sets the color of the left border of the table \"name\". to \"color\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError the table does not exist.\n\
"));
/*! Set table left border color */
PyObject *scribus_settableleftbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettablerightbordercolor__doc__,
QT_TR_NOOP("getTableRightBorderColor([\"name\"]) -> string\n\
\n\
Returns the color of the right border of the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table right border color */
PyObject *scribus_gettablerightbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settablerightbordercolor__doc__,
QT_TR_NOOP("setTableRightBorderColor(color, [\"name\"])\n\
\n\
Sets the color of the right border of the table \"name\" to \"color\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError the table does not exist.\n\
"));
/*! Set table right border color */
PyObject *scribus_settablerightbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettabletopbordercolor__doc__,
QT_TR_NOOP("getTableTopBorderColor([\"name\"]) -> string\n\
\n\
Returns the color of the top border of the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table top border color */
PyObject *scribus_gettabletopbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settabletopbordercolor__doc__,
QT_TR_NOOP("setTableTopBorderColor(color, [\"name\"])\n\
\n\
Sets the color of the top border of the table \"name\" to \"color\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError the table does not exist.\n\
"));
/*! Set table top border color */
PyObject *scribus_settabletopbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_gettablebottombordercolor__doc__,
QT_TR_NOOP("getTableBottomBorderColor([\"name\"]) -> string\n\
\n\
Returns the color of the bottom border of the table \"name\".\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get table bottom border color */
PyObject *scribus_gettablebottombordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settablebottombordercolor__doc__,
QT_TR_NOOP("setTableBottomBorderColor(color, [\"name\"])\n\
\n\
Sets the color of the bottom border of the table \"name\" to \"color\".\n\
If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError the table does not exist.\n\
"));
/*! Set table bottom border color */
PyObject *scribus_settablebottombordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_settableborderdrawingoptions__doc__,
QT_TR_NOOP("setTableBorderDrawingOptions(options, [\"name\"])\n\
\n\
Sets the border drawing options of the table \"name\" to \"options\".\n\
\"options\" can be either HORIZONTAL_FIRST or VERTICAL_FIRST, optionally\n\
combined with JOINED. If \"name\" is not given the currently selected item\n\
is used.\n\
"));
/*! Set table border drawing options */
PyObject *scribus_settableborderdrawingoptions(PyObject * /*self*/, PyObject* args);

#endif // CMDTABLE_H
