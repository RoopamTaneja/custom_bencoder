#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>

namespace bencoding
{
    class bencode_base
    {
    public:
        static const char delimiter_token = ':';
        static const char end_token = 'e';
        static const char integer_token = 'i';
        static const char list_token = 'l';
        static const char dict_token = 'd';

        virtual std::string encode() const = 0;
        virtual void encode_n_dump(std::ostream &out) const = 0;
        virtual void decode(std::string &in, std::string::const_iterator &start) = 0;
    };

    template <typename intT = int64_t>
    std::unique_ptr<bencode_base> make_value(std::string &in, std::string::const_iterator &start);

    template <typename intT = int64_t>
    class bencode_integer : public bencode_base
    {
    public:
        using value_type = intT;

        explicit bencode_integer(value_type value) : integer_value_(value) {}
        value_type get() const
        {
            return integer_value_;
        }

        std::string encode() const override
        {
            return std::string(1, integer_token) + std::to_string(integer_value_) + std::string(1, end_token);
        }
        void encode_n_dump(std::ostream &out) const override
        {
            out << integer_token << integer_value_ << end_token;
        }
        void decode(std::string &in, std::string::const_iterator &start) override
        {
            // Assuming *start == 'i
            start++;
            value_type value = 0;
            bool neg = false;
            if (*start == '-')
                neg = true, start++;

            while (start != in.end() && *start != end_token)
                value = value * 10 + (*start - '0'), start++;

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

    class bencode_string : public bencode_base
    {
    public:
        using string_type = std::string;
        using CharT = char;
        bencode_string() {}
        explicit bencode_string(string_type &str) : str_(str) {}
        explicit bencode_string(const CharT *cstr) : str_(cstr) {}

        size_t size() const { return str_.length(); }
        string_type get() { return str_; }

        std::string encode() const override
        {
            return std::to_string(str_.length()) + std::string(1, delimiter_token) + str_;
        }

        void encode_n_dump(std::ostream &out) const override
        {
            out << str_.length() << delimiter_token << str_;
        }

        void decode(std::string &in, std::string::const_iterator &start) override
        {
            // Assuming it starts with length
            size_t len = 0;
            while (start != in.end() && *start != ':')
                len = len * 10 + (*start - '0'), start++;

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

    template <typename intT = int64_t>
    class bencode_list : public bencode_base
    {
    public:
        using value_ptr_type = std::unique_ptr<bencode_base>;
        using container_type = std::vector<value_ptr_type>;
        using iterator = container_type::iterator;
        using const_iterator = container_type::const_iterator;

        // bencode_list()
        // {
        // }

        std::string encode() const override
        {
            std::string enc_str(1, list_token);
            for (const value_ptr_type &ptr_ : list_)
                if (ptr_)
                    enc_str += (ptr_->encode());
            enc_str += end_token;
            return enc_str;
        }

        void encode_n_dump(std::ostream &out) const override
        {
            out << list_token;
            for (const value_ptr_type &ptr_ : list_)
                if (ptr_)
                    ptr_->encode_n_dump(out);
            out << end_token;
        }

        void decode(std::string &in, std::string::const_iterator &start) override
        {
            start++; // assuming *start = 'l'
            while (start != in.end() && *start != end_token)
                list_.push_back(make_value<intT>(in, start));
            start++; // assuming *start = 'e'
        }

    private:
        container_type list_;
    };

    template <typename intT = int64_t, typename... Args>
    std::unique_ptr<bencode_list<intT>> make_list(Args &&...args)
    {
        return std::make_unique<bencode_list>(std::forward<Args>(args)...);
    }

    template <typename intT = int64_t>
    class bencode_dict : public bencode_base
    {
    public:
        using key_type = bencode_string;
        using value_ptr_type = std::unique_ptr<bencode_base>;
        using value_type = std::pair<key_type, value_ptr_type>;
        using container_type = std::map<key_type, value_ptr_type>;
        std::string encode() const override
        {
            std::string enc_str(1, dict_token);
            for (const auto &[key_, ptr_] : dict_)
            {
                if (ptr_)
                {
                    key_.encode();
                    ptr_->encode();
                }
            }
            enc_str += end_token;
            return enc_str;
        }
        void encode_n_dump(std::ostream &out) const override
        {
            out << dict_token;
            for (const auto &[key_, ptr_] : dict_)
            {
                if (ptr_)
                {
                    key_.encode_n_dump(out);
                    ptr_->encode_n_dump(out);
                }
            }
            out << end_token;
        }
        void decode(std::string &in, std::string::const_iterator &start) override
        {
            start++; // assuming *start = 'd'
            while (start != in.end() && *start != end_token)
            {
                key_type key_;
                key_.decode(in, start);
                start++; // assuming *start = ':'
                dict_[key_] = make_value<intT>(in, start);
            }
            start++; // assuming *start = 'e'
        }

    private:
        container_type dict_;
    };

    template <typename intT = int64_t, typename... Args>
    std::unique_ptr<bencode_dict<intT>> make_dict(Args &&...args)
    {
        return std::make_unique<bencode_dict>(std::forward<Args>(args)...);
    }

    template <typename intT = int64_t>
    std::unique_ptr<bencode_base> make_value(std::string &in, std::string::const_iterator &start)
    {
        std::unique_ptr<bencode_base> ptr_;
        if (*start == bencode_base::integer_token)
            ptr_ = std::make_unique<bencode_integer<intT>>();
        else if (*start == bencode_base::list_token)
            ptr_ = std::make_unique<bencode_list<intT>>();
        else if (*start == bencode_base::dict_token)
            ptr_ = std::make_unique<bencode_dict<intT>>();
        else
            ptr_ = std::make_unique<bencode_string>();
        ptr_->decode(in, start);
        return ptr_;
    }
}