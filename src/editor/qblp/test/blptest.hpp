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

#ifndef WC3LIB_EDITOR_QBLP_BLPTEST_HPP
#define WC3LIB_EDITOR_QBLP_BLPTEST_HPP

#include <QObject>
#include <QPluginLoader>

#include "../blpiohandler.hpp"
#include "../blpioplugin.hpp"

namespace wc3lib
{

namespace editor
{

class BlpTest : public QObject
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanupTestCase();

		void init();
		void cleanup();

		void supportedTest();
		void ioHandlerReadTest();
		void ioHandlerWriteTest();
		void ioHandlerPalettedAlphaWriteTest();

		/**
		 * These test cases use the installed qblp plugin and test if the plugin
		 * is integrated and loaded automatically.
		 * They use QImageReader and QImageWriter which automatically checks for all supported image formats
		 * of Qt.
		 *
		 * \todo the plugin should be loaded from the built file not from any installed file
		 *
		 * @{
		 */
		void writeTest();
		void readTest();
		/**
		 * @}
		 */

	private:
		QPluginLoader *m_loader;
		BlpIOPlugin *m_plugin;
};

}

}

#endif // BLPTEST_H
