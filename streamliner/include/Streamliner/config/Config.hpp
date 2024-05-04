#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace sl::config {

    enum Type {
        kUndefined = 0,
        kNull,
        kBool,
        kInt,
        kDouble,
        kString,
        kArray,
        kDict
    };

    struct Node;

    struct Undefined {};

    using Null = std::nullptr_t;

    using String = std::string;

    using Array = std::vector<std::shared_ptr<Node>>;

    using Dict = std::unordered_map<std::string, std::shared_ptr<Node>>;

    using NodeVariant = std::variant<Null, Undefined, bool, int, double, String, Array, Dict>;

    struct Node : public NodeVariant {
        public:
            using NodeVariant::NodeVariant;

            Node(const String &_str);
            ~Node() = default;

            static Node MakeArray(const std::initializer_list<Node> &_val);
            static Node MakeArray(const std::vector<Node> &_val);
            static Node MakeDict(const std::initializer_list<std::pair<std::string, Node>> &_val);
            static Node MakeDict(const std::unordered_map<std::string, Node> &_val);

            Node &operator[](const char *_key) const;
            Node &operator[](const std::string &_key) const;
            Node &operator[](size_t _idx) const;

            bool asBool() const;
            const Dict &asDict() const;
            const String &asString() const;
            const Array &asArray() const;
            int asInt() const;
            double asDouble() const;

            bool IsNull() const;
            bool IsUndefined() const;

            operator bool() const;
    };
}