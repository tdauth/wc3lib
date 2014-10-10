/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TEST_MPQ_URL
#error Define TEST_MPQ_URL.
#endif

#ifndef TEST_DIR_URL
#error Define TEST_DIR_URL.
#endif

/**
 * \file
 * Unit Test which tests shared data functions such as getting icons.
 */
#define BOOST_TEST_MODULE MpqPriorityList
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <QtGui>

#include "../mpqprioritylist.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::editor;

BOOST_AUTO_TEST_CASE(EntryMpq)
{
	/*
	 * Create a Qt Desktop application that the desktop widget can be passed as widget for the synchronous download.
	 */
	int argc = 0;
	char *argv = 0;
	QApplication app(argc, &argv);

	MpqPriorityList source;
	BOOST_REQUIRE(source.addSource(KUrl(TEST_MPQ_URL), 0));
	BOOST_REQUIRE(source.sources().size() == 1);
	BOOST_CHECK(!source.sources().begin()->isDirectory(QApplication::desktop()));
	BOOST_CHECK(source.sources().begin()->priority() == 0);
}

BOOST_AUTO_TEST_CASE(ExistsMpq)
{
	/*
	 * Create a Qt Desktop application that the desktop widget can be passed as widget for the synchronous download.
	 */
	int argc = 0;
	char *argv = 0;
	QApplication app(argc, &argv);

	MpqPriorityList source;
	BOOST_REQUIRE(source.addSource(KUrl(TEST_MPQ_URL)));
	BOOST_REQUIRE(source.sources().size() == 1);
	BOOST_CHECK(source.exists(KUrl("ReplaceableTextures\\TeamColor00.blp"), QApplication::desktop()));
}

BOOST_AUTO_TEST_CASE(DownloadMpq)
{
	/*
	 * Create a Qt Desktop application that the desktop widget can be passed as widget for the synchronous download.
	 */
	int argc = 0;
	char *argv = 0;
	QApplication app(argc, &argv);

	MpqPriorityList source;
	BOOST_REQUIRE(source.addSource(KUrl(TEST_MPQ_URL)));
	BOOST_REQUIRE(source.sources().size() == 1);
	QString file;
	BOOST_CHECK(source.download(KUrl("ReplaceableTextures\\TeamColor00.blp"), file, QApplication::desktop()));
	BOOST_CHECK(QFileInfo(file).exists());
}

BOOST_AUTO_TEST_CASE(EntryDir)
{
	/*
	 * Create a Qt Desktop application that the desktop widget can be passed as widget for the synchronous download.
	 */
	int argc = 0;
	char *argv = 0;
	QApplication app(argc, &argv);

	MpqPriorityList source;
	BOOST_REQUIRE(source.addSource(KUrl(TEST_DIR_URL), 0));
	BOOST_REQUIRE(source.sources().size() == 1);
	BOOST_CHECK(source.sources().begin()->isDirectory(QApplication::desktop()));
	BOOST_CHECK(source.sources().begin()->priority() == 0);
}

BOOST_AUTO_TEST_CASE(ExistsDir)
{
	/*
	 * Create a Qt Desktop application that the desktop widget can be passed as widget for the synchronous download.
	 */
	int argc = 0;
	char *argv = 0;
	QApplication app(argc, &argv);

	MpqPriorityList source;
	BOOST_REQUIRE(source.addSource(KUrl(TEST_DIR_URL)));
	BOOST_REQUIRE(source.sources().size() == 1);
	BOOST_CHECK(source.exists(KUrl("test.mpq"), QApplication::desktop()));
}

BOOST_AUTO_TEST_CASE(DownloadDir)
{
	/*
	 * Create a Qt Desktop application that the desktop widget can be passed as widget for the synchronous download.
	 */
	int argc = 0;
	char *argv = 0;
	QApplication app(argc, &argv);

	MpqPriorityList source;
	BOOST_REQUIRE(source.addSource(KUrl(TEST_DIR_URL)));
	BOOST_REQUIRE(source.sources().size() == 1);
	QString file;
	BOOST_CHECK(source.download(KUrl("test.mpq"), file, QApplication::desktop()));
	BOOST_CHECK(QFileInfo(file).exists());
}
