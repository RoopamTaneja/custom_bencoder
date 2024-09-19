#include "../include/custom_bencoder.h"
#include <iostream>
#include <memory>
#include <vector>
#include <map>
using namespace std;
using namespace bencoding;

int main()
{
    bencode_dict bd;
    bd.insert_or_assign<bencode_integer>("key1", 42);
    bd["key1"]->encode_n_dump(std::cout);
    bd.insert("key2", make_unique<bencode_string>("ok bye"));
    return 0;
}