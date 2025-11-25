#pragma once

#include "RespValue.hpp"

#include <optional>

class RespParser {
public:
    RespParser();

    std::optional<RespValue> parse_char(char c);
    void reset();
private:
    enum class State {
        ParsingType,
        ParsingSimpleString,
        ParsingBulkStringLength,
        ParsingBulkStringData,
        ParsingError,
        ParsingInteger,
        ParsingArrayLength,
    };

    struct Frame {
        std::vector<RespValue> array;
        size_t array_length;
    };

    std::optional<RespValue> complete_value(RespValue val);
    void prepare_for_next_value();

    std::optional<RespValue> parse_type(char c);
    std::optional<RespValue> parse_simple_string(char c);
    std::optional<RespValue> parse_bulk_string_length(char c);
    std::optional<RespValue> parse_bulk_string_data(char c);
    std::optional<RespValue> parse_error(char c);
    std::optional<RespValue> parse_integer(char c);
    std::optional<RespValue> parse_array_length(char c);

    std::optional<std::string> try_read_line();

    int64_t bulk_string_length_;
    std::string buffer_;
    State state_;
    std::vector<Frame> stack_;
};