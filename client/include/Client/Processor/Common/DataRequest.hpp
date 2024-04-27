#pragma once

#include "Client/Processor/IProcessor.hpp"

namespace imp
{
    template<typename... Ts>
    struct is_buildfix_base;

    template<>
    struct is_buildfix_base<> : std::true_type {};

    template<typename T, typename... Ts>
    struct is_buildfix_base<T, Ts...> :
        std::conditional<std::is_base_of<proc::IBuildFIX, T>::value,
                        imp::is_buildfix_base<Ts...>,
                        std::false_type>::type {};
}

template<typename... Ts>
constexpr bool is_buildfix_base() {
    return imp::is_buildfix_base<Ts...>::value;
}

template<class ...Ts>
concept IsBuildFIX = is_buildfix_base<Ts...>();

namespace proc::com
{
    template<class ...Ts>
    requires IsBuildFIX<Ts...>
    class DataRequest : public Ts...
    {
        public:
            [[nodiscard]] virtual std::optional<fix::Message> build(char _tag, const Context &_ctx) const override final;

        private:
            [[nodiscard]] fix::Message buildFullRefresh() const;
            [[nodiscard]] fix::Message buildIncrRefresh() const;
    };
}

#include "Client/Processor/Common/DataRequest.inl"