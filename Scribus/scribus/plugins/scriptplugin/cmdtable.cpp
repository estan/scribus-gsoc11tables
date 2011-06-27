/*
 * Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>
 *
 * For general Scribus (>=1.3.2) copyright and licensing information please refer
 * to the COPYING file provided with the program. Following this notice may exist
 * a copyright and/or license notice that predates the release of Scribus 1.3.2
 * for which a new license (GPL+exception) is in place.
 */

#include "cmdtable.h"
#include "cmdutil.h"
#include "pageitem_table.h"

PyObject *scribus_gettablerows(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;

	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table row count of non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyInt_FromLong(static_cast<long>(table->rows()));
}

PyObject *scribus_gettablecolumns(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;

	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table column count of non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyInt_FromLong(static_cast<long>(table->columns()));
}

PyObject *scribus_inserttablerows(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int index, numRows;
	if (!PyArg_ParseTuple(args, "ii|es", &index, &numRows, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot insert rows on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (index < 0 || index > table->rows())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table row index out of bounds, must be >= 0 and < %1", "python error").arg(table->rows()).toLocal8Bit().constData());
		return NULL;
	}
	if (numRows < 1)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table row count out of bounds, must be >= 1", "python error").toLocal8Bit().constData());
		return NULL;
	}
	table->insertRows(index, numRows);
	Py_RETURN_NONE;
}

PyObject *scribus_removetablerows(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int index, numRows;
	if (!PyArg_ParseTuple(args, "ii|es", &index, &numRows, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot remove rows from a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (index < 0 || index >= table->rows())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table row index out of bounds, must be >= 0 and < %1", "python error").arg(table->rows()).toLocal8Bit().constData());
		return NULL;
	}
	if (numRows < 1 || numRows >= table->rows())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table row count out of bounds, must be >= 1 and < %1", "python error").arg(table->rows()).toLocal8Bit().constData());
		return NULL;
	}
	if (index + numRows > table->rows())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Row deletion range out of bounds, index + numRows must be <= %1", "python error").arg(table->rows()).toLocal8Bit().constData());
		return NULL;
	}
	table->removeRows(index, numRows);
	Py_RETURN_NONE;
}

PyObject *scribus_gettablerowheight(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int row;
	if (!PyArg_ParseTuple(args, "i|es", &row, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get row height from non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyFloat_FromDouble(static_cast<double>(table->rowHeight(row)));
}

PyObject *scribus_settablerowheight(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int row;
	double height;
	if (!PyArg_ParseTuple(args, "id|es", &row, &height, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set row height on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (row < 0 || row >= table->rows())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table row index out of bounds, must be >= 0 and < %1", "python error").arg(table->rows()).toLocal8Bit().constData());
		return NULL;
	}
	if (height <= 0.0)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table row height must be > 0.0", "python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setRowHeight(row, height);
	Py_RETURN_NONE;
}

PyObject *scribus_inserttablecolumns(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int index, numColumns;
	if (!PyArg_ParseTuple(args, "ii|es", &index, &numColumns, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot insert columns on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (index < 0 || index > table->columns())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table column index out of bounds, must be >= 0 and < %1", "python error").arg(table->columns()).toLocal8Bit().constData());
		return NULL;
	}
	if (numColumns < 1)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table column count out of bounds, must be >= 1", "python error").toLocal8Bit().constData());
		return NULL;
	}
	table->insertColumns(index, numColumns);
	Py_RETURN_NONE;
}

PyObject *scribus_removetablecolumns(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int index, numColumns;
	if (!PyArg_ParseTuple(args, "ii|es", &index, &numColumns, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot remove columns from a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (index < 0 || index >= table->columns())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table column index out of bounds, must be >= 0 and < %1", "python error").arg(table->columns()).toLocal8Bit().constData());
		return NULL;
	}
	if (numColumns < 1 || numColumns >= table->columns())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table column count out of bounds, must be >= 1 and < %1", "python error").arg(table->columns()).toLocal8Bit().constData());
		return NULL;
	}
	if (index + numColumns > table->columns())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Column deletion range out of bounds, index + numColumns must be <= %1", "python error").arg(table->columns()).toLocal8Bit().constData());
		return NULL;
	}
	table->removeColumns(index, numColumns);
	Py_RETURN_NONE;
}

PyObject *scribus_gettablecolumnwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int column;
	if (!PyArg_ParseTuple(args, "i|es", &column, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get column width from non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyFloat_FromDouble(static_cast<double>(table->columnWidth(column)));
}

PyObject *scribus_settablecolumnwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int column;
	double width;
	if (!PyArg_ParseTuple(args, "id|es", &column, &width, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set column width on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (column < 0 || column >= table->columns())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table column index out of bounds, must be >= 0 and < %1", "python error").arg(table->columns()).toLocal8Bit().constData());
		return NULL;
	}
	if (width <= 0.0)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table column width must be > 0.0", "python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setColumnWidth(column, width);
	Py_RETURN_NONE;
}

PyObject *scribus_mergetablecells(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int row, column, numRows, numColumns;
	if (!PyArg_ParseTuple(args, "iiii|es", &row, &column, &numRows, &numColumns, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot merge cells on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (numRows < 1 || numColumns < 1)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Number of rows and columns must both be > 0.", "python error").toLocal8Bit().constData());
		return NULL;
	}
	if (row < 0 || row >= table->rows() || column < 0 || column >= table->columns() ||
			row + numRows - 1 < 0 || row + numRows - 1 >= table->rows() ||
			column + numColumns - 1 < 0 || column + numColumns - 1 >= table->columns())
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("The area %1,%2 %3x%4 is not inside the table.", "python error").arg(row).arg(column).arg(numColumns).arg(numRows).toLocal8Bit().constData());
		return NULL;
	}
	table->mergeCells(row, column, numRows, numColumns);
	Py_RETURN_NONE;
}

PyObject *scribus_gettablestyle(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table style on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyString_FromString(table->style().toUtf8());
}

PyObject *scribus_settablestyle(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	char *style;
	if (!PyArg_ParseTuple(args, "es|es", "utf-8", &style, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table style on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setStyle(QString::fromUtf8(style));
	Py_RETURN_NONE;
}

PyObject *scribus_gettablefillcolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table fill color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyString_FromString(table->fillColor().toUtf8());
}

PyObject *scribus_settablefillcolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	char *color;
	if (!PyArg_ParseTuple(args, "es|es", "utf-8", &color, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table fill color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setFillColor(QString::fromUtf8(color));
	Py_RETURN_NONE;
}

PyObject *scribus_gettableleftborderwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table left border width from non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyFloat_FromDouble(static_cast<double>(table->leftBorderWidth()));
}

PyObject *scribus_settableleftborderwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	double width;
	if (!PyArg_ParseTuple(args, "d|es", &width, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table left border width on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (width < 0.0)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table left border width must be >= 0.0", "python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setLeftBorderWidth(width);
	Py_RETURN_NONE;
}

PyObject *scribus_gettablerightborderwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table right border width from non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyFloat_FromDouble(static_cast<double>(table->rightBorderWidth()));
}

PyObject *scribus_settablerightborderwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	double width;
	if (!PyArg_ParseTuple(args, "d|es", &width, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table right border width on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (width < 0.0)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table right border width must be >= 0.0", "python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setRightBorderWidth(width);
	Py_RETURN_NONE;
}

PyObject *scribus_gettabletopborderwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table top border width from non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyFloat_FromDouble(static_cast<double>(table->topBorderWidth()));
}

PyObject *scribus_settabletopborderwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	double width;
	if (!PyArg_ParseTuple(args, "d|es", &width, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table top border width on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (width < 0.0)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table top border width must be >= 0.0", "python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setTopBorderWidth(width);
	Py_RETURN_NONE;
}

PyObject *scribus_gettablebottomborderwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table bottom border width from non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyFloat_FromDouble(static_cast<double>(table->bottomBorderWidth()));
}

PyObject *scribus_settablebottomborderwidth(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	double width;
	if (!PyArg_ParseTuple(args, "d|es", &width, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table bottom border width on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	if (width < 0.0)
	{
		PyErr_SetString(PyExc_ValueError, QObject::tr("Table bottom border width must be >= 0.0", "python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setBottomBorderWidth(width);
	Py_RETURN_NONE;
}

PyObject *scribus_gettableleftbordercolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table left border color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyString_FromString(table->leftBorderColor().toUtf8());
}

PyObject *scribus_settableleftbordercolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	char *color;
	if (!PyArg_ParseTuple(args, "es|es", "utf-8", &color, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table left border color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setLeftBorderColor(QString::fromUtf8(color));
	Py_RETURN_NONE;
}

PyObject *scribus_gettablerightbordercolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table right border color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyString_FromString(table->rightBorderColor().toUtf8());
}

PyObject *scribus_settablerightbordercolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	char *color;
	if (!PyArg_ParseTuple(args, "es|es", "utf-8", &color, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table right border color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setRightBorderColor(QString::fromUtf8(color));
	Py_RETURN_NONE;
}

PyObject *scribus_gettabletopbordercolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table top border color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyString_FromString(table->topBorderColor().toUtf8());
}

PyObject *scribus_settabletopbordercolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	char *color;
	if (!PyArg_ParseTuple(args, "es|es", "utf-8", &color, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table top border color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setTopBorderColor(QString::fromUtf8(color));
	Py_RETURN_NONE;
}

PyObject *scribus_gettablebottombordercolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	if (!PyArg_ParseTuple(args, "|es", "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot get table bottom border color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	return PyString_FromString(table->bottomBorderColor().toUtf8());
}

PyObject *scribus_settablebottombordercolor(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	char *color;
	if (!PyArg_ParseTuple(args, "es|es", "utf-8", &color, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set table bottom border color on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	table->setBottomBorderColor(QString::fromUtf8(color));
	Py_RETURN_NONE;
}

PyObject *scribus_settableborderdrawingoptions(PyObject* /* self */, PyObject* args)
{
	char *Name = const_cast<char*>("");
	int optionsInt;
	if (!PyArg_ParseTuple(args, "i|es", &optionsInt, "utf-8", &Name))
		return NULL;
	if(!checkHaveDocument())
		return NULL;
	PageItem *i = GetUniqueItem(QString::fromUtf8(Name));
	if (i == NULL)
		return NULL;
	PageItem_Table *table = i->asTable();
	if (!table)
	{
		PyErr_SetString(WrongFrameTypeError, QObject::tr("Cannot set border drawing options on a non-table item.","python error").toLocal8Bit().constData());
		return NULL;
	}
	TableStyle::BorderDrawingOptions options = static_cast<TableStyle::BorderDrawingOptions>(optionsInt);
	if ((options & TableStyle::HorizontalFirst) && (options & TableStyle::VerticalFirst))
	{
		// Both VerticalFirst and HorizontalFirst set, so use HorizontalFirst.
		options = options & ~TableStyle::VerticalFirst;
	}
	table->setBorderDrawingOptions(options);
	Py_RETURN_NONE;
}

/*! HACK: this removes "warning: 'blah' defined but not used" compiler warnings
with header files structure untouched (docstrings are kept near declarations)
PV */
void cmdtabledocwarnings()
{
	QStringList s;
	s << scribus_gettablecolumns__doc__ << scribus_gettablerows__doc__
	  << scribus_inserttablerows__doc__ << scribus_removetablerows__doc__
	  << scribus_inserttablecolumns__doc__ << scribus_removetablecolumns__doc__
	  << scribus_gettablerowheight__doc__ << scribus_gettablecolumnwidth__doc__
	  << scribus_settablerowheight__doc__ << scribus_settablecolumnwidth__doc__
	  << scribus_mergetablecells__doc__ << scribus_settablestyle__doc__
	  << scribus_gettablestyle__doc__ << scribus_settablefillcolor__doc__
	  << scribus_gettablefillcolor__doc__ << scribus_gettableleftborderwidth__doc__
	  << scribus_settableleftborderwidth__doc__ << scribus_gettablerightborderwidth__doc__
	  << scribus_settablerightborderwidth__doc__ << scribus_gettabletopborderwidth__doc__
	  << scribus_settabletopborderwidth__doc__ << scribus_gettablebottomborderwidth__doc__
	  << scribus_settablebottomborderwidth__doc__ << scribus_gettableleftbordercolor__doc__
	  << scribus_settableleftbordercolor__doc__ << scribus_gettablerightbordercolor__doc__
	  << scribus_settablerightbordercolor__doc__ << scribus_gettabletopbordercolor__doc__
	  << scribus_settabletopbordercolor__doc__ << scribus_gettablebottombordercolor__doc__
	  << scribus_settablebottombordercolor__doc__ << scribus_settableborderdrawingoptions__doc__;
}
