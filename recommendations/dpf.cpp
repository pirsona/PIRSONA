#include <type_traits>
#include <set>
#include <vector>
#include "dpf.h"

using namespace dpf;

typedef char leaf_t;
typedef __m128i node_t;
typedef AES_KEY prgkey_t;

int main(int argc, char * argv[])
{
	prgkey_t prgkey;

	const size_t nitems = 1ULL << 26;
	const size_t target = atoi(argv[1]);
	const leaf_t val = 0x12;//_mm_set1_epi8(0x12);

	//auto [dpfkey0, dpfkey1] = dpf_key<leaf_t, node_t, prgkey_t>::gen(prgkey, nitems, target, val, &dpf_key<leaf_t, node_t, prgkey_t>::make_shared_finalizer);
	auto [dpfkey0, dpfkey1] = dpf_key<leaf_t, node_t, prgkey_t>::gen(prgkey, nitems, target, val);

	leaf_t * output0 = (leaf_t *)std::aligned_alloc(sizeof(node_t), dpfkey0.full_bytes());
	leaf_t * output1 = (leaf_t *)std::aligned_alloc(sizeof(node_t), dpfkey1.full_bytes());

	dpfkey0.evalfull(output0);

	dpfkey1.evalfull(output1);

	dpf_key<leaf_t, node_t, prgkey_t>::block_t x;

	std::set<size_t> s;
	s.insert(5);
	s.insert(10);
	s.insert(31337);
	s.insert(10000);

	auto v0 = dpfkey0.evallist(std::cbegin(s), std::cend(s));
	auto v1 = dpfkey1.evallist(std::cbegin(s), std::cend(s));

	for (auto x : v0)
	{
		printf("%lu\n", x);
	}
	for (auto [val,input] : dpfkey0)
	{
		printf("%lu->%u\n", input, val);
	}

	for (auto [val,input] : dpfkey0.filtered_by(s.begin(), s.end()))
	{
		printf(":::%lu->%u\n", input, val);
	}

	free(output0);
	free(output1);

	return 0;
}
