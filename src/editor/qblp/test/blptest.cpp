/***************************************************************************
 *   Copyright (C) 2013 by Tamino Dauth                                    *
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

#include "blptest.hpp"
//#include "blptestConfig.h"

//#ifndef QBLP_ABSOLUTE_PATH
//#error Define QBLP_ABSOLUTE_PATH! // has to contain the absolute file path of the plugin
//#endif

#include <QtGui>
#include <QtTest>

QTEST_MAIN(wc3lib::editor::BlpTest);

namespace wc3lib
{

namespace editor
{

void BlpTest::initTestCase()
{
	// Called before the first testfunction is executed
	/*
	m_loader = new QPluginLoader(QBLP_ABSOLUTE_PATH, this);
	QVERIFY2(m_loader->load(), m_loader->errorString().toStdString());
	QVERIFY(m_loader->isLoaded());

	m_plugin = dynamic_cast<BlpIOPlugin*>(m_loader->instance());
	QVERIFY(m_plugin != 0);
	*/
}

void BlpTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
}

void BlpTest::init()
{
    // Called before each testfunction is executed
}

void BlpTest::cleanup()
{
    // Called after every testfunction
}

void BlpTest::ioHandlerReadTest()
{
	ifstream in("TeamColor00.blp", std::ios::in | std::ios::binary);
	QVERIFY(in);
	blp::Blp blpImage;
	bool success = true;

	try
	{
		blpImage.read(in);
	}
	catch (Exception &e)
	{
		success = false;
		std::cerr << e.what() << std::endl;
	}

	QVERIFY(success);

	BlpIOHandler handler;

	QImage image;
	QVERIFY(image.isNull());

	QVERIFY(handler.read(&image, blpImage));

	QVERIFY(!image.isNull());
	QVERIFY(image.size() == QSize(8, 8));
	QVERIFY(image.format() == QImage::Format_ARGB32);
}

void BlpTest::ioHandlerWriteTest()
{
	ifstream in("TeamColor00.blp");
	QVERIFY(in);
	blp::Blp blpImage;
	bool success = true;

	try
	{
		blpImage.read(in);
	}
	catch (Exception &e)
	{
		success = false;
		std::cerr << e.what() << std::endl;
	}

	QVERIFY(success);

	BlpIOHandler handler;

	QImage image;
	QVERIFY(image.isNull());

	QVERIFY(handler.read(&image, blpImage));

	QVERIFY(!image.isNull());
	QVERIFY(image.size() == QSize(8, 8));
	QVERIFY(image.format() == QImage::Format_ARGB32);

	blpImage.clear();

	QVERIFY(handler.write(image, &blpImage));
	QVERIFY(blpImage.mipMaps()[0].width() == 8);
	QVERIFY(blpImage.mipMaps()[0].height() == 8);
}

void BlpTest::writeTest()
{
	QImage image("DISBTNMagic.blp");
	QVERIFY(!image.isNull());
	const int byteCount = image.byteCount();

	QVERIFY(image.size() == QSize(64, 64));
	QVERIFY(image.format() == QImage::Format_ARGB32);

	QImageWriter writer("OutDISBTNMagic.blp");
	QVERIFY(writer.write(image));

	QImageReader reader("OutDISBTNMagic.blp");
	QVERIFY(reader.read(&image));

	QCOMPARE(byteCount, image.byteCount()); // new byte count == old byte count
}

void BlpTest::readTest()
{
	QImageReader reader("DISBTNMagic.blp"); // JPEG
	QImage image;
	QVERIFY(reader.read(&image));

	QVERIFY(image.size() == QSize(64, 64));
	QVERIFY(image.format() == QImage::Format_ARGB32);
}

}

}
