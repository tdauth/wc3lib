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

#include <QtGui>

#include <KFileDialog>
#include <KMessageBox>

#include "weathereditor.hpp"
#include "weathertreemodel.hpp"
#include "weatherdata.hpp"

namespace wc3lib
{

namespace editor
{

WeatherEditor::WeatherEditor(MpqPriorityList* source, QWidget* parent, Qt::WindowFlags f) : ObjectEditorTab(source, new WeatherData(source), parent, f)
{
	setupUi();
}

WeatherEditor::~WeatherEditor()
{
}

void WeatherEditor::onSwitchToMap(Map *map)
{
}

void WeatherEditor::onNewObject()
{
}

void WeatherEditor::onRenameObject()
{
}

void WeatherEditor::onDeleteObject()
{
}

void WeatherEditor::onResetObject()
{
}

void WeatherEditor::onResetAllObjects()
{
}

void WeatherEditor::onExportAllObjects()
{
	const QString slkSuffix = "slk";

	const KUrl url = KFileDialog::getSaveUrl(KUrl(), QString("*\nSLK *.%1").arg(slkSuffix), this, exportAllObjectsText());

	if (!url.isEmpty())
	{
		ofstream out(url.toLocalFile().toUtf8().constData());

		if (out)
		{
			try
			{
				const ObjectData::Slks slks = this->objectData()->slks();

				BOOST_FOREACH(ObjectData::Slks::const_reference ref, slks)
				{
					ref.write(out);
				}
			}
			catch (Exception &e)
			{
				KMessageBox::error(this, tr("Error on exporting"), e.what());
			}
		}
	}
}

void WeatherEditor::onImportAllObjects()
{
}

void WeatherEditor::onCopyObject()
{
}

void WeatherEditor::onPasteObject()
{
}

void WeatherEditor::onShowRawData(bool show)
{
}

void WeatherEditor::activateFolder(ObjectTreeItem* item)
{
}

void WeatherEditor::activateObject(ObjectTreeItem* item)
{
}

ObjectTreeModel* WeatherEditor::createTreeModel()
{
	return new WeatherTreeModel(this->source(), this);
}

WeatherData* WeatherEditor::weatherData() const
{
	return boost::polymorphic_cast<WeatherData*>(objectData());
}

QString WeatherEditor::name() const
{
	return tr("Weather Editor");
}

}

}
