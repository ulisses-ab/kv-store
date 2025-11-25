#include "catch2.hpp"
#include "network/resp/RespParser.hpp"

using namespace std;

TEST_CASE("RespParserSimpleString", "[RespParser]") {
    RespParser parser;

    REQUIRE(!parser.parse_char('+').has_value());
    REQUIRE(!parser.parse_char('P').has_value());
    REQUIRE(!parser.parse_char('I').has_value());
    REQUIRE(!parser.parse_char('N').has_value());
    REQUIRE(!parser.parse_char('G').has_value());
    REQUIRE(!parser.parse_char('\r').has_value());
    REQUIRE(
        parser.parse_char('\n').value().encode() == 
        RespValue::simple_string("PING").encode()
    );
}

TEST_CASE("RespParserBulkString", "[RespParser]") {
    RespParser parser;

    REQUIRE(!parser.parse_char('$').has_value());
    REQUIRE(!parser.parse_char('4').has_value());
    REQUIRE(!parser.parse_char('\r').has_value());
    REQUIRE(!parser.parse_char('\n').has_value());
    REQUIRE(!parser.parse_char('P').has_value());
    REQUIRE(!parser.parse_char('I').has_value());
    REQUIRE(!parser.parse_char('N').has_value());
    REQUIRE(!parser.parse_char('G').has_value());
    REQUIRE(!parser.parse_char('\r').has_value());
    REQUIRE(
        parser.parse_char('\n').value().encode() == 
        RespValue::bulk_string("PING").encode()
    );
}

TEST_CASE("RespParserError", "[RespParser]") {
    RespParser parser;

    REQUIRE(!parser.parse_char('-').has_value());
    REQUIRE(!parser.parse_char('E').has_value());
    REQUIRE(!parser.parse_char('R').has_value());
    REQUIRE(!parser.parse_char('R').has_value());
    REQUIRE(!parser.parse_char('!').has_value());
    REQUIRE(!parser.parse_char('\r').has_value());

    auto val = parser.parse_char('\n');
    REQUIRE(val.has_value());
    REQUIRE(val->encode() == RespValue::error("ERR!").encode());
}

TEST_CASE("RespParserInteger", "[RespParser]") {
    RespParser parser;

    REQUIRE(!parser.parse_char(':').has_value());
    REQUIRE(!parser.parse_char('1').has_value());
    REQUIRE(!parser.parse_char('2').has_value());
    REQUIRE(!parser.parse_char('3').has_value());
    REQUIRE(!parser.parse_char('\r').has_value());

    auto val = parser.parse_char('\n');
    REQUIRE(val.has_value());
    REQUIRE(val->encode() == RespValue::integer(123).encode());
}

TEST_CASE("RespParserNullBulkString", "[RespParser]") {
    RespParser parser;

    REQUIRE(!parser.parse_char('$').has_value());
    REQUIRE(!parser.parse_char('-').has_value());
    REQUIRE(!parser.parse_char('1').has_value());
    REQUIRE(!parser.parse_char('\r').has_value());

    auto val = parser.parse_char('\n');
    REQUIRE(val.has_value());
    REQUIRE(val->encode() == RespValue::null_string().encode());
}

TEST_CASE("RespParserEmptyBulkString", "[RespParser]") {
    RespParser parser;

    const char* input = "$0\r\n\r\n";
    optional<RespValue> val;

    for (char c : string(input)) {
        val = parser.parse_char(c);
    }

    REQUIRE(val.has_value());
    REQUIRE(val->encode() == RespValue::bulk_string("").encode());
}

TEST_CASE("RespParserArrayOfIntegers", "[RespParser]") {
    RespParser parser;

    const char* input =
        "*3\r\n"
        ":1\r\n"
        ":2\r\n"
        ":3\r\n";

    optional<RespValue> val;
    for (char c : string(input)) val = parser.parse_char(c);

    REQUIRE(val.has_value());
    REQUIRE(val->encode() ==
        RespValue::array({
            RespValue::integer(1),
            RespValue::integer(2),
            RespValue::integer(3)
        }).encode());
}

TEST_CASE("RespParserNestedArrays", "[RespParser]") {
    RespParser parser;

    const char* input =
        "*2\r\n"
        "*2\r\n:1\r\n:2\r\n"
        "*1\r\n+OK\r\n";

    optional<RespValue> val;
    for (char c : string(input)) val = parser.parse_char(c);

    REQUIRE(val.has_value());
    REQUIRE(val->encode() ==
        RespValue::array({
            RespValue::array({ RespValue::integer(1), RespValue::integer(2) }),
            RespValue::array({ RespValue::simple_string("OK") })
        }).encode());
}

TEST_CASE("RespParserArrayWithNulls", "[RespParser]") {
    RespParser parser;

    const char* input =
        "*3\r\n"
        "$-1\r\n"
        ":5\r\n"
        "$0\r\n\r\n";

    optional<RespValue> val;
    for (char c : string(input)) val = parser.parse_char(c);

    REQUIRE(val.has_value());
    REQUIRE(val->encode() ==
        RespValue::array({
            RespValue::null_string(),
            RespValue::integer(5),
            RespValue::bulk_string("")
        }).encode());
}

TEST_CASE("RespParserNullArray", "[RespParser]") {
    RespParser parser;

    const char* input = "*-1\r\n";
    optional<RespValue> val;

    for (char c : string(input))
        val = parser.parse_char(c);

    REQUIRE(val.has_value());
    REQUIRE(val->encode() == RespValue::null_array().encode());
}

TEST_CASE("RespParserEmptyArray", "[RespParser]") {
    RespParser parser;

    const char* input = "*0\r\n";
    optional<RespValue> val;

    for (char c : string(input))
        val = parser.parse_char(c);

    REQUIRE(val.has_value());
    REQUIRE(val->encode() ==
        RespValue::array({}).encode());
}

TEST_CASE("RespParserInvalidType", "[RespParser]") {
    RespParser parser;

    REQUIRE_THROWS_AS(parser.parse_char('X'), runtime_error);
}
