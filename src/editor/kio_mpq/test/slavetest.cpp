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

#include "slavetest.hpp"
//#include "blptestConfig.h"

//#ifndef QBLP_ABSOLUTE_PATH
//#error Define QBLP_ABSOLUTE_PATH! // has to contain the absolute file path of the plugin
//#endif

#include <QtGui>
#include <QtTest>

#include <KIO/Scheduler>
#include <KIO/JobClasses>
#include <KService>

#include "kiotestConfig.h"

QTEST_MAIN(wc3lib::editor::SlaveTest);

namespace wc3lib
{

namespace editor
{

void SlaveTest::initTestCase()
{
	//KConfig::set

	// Called before the first testfunction is executed
	m_loader = new QPluginLoader(KIO_MPQ_ABSOLUTE_PATH, this);
	const bool result = m_loader->load();
	qDebug() << "Error:" << m_loader->errorString();
	QVERIFY(result);
	QVERIFY(m_loader->isLoaded());
}

void SlaveTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
}

void SlaveTest::init()
{
    // Called before each testfunction is executed
}

void SlaveTest::cleanup()
{
    // Called after every testfunction
}

void SlaveTest::downloadTest()
{
	KUrl url = QDir::currentPath();
	url.setProtocol("mpq");
	url.addPath("testattributes.mpq");
	url.addPath("test.txt");

	QVERIFY(url.isValid());

	//m_protocol.listDir();

	//KIO::TransferJob *job = KIO::get(url);
	//connect(job, SIGNAL(
	//connect(job, SIGNAL(data(KIO::Job*,QByteArray)), this, SLOT
	QString file;

	const bool success = KIO::NetAccess::download(url, file, 0);

	QVERIFY(success);
	QVERIFY(QFile::exists(file));


	//QCOMPARE(byteCount, image.byteCount()); // new byte count == old byte count
}

}

}
