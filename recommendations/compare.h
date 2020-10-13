#define DIM 8

constexpr uint64_t precision = 16;
constexpr uint64_t fracpart = 6;
constexpr uint64_t intpart = 16;
constexpr uint64_t nitems = 1ULL << (intpart+fracpart+1);
constexpr uint64_t scale = 1ULL << precision;
constexpr uint64_t scale2 = scale * scale;

 
const size_t nrecords  = 9000;
const size_t nprefixes = std::ceil(std::log2(nrecords));

const size_t depth = 16;//dpfkey1.depth(nitems);  
const size_t nodes_per_leaf = 1;// dpfkey1.nodes_per_leaf;

 
struct dpfkey_struct
 {

 	size_t dpfnitems;
 	size_t dpfdepth;
 	node_t cw[depth]; 	
 	node_t root;
 	std::array<node_t, nodes_per_leaf> finalizer;
	static constexpr size_t size = sizeof(std::array<node_t, nodes_per_leaf>) + 
							      ((depth + 1) * sizeof(node_t)) + 2 * sizeof(size_t);
 }; 
 
inline constexpr uint64_t extract_bits(uint64_t input, uint64_t lo, uint64_t hi)
{
	return (input & ((1ULL << hi) - 1)) >> lo;
}


 dpfkey_struct    * carry_dpf_read          = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 dpfkey_struct    * carry_dpf_read_p3          = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));

 dpfkey_struct    * hi_dpf_read          = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 dpfkey_struct    * lo_dpf_read          = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 dpfkey_struct    * dpfkey_written_b	 = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 
 dpfkey_struct    * carry_dpf_written_other = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 dpfkey_struct    * carry_dpf_written_b     = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 dpfkey_struct    * carry_dpf_read_b     = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));


 dpfkey_struct    * hi_dpf_written_b = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 dpfkey_struct    * hi_dpf_written_other = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 dpfkey_struct    * hi_dpf_read_b = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));

 dpfkey_struct    * lo_dpf_written_b = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 dpfkey_struct    * lo_dpf_read_b = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));
 dpfkey_struct    * lo_dpf_written_other = (dpfkey_struct *) std::aligned_alloc(sizeof(__m256i), nrecords * sizeof(dpfkey_struct));