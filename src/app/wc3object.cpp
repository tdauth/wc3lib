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

#include <QScopedPointer>

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KMessageBox>

#include "../editor.hpp"
#include "../exception.hpp"

#include <editor/objecteditor/objecteditor.hpp>

using namespace wc3lib::editor;

int main(int argc, char *argv[])
{
	KAboutData aboutData(Editor::aboutData());

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineOptions options;
	options.add("", ki18n("Additional help."));
	options.add("+[file]", ki18n("File to open"));
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication app;

	QScopedPointer<MpqPriorityList> source(new MpqPriorityList());
	/*
	 * The independent editor uses its own stored settings for its sources!
	 */
	source->readSettings("objecteditor"); // TODO use getter function

	try
	{
		source->sharedData()->refreshDefaultFiles(0); // TODO we need a GUI
	}
	catch (wc3lib::Exception &e)
	{
		KMessageBox::error(0, i18n("Error when loading default files: %1", e.what()));
	}

	ObjectEditor editor(source.data());
	editor.show();

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	if (args != 0)
	{
		for (int i = 0; i < args->count(); ++i)
		{
			editor.importAll(args->url(i));
		}
	}

	return app.exec();
}
