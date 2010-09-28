// Copyright (c) 2010 ANH Studios. All rights reserved.
// Use of this source code is governed by a GPL-style license that can be
// found in the COPYING file.

#include "anh/memcrc.h"

#include <gtest/gtest.h>

// Wrapping tests in an anonymous namespace prevents potential name conflicts.
namespace {
    
/// This test shows how to find the 32bit checksum of a c-style string.
TEST(CrcTests, CanCrcCstyleStrings) {
    EXPECT_EQ(uint32_t(0x338BCFAC), anh::memcrc("test"));
    EXPECT_EQ(uint32_t(0x2643D57C), anh::memcrc("anothertest"));
    EXPECT_EQ(uint32_t(0x19522193), anh::memcrc("aThirdTest"));
}

/// This test shows how to find the 32bit checksum of a std::string.
TEST(CrcTests, CanCrcStdStrings) {
    EXPECT_EQ(uint32_t(0x338BCFAC), anh::memcrc(std::string("test")));
    EXPECT_EQ(uint32_t(0x2643D57C), anh::memcrc(std::string("anothertest")));
    EXPECT_EQ(uint32_t(0x19522193), anh::memcrc(std::string("aThirdTest")));
}

}  // namespace
