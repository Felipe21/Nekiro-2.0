/**
 * The Forgotten Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2019  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FS_RSA_H
#define FS_RSA_H

#include <cryptopp/rsa.h>

class RSA
{
	public:
		RSA() = default;

		// non-copyable
		RSA(const RSA&) = delete;
		RSA& operator=(const RSA&) = delete;

		void loadPEM(const std::string& filename);
		void decrypt(char* msg) const;

	private:
		CryptoPP::RSA::PrivateKey pk;
};

#endif // FS_RSA_H
