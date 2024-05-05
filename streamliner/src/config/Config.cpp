#include "Streamliner/config/Config.hpp"

namespace sl::config
{
    Node::Node(const std::string &_str)
        : NodeVariant(_str)
    {
    }

    std::shared_ptr<Node> Node::operator[](const std::string &_key) const
    {
        return asDict().at(_key);
    }

    std::shared_ptr<Node> Node::operator[](size_t _idx) const
    {
        return asArray().at(_idx);
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

    bool Node::Is(Type _type) const
    {
        return index() == _type;
    }

    bool Node::IsNull() const
    {
        return Is(Type::kNull);
    }

    bool Node::IsUndefined() const
    {
        return Is(Type::kUndefined);
    }

    Node::operator bool() const
    {
        return !IsNull() && !IsUndefined();
    }

    std::shared_ptr<Node> Node::MakeArray(const std::initializer_list<Node> &_vals)
    {
        Array res{};

        for (const auto &_val : _vals)
            res.push_back(std::make_shared<Node>(_val));
        return std::make_shared<Node>(res);
    }

    std::shared_ptr<Node> Node::MakeArray(const std::vector<Node> &_vals)
    {
        Array res{};

        for (const auto &_val : _vals)
            res.push_back(std::make_shared<Node>(_val));
        return std::make_shared<Node>(res);
    }

    std::shared_ptr<Node> Node::MakeDict(const std::initializer_list<std::pair<std::string, Node>> &_vals)
    {
        Dict res{};

        for (const auto &[_key, _val] : _vals)
            res[_key] = std::make_shared<Node>(_val);
        return std::make_shared<Node>(res);
    }

    std::shared_ptr<Node> Node::MakeDict(const std::unordered_map<std::string, Node> &_vals)
    {
        Dict res{};

        for (const auto &[_key, _val] : _vals)
            res[_key] = std::make_shared<Node>(_val);
        return std::make_shared<Node>(res);
    }

    void Node::merge(const std::shared_ptr<Node> _node)
    {
        if (!Is(Type::kDict) || !_node->Is(Type::kDict))
            return;
        std::get<Dict>(*this).insert(_node->asDict().begin(), _node->asDict().end());
    }
}