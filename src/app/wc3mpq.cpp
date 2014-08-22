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

#include "../editor.hpp"

#include "../editor/mpqeditor/mpqeditor.hpp"

using namespace wc3lib::editor;

int main(int argc, char *argv[])
{
	KAboutData aboutData(Editor::aboutData());

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineOptions options;
	options.add("+[file]", ki18n("File to open"));
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication app;

	QScopedPointer<MpqPriorityList> source(new MpqPriorityList());
	MpqEditor editor(source.data());
	editor.show();

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	// TODO determine by extension what should be open!
	if (args != 0)
	{
		//for (int i = 0; i < args->count(); ++i)
		//	editor->openTriggersUrl(args->url(i));
	}

	return app.exec();
}
