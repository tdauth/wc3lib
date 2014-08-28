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

#include <KMessageBox>

#include "unitmetadata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"


namespace wc3lib
{

namespace editor
{

UnitMetaData::UnitMetaData(MpqPriorityList *source) : m_source(source)
{

}

void UnitMetaData::load(QWidget *widget)
{
	this->m_unitData = new MetaData(KUrl("Units/UnitData.slk"));
	this->m_unitData->setSource(this->source());

	try
	{
		this->m_unitData->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitData->url().toEncoded().constData(), e.what()));
	}

	this->m_unitUi = new MetaData(KUrl("Units/unitUI.slk"));
	m_unitUi->setSource(this->source());

	try
	{
		this->m_unitUi->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitUi->url().toEncoded().constData(), e.what()));
	}

	this->m_unitBalance = new MetaData(KUrl("Units/UnitBalance.slk"));
	m_unitBalance->setSource(this->source());

	try
	{
		this->m_unitBalance->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitBalance->url().toEncoded().constData(), e.what()));
	}

	this->m_humanUnitStrings = new MetaData(KUrl("Units/HumanUnitStrings.txt"));
	m_humanUnitStrings->setSource(this->source());

	try
	{
		this->m_humanUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_humanUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_humanUnitFunc = new MetaData(KUrl("Units/HumanUnitFunc.txt"));
	this->m_humanUnitFunc->setSource(this->source());

	try
	{
		this->m_humanUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_humanUnitFunc->url().toEncoded().constData(), e.what()));
	}

	this->m_orcUnitStrings = new MetaData(KUrl("Units/OrcUnitStrings.txt"));
	m_orcUnitStrings->setSource(this->source());

	try
	{
		this->m_orcUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_orcUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_orcUnitFunc = new MetaData(KUrl("Units/OrcUnitFunc.txt"));
	m_orcUnitFunc->setSource(this->source());

	try
	{
		this->m_orcUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_orcUnitFunc->url().toEncoded().constData(), e.what()));
	}

	this->m_undeadUnitStrings = new MetaData(KUrl("Units/UndeadUnitStrings.txt"));
	m_undeadUnitStrings->setSource(this->source());

	try
	{
		this->m_undeadUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_undeadUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_undeadUnitFunc = new MetaData(KUrl("Units/UndeadUnitFunc.txt"));
	m_undeadUnitFunc->setSource(this->source());

	try
	{
		this->m_undeadUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_undeadUnitFunc->url().toEncoded().constData(), e.what()));
	}

	this->m_nightElfUnitStrings = new MetaData(KUrl("Units/NightElfUnitStrings.txt"));
	m_nightElfUnitStrings->setSource(this->source());

	try
	{
		this->m_nightElfUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_nightElfUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_nightElfUnitFunc = new MetaData(KUrl("Units/NightElfUnitFunc.txt"));
	m_nightElfUnitFunc->setSource(this->source());

	try
	{
		this->m_nightElfUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_nightElfUnitFunc->url().toEncoded().constData(), e.what()));
	}
}
}

}
