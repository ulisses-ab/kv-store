#include "RespParser.hpp"

#include <stdexcept>

using namespace std;

RespParser::RespParser() {
    reset();
}

optional<RespValue> RespParser::parse_char(char c) {
    optional<RespValue> parsed;

    switch (state_) {
        case State::ParsingType: parsed = parse_type(c); break;
        case State::ParsingSimpleString: parsed = parse_simple_string(c); break;
        case State::ParsingBulkStringLength: parsed = parse_bulk_string_length(c); break;
        case State::ParsingBulkStringData: parsed = parse_bulk_string_data(c); break;
        case State::ParsingError: parsed = parse_error(c); break;
        case State::ParsingInteger: parsed = parse_integer(c); break;
        case State::ParsingArrayLength: parsed = parse_array_length(c); break;
        default: 
            throw logic_error("RespParser::parse_char() called on invalid State");
    }

    if(!parsed.has_value()) return nullopt;

    return complete_value(move(*parsed));
}

std::optional<RespValue> RespParser::complete_value(RespValue val) {
    prepare_for_next_value();

    while(!stack_.empty()) {
        auto& frame = stack_.back();
        frame.array.push_back(move(val));
        if(frame.array.size() == frame.array_length) {
            val = RespValue::array(move(frame.array));
            stack_.pop_back();
        }
        else {
            return nullopt;
        }
    }

    return val;
}

void RespParser::prepare_for_next_value() {
    bulk_string_length_= 0;
    buffer_.clear();
    state_ = State::ParsingType;
}

optional<RespValue> RespParser::parse_type(char c) {
    switch (c) {
        case '+': state_ = State::ParsingSimpleString; break;
        case '-': state_ = State::ParsingError; break;
        case ':': state_ = State::ParsingInteger; break;
        case '*': state_ = State::ParsingArrayLength; break;
        case '$': state_ = State::ParsingBulkStringLength; break;
        default: 
            throw runtime_error(
                string("RespParser::parse_type() called on invalid char: '") + c + '\''
            );
    }

    return nullopt;
}

optional<RespValue> RespParser::parse_simple_string(char c) {
    buffer_ += c;

    auto line = try_read_line();
    if(!line.has_value()) return nullopt;

    return RespValue::simple_string(line.value());
}

optional<RespValue> RespParser::parse_bulk_string_length(char c) {
    buffer_ += c;

    auto line = try_read_line();
    if(!line.has_value()) return nullopt;

    try {
        bulk_string_length_ = stoll(line.value());
    }
    catch(const exception& e) {
        throw runtime_error(
            "Failed to parse bulk string length from line: '" 
            + line.value() + "'. Original error: " + e.what()
        );
    }

    if(bulk_string_length_ == -1) {
        return RespValue::null_string();
    }

    if(bulk_string_length_ < 0) {
        throw runtime_error {
            "Invalid length for bulk string: '" + line.value() + "'" 
        };
    }

    state_ = State::ParsingBulkStringData;

    return nullopt;
}

optional<RespValue> RespParser::parse_bulk_string_data(char c) {
    buffer_ += c;

    if(buffer_.size() < size_t(bulk_string_length_) + 2) {
        return nullopt;
    }

    auto line = try_read_line();
    if(!line.has_value()) {
        throw runtime_error(
            "Invalid bulk string ending: '" 
            + buffer_ + "'"
        );
    }

    return RespValue::bulk_string(line.value());
}

optional<RespValue> RespParser::parse_error(char c) {
    buffer_ += c;

    auto line = try_read_line();
    if(!line.has_value()) return nullopt;

    return RespValue::error(line.value());
}

optional<RespValue> RespParser::parse_integer(char c) {
    buffer_ += c;

    auto line = try_read_line();
    if(!line.has_value()) return nullopt;

    try {
        int64_t i = stoll(line.value());
        return RespValue::integer(i);
    }
    catch(const exception& e) {
        throw runtime_error(
            "Failed to parse integer from line: '" 
            + line.value() + "'. Original error: " + e.what()
        );
    }
}

optional<RespValue> RespParser::parse_array_length(char c) {
    buffer_ += c;

    auto line = try_read_line();
    if(!line.has_value()) return nullopt;

    int64_t length;

    try {
        length = stoll(line.value());
    }
    catch(const exception& e) {
        throw runtime_error(
            "Failed to parse array length from line: '" 
            + line.value() + "'. Original error: " + e.what()
        );
    }

    if(length == -1) {
        return RespValue::null_array();
    }

    if(length < 0) {
        throw runtime_error {
            "Invalid length for array: '" + line.value() + "'" 
        };
    }

    if(length == 0) {
        return RespValue::array({});
    }

    stack_.push_back({vector<RespValue>(), size_t(length)});
    prepare_for_next_value();

    return nullopt;
}

optional<string> RespParser::try_read_line() {
    size_t bufsize = buffer_.size();

    if(buffer_.size() < 2) return nullopt;
    if(buffer_[bufsize-1] != '\n' || buffer_[bufsize-2] != '\r') return nullopt;

    string line = move(buffer_);
    line.resize(line.size() - 2);
    buffer_.clear();
    return line;
}

void RespParser::reset() {
    stack_.clear();
    buffer_ = "";
    state_ = State::ParsingType;
    bulk_string_length_ = 0;
}
