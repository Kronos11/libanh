// Copyright (c) 2010 ANH Studios. All rights reserved.
// Use of this source code is governed by a GPL-style license that can be
// found in the COPYING file.

#ifndef ANH_CRC_H_
#define ANH_CRC_H_

#include <cstdint>
#include <string>

/// The anh namespace hosts a number of useful utility classes intended
/// to be used and reused in domain specific classes.
namespace anh {

/**
 * Calculates a 32-bit checksum of a c-style string.
 *
 * Variation on the memcrc function which is part of the cksum utility.
 *
 * \see http://www.opengroup.org/onlinepubs/009695399/utilities/cksum.html
 *
 * \param source_string The string to use as the basis for generating the checksum.
 * \param length The length of the source_string.
 * \returns A 32-bit checksum of the string.
 */
uint32_t memcrc(char const * const source_string, uint32_t length);


/**
 * Calculates a 32-bit checksum of a std::string.
 *
 * Variation on the memcrc function which is part of the cksum utility.
 *
 * \see http://www.opengroup.org/onlinepubs/009695399/utilities/cksum.html
 *
 * \param source_string The string to use as the basis for generating the checksum.
 * \returns A 32-bit checksum of the string.
 */
uint32_t memcrc(const std::string& source_string);

}  // namespace anh

#endif  // ANH_CRC_H_
