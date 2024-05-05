#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

            [[nodiscard]] std::shared_ptr<Node> operator[](const std::string &_key) const;
            [[nodiscard]] std::shared_ptr<Node> operator[](size_t _idx) const;

            [[nodiscard]] bool asBool() const;
            [[nodiscard]] const Dict &asDict() const;
            [[nodiscard]] const String &asString() const;
            [[nodiscard]] const Array &asArray() const;
            [[nodiscard]] int asInt() const;
            [[nodiscard]] double asDouble() const;

            [[nodiscard]] bool Is(Type _type) const;
            [[nodiscard]] bool IsNull() const;
            [[nodiscard]] bool IsUndefined() const;

            [[nodiscard]] operator bool() const;

            [[nodiscard]] static std::shared_ptr<Node> MakeArray(const std::initializer_list<Node> &_val);
            [[nodiscard]] static std::shared_ptr<Node> MakeArray(const std::vector<Node> &_val);
            [[nodiscard]] static std::shared_ptr<Node> MakeDict(const std::initializer_list<std::pair<std::string, Node>> &_val);
            [[nodiscard]] static std::shared_ptr<Node> MakeDict(const std::unordered_map<std::string, Node> &_val);

            void merge(const std::shared_ptr<Node> _node);
    };
}