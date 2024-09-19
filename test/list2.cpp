#include "../include/custom_bencoder.h"
#include <iostream>
#include <memory>
#include <vector>
using namespace std;
using namespace bencoding;

int main()
{
    bencode_list bl(5);
    bl[0] = make_unique<bencode_integer>(7);
    cout << bl[0]->encode() << "\n";
    bl[1] = make_unique<bencode_string>("hiaf");
    bencode_integer c = 999;
    bl[2] = make_unique<bencode_integer>(c);
    bl.encode_n_dump(cout);
    cout << "\n";
    bencode_string str("ok");
    bl[2] = make_unique<bencode_string>(str);
    bl.encode_n_dump(cout);
    cout << "\n";
    bl[1]->encode_n_dump(cout);
    cout << "\n";
    return 0;
}