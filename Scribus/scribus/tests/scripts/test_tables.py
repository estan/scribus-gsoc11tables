#!/usr/bin/env python

"""
Test script for tables.

Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.

To add a new test, simply add a new test method named 'test_mytest' to the
TableTests class, and the test will be runned automatically.

Use check() to check a condition and fail(msg) to manually fail a test. The
tests are runned in a "fail fast" fashion; on failure, the test method will
stop executing and testing move on to the next test method.
"""

from scribus import *
from traceback import print_exc
from sys import stdout
from inspect import getmembers, ismethod

class TableTests:
    """ Tests for tables """
    def test_construction(self):
        """ Test for createTable(...) """
        # Test valid construction.
        table = createTable(50, 50, 90, 90, 3, 3)
        check(objectExists(table))
        check(getTableColumns(table) == 3)
        check(getTableRows(table) == 3)
        check(getTableColumnWidth(0, table) == 30)
        check(getTableColumnWidth(1, table) == 30)
        check(getTableColumnWidth(2, table) == 30)
        check(getTableRowHeight(0, table) == 30)
        check(getTableRowHeight(1, table) == 30)
        check(getTableRowHeight(2, table) == 30)
        deleteObject(table)

        # Test invalid construction.
        bad_args = [(-1, 3), (3, -1), (0, 3), (3, 0)]
        for args in bad_args:
            try:
                table = createTable(50, 50, 90, 90, args[0], args[1]);
                fail('Expected ValueError, numRows=%i, numColumns=%i' % (args[0], args[1]))
            except ValueError:
                pass # Expected.
            finally:
                if objectExists(table):
                    deleteObject(table)

    def test_set_row_height(self):
        """ Test for setTableRowHeight(...) """
        pass # TODO

    def test_set_column_width(self):
        """ Test for setTableColumnWidth(...) """
        pass # TODO

    def test_merge_cells(self):
        """ Test for mergeTableCells(...) """
        pass # TODO

#
# Test "framework" code below.
#
class TestFailure(Exception):
    """ Raised by fail() """
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return repr(self.msg)

def check(condition):
    """ Fails test if condition is false """
    if not condition:
        fail('Check failed')

def fail(msg):
    """ Fails test with msg """
    raise TestFailure(msg);

def is_test_method(obj):
    """ Returns True if obj is a test method """
    return ismethod(obj) and obj.__name__.startswith('test_')

if __name__ == '__main__':
    print 'Running table tests...'
    tests = TableTests()
    methods = getmembers(tests, is_test_method)
    ntests = len(methods)
    nfailed = 0
    for testnr, (name, method) in enumerate(methods):
        print '\t%i/%i: %s()%s' % (testnr + 1, ntests, name, '.' * (30 - len(name))),
        try:
            method()
        except:
            print 'Failed'
            print_exc(file=stdout)
            nfailed += 1
        else:
            print 'Passed'
    print '%i%% passed, %i tests failed out of %i' % (int(round((float(ntests - nfailed)/ntests)*100)), nfailed, ntests)
