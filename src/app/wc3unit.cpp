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

#include "../editor/objecteditor/uniteditor.hpp"
#include "../editor/objecteditor/objecttreemodel.hpp"

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
	source->sharedData()->sharedObjectData()->unitEditorData()->setSource(source.data());

	try
	{
		source->sharedData()->sharedObjectData()->unitEditorData()->load();
	}
	catch (wc3lib::Exception &e)
	{
		KMessageBox::error(0, e.what());
	}

	QScopedPointer<UnitData> unitData(new UnitData(source.data()));
	UnitEditor editor(source.data(), unitData.data(), 0);

	try
	{
		unitData->load(&editor);
		editor.treeModel()->load(source.data(), unitData.data(), &editor);
	}
	catch (wc3lib::Exception &e)
	{
		KMessageBox::error(0, e.what());
	}

	QMainWindow mainWindow;
	mainWindow.setCentralWidget(&editor);
	mainWindow.show();

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	if (args != 0)
	{
		for (int i = 0; i < args->count(); ++i)
		{
			// TODO import all objects into Unit Editor
		}
	}

	return app.exec();
}
