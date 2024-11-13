/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MDLX_MODEL_HPP
#define WC3LIB_MDLX_MODEL_HPP

#include "bounds.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDX tag "MODL".
 * MDL tag "Model".
 */
class Model : public Format
{
	public:
		static const std::size_t nameSize = 80;
		static const std::size_t animationFileNameSize = 260;

		Model();
		virtual ~Model();

		void setBounds(const Bounds &bounds);
		const Bounds& bounds() const;

		void setName(const string &name);
		void setName(const byte name[nameSize]);
		/**
		 * \return Returns name with size of \ref nameSize.
		 */
		const byte* name() const;
		/**
		 * \return Returns name with size of \ref animationFileNameSize.
		 */
		const byte* animationFileName() const;
		void setBlendTime(long32 blendTime);
		long32 blendTime() const;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

	protected:
		byte m_name[nameSize];
		byte m_animationFileName[animationFileNameSize];
		Bounds m_bounds;
		long32 m_blendTime;
};

inline void Model::setBounds(const Bounds& bounds)
{
	this->m_bounds = bounds;
}

inline const Bounds& Model::bounds() const
{
	return this->m_bounds;
}

inline void Model::setName(const string &name)
{
	if (name.size() + 1 > Model::nameSize)
	{
		throw Exception();
	}

	memset(this->m_name, 0, nameSize); // init name with 0 bytes
	memcpy(this->m_name, name.c_str(), name.size() + 1); // copy with 0 terminating string
}

inline void Model::setName(const byte name[Model::nameSize])
{
	memcpy(this->m_name, name, nameSize);
}

inline const byte* Model::name() const
{
	return this->m_name;
}

inline const byte* Model::animationFileName() const
{
	return this->m_animationFileName;
}

inline void Model::setBlendTime(long32 blendTime)
{
	this->m_blendTime = blendTime;
}

inline long32 Model::blendTime() const
{
	return this->m_blendTime;
}

}

}

#endif
