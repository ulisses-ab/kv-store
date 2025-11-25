#pragma once

#include <string>
#include <vector>

enum class RespType {
    SimpleString,
    BulkString,
    Error,
    Integer,
    Array,
    NullString,
    NullArray
};

class RespValue {
public:
    RespType type() const;

    bool is(RespType type) const;
    bool is_string() const;

    const std::string& as_string() const;
    int64_t as_integer() const;
    const std::vector<RespValue>& as_array() const;

    std::string encode() const;
    std::string display() const;

    static RespValue simple_string(std::string s);
    static RespValue bulk_string(std::string s);
    static RespValue error(std::string s);
    static RespValue integer(int64_t i);
    static RespValue array(std::vector<RespValue> a);
    static RespValue null_string();
    static RespValue null_array();
private:
    RespValue(RespType type, int64_t i, std::string s, std::vector<RespValue> a)
        : type_(type), integer_(i), string_(std::move(s)), array_(std::move(a)) {}

    std::string encode_simple_string() const;
    std::string encode_bulk_string() const;
    std::string encode_error() const;
    std::string encode_integer() const;
    std::string encode_array() const;
    std::string encode_null_string() const;
    std::string encode_null_array() const;

    std::string display_simple_string() const;
    std::string display_bulk_string() const;
    std::string display_error() const;
    std::string display_integer() const;
    std::string display_array() const;
    std::string display_null_string() const;
    std::string display_null_array() const;
 
    RespType type_;

    int64_t integer_ = 0;
    std::string string_;
    std::vector<RespValue> array_;
};