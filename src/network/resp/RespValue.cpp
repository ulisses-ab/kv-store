#include "RespValue.hpp"

#include <stdexcept>

using namespace std;

RespType RespValue::type() const {
    return type_;
}

bool RespValue::is(RespType type) const {
    return type_ == type;
}

bool RespValue::is_string() const {
    return type_ == RespType::SimpleString || type_ == RespType::BulkString;
}

const string& RespValue::as_string() const {
    if(
        type_ == RespType::SimpleString ||
        type_ == RespType::BulkString ||
        type_ == RespType::Error
    ) {
        return string_;
    }

    throw runtime_error("RespValue::as_string() called on a non-string value");
}

int64_t RespValue::as_integer() const {
    if(type_ == RespType::Integer) {
        return integer_;
    }

    throw runtime_error("RespValue::as_integer() called on a non-integer value");
}

const vector<RespValue>& RespValue::as_array() const {
    if(type_ == RespType::Array) {
        return array_;
    }

    throw runtime_error("RespValue::as_array()) called on a non-array value");
}

string RespValue::encode() const
{
    switch(type_) {
        case RespType::SimpleString: return encode_simple_string();
        case RespType::BulkString: return encode_bulk_string();
        case RespType::Error: return encode_error();
        case RespType::Integer: return encode_integer();
        case RespType::Array: return encode_array();
        case RespType::NullString: return encode_null_string();
        case RespType::NullArray: return encode_null_array();
        default: throw logic_error("RespValue::encode() called on invalid RespType");
    }
}

string RespValue::encode_simple_string() const {
    return "+" + string_ + "\r\n";
}

string RespValue::encode_bulk_string() const {
    return "$" + to_string(string_.size()) + "\r\n" + string_ + "\r\n";
}

string RespValue::encode_error() const {
    return "-" + string_ + "\r\n";
}

string RespValue::encode_integer() const {
    return ":" + to_string(integer_) + "\r\n";
}

string RespValue::encode_array() const {
    string encoded = "*" + to_string(array_.size()) + "\r\n";
    for(const auto& item : array_) {
        encoded += item.encode();
    }
    return encoded;
}

string RespValue::encode_null_string() const {
    return "$-1\r\n";
}

string RespValue::encode_null_array() const {
    return "*-1\r\n";
}

string RespValue::display() const {
    switch(type_) {
        case RespType::SimpleString: return display_simple_string();
        case RespType::BulkString: return display_bulk_string();
        case RespType::Error: return display_error();
        case RespType::Integer: return display_integer();
        case RespType::Array: return display_array();
        case RespType::NullString: return display_null_string();
        case RespType::NullArray: return display_null_array();
        default: 
            throw logic_error("RespValue::display() called on invalid RespType");
    }
}

string RespValue::display_simple_string() const {
    return  "\"" + string_ + "\"";
}

string RespValue::display_bulk_string() const {
    return "\"" + string_ + "\"";
}

string RespValue::display_error() const {
    return "error(\"" + string_ + "\")";
}

string RespValue::display_integer() const {
    return to_string(integer_);
}

string RespValue::display_array() const {
    string displayed = "[";
    for(size_t i = 0; i < array_.size(); i++) {
        displayed += array_[i].display();
        if(i+1 < array_.size()) {
            displayed += ", ";
        }
    }
    return displayed + "]";
}

string RespValue::display_null_string() const {
    return "null";
}

string RespValue::display_null_array() const {
    return "null";
}

RespValue RespValue::simple_string(string s) {
    return RespValue(RespType::SimpleString, 0, move(s), {});
}

RespValue RespValue::bulk_string(string s) {
    return RespValue(RespType::BulkString, 0, move(s), {});
}

RespValue RespValue::error(string s) {
    return RespValue(RespType::Error, 0, move(s), {});
}

RespValue RespValue::integer(int64_t i) {
    return RespValue(RespType::Integer, i, "", {});
}

RespValue RespValue::array(vector<RespValue> a) {
    return RespValue(RespType::Array, 0, "", move(a));
}

RespValue RespValue::null_string() {
    return RespValue(RespType::NullString, 0, "", {});
}

RespValue RespValue::null_array() {
    return RespValue(RespType::NullArray, 0, "", {});
}