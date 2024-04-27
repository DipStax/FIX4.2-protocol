#pragma once

#include <optional>

#include "Client/Handler/Entry.hpp"
#include "Client/Context.hpp"
#include "Common/Message/Fix.hpp"
#include "Common/Message/Serializer.hpp"
#include "Common/Network/UDPPackage.hpp"

namespace proc
{
    class IBuildFIX
    {
        public:
            /// @brief Doit verifier qu'il peut build le type de message
            ///         Puis géré le message
            ///         Renvoie le message ou rien (std::optional)
            /// @param _tag type de message à build
            /// @return le message généré
            [[nodiscard]] virtual std::optional<fix::Message> build(char _tag, const Context &_ctx) const = 0;
    };

    class IMessage : public IBuildFIX
    {
        public:
            /// @brief Convert the object to true if it can handle the type of message, otherwith it's converted to false.
            [[nodiscard]] virtual bool handle(fix::Serializer::AnonMessage &_entry, const Context &_ctx) const = 0;

            /// @brief Doit verifier qu'il peut géré le type de message
            ///         Puis faire des actions avec (set un status, changer une valeur) (optionnel)
            ///         puis repond un message qui va être envoié au server
            /// @param _entry message recu par le tcp
            /// @return message envoie ensuite au server
            [[nodiscard]] virtual std::optional<fix::Message> process(fix::Serializer::AnonMessage &_msg, Context &_ctx) = 0;
    };

    class IUDP
    {
        public:
            /// @brief Doit verifier qu'il peut géré le type de message
            ///         Puis faire des actions avec (set un status, changer une valeur) (optionnel)
            ///         puis repond un message qui va être envoié au server
            /// @param _entry package de l'udp
            /// @return message envoie ensuite au server
            [[nodiscard]] virtual std::optional<data::UDPPackage> process(const data::UDPPackage &_entry, Context &_ctx) = 0;
    };

    class IEntry : public IBuildFIX
    {
        public:
            /// @brief Convert the object to true if it can handle the type of entry, otherwith it's converted to false.
            [[nodiscard]] virtual bool handle(const Entry &_entry, const Context &_ctx) const = 0;

            /// @brief Doit verifier qu'il peut géré le type de message
            ///         Puis faire des actions avec (set un status, changer une valeur) (optionnel)
            ///         puis repond un message qui va être envoié au server
            /// @param _entry l'entré du client
            /// @return message envoie ensuite au server
            [[nodiscard]] virtual std::optional<fix::Message> process(const Entry &_entry, Context &_ctx) = 0;
    };
}