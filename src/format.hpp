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

#include <istream>
#include <ostream>
#include <sstream>

#include <boost/serialization/serialization.hpp>
/*
#include <boost/archive/basic_archive.hpp>
#include <boost/archive/basic_binary_iarchive.hpp>
#include <boost/archive/basic_binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
*/
#include <boost/cstdint.hpp>

#include "exception.hpp"
#include "utilities.hpp"

namespace wc3lib
{

/// @brief Abstract class for formats.
template<typename _CharT>
class Format
{
	public:
		typedef _CharT CharType;
		typedef std::basic_istream<_CharT> InputStream;
		typedef std::basic_ostream<_CharT> OutputStream;

		virtual std::streamsize read(InputStream &istream) throw (class Exception) = 0;
		/// Reads input from another format object (\p other).
		std::streamsize read(const Format &other) throw (class Exception)
		{
			std::basic_stringstream<CharType> sstream;
			other.write(sstream);
			
			return read(sstream);
		}
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception) = 0;
		/// Writes output into another format object (\p other).
		std::streamsize write(Format &other) const throw (class Exception)
		{
			std::basic_stringstream<CharType> sstream;
			write(sstream);
			
			return other.read(sstream);
		}
		
		friend class boost::serialization::access;
		
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class _ArchiveT>
		void save(_ArchiveT &ar, const unsigned int version)
		{
			OutputStream stream;
			this >> stream;
			const std::size_t bufferSize = wc3lib::endPosition(stream) + 1;
			_CharT *buffer = new _CharT[bufferSize];
			stream.rdbuf()->sgetn(buffer, bufferSize);
			ar.save_binary(static_cast<const void*>(buffer), bufferSize);
			delete[] buffer;
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


		class Format& operator<<(InputStream &istream) throw (class Exception);
		const class Format& operator>>(OutputStream &ostream) const throw (class Exception);
		
		/**
		 * Version is required for Boost-like serialization. Most of Warcraft 3's formats do also have a version number.
		 */
		virtual uint32_t version() const = 0;
};

template<typename _CharT>
inline class Format<_CharT>& Format<_CharT>::operator<<(Format<_CharT>::InputStream &istream) throw (class Exception)
{
	this->read(istream);

	return *this;
}

template<typename _CharT>
inline const class Format<_CharT>& Format<_CharT>::operator>>(Format<_CharT>::OutputStream &ostream) const throw (class Exception)
{
	this->write(ostream);

	return *this;
}

template<typename _CharT>
inline std::basic_istream<_CharT>& operator>>(std::basic_istream<_CharT> &istream, class Format<_CharT> &format) throw (class Exception)
{
	format.read(istream);

	return istream;
}

template<typename _CharT>
inline std::basic_ostream<_CharT>& operator<<(std::basic_ostream<_CharT> &ostream, const class Format<_CharT> &format) throw (class Exception)
{
	format.write(ostream);

	return ostream;
}

}

#include "format.cpp"

#endif
