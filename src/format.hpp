/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_FORMAT_HPP
#define WC3LIB_FORMAT_HPP

/**
 * \defgroup io I/O
 * wc3lib supports I/O operations for most of Warcraft III's file formats.
 * Base class of all formats is \ref Format.
 */

#include <istream>
#include <ostream>
#include <sstream>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/scoped_array.hpp>
/*
#include <boost/archive/basic_archive.hpp>
#include <boost/archive/basic_binary_iarchive.hpp>
#include <boost/archive/basic_binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
*/
#include <boost/cstdint.hpp>

#include "config.h"
#include "exception.hpp"
#include "utilities.hpp"
#include "platform.hpp"

namespace wc3lib
{

/**
 * \brief Abstract class for serializable file formats which supports basic I/O operations "read" and "write".
 *
 * Provides some abstract functions and operators which allow you to serialize your formats and read from each other (>> and << operators).
 * Besides it implements functions \ref save() and \ref load() from Boost serialization library which allow you to use your formats with other Boost compliant libraries.
 *
 * \ingroup io
 */
class Format
{
	public:
		typedef byte CharType;
		typedef std::basic_istream<byte> InputStream;
		typedef std::basic_ostream<byte> OutputStream;

		virtual ~Format();

		/**
		 * \return Usually this function should returns the number of read bytes. For non-binary formats or formats which won't store any byte count this value can be 0. It has been introduced for convenience for example when reading chunks of the MDX format where you do have to know how many bytes are still left.
		 */
		virtual std::streamsize read(InputStream &istream) throw (class Exception) = 0;
		/// Reads input from another format object (\p other).
		// TODO stringstream is bad for binary data
		std::streamsize read(const Format &other) throw (class Exception)
		{
			std::stringstream stream;
			//boost::iostreams::basic_array<CharType> stream;
			other.write(stream);

			return read(stream);
		}

		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception) = 0;
		/// Writes output into another format object (\p other).
		// TODO stringstream is bad for binary data
		std::streamsize write(Format &other) const throw (class Exception)
		{
			std::stringstream stream;
			//boost::iostreams::basic_array<CharType> stream;
			write(stream);

			return other.read(stream);
		}

		friend class boost::serialization::access;

		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class _ArchiveT>
		void save(_ArchiveT &ar, const unsigned int version) const
		{
			/*
			std::basic_ostringstream<_CharT> stream; // use buffered stream!
			*this >> stream;
			const std::size_t bufferSize = wc3lib::endPosition(stream) + 1;
			boost::scoped_array<_CharT> buffer(new _CharT[bufferSize]);
			stream.rdbuf()->sgetn(buffer.get(), bufferSize);
			ar.save_binary(static_cast<const void*>(buffer.get()), bufferSize);
			*/
			//boost::archive::basic_binary_oarchive<_ArchiveT>
			//boost::basic_a
			//boost::archive::basic_binary_oarchive::
		}

		template<class _ArchiveT>
		void load(_ArchiveT &ar, const unsigned int version)
		{
			/*
			 * TODO how to get stream of archive \p ar.
			 * Read from archive stream into object by using << operator.
			boost::archive::binary_iarchive::
			OutputStream stream;
			this >> stream;
			const std::size_t bufferSize = wc3lib::endPosition(stream) + 1;
			_CharT *buffer = new _CharT[bufferSize];
			stream.rdbuf()->sgetn(buffer, bufferSize);
			ar.save_binary(static_cast<const void*>(buffer), bufferSize);
			delete[] buffer;
			*/
		}

		template<class _ArchiveT>
		void save(_ArchiveT &ar) { save(ar, version()); }
		template<class _ArchiveT>
		void load(_ArchiveT &ar) { load(ar, version()); }

		BOOST_SERIALIZATION_SPLIT_MEMBER()


		Format& operator<<(InputStream &istream) throw (Exception);
		const Format& operator>>(OutputStream &ostream) const throw (Exception);

		/**
		 * Version is required for Boost-like serialization. Most of Warcraft III's formats do also have a version number.
		 */
		virtual uint32_t version() const
		{
			return 0;
		}
};

inline Format& Format::operator<<(InputStream &istream) throw (Exception)
{
	this->read(istream);

	return *this;
}

inline const Format& Format::operator>>(OutputStream &ostream) const throw (Exception)
{
	this->write(ostream);

	return *this;
}

inline Format::InputStream& operator>>(Format::InputStream &istream, Format &format) throw (Exception)
{
	format.read(istream);

	return istream;
}

inline Format::OutputStream& operator<<(Format::OutputStream &ostream, const Format &format) throw (Exception)
{
	format.write(ostream);

	return ostream;
}

}

#endif
