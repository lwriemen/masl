/*
 * ----------------------------------------------------------------------------
 * (c) 2005-2023 - CROWN OWNED COPYRIGHT. All rights reserved.
 * The copyright of this Software is vested in the Crown
 * and the Software is the property of the Crown.
 * ----------------------------------------------------------------------------
 * SPDX-License-Identifier: Apache-2.0
 * ----------------------------------------------------------------------------
 */

#ifndef Sql_Util_HH
#define Sql_Util_HH

#include <cerrno>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <netinet/in.h>

#include "boost/functional/hash.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/tuple/tuple.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Exception.hh"
#include "swa/Duration.hh"
#include "swa/ObjectPtr.hh"
#include "swa/Set.hh"
#include "swa/String.hh"
#include "swa/Timestamp.hh"
#include "swa/tuple_hash.hh"
#include "swa/types.hh"

namespace SQL {

    //*****************************************************************************
    //*****************************************************************************
    template <class T>
    struct PairFirst {
        typename T::first_type operator()(const T &pair) {
            return pair.first;
        }
    };

    //*****************************************************************************
    //*****************************************************************************
    template <class T>
    struct PairSecond {
        typename T::second_type operator()(const T &pair) {
            return pair.second;
        }
    };

    // ***********************************************************************
    // ***********************************************************************
    template <class T>
    struct Subscript {
        typedef typename T::mapped_type result_type;
        typedef typename T::key_type argument_type;

        Subscript(T &container)
            : container(container) {}

        const result_type &operator()(const argument_type &index) const {
            return container[index];
        }

      private:
        T &container;
    };

    // ********************************************************************
    // ! @brief Define the types associated with an generated implementation Object
    //
    // ! The generated  sql objects use a number of defined types throughout the
    // ! implementation, rather than scattering these type definitions group them
    // together ! into this Persistent Object traits type.
    // !
    // ********************************************************************
    template <class T>
    struct PsObject_Traits {
        typedef T PsObject;
        typedef ::SWA::ObjectPtr<T> PsObjectPtr;
        typedef std::shared_ptr<PsObject> PsSharedPtr;

        typedef typename ::SWA::Set<PsObjectPtr> PsObjectPtrSwaSet;

        typedef SWA::Set<::SWA::IdType> PsObjectIdSet;
        typedef std::unordered_set<PsObjectPtr> PsObjectPtrSet;

        typedef std::unordered_set<::SWA::IdType> PsCachedPtrSet;
        typedef std::unordered_map<::SWA::IdType, PsSharedPtr> PsCachedPtrMap;
    };

    // ***********************************************************************
    // ***********************************************************************
    template <class T>
    class TableStatementGenerator {
      public:
        typedef std::function<void(const T &, const typename T::p_object_ptr &, std::string &)> MethodType;

      public:
        TableStatementGenerator(const T &table, MethodType method, std::string &statements)
            : table_(table), method_(method), statements_(statements) {}

        ~TableStatementGenerator() {}

        void operator()(const typename T::p_object_ptr &obj) {
            method_(table_, obj, statements_);
        }

      private:
        const T &table_;
        MethodType method_;
        std::string &statements_;
    };

    // ***********************************************************************
    // ***********************************************************************
    template <class T>
    TableStatementGenerator<T> StatementGenerator(
        const T &table, typename TableStatementGenerator<T>::MethodType method, std::string &statements
    ) {
        return TableStatementGenerator<T>(table, method, statements);
    }

    // ***********************************************************************
    // ***********************************************************************
    template <class T, class P = std::equal_to<typename T::first_type>>
    class AssociativeKeyAccessor {
      public:
        AssociativeKeyAccessor(const typename T::first_type &key)
            : key_(key) {}
        ~AssociativeKeyAccessor() {}

        bool operator()(const T &value) {
            return operation_(value.first, key_);
        }

      private:
        typename T::first_type key_;
        P operation_;
    };

    // ***********************************************************************
    // ***********************************************************************
    template <class T>
    AssociativeKeyAccessor<typename T::value_type>
    matchKey(const T &container, const typename T::value_type::first_type &key) {
        return AssociativeKeyAccessor<typename T::value_type>(key);
    }

    // ***********************************************************************
    // ***********************************************************************
    template <class C>
    class MatchObjectIdentity {
      public:
        typedef typename C::key_type key_type;
        typedef typename C::value_type value_type;

      public:
        MatchObjectIdentity(const key_type &identity)
            : identity_(identity) {}
        ~MatchObjectIdentity() {}

        bool operator()(const value_type value) const {
            return value.first == identity_;
        }

      private:
        key_type identity_;
    };

    // ***********************************************************************
    // ***********************************************************************
    template <class T, class K>
    class ArchitectureKeyMatch {
      public:
        ArchitectureKeyMatch(const K &key)
            : key_(key) {}
        ~ArchitectureKeyMatch() {}

        bool operator()(const T &value) {
            return value->getUniqueIdentifier() == key_;
        }

      private:
        K key_;
    };

    // ***********************************************************************
    // ***********************************************************************
    template <class O, class A>
    class OneAttributeSelector {
      public:
        typedef A (O::*GetMethodType)();

      public:
        OneAttributeSelector(GetMethodType getMethod, const A &attributeValue)
            : getMethod_(getMethod), attributeValue_(attributeValue) {}
        ~OneAttributeSelector() {}

        bool operator()(const std::shared_ptr<O> &object) const {
            return ((&(*object))->*getMethod_)() == attributeValue_;
        }

      private:
        GetMethodType getMethod_;
        A attributeValue_;
    };

    // ***********************************************************************
    // ***********************************************************************
    template <class O, class A, class B>
    class TwoAttributeSelector {
      public:
        typedef A (O::*GetMethodOneType)();
        typedef B (O::*GetMethodTwoType)();

      public:
        TwoAttributeSelector(
            GetMethodOneType getMethodOne,
            GetMethodTwoType getMethodTwo,
            const A &attributeOneValue,
            const B &attributeTwoValue
        )
            : getMethodOne_(getMethodOne),
              getMethodTwo_(getMethodTwo),
              attributeOneValue_(attributeOneValue),
              attributeTwoValue_(attributeTwoValue) {}

        ~TwoAttributeSelector() {}

        bool operator()(const std::shared_ptr<O> &object) const {
            return ((&(*object))->*getMethodOne_)() == attributeOneValue_ &&
                   ((&(*object))->*getMethodTwo_)() == attributeTwoValue_;
        }

      private:
        GetMethodOneType getMethodOne_;
        GetMethodTwoType getMethodTwo_;
        A attributeOneValue_;
        B attributeTwoValue_;
    };

    //******************************************************************************
    //******************************************************************************
    inline bool isBigEndian() {
        // attempt runtime determination of endianess.
        const unsigned int littleEndian = 0x01020304;
        const unsigned int bigEndian = 0x04030201;
        const unsigned int actualIndian = htonl(littleEndian);
        return actualIndian == bigEndian;
    }

    //******************************************************************************
    //******************************************************************************
    inline void checkStream(const std::ostringstream &str, const char *const location) {
        if (str.bad() || (str.fail() && !str.eof())) {
            std::string message("checkStream failed in ");
            message += location;
            message += " : ";
            message += strerror(errno);
            throw std::runtime_error(message);
        }
    }

    // ********************************************************************
    // ********************************************************************
    inline std::string escapeSingleQuote(const std::string &value) {
        if (value.find('\'') == std::string::npos) {
            return value;
        }

        std::string escapedString;
        for (std::string::size_type index = 0; index < value.size(); ++index) {
            escapedString += value[index];
            if (value[index] == '\'') {
                escapedString += '\'';
            }
        }
        return escapedString;
    }

    // ********************************************************************
    // ********************************************************************
    template <class T>
    std::string convertToColumnValue(T value) {
        return std::format("{}", value);
    }

    inline std::string convertToColumnValue(const ::SWA::Timestamp &value) {
        return convertToColumnValue<int64_t>(value.nanosSinceEpoch());
    }
    inline std::string convertToColumnValue(const ::SWA::Duration &value) {
        return convertToColumnValue<int64_t>(value.nanos());
    }
    inline std::string convertToColumnValue(const ::SWA::String &value) {
        return std::string("'") + escapeSingleQuote(value) + std::string("'");
    }
    inline std::string convertToColumnValue(const std::string &value) {
        return std::string("'") + escapeSingleQuote(value) + std::string("'");
    }
    inline std::string convertToColumnValue(char *value) {
        return std::string("'") + escapeSingleQuote(value) + std::string("'");
    }
    inline std::string convertToColumnValue(const char *const value) {
        return std::string("'") + escapeSingleQuote(value) + std::string("'");
    }

    //******************************************************************************
    //******************************************************************************
    template <class T>
    inline std::string valueToString(const T value) {
        return std::format("{}", value);
    }

    //******************************************************************************
    //******************************************************************************
    template <>
    inline std::string valueToString<const char *const>(const char *const value) {
        return value;
    }
    template <>
    inline std::string valueToString<const char *>(const char *value) {
        return value;
    }
    template <>
    inline std::string valueToString<char *>(char *value) {
        return value;
    }

    //******************************************************************************
    //******************************************************************************
    template <class T>
    T stringToValue(const std::string &iText) {
        return ::boost::lexical_cast<T>(iText);
    }

    //******************************************************************************
    //******************************************************************************
    template <class T>
    T stringToValue(const unsigned char *const iText) {
        return stringToValue<T>(std::string(reinterpret_cast<const char *const>(iText)));
    }

    //******************************************************************************
    //******************************************************************************
    template <>
    inline std::string stringToValue(const unsigned char *const iText) {
        return std::string(reinterpret_cast<const char *const>(iText));
    }

    //******************************************************************************
    //******************************************************************************
    template <>
    inline SWA::String stringToValue(const unsigned char *const iText) {
        return SWA::String(reinterpret_cast<const char *const>(iText));
    }

    //******************************************************************************
    //******************************************************************************
    template <>
    inline int64_t stringToValue<int64_t>(const std::string &text) {
        return atoll(text.c_str());
    }

    //******************************************************************************
    //******************************************************************************
    template <>
    inline int stringToValue<int>(const std::string &text) {
        return atoi(text.c_str());
    }

    //******************************************************************************
    //******************************************************************************
    template <>
    inline SWA::Duration stringToValue<SWA::Duration>(const std::string &text) {
        return SWA::Duration::fromNanos(atoll(text.c_str()));
    }

    template <>
    inline SWA::Timestamp stringToValue<SWA::Timestamp>(const std::string &text) {
        return SWA::Timestamp::fromNanosSinceEpoch(atoll(text.c_str()));
    }

} // namespace SQL

#endif
