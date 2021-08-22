#include <czt/test_base.hpp>

#include <cz/buffer_array.hpp>
#include <cz/panic_allocator.hpp>
#include "xml.hpp"

static void expect_text_token(cz::Str expected, xml::Token token) {
    REQUIRE(token.tag == xml::Token::Text);
    CHECK(token.v.text == expected);
}

static void expect_tag_token(xml::Tag_Token::Type etype,
                             cz::Str ename,
                             cz::Slice<const xml::Tag_Token::Pair> epairs,
                             xml::Token token) {
    REQUIRE(token.tag == xml::Token::Tag);
    xml::Tag_Token tag = token.v.tag;
    CHECK(etype == tag.type);
    CHECK(ename == tag.name);
    REQUIRE(epairs.len == tag.pairs.len());
    for (size_t i = 0; i < epairs.len; ++i) {
        CHECK(epairs[i].key == tag.pairs[i].key);
        CHECK(epairs[i].value == tag.pairs[i].value);
    }
}

TEST_CASE("parse text token") {
    size_t index = 0;
    cz::Str string = "hello world";
    cz::Allocator allocator = cz::panic_allocator();
    xml::Token token;
    REQUIRE(xml::next_token(&index, string, cz::heap_allocator(), &token) == xml::Success);
    expect_text_token("hello world", token);

    REQUIRE(xml::next_token(&index, string, cz::heap_allocator(), &token) == xml::EndOfFile);
}

TEST_CASE("parse text token skip whitespace") {
    size_t index = 0;
    cz::Str string = "  hello world  ";
    cz::Allocator allocator = cz::panic_allocator();
    xml::Token token;
    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::Success);
    expect_text_token("hello world", token);

    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::EndOfFile);
}

TEST_CASE("parse open token unterminated") {
    size_t index = 0;
    cz::Str string = "<tag ";
    cz::Allocator allocator = cz::panic_allocator();
    xml::Token token;
    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::UnclosedTag);

    CHECK(string.len == index);
}

TEST_CASE("parse open token no pairs") {
    size_t index = 0;
    cz::Str string = "<tag>";
    cz::Allocator allocator = cz::panic_allocator();
    xml::Token token;
    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::Success);
    expect_tag_token(xml::Tag_Token::Open, "tag", {}, token);

    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::EndOfFile);
}

TEST_CASE("parse open token 1 simple k/v dq") {
    cz::Buffer_Array ba;
    ba.init();
    CZ_DEFER(ba.drop());

    size_t index = 0;
    cz::Str string = "<tag k=\"v\">";
    cz::Allocator allocator = ba.allocator();
    xml::Token token;
    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::Success);
    const xml::Tag_Token::Pair pairs[] = {{"k", "v"}};
    expect_tag_token(xml::Tag_Token::Open, "tag", pairs, token);

    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::EndOfFile);
}

TEST_CASE("parse open token 1 simple k/v sq") {
    cz::Buffer_Array ba;
    ba.init();
    CZ_DEFER(ba.drop());

    size_t index = 0;
    cz::Str string = "<tag k='v'>";
    cz::Allocator allocator = ba.allocator();
    xml::Token token;
    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::Success);
    const xml::Tag_Token::Pair pairs[] = {{"k", "v"}};
    expect_tag_token(xml::Tag_Token::Open, "tag", pairs, token);

    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::EndOfFile);
}

TEST_CASE("parse open token 2 simple k/v") {
    cz::Buffer_Array ba;
    ba.init();
    CZ_DEFER(ba.drop());

    size_t index = 0;
    cz::Str string = "<tag k1='v1' k2='v2'>";
    cz::Allocator allocator = ba.allocator();
    xml::Token token;
    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::Success);
    const xml::Tag_Token::Pair pairs[] = {{"k1", "v1"}, {"k2", "v2"}};
    expect_tag_token(xml::Tag_Token::Open, "tag", pairs, token);

    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::EndOfFile);
}

TEST_CASE("parse close token") {
    cz::Buffer_Array ba;
    ba.init();
    CZ_DEFER(ba.drop());

    size_t index = 0;
    cz::Str string = "</tag>";
    cz::Allocator allocator = ba.allocator();
    xml::Token token;
    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::Success);
    expect_tag_token(xml::Tag_Token::Close, "tag", {}, token);

    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::EndOfFile);
}

TEST_CASE("parse selfclose token") {
    cz::Buffer_Array ba;
    ba.init();
    CZ_DEFER(ba.drop());

    size_t index = 0;
    cz::Str string = "<tag/>";
    cz::Allocator allocator = ba.allocator();
    xml::Token token;
    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::Success);
    expect_tag_token(xml::Tag_Token::SelfClose, "tag", {}, token);

    REQUIRE(xml::next_token(&index, string, allocator, &token) == xml::EndOfFile);
}
