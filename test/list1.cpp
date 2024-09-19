#include "../include/custom_bencoder.h"
#include <iostream>
#include <memory>
#include <vector>
using namespace std;
using namespace bencoding;

int main()
{
    bencode_list ls;
    bencode_integer a(8);
    bencode_string b("hi");
    ls.push_back<bencode_integer>(a);
    ls.push_back<bencode_integer>(bencode_integer(777));
    ls.push_back<bencode_integer>(777);
    ls.push_back<bencode_string>("bye");
    ls.encode_n_dump(cout);
    cout << "\n";
    ls.pop_back();
    ls.encode_n_dump(cout);
    cout << "\n";
    string_subs sa(ls.encode());
    bencode_list lsl;
    cout << sa.str << "\n";
    lsl.decode(sa.str, sa.citer);
    lsl.encode_n_dump(cout);
    cout << "\n";
    return 0;
}
