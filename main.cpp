#include "test/test_b_trie.h"
#include <ipcreator.h>
#include <lpm6_data.h>
#include <rte_eal.h>
#include <rte_common.h>
 #include "test/dir24_8_lpmv4_perf.h" 
 #include "test/full_cover_multibit_test.h"

int main (int argc, char **argv)
{
    int ret;
    ret = rte_eal_init(argc, argv);
    if (ret < 0)
	rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");

    std::vector<rules_tbl_entry> vector_rules = std::vector<rules_tbl_entry>();
    std::vector<ips_tbl_entry> vector_ips = {};
    static constexpr int ROUTE_MAX = 20000;
    static constexpr int IPS_MAX = 20000;
    rte_srand(rte_rdtsc());
    generate_long_ip_table_vec<rules_tbl_entry>(vector_rules,ROUTE_MAX,128, 90);
    generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, vector_ips,IPS_MAX,5,10);

    print_route_distribution(&vector_rules[0], vector_ips.size());
	
    /* if(test2() == 0) */
    /* 	std::cout << "Success\n"; */
    /* else */
    /* 	std:: cout << "failure\n"; */

    //test_lpm6();
    std::cout <<"No. routes = "<< vector_rules.size() <<'\n';
    std::cout << "No. IPS = " << vector_ips.size() << '\n';


    test_lpm6_perf(vector_rules, vector_ips);
    std::cout <<"NEXT\n\n";
    test_multibit_perf(vector_rules, vector_ips);
    std::cout <<"NEXT\n\n";
    tests_fib6_perf(vector_rules, vector_ips);
    //test_fib_perf();
}
