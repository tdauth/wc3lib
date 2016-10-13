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
#include "../../platform.hpp"
#include "blptestConfig.h"

#ifndef QBLP_ABSOLUTE_PATH
#error Define QBLP_ABSOLUTE_PATH! // has to contain the absolute file path of the plugin
#endif

#include <QtGui>
#include <QtTest>

QTEST_MAIN(wc3lib::editor::BlpTest);

namespace wc3lib
{

namespace editor
{

void BlpTest::initTestCase()
{
	qDebug() << "Env value" << qgetenv("QT_DEBUG_PLUGINS");
	QVERIFY(qgetenv("QT_DEBUG_PLUGINS") == QByteArray("1"));
	// Called before the first testfunction is executed
	m_loader = new QPluginLoader(QBLP_ABSOLUTE_PATH, this);
	qDebug() << "Loading from path:" << QBLP_ABSOLUTE_PATH;
	const bool loaded = m_loader->load();
	const QString errorString = m_loader->errorString();
	qDebug() << "Error string:" << errorString;
	QVERIFY(loaded);
	QVERIFY(m_loader->isLoaded());
	QVERIFY(m_loader->instance() != nullptr);

	//m_plugin = dynamic_cast<BlpIOPlugin*>(m_loader->instance());
	//QVERIFY(m_plugin != 0);
}

void BlpTest::cleanupTestCase()
{
	m_plugin = 0;
	QVERIFY(m_loader->unload());
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

void BlpTest::supportedTest()
{
	qDebug() << "Formats:" << QImageReader::supportedImageFormats();
	QVERIFY(QImageReader::supportedImageFormats().contains("blp"));
}

void BlpTest::ioHandlerReadTest()
{
	ifstream in("TeamColor00.blp", std::ios::in | std::ios::binary);
	QVERIFY(in.is_open());
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
	QVERIFY(blpImage.format() == blp::Blp::Format::Blp1);
	QVERIFY(blpImage.compression() == blp::Blp::Compression::Jpeg);
	QVERIFY(blpImage.mipMaps().size() >= 1);
	QVERIFY(blpImage.mipMaps().front().width() == 8);
	QVERIFY(blpImage.mipMaps().front().height() == 8);
	const blp::color blpPixel = blpImage.mipMaps().front().colorAt(0, 0).rgba();

	BlpIOHandler handler;

	QImage image;
	QVERIFY(image.isNull());

	QVERIFY(handler.read(&image, blpImage));

	QVERIFY(!image.isNull());
	QVERIFY(image.size() == QSize(8, 8));
	QVERIFY(image.format() == QImage::Format_ARGB32);
	const blp::color qtPixel = argbToColor(image.pixel(0, 0));

	QVERIFY(blpPixel == qtPixel);
}

void BlpTest::ioHandlerWriteTest()
{
	ifstream in("TeamColor00.blp");
	QVERIFY(in.is_open());
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

	// now read again
	image = QImage();
	QVERIFY(image.isNull());
	QVERIFY(handler.read(&image, blpImage));
}

void BlpTest::ioHandlerPalettedAlphaWriteTest()
{
	ifstream in("HumanCampaignCastle.blp", std::ios::in | std::ios::binary);
	QVERIFY(in.is_open());
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
	QVERIFY(blpImage.format() == blp::Blp::Format::Blp1);
	QVERIFY(blpImage.compression() == blp::Blp::Compression::Paletted);
	QVERIFY(blpImage.pictureType() == blp::Blp::PictureType::PalettedWithAlpha2);
	QVERIFY(blpImage.mipMaps().size() >= 1);
	QVERIFY(blpImage.mipMaps().front().width() == 512);
	QVERIFY(blpImage.mipMaps().front().height() == 512);
	const blp::color blpRgb = blpImage.palette()[blpImage.mipMaps().front().paletteIndexAt(0, 0)];
	const blp::byte blpAlpha =  blpImage.mipMaps().front().alphaAt(0, 0);
	const blp::color blpPixel = blpImage.mipMaps().front().colorAt(0, 0).paletteColor(blpImage.palette().get());

	BlpIOHandler handler;

	QImage image;
	QVERIFY(image.isNull());

	QVERIFY(handler.read(&image, blpImage));

	QVERIFY(!image.isNull());
	QVERIFY(image.size() == QSize(512, 512));
	QVERIFY(image.format() == QImage::Format_ARGB32);

	const blp::color qtPixel = argbToColor(image.pixel(0, 0));

	std::cerr << std::hex << "BLP RGB: " << blpRgb << std::endl;
	std::cerr << std::hex << "BLP Alpha: "<< blpAlpha << std::endl;
	std::cerr << std::hex << "BLP RGBA: "<< blpPixel << std::endl;
	std::cerr << std::hex << "Qt RGBA: "<< qtPixel << std::endl;
	std::cerr.unsetf(std::ios::hex);

	QVERIFY(blpPixel == qtPixel);
}

void BlpTest::writeTest()
{
	QImage image("DISBTNMagic.blp");
	QVERIFY(!image.isNull());
	const int byteCount = image.byteCount();

	QVERIFY(image.size() == QSize(64, 64));
	QVERIFY(image.format() == QImage::Format_ARGB32);

	QImageWriter writer("OutDISBTNMagic.blp");
	QVERIFY(writer.canWrite());
	QVERIFY(writer.write(image));

	QImageReader reader("OutDISBTNMagic.blp");
	QVERIFY(reader.canRead());
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
