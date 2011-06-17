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

/*! Table cell handling */

#ifndef CMDCELL_H
#define CMDCELL_H

/*! docstring */
PyDoc_STRVAR(scribus_getcellrowspan__doc__,
QT_TR_NOOP("getCellRowSpan(row, column, [\"name\"]) -> int\n\
\n\
Returns the row span of the cell at \"row\", \"column\" in the table \"name\" or -1 if the\n\
cell does not exist. If the cell is covered by another spanning cell, the row span of the\n\
spanning cell is returned. If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get cell row span */
PyObject *scribus_getcellrowspan(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_getcellcolumnspan__doc__,
QT_TR_NOOP("getCellColumnSpan(row, column, [\"name\"]) -> int\n\
\n\
Returns the column span of the cell at \"row\", \"column\" in the table \"name\" or -1 if the\n\
cell does not exist. If the cell is covered by another spanning cell, the column span of the\n\
spanning cell is returned. If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get cell column span */
PyObject *scribus_getcellcolumnspan(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_getcellleftborderwidth__doc__,
QT_TR_NOOP("getCellLeftBorderWidth(row, column, [\"name\"]) -> float\n\
\n\
Returns the width of the left border of the cell at \"row\", \"column\" in the table \"name\"\n\
expressed in points, or 0.0 if the cell does not exist. If \"name\" is not given the currently\n\
selected item is used.\n\
"));
/*! Get cell left border width */
PyObject *scribus_getcellleftborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_setcellleftborderwidth__doc__,
QT_TR_NOOP("setCellLeftBorderWidth(row, column, width, [\"name\"])\n\
\n\
Sets the width of the left border of the cell at \"row\", \"column\" in the table \"name\"\n\
to \"width\". If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the width is less than 0 or the cell does not exist.\n\
"));
/*! Set cell left border width */
PyObject *scribus_setcellleftborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_getcellrightborderwidth__doc__,
QT_TR_NOOP("getCellRightBorderWidth(row, column, [\"name\"]) -> float\n\
\n\
Returns the width of the right border of the cell at \"row\", \"column\" in the table \"name\"\n\
expressed in points, or 0.0 if the cell does not exist. If \"name\" is not given the currently\n\
selected item is used.\n\
"));
/*! Get cell right border width */
PyObject *scribus_getcellrightborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_setcellrightborderwidth__doc__,
QT_TR_NOOP("setCellRightBorderWidth(row, column, width, [\"name\"])\n\
\n\
Sets the width of the right border of the cell at \"row\", \"column\" in the table \"name\"\n\
to \"width\". If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the width is less than 0 or the cell does not exist.\n\
"));
/*! Set cell right border width */
PyObject *scribus_setcellrightborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_getcelltopborderwidth__doc__,
QT_TR_NOOP("getCellTopBorderWidth(row, column, [\"name\"]) -> float\n\
\n\
Returns the width of the top border of the cell at \"row\", \"column\" in the table \"name\"\n\
expressed in points, or 0.0 if the cell does not exist. If \"name\" is not given the currently\n\
selected item is used.\n\
"));
/*! Get cell top border width */
PyObject *scribus_getcelltopborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_setcelltopborderwidth__doc__,
QT_TR_NOOP("setCellTopBorderWidth(row, column, width, [\"name\"])\n\
\n\
Sets the width of the top border of the cell at \"row\", \"column\" in the table \"name\"\n\
to \"width\". If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the width is less than 0 or the cell does not exist.\n\
"));
/*! Set cell top border width */
PyObject *scribus_setcelltopborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_getcellbottomborderwidth__doc__,
QT_TR_NOOP("getCellBottomBorderWidth(row, column, [\"name\"]) -> float\n\
\n\
Returns the width of the bottom border of the cell at \"row\", \"column\" in the table \"name\"\n\
expressed in points, or 0.0 if the cell does not exist. If \"name\" is not given the currently\n\
selected item is used.\n\
"));
/*! Get cell bottom border width */
PyObject *scribus_getcellbottomborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_setcellbottomborderwidth__doc__,
QT_TR_NOOP("setCellBottomBorderWidth(row, column, width, [\"name\"])\n\
\n\
Sets the width of the bottom border of the cell at \"row\", \"column\" in the table \"name\"\n\
to \"width\". If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError if the width is less than 0 or the cell does not exist.\n\
"));
/*! Set cell bottom border width */
PyObject *scribus_setcellbottomborderwidth(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_getcellleftbordercolor__doc__,
QT_TR_NOOP("getCellLeftBorderColor(row, column, [\"name\"]) -> string\n\
\n\
Returns the color of the left border of the cell at \"row\", \"column\" in the table \"name\"\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get cell left border color */
PyObject *scribus_getcellleftbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_setcellleftbordercolor__doc__,
QT_TR_NOOP("setCellLeftBorderColor(row, column, color, [\"name\"])\n\
\n\
Sets the color of the left border of the cell at \"row\", \"column\" in the table \"name\"\n\
to \"color\". If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError the cell does not exist.\n\
"));
/*! Set cell left border color */
PyObject *scribus_setcellleftbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_getcellrightbordercolor__doc__,
QT_TR_NOOP("getCellRightBorderColor(row, column, [\"name\"]) -> string\n\
\n\
Returns the color of the right border of the cell at \"row\", \"column\" in the table \"name\"\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get cell right border color */
PyObject *scribus_getcellrightbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_setcellrightbordercolor__doc__,
QT_TR_NOOP("setCellRightBorderColor(row, column, color, [\"name\"])\n\
\n\
Sets the color of the right border of the cell at \"row\", \"column\" in the table \"name\"\n\
to \"color\". If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError the cell does not exist.\n\
"));
/*! Set cell right border color */
PyObject *scribus_setcellrightbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_getcelltopbordercolor__doc__,
QT_TR_NOOP("getCellTopBorderColor(row, column, [\"name\"]) -> string\n\
\n\
Returns the color of the top border of the cell at \"row\", \"column\" in the table \"name\"\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get cell top border color */
PyObject *scribus_getcelltopbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_setcelltopbordercolor__doc__,
QT_TR_NOOP("setCellTopBorderColor(row, column, color, [\"name\"])\n\
\n\
Sets the color of the top border of the cell at \"row\", \"column\" in the table \"name\"\n\
to \"color\". If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError the cell does not exist.\n\
"));
/*! Set cell top border color */
PyObject *scribus_setcelltopbordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_getcellbottombordercolor__doc__,
QT_TR_NOOP("getCellBottomBorderColor(row, column, [\"name\"]) -> string\n\
\n\
Returns the color of the bottom border of the cell at \"row\", \"column\" in the table \"name\"\n\
If \"name\" is not given the currently selected item is used.\n\
"));
/*! Get cell bottom border color */
PyObject *scribus_getcellbottombordercolor(PyObject * /*self*/, PyObject* args);

/*! docstring */
PyDoc_STRVAR(scribus_setcellbottombordercolor__doc__,
QT_TR_NOOP("setCellBottomBorderColor(row, column, color, [\"name\"])\n\
\n\
Sets the color of the bottom border of the cell at \"row\", \"column\" in the table \"name\"\n\
to \"color\". If \"name\" is not given the currently selected item is used.\n\
\n\
May throw ValueError the cell does not exist.\n\
"));
/*! Set cell bottom border color */
PyObject *scribus_setcellbottombordercolor(PyObject * /*self*/, PyObject* args);

#endif // CMDCELL_H
