/***************************************************************************
 *   Copyright (C) 2022 by Tamino Dauth                                    *
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

#include <QtCore>
#include <QtWidgets/QtWidgets>

#include "ogreterrain.hpp"
#include "modelview.hpp"
#include "texture.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

OgreTerrain::OgreTerrain(const QUrl &url, ModelView *modelView) : Resource(url, Resource::Type::Model), m_modelView(modelView), m_sceneNode(0)
{
}

OgreTerrain::~OgreTerrain()
{
	clear();
}

void OgreTerrain::clear() throw ()
{
	m_environment.reset();
}

void OgreTerrain::load()
{
	QString tmpFile;

	if (!source()->download(url(), tmpFile, modelView()))
	{
		throw Exception(boost::format(_("Unable to download file from URL \"%1%\".")) % url().toLocalFile().toUtf8().constData());
	}

	EnvironmentPtr environment(new map::Environment());
	std::ios_base::openmode openmode = std::ios_base::in | std::ios_base::binary;
	ifstream ifstream(tmpFile.toStdString(), openmode);
	environment->read(ifstream);

	this->source()->removeTempFile(tmpFile);
    
	this->m_environment.swap(environment);
    
    // TODO Build up an OGRE scene with the terrain.
}

void OgreTerrain::reload()
{
	clear();
	load();
}

void OgreTerrain::save(const QUrl &url, const QString &format) const
{
	QString realFormat = format;

	if (realFormat.isEmpty())
	{
		QString extension(QFileInfo(url.toLocalFile()).suffix().toLower());

		qDebug() << "Extension: " << QFileInfo(url.toLocalFile()).suffix().toLower();
	}

	if (realFormat == "w3e")
	{
		QTemporaryFile tmpFile;

		if (!tmpFile.open()) // creates file
		{
			throw Exception(_("Unable to create temporary file."));
		}

		std::ios_base::openmode openmode = std::ios_base::out | std::ios_base::binary;
		ofstream ofstream(tmpFile.fileName().toStdString(), openmode);

		if (!ofstream)
		{
			throw Exception(boost::format(_("Error when opening file \"%1%\".")) % tmpFile.fileName().toStdString());
		}
		
		this->environment()->write(ofstream);

		if (!source()->upload(tmpFile.fileName(), url, modelView()))
		{
			throw Exception(boost::format(_("Error while uploading file \"%1%\" to destination \"%2%\".")) % tmpFile.fileName().toStdString() % url.toEncoded().constData());
		}

		QMessageBox::information(modelView(), QObject::tr("Done"), QObject::tr("Wrote file \"%2\" successfully.").arg(url.toEncoded().constData()));
	}
	else
	{
		throw Exception(boost::format(_("Format \"%1%\" is not supported.")) % realFormat.toStdString());
	}
}

}

}
