#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace bencoding
{
    template <typename Item>
    class bencode_base
    {
    public:
        const std::string delimiter_token = ":";
        const std::string end_token = "e";

        void encode_n_dump(std::ostream &) const
        {
            static_cast<Item const &>(*this).encode_n_dump();
        }

        void load(std::istream &) const
        {
            static_cast<Item const &>(*this).load();
        }
    };

    template <typename intT = int64_t>
    class bencode_integer : public bencode_base<bencode_integer<intT>>
    {

    public:
        using value_type = intT;
        explicit bencode_integer(value_type value) : integer_value_(value) {}

        value_type get() const
        {
            return integer_value_;
        }

        std::string encode() const
        {
            return integer_token + std::to_string(integer_value_) + end_token;
        }
        void encode_n_dump(std::ostream &out) const
        {
            out << integer_token << integer_value_ << end_token;
        }
        // construct from base class??
        // void load() const
        // {

        // }
        // make_integer??

    private:
        value_type integer_value_;

        const std::string integer_token = "i";
    };

    class bencode_string : public bencode_base<bencode_string>
    {
    public:
        using string_type = std::string;
        using CharT = char;
        explicit bencode_string(string_type &str) : str_(str) {}

        explicit bencode_string(const CharT *cstr) : str_(cstr) {}

        size_t size() const { return str_.length(); }

        string_type get() { return str_; }

        std::string encode() const
        {
            return std::to_string(str_.length()) + delimiter_token + str_;
        }

        void encode_n_dump(std::ostream &out) const
        {
            out << str_.length() << delimiter_token << str_;
        }

        // construct from base class??
        // void load() const
        // {

        // }
        // make_integer??

    private:
        string_type str_;
    };

    class bencode_list : public bencode_base<bencode_list>
    {
    public:
        bencode_list()
        {
        }

        void encode_n_dump(std::ostream &out) const
        {
            //    std::for_each(list_.begin(), list_.end())
        }

    private:
        std::vector<bencode_base> list_;

        const std::string list_token = "l";
    };

    class bencode_dict : public bencode_base<bencode_dict>
    {
    public:
    private:
        std::map<bencode_string, bencode_base> dict_;

        const std::string dict_token = "d";
    };

}