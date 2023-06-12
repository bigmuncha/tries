#include "test/test_b_trie.h"
#include <ipcreator.h>
#include <lpm6_data.h>
#include <rte_eal.h>
#include <rte_common.h>
#include "test/dir24_8_lpmv4_perf.h"

int main (int argc, char **argv)
{
    int ret;
    ret = rte_eal_init(argc, argv);
    if (ret < 0)
	rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");

    if(test2() == 0)
	std::cout << "Success\n";
    else
	std:: cout << "failure\n";

    /* test_lpm6_perf(); */
    /* test_fib6_perf(); */
    //test_fib_perf();
}
