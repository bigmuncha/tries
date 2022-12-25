#include "test/test_b_trie.h"
int main ()
{
    using namespace buddy_lpm;
//    LLOG_DEBUG() << "OGOR";
    static constexpr uint32_t omar = 0xffffffff;
    static constexpr uint32_t said = 0xabcdef11;
    static constexpr uint8_t oleg  = 1 << 2;
    static constexpr uint8_t oid  = 2 | 1;
//    static constexpr uint32_t osid  =  1 >>(omar << (10 -1));
//    static constexpr size_t sss = __builtin_popcount(0xffffffff << 10);
//    static constexpr __int128 tmp = 0x00112233445566778899aabbccddeeffff;
//    std::bitset<128>(tmp);
/*     std::cout <<sss << "\n"; */
//    std::cout <<std::bitset<128>(tmp)  << "\n";
/*     std::cout <<std::bitset<32>(said)  << "\n"; */
/*     std::cout <<std::bitset<32>(~said)  << "\n"; */
/*     std::cout <<(said & (1 << 4))  << "\n"; */
/*     std::cout <<(~said & (1 << 4))  << "\n"; */
/*     std::cout <<std::bitset<32>(1<< 10 )  << "\n"; */
/*     std::cout <<std::bitset<32>(said << 10 )  << "\n"; */
/*     std::cout <<std::bitset<32>(said  &(said << 10))  << "\n"; */
/*     std::cout <<std::bitset<32>(said) << "\n"; */
/*     std::cout <<std::bitset<8>(oid) << "\n"; */
/*     //  std::cout <<std::bitset<8>(osid) << "\n"; */
/*     std::cout << (oleg & oid) << "\n"; */
/*     std::cout << "\n\n"; */
     Btrie trie; 
/*     std::cout << trie.lookup(0xffff1122).has_value() << '\n'; */
/*     trie.PrintValues(); */
/*     std::cout << trie.insert(0xffff1122,0xffffff00, 12) << '\n'; */
/*     std::cout << trie.insert(0xffff3333,0xffffff00, 15) << '\n'; */
/*     std::cout << trie.insert(0xffff1144,0xffffff00, 18) << '\n'; */
/*     std::cout << trie.lookup(0xffff1122).has_value() << '\n'; */
//    trie.PrintValues();
     LLOG_DEBUG() << "OLEG";
    test_net_24();
//    test_net_16();
    test_fill_nh_table();
    show_nets(10);
//    test_get_longest_match();
    std::cout << "SUCCESS\n";
    test_get_longest_match();
}
