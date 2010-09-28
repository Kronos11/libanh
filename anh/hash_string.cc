// Copyright (c) 2010 ANH Studios. All rights reserved.
// Use of this source code is governed by a GPL-style license that can be
// found in the COPYING file.

#include "anh/hash_string.h"

#include <cstring>

#include "anh/memcrc.h"

/// The anh namespace hosts a number of useful utility classes intended
/// to be used and reused in domain specific classes.
namespace anh {

std::ostream& operator<<(std::ostream& message, const HashString& string) {
    message << string.ident_string();
    return message;
}

HashString::HashString(char const * const ident_string)
    : ident_(reinterpret_cast<void*>(0))
    , ident_string_(ident_string) {
    if (ident_string) {
        ident_ = reinterpret_cast<void*>(memcrc(ident_string, strlen(ident_string)));
    }
}

HashString::~HashString() {}

bool HashString::operator<(const HashString& other) const {
    bool r = (ident() < other.ident());
    return r;
}

bool HashString::operator>(const HashString& other) const {
    bool r = (ident() > other.ident());
    return r;
}

bool HashString::operator==(const HashString& other) const {
    bool r = (ident() == other.ident());
    return r;
}

bool HashString::operator!=(const HashString& other) const {
    bool r = (ident() != other.ident());
    return r;
}

uint32_t HashString::ident() const {
    return static_cast<uint32_t>(reinterpret_cast<uint64_t>(ident_));
}

const std::string& HashString::ident_string() const {
    return ident_string_;
}

}  // namespace anh
