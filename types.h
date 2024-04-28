#pragma once

#include <map>
#include <vector>
#include <utility>
#include <fmt/format.h>

using ColSpMatrix = std::map<int, std::vector<std::pair<int,double>>>;
using SpVector = std::map<int,double>;
using CoordsMap = std::map<std::pair<int,int>, int>;
using VecMap = std::map<int, int>;

struct Message {
    int type;
    int dest;
    int coord;
    double payload;
    Message(int _type, int _dest, int _coord, double _payload) : 
        type(_type), dest(_dest), coord(_coord), payload(_payload) {}
};

template <>
struct fmt::formatter<Message> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Message& msg, FormatContext& ctx) const {
        return format_to(ctx.out(), "Message(type={}, dest={}, coord={}, payload={})", 
                msg.type, msg.dest, msg.coord, msg.payload);
    }
};