/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MPQ_SIGNATURE_HPP
#define WC3LIB_MPQ_SIGNATURE_HPP

#include "platform.hpp"
#include "file.hpp"

namespace wc3lib
{

namespace mpq
{

/**
 * Provides access to the MPQ archive's weak digital signature file ("(signature)") which is an RSA 512-bit encrypted MD5 checksum created by all of the archive's data (including its header etc.).
 * For more relyable signing you could use a strong digital signature as well which is simply appended to the MPQ archive with the leading indicating characters 'NGIS' and contains an RSA 2048-bit encrypted SHA1 digest.
 * \sa Archive:strongDigitalSignature()
 * \sa Listfile
 * \sa Attributes
 */
class Signature : public File
{
	public:
		/// \note Doesn't include any header data.
		static const std::size_t size = 64;
		/**
		 * Array with size of \ref size.
		 * \note Doesn't include any header data.
		 */
		typedef boost::scoped_array<byte> WeakSignature;

		Signature();

		virtual const char* fileName() const;

	protected:
		friend Archive;

		Signature(Archive *mpq, Hash *hash);
};

inline const char* Signature::fileName() const
{
	return "(signature)";
}

}

}

#endif
