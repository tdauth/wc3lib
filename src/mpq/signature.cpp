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

#include "signature.hpp"

namespace wc3lib
{

namespace mpq
{

#ifdef USE_ENCRYPTION
bool Signature::check(const CryptoPP::RSA::PrivateKey &privateKey) const
{

}

void Signature::sign(WeakSignature &signature, const CryptoPP::RSA::PublicKey &publicKey) const
{
	// encrypting digest
	CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
	CryptoPP::RandomNumberGenerator rng;
	MD5 checksum = this->checksum();
	signature.reset(new byte[size]);
	encryptor.Encrypt(rng, (unsigned char*)&checksum, sizeof(checksum), (unsigned char*)signature.get());
	/// TODO set before parameters (512 bit RSA key) or provide key pair generation functionality in algorithm.hpp?
}

std::streamsize Signature::sign(const CryptoPP::RSA::PublicKey &publicKey)
{
	WeakSignature signature;
	this->sign(signature, publicKey);
	const std::size_t dataSize = size + 2 * sizeof(int32);
	boost::scoped_array<byte> data(new byte[dataSize]);
	memset(data.get(), 0, 8); // set first two int32 values to 0
	memcpy(&data[8], signature.get(), size);
	signature.reset(); // free already copied signature
	iarraystream istream(data.get(), dataSize);

	return this->readData(istream);
}

MD5 Signature::checksum() const
{
	// TODO implement similar to \ref Mpq::digest() when it's finished
	return 0;
}

MD5 Signature::storedChecksum(const CryptoPP::RSA::PrivateKey &privateKey) const
{
	arraystream stream;
	std::streamsize bufferSize = this->writeData(stream);
	byte *buffer;
	stream >> buffer; // TODO Don't copy!
	// decrypting digest
	CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(privateKey);
	CryptoPP::RandomNumberGenerator rng;
	MD5 result;
	decryptor.Decrypt(rng, (unsigned char*)&buffer[8], bufferSize - 8, (unsigned char*)&result); // [8] because we skip the first two 0 int32 values

	return result;
}
#endif

Signature::Signature(Mpq* mpq, Hash* hash): MpqFile(mpq, hash)
{
	this->m_path = fileName();
}

}

}
