// Copyright (c) 2010 ANH Studios. All rights reserved.
// Use of this source code is governed by a GPL-style license that can be
// found in the COPYING file.

#include "anh/hash_string.h"

#include <map>
#include <gtest/gtest.h>

using anh::HashString;

// Wrapping tests in an anonymous namespace prevents potential name conflicts.
namespace {

/// This test shows how to create a hash string and check it's generated identifier.
TEST(HashStringTests, CreatingEventTypeFromStringGeneratesUniqueIdentifier) {
    HashString hash_string("test_hash_string");
    
    EXPECT_EQ(uint32_t(0x107D0089), hash_string.ident()) << "HashString did not create the expected identifier";
}

/// This test shows how to get the string value back from the hash.
TEST(HashStringTests, CanRetrieveStringNameBackFromHashString) {
    HashString hash_string("test_hash_string");
    
    EXPECT_EQ("test_hash_string", hash_string.ident_string()) << "HashString did not store the string identifier correctly";
}

/// This test shows how to compare two hash strings.
TEST(HashStringTests, CanCompareTwoHashStrings) {
    HashString hash_string1("test_string1");
    HashString hash_string2("another_hash_string");
    HashString hash_string3("another_hash_string");

    EXPECT_EQ(true, (hash_string1 != hash_string2)) << "hash_string1 and hash_string2 should not match."; 
    EXPECT_EQ(true, (hash_string2 == hash_string3)) << "hash_string2 and hash_string3 should match";
}

/// This test shows how to use HashString as a key in std containers.
TEST(HashStringTests, CanUseHashStringAsContainerKey) {
    // Create a simple std::map<HashString, int> map.
    std::map<HashString, int> map;

    // Seed the map with some entries for testing.
    map.insert(std::pair<HashString, int>(HashString("my_key1"), 2000));
    map.insert(std::pair<HashString, int>(HashString("my_key2"), 9001));
    map.insert(std::pair<HashString, int>(HashString("my_key3"), 400));

    // Now make sure we can access the map appropriately.
    EXPECT_EQ(2000, map[HashString("my_key1")]);
    EXPECT_EQ(9001, map[HashString("my_key2")]);
    EXPECT_EQ(400, map[HashString("my_key3")]);

    // Try searching the map by the key and make sure it returns the correct result.
    std::map<HashString, int>::const_iterator it = map.find(HashString("my_key1"));
    EXPECT_EQ(2000, it->second);
}

}  // namespace
