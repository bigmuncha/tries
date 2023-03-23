#include "test/test_b_trie.h"
#include <ipcreator.h>
#include <test_fx_trie.h>
int main ()
{
    using namespace buddy_lpm;
//    LLOG_DEBUG() << "OGOR";
    static constexpr uint32_t omar = 0xffffffff;

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
//    test_net_24();
//    test_net_16();
     //  test_fill_nh_table();
    show_nets(10);
//    test_get_longest_match();
    std::cout << "SUCCESS\n";
//    test_get_longest_match();
    static constexpr uint32_t said = 0xabcdef11;
    std::cout << std::bitset<32>(said) <<"\n";

    u_int8_t FFull[16] =
	{0x01,0x02,0xff,0xaa,0xff,0xff,0xff,0x11,
	 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1f};
    ipv6 somar = reinterpret_cast<ipv6&>(FFull);
    ipv6 sssss = applyMask(FFull, 71);
    long long mmar = *reinterpret_cast<long long*>(FFull);
    std::cout << getStrIpv6(FFull) << "\n";
    std::cout << getStrIpv6(somar) << "\n";
    std::cout << getStrIpv6(sssss) << "\n";
    std::cout <<std::bitset<8> (somar.buff[3]) << '\n';
    std::cout <<std::bitset<8> (sssss.buff[10]) << '\n'; 
    //struct ipv6 ip =  reinterpret_cast<ipv6&>(FFull);
    std::cout << "\n" <<sizeof(uintmax_t) <<'\n';

    std::cout << "FIX TEST" <<sizeof(uintmax_t) <<'\n';
    fx_trie::test_init();
}
