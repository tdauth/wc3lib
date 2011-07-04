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

#include "mdxblock.hpp"
#include "bounds.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDX tag "MODL".
 * MDL tag "Model".
 */
class Model : public MdxBlock, public Bounds
{
	public:
		static const std::size_t nameSize = 80;
		static const std::size_t animationFileNameSize = 260;
		
		Model(class Mdlx *mdlx);
		virtual ~Model();

		class Mdlx* mdlx() const;
		/**
		 * \return Returns name with size of \ref nameSize.
		 */
		const ascii* name() const;
		/**
		 * \returns Returns animation file name with size of \ref animationFileNameSize.
		 */
		const ascii* animationFileName() const;
		long32 blendTime() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		class Mdlx *m_mdlx;
		//long nbytes;
		ascii m_name[nameSize];
		ascii m_animationFileName[animationFileNameSize];
		long32 m_blendTime;
};

inline class Mdlx* Model::mdlx() const
{
	return this->m_mdlx;
}

inline const ascii* Model::name() const
{
	return this->m_name;
}

inline const ascii* Model::animationFileName() const
{
	return this->m_animationFileName;
}

inline long32 Model::blendTime() const
{
	return this->m_blendTime;
}

}

}

#endif
