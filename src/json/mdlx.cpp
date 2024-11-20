/***************************************************************************
 *   Copyright (C) 2024 by Tamino Dauth                                    *
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

#include "mdlx.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace json
{

Mdlx::Mdlx(mdlx::Mdlx *model) : m_model(model)
{
}

Mdlx::~Mdlx()
{
}

std::streamsize Mdlx::read(InputStream &istream)
{
	return 0;
}

std::streamsize Mdlx::write(OutputStream &ostream) const
{
	writeJsonKeyObjectStart(ostream);
    writeJsonKeyObjectStart(ostream, "Model");
	writeJsonKeyValue(ostream, "Name", m_model->model().name());
	writeJsonKeyValue(ostream, "AnimationFileName", m_model->model().animationFileName());
	writeJsonKeyValue(ostream, "BlendTime", m_model->model().blendTime());
    writeJsonKeyValue(ostream, m_model->model().bounds());
    writeJsonKeyObjectStart(ostream, "MinimumExtent");
    writeJsonKeyObjectEnd(ostream);
    writeJsonKeyObjectEnd(ostream);

    return 0;
}

}

}