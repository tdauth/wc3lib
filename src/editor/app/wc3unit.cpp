/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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
#include <QMainWindow>
#include <QApplication>
#include <QMessageBox>

#include "../../editor.hpp"
#include "../../exception.hpp"

#include "../objecteditor/uniteditor.hpp"
#include "../objecteditor/objecttreemodel.hpp"

using namespace wc3lib::editor;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QScopedPointer<MpqPriorityList> source(new MpqPriorityList());
	source->sharedData()->sharedObjectData()->unitEditorData()->setSource(source.data());

	try
	{
		source->sharedData()->sharedObjectData()->unitEditorData()->load();
	}
	catch (const wc3lib::Exception &e)
	{
		QMessageBox::critical(0, QObject::tr("Error"), e.what());
	}

	QScopedPointer<UnitData> unitData(new UnitData(source.data()));
	UnitEditor editor(source.data(), unitData.data(), 0);

	try
	{
		unitData->load(&editor);
		editor.treeModel()->load(source.data(), unitData.data(), &editor);
	}
	catch (const wc3lib::Exception &e)
	{
		QMessageBox::critical(0, QObject::tr("Error"), e.what());
	}

	QMainWindow mainWindow;
	mainWindow.setCentralWidget(&editor);
	mainWindow.show();

	return app.exec();
}
