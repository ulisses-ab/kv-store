#include "catch2.hpp"
#include "network/resp/RespValue.hpp"

TEST_CASE("RespValueSimpleString", "[RespValue]") {
    RespValue resp_value = RespValue::simple_string("abcd");

    REQUIRE(resp_value.is(RespType::SimpleString));
    REQUIRE(resp_value.is_string());
    REQUIRE(resp_value.type() == RespType::SimpleString);

    REQUIRE(resp_value.as_string() == "abcd");
    REQUIRE_THROWS(resp_value.as_integer());
    REQUIRE_THROWS(resp_value.as_array());

    REQUIRE(resp_value.encode() == "+abcd\r\n");
    REQUIRE(resp_value.display() == "\"abcd\"");
}

TEST_CASE("RespValueBulkString", "[RespValue]") {
    RespValue resp_value = RespValue::bulk_string("abcd");

    REQUIRE(resp_value.is(RespType::BulkString));
    REQUIRE(resp_value.is_string());
    REQUIRE(resp_value.type() == RespType::BulkString);

    REQUIRE(resp_value.as_string() == "abcd");
    REQUIRE_THROWS(resp_value.as_integer());
    REQUIRE_THROWS(resp_value.as_array());

    REQUIRE(resp_value.encode() == "$4\r\nabcd\r\n");
    REQUIRE(resp_value.display() == "\"abcd\"");
}

TEST_CASE("RespValueError", "[RespValue]") {
    RespValue resp_value = RespValue::error("error");

    REQUIRE(resp_value.is(RespType::Error));
    REQUIRE(!resp_value.is_string());
    REQUIRE(resp_value.type() == RespType::Error);

    REQUIRE(resp_value.as_string() == "error");
    REQUIRE_THROWS(resp_value.as_integer());
    REQUIRE_THROWS(resp_value.as_array());

    REQUIRE(resp_value.encode() == "-error\r\n");
    REQUIRE(resp_value.display() == "error(\"error\")");
}

TEST_CASE("RespValueInteger", "[RespValue]") {
    RespValue resp_value = RespValue::integer(123);

    REQUIRE(resp_value.is(RespType::Integer));
    REQUIRE(!resp_value.is_string());
    REQUIRE(resp_value.type() == RespType::Integer);

    REQUIRE(resp_value.as_integer() == 123);
    REQUIRE_THROWS(resp_value.as_string());
    REQUIRE_THROWS(resp_value.as_array());

    REQUIRE(resp_value.encode() == ":123\r\n");
    REQUIRE(resp_value.display() == "123");
}

TEST_CASE("RespValueArray", "[RespValue]") {
    std::vector<RespValue> values = {RespValue::simple_string("a"), RespValue::integer(123)};


    RespValue resp_value = RespValue::array(values);

    REQUIRE(resp_value.is(RespType::Array));
    REQUIRE(!resp_value.is_string());
    REQUIRE(resp_value.type() == RespType::Array);

    std::vector<RespValue> output = resp_value.as_array();
    for(size_t i = 0; i < output.size(); i++) {
        REQUIRE(output[i].encode() == values[i].encode());
    }
    REQUIRE_THROWS(resp_value.as_string());
    REQUIRE_THROWS(resp_value.as_integer());

    REQUIRE(resp_value.encode() == "*2\r\n+a\r\n:123\r\n");
    REQUIRE(resp_value.display() == "[\"a\", 123]");
}

TEST_CASE("RespValueNullString", "[RespValue]") {
    RespValue resp_value = RespValue::null_string();

    REQUIRE(resp_value.is(RespType::NullString));
    REQUIRE(!resp_value.is_string());
    REQUIRE(resp_value.type() == RespType::NullString);

    REQUIRE_THROWS(resp_value.as_string());
    REQUIRE_THROWS(resp_value.as_integer());
    REQUIRE_THROWS(resp_value.as_array());

    REQUIRE(resp_value.encode() == "$-1\r\n");
    REQUIRE(resp_value.display() == "null");
}

TEST_CASE("RespValueNullArray", "[RespValue]") {
    RespValue resp_value = RespValue::null_array();

    REQUIRE(resp_value.is(RespType::NullArray));
    REQUIRE(!resp_value.is_string());
    REQUIRE(resp_value.type() == RespType::NullArray);

    REQUIRE_THROWS(resp_value.as_string());
    REQUIRE_THROWS(resp_value.as_integer());
    REQUIRE_THROWS(resp_value.as_array());

    REQUIRE(resp_value.encode() == "*-1\r\n");
    REQUIRE(resp_value.display() == "null");
}