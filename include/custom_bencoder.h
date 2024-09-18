#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>

namespace bencoding
{
    template <typename Item>
    class bencode_base
    {
    public:
        const std::string delimiter_token = ":";
        const std::string end_token = "e";
        const std::string integer_token = "i";
        const std::string list_token = "l";
        const std::string dict_token = "d";

        std::string encode() const
        {
            return static_cast<Item const &>(*this).encode();
        }

        void encode_n_dump(std::ostream &out) const
        {
            static_cast<Item const &>(*this).encode_n_dump(out);
        }

        void decode(std::string &in, std::string::const_iterator &start) const
        {
            static_cast<Item const &>(*this).decode(in, start);
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

        void encode_n_dump(std::ostream &out) const
        {
            out << this->integer_token << integer_value_ << this->end_token;
        }

        std::string encode() const
        {
            return this->integer_token + std::to_string(integer_value_) + this->end_token;
        }

        void decode(std::string &in, std::string::const_iterator &start)
        {
            // Assuming *start == i
            start++;
            value_type value = 0;
            bool neg = 0;
            if (*start == '-')
            {
                neg = 1;
                start++;
            }
            while (start != in.end() && *start != this->end_token)
            {
                value = value * 10 + (*start - '0');
            }
            integer_value_ = neg ? -value : value;
            if (start != in.end())
                start++; // assuming it ends with e
        }

    private:
        value_type integer_value_;
    };

    template <typename intT = int64_t, typename... Args>
    std::unique_ptr<bencode_integer<intT>> make_integer(Args &&...args)
    {
        return std::make_unique<bencode_integer<intT>>(std::forward<Args>(args)...);
    }

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
            return std::to_string(str_.length()) + this->delimiter_token + str_;
        }

        void encode_n_dump(std::ostream &out) const
        {
            out << str_.length() << this->delimiter_token << str_;
        }

        void decode(std::string &in, std::string::const_iterator &start)
        {
            // Assuming it starts with length
            size_t len = 0;
            while (start != in.end() && *start != ':')
            {
                len = len * 10 + (*start - '0');
                start++;
            }
            start++; // to move past :
            str_ = in.substr(start - in.begin(), len);
            start += len;
        }

    private:
        string_type str_;
    };

    template <typename... Args>
    std::unique_ptr<bencode_string> make_string(Args &&...args)
    {
        return std::make_unique<bencode_string>(std::forward<Args>(args)...);
    }

    class bencode_list : public bencode_base<bencode_list>
    {
    public:
        bencode_list()
        {
        }

        template <typename intT = int64_t>
        std::unique_ptr<bencode_base> make_value(std::string &in, std::string::const_iterator &start);

        void encode_n_dump(std::ostream &out) const
        {
            //    std::for_each(list_.begin(), list_.end())
        }

    private:
        std::vector<bencode_base> list_;
    };

    template <typename... Args>
    std::unique_ptr<bencode_list> make_list(Args &&...args)
    {
        return std::make_unique<bencode_list>(std::forward<Args>(args)...);
    }

    class bencode_dict : public bencode_base<bencode_dict>
    {
    public:
    private:
        std::map<bencode_string, bencode_base> dict_;
    };

    template <typename... Args>
    std::unique_ptr<bencode_dict> make_dict(Args &&...args)
    {
        return std::make_unique<bencode_dict>(std::forward<Args>(args)...);
    }

    // template <typename intT = int64_t>
    // std::unique_ptr<bencode_base>
    // bencode_list::make_value(std::string &in, std::string::const_iterator &start)
    // {
    //     std::unique_ptr<bencode_base> ptr_;
    //     std::string start_token = std::string(1, *start);
    //     if (start_token == this->integer_token)
    //         ptr_ = std::make_unique<bencode_integer<intT>>();
    //     else if (start_token == this->list_token)
    //         ptr_ = std::make_unique<bencode_list>();
    //     else if (start_token == this->dict_token)
    //         ptr_ = std::make_unique<bencode_dict>();
    //     ptr_->decode(in, start);
    //     return ptr_;
    // }
}

// make_base for parsing??