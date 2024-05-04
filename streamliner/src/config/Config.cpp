#include "Streamliner/config/Config.hpp"

namespace sl::config
{
    Node Node::MakeArray(const std::initializer_list<Node> &_vals)
    {
        Array res;

        for (const auto &_val : _vals)
            res.push_back(std::make_shared<Node>(_val));
        return res;
    }

    Node Node::MakeArray(const std::vector<Node> &_vals)
    {
        Array res;

        for (const auto &_val : _vals)
            res.push_back(std::make_shared<Node>(_val));
        return res;
    }

    /// Simpler way to construct a Dict node, it instantiates an std::shared_ptr for each value
    Node Node::MakeDict(const std::initializer_list<std::pair<std::string, Node>> &_vals)
    {
        Dict res;

        for (const auto &[_key, _val] : _vals)
            res[_key] = std::make_shared<Node>(_val);
        return res;
    }

    /// Constructs a Dict node from a vector, it instantiates an std::shared_ptr for each value
    Node Node::MakeDict(const std::unordered_map<std::string, Node> &_vals)
    {
        Dict dict;

        for (const auto &[_key, _val] : _vals)
            dict[_key] = std::make_shared<Node>(_val);
        return dict;
    }

    /// Used to construct a Node from a string
    Node::Node(const std::string &_str)
        : NodeVariant(_str)
    {
    }

    Node &Node::operator[](const std::string &_key) const
    {
        return *asDict().at(_key);
    }

    Node &Node::operator[](size_t _idx) const
    {
        return *asArray().at(_idx);
    }

    bool Node::asBool() const
    {
        return std::get<bool>(*this);
    }

    const Dict &Node::asDict() const
    {
        return std::get<Dict>(*this);
    }

    const String &Node::asString() const
    {
        return std::get<String>(*this);
    }

    const Array &Node::asArray() const
    {
        return std::get<Array>(*this);
    }

    int Node::asInt() const
    {
        return std::get<int>(*this);
    }

    double Node::asDouble() const
    {
        return std::get<double>(*this);
    }

    bool Node::IsNull() const
    {
        return index() == Type::kNull;
    }

    bool Node::IsUndefined() const
    {
        return index() == Type::kUndefined;
    }

    Node::operator bool() const
    {
        return !IsNull() && !IsUndefined();
    }
}