#include "ipsaver.h"
#include "test/test_b_trie.h"
#include <ipcreator.h>
#include <lpm6_data.h>
#include <rte_eal.h>
#include <rte_common.h>
#include <string>
#include <vector>
 #include "test/dir24_8_lpmv4_perf.h" 
 #include "test/full_cover_multibit_test.h"

int main (int argc, char **argv)
{
    int ret;
    ret = rte_eal_init(argc, argv);
    if (ret < 0)
	rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");

    std::vector<rules_tbl_entry> vector_rules = std::vector<rules_tbl_entry>();
    std::vector<std::pair<std::string, double>>statistic = {};
    static constexpr int ROUTE_MAX = 50000;
    static constexpr int IPS_MAX = 100000;

    generate_long_ip_table_vec<rules_tbl_entry>(vector_rules,ROUTE_MAX,48, 32, 24);
    rte_srand(rte_rdtsc());
    {
	set_new_mask_ip_table_vec<rules_tbl_entry>(vector_rules,ROUTE_MAX,48, 32, 24);
	std::vector<std::pair<std::string,std::vector<ips_tbl_entry>>>vect;
	std::vector<ips_tbl_entry> tmp = {};
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,1,1);
	vect.push_back({"1/1",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,2,2);
	vect.push_back({"2/2",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,2,5);
	vect.push_back({"5/2",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,5,5);
	vect.push_back({"5/5",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,3,10);
	vect.push_back({"10/3",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,5,20);
	vect.push_back({"20/5",tmp});

//	print_route_distribution(&vector_rules[0], vector_rules.size());

	/* if(test2() == 0) */
	/* 	std::cout << "Success\n"; */
	/* else */
	/* 	std:: cout << "failure\n"; */

	//test_lpm6();
	std::cout <<"No. routes = "<< vector_rules.size() <<'\n';

	test_lpm6_perf(vector_rules, vect, statistic);
	std::cout <<"NEXT\n\n";
	test_multibit_perf(vector_rules, vect,statistic);
	std::cout <<"NEXT\n\n";
	//tests_fib6_perf(vector_rules, vector_ips);
	//test_fib_perf();
    }
    {
	set_new_mask_ip_table_vec<rules_tbl_entry>(vector_rules,ROUTE_MAX,70, 64, 50);
	std::vector<std::pair<std::string,std::vector<ips_tbl_entry>>>vect;
	std::vector<ips_tbl_entry> tmp = {};
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,1,1);
	vect.push_back({"1/1",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,2,2);
	vect.push_back({"2/2",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,2,5);
	vect.push_back({"5/2",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,5,5);
	vect.push_back({"5/5",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,3,10);
	vect.push_back({"10/3",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,5,20);
	vect.push_back({"20/5",tmp});

///	print_route_distribution(&vector_rules[0], vector_rules.size());

	/* if(test2() == 0) */
	/* 	std::cout << "Success\n"; */
	/* else */
	/* 	std:: cout << "failure\n"; */

	//test_lpm6();
	std::cout <<"No. routes = "<< vector_rules.size() <<'\n';

	test_lpm6_perf(vector_rules, vect, statistic);
	std::cout <<"NEXT\n\n";
	test_multibit_perf(vector_rules, vect,statistic);
	std::cout <<"NEXT\n\n";
	//tests_fib6_perf(vector_rules, vector_ips);
	//test_fib_perf();

    }
    {
	set_new_mask_ip_table_vec<rules_tbl_entry>(vector_rules,ROUTE_MAX,127, 100, 80);
	std::vector<std::pair<std::string,std::vector<ips_tbl_entry>>>vect;
	std::vector<ips_tbl_entry> tmp = {};
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,1,1);
	vect.push_back({"1/1",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,2,2);
	vect.push_back({"2/2",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,2,5);
	vect.push_back({"5/2",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,5,5);
	vect.push_back({"5/5",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,3,10);
	vect.push_back({"10/3",tmp});
	tmp.clear();
	generate_large_ips_table_vec<rules_tbl_entry, ips_tbl_entry>(vector_rules, tmp,IPS_MAX,5,20);
	vect.push_back({"20/5",tmp});

//	print_route_distribution(&vector_rules[0], vector_rules.size());

	/* if(test2() == 0) */
	/* 	std::cout << "Success\n"; */
	/* else */
	/* 	std:: cout << "failure\n"; */

	//test_lpm6();
	std::cout <<"No. routes = "<< vector_rules.size() <<'\n';

	test_lpm6_perf(vector_rules, vect, statistic);
	std::cout <<"NEXT\n\n";
	test_multibit_perf(vector_rules, vect,statistic);
	std::cout <<"NEXT\n\n";
	//tests_fib6_perf(vector_rules, vector_ips);
	//test_fib_perf();

    }

}
