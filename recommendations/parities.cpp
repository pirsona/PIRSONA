#include <assert.h>
#include <bsd/stdlib.h>
#include "dpf.h"
#include <chrono>
using namespace std::chrono;

constexpr uint64_t precision = 16;
constexpr uint64_t fracpart = 3;
constexpr uint64_t intpart = 16;
constexpr uint64_t scale = 1ULL << precision;
constexpr uint64_t scale2 = scale * scale;

constexpr size_t nrecords = 9000;

using namespace dpf;

inline constexpr uint64_t extract_bits(uint64_t input, uint64_t lo, uint64_t hi)
{
	return (input & ((1ULL << hi) - 1)) >> lo;
}

inline constexpr uint64_t append_bits(uint64_t input, uint64_t bit, size_t lowbits)
{
	return (input | (bit >> (lowbits + 1)));
}

int main(int argc, char * argv[])
{
	   

	   AES_KEY prgkey;
 

		size_t prefixlen = 6;
	    // construct sharing of a
	    uint64_t a0, a1, a = atof(argv[1]) * scale2;
	    

	    arc4random_buf(&a0, sizeof(a0));
	    a1 = a - a0;
	    assert(a == a0+a1);

	    // construct sharing of b
	    uint64_t b0, b1, b = atof(argv[2]) * scale2;
	    arc4random_buf(&b0, sizeof(b0));
	    b1 = b - b0;
	    assert(b == b0+b1);


	    printf("AA = %d = %d + %d\n", a, a0, a1);

	    std::cout << "Does `a` have an m-bit zero prefix? " << (extract_bits(a, 2*precision+intpart-prefixlen, 2*precision+intpart) == 0 ? "YES" : "NO") << std::endl;


	    double r0[nrecords];
		double r1[nrecords];

		double R0[nrecords];
		double R1[nrecords];

		uint64_t R64_0[nrecords];
		uint64_t R64_1[nrecords];

		uint64_t uv0[nrecords];
		uint64_t uv1[nrecords];
		
		for(size_t j = 0; j < nrecords; ++j)
		{
			double r_ = ((double) rand() / (RAND_MAX)) * 100;
			
			int64_t uv = r_ * scale2;
			// arc4random_buf(&uv, sizeof(int64_t));
			// //printf("%d \n", uv);
			// uv = a; 
			arc4random_buf(&uv0[j], sizeof(uint64_t));
			uv1[j] = uv - uv0[j]; 
			
			//printf("%d <-> %d\n\n", uv, uv0[j] + uv1[j]);

			R0[j] = 0.0;
			R1[j] = 0.0;
			
			double r0_ = ((double) rand() / (RAND_MAX)) * 100;
			r0[j] = r0_;

			double r1_ = ((double) rand() / (RAND_MAX));
			r1[j] = r1_;	

			uint64_t RR = r0_ * scale2;
 			arc4random_buf(&R64_0[j], sizeof(R64_0[j]));
	   		R64_1[j] = RR - R64_0[j];

		}

 

		for(size_t j = 0; j < nrecords; ++j)
		{	
			for(size_t i = 0; i < j; ++i)
			{ 
				R0[j] += r0[i]; 				
				R1[j] += r1[i];
			}
		}

		for(size_t j = 0; j < nrecords; ++j)
		{
			R64_0[j] = R0[j] * scale2;
			R64_1[j] = R1[j] * scale2;
		}


		// P2 choses a random number
		double r20 = ((double) rand() / (RAND_MAX));
		uint64_t R20 = r20 * R64_0[nrecords-1];



		double r21 = ((double) rand() / (RAND_MAX));
		uint64_t R21 = r21 * R64_1[nrecords-1]; 


		std::cout << "R20 = " << R20 << std::endl;
		std::cout << "R21 = " << R21 << std::endl << std::endl;
		for(size_t j = 0; j < nrecords-2; ++j)
		{
			assert(R64_0[j] < R64_0[j+1]);
			assert(R64_1[j] < R64_1[j+1]);

			printf("R64_0[j] = %llu \n", R64_0[j]);
			printf("R64_1[j] = %llu \n\n", R64_1[j]);
		}
		a0 = R64_0[3];
		a1 = R64_1[3];
 
 		a = a0 + a1;


 
 	
 		int8_t eval_vector0[nrecords];
		int8_t eval_vector1[nrecords];
		int64_t eval64_vector0[nrecords];
		int64_t eval64_vector1[nrecords];
 
 
 		for(size_t j = 0; j < nrecords; ++j)
 		{
 			uint8_t x;	
 			arc4random_buf(&x, sizeof(uint8_t));
 			x = x % 2;
 			eval_vector0[j] = 1;
 			if(x == 0) eval_vector0[j] = 0;
 			 
 			arc4random_buf(&x, sizeof(uint8_t));
 			x = x % 2;
 			eval_vector1[j] = 1;
 			if(x == 0) eval_vector1[j] = 0;
 		}

 
		for(size_t j = 0; j < nrecords; ++j)
 		{
	 	   eval64_vector0[j] =  eval_vector0[j];
		   eval64_vector1[j] = -eval_vector1[j];
 		}

 		//P2s blinding factors.

 		uint64_t  blinds0[2][nrecords];
 		uint64_t blinds1[2][nrecords];
 		uint64_t alpha0[nrecords];
 		uint64_t alpha1[nrecords];




 		for(size_t j = 0; j < nrecords; ++j)
 		{
 			arc4random_buf(&blinds0[0][j], sizeof(uint64_t));
 			arc4random_buf(&blinds1[0][j], sizeof(uint64_t));

 			arc4random_buf(&blinds0[1][j], sizeof(uint64_t));
 			arc4random_buf(&blinds1[1][j], sizeof(uint64_t));

 			alpha0[j] = blinds0[0][j] * blinds1[1][j];
 			alpha1[j] = blinds1[0][j] * blinds0[1][j];
 		}

 	
 	auto start_gen = high_resolution_clock::now();	

 	for(size_t tt = 0; tt < nrecords; ++tt)
 	{	
		int8_t sq_blinds0[2][nrecords];
 		int8_t sq_blinds1[2][nrecords];
 		int8_t sq_alpha0[nrecords];
 		int8_t sq_alpha1[nrecords];
 	    
 	    int64_t out0[nrecords];
 		int64_t out1[nrecords];
 	    
 	    for(size_t j = 0; j < nrecords; ++j)
 		{
 			int8_t x0, x1, X0, X1; 
 			int8_t y0, y1, Y0, Y1;

 			x0 =  eval_vector0[j];
 			x1 = -eval_vector1[j];

			y0 =  eval_vector0[j];
 			y1 = -eval_vector1[j];

 			X0 = sq_blinds1[0][j];
 			X1 = sq_blinds1[1][j];


 			Y0 = sq_blinds0[0][j];
 			Y1 = sq_blinds0[1][j];

 			sq_alpha0[j] = X0 * Y1;
 			sq_alpha1[j] = X1 * Y0;
 
	 		
 

 			int8_t z0 = x0 * (y0 + (y1 + Y1)) - (Y0 * (x1 + X1)) + sq_alpha0[j];
 			int8_t z1 = x1 * (y1 + (y0 + Y0)) - (Y1 * (x0 + X0)) + sq_alpha1[j];

 			out0[j] = z0;
 			out1[j] = z1;

 			int8_t zz = z0 + z1;
 			//printf("zz = %d\n", zz);

 		}



 		for(size_t j = 0; j < nrecords; ++j)
 		{
 			uint64_t x0, x1, X0, X1; 
 			uint64_t y0, y1, Y0, Y1;

 			x0 = uv0[j];
 			x1 = uv1[j];
			y0 = out0[j];
 			y1 = out1[j];

 			X0 = blinds1[0][j];
 			X1 = blinds1[1][j];


 			Y0 = blinds0[0][j];
 			Y1 = blinds0[1][j];

 			alpha0[j] = X0 * Y1;
 			alpha1[j] = X1 * Y0;
 
 			int64_t z0 = x0 * (y0 + (y1 + Y1)) - (Y0 * (x1 + X1)) + alpha0[j];
 			int64_t z1 = x1 * (y1 + (y0 + Y0)) - (Y1 * (x0 + X0)) + alpha1[j];


 			int64_t z = z0 + z1;  		 
 			uint64_t r = uv0[j] + uv1[j];
 			//printf("z = %d\n", z );  
 		}
 	}

   	 auto stop_gen = high_resolution_clock::now();   	
  	 size_t duration_gen = duration_cast<milliseconds>(stop_gen - start_gen).count();

 	 printf("gentime =  %zu\n", duration_gen);



auto start_comp = high_resolution_clock::now();	
 	   for(size_t jj = 0; jj < 9000; ++jj)
	   {
 
	     // extract prefixlen msbs of intpart
	    uint64_t a_hi[2] = {
	        extract_bits(a0, 2*precision+intpart-prefixlen, 2*precision+intpart),
	        extract_bits(a1, 2*precision+intpart-prefixlen, 2*precision+intpart)
	    };

	    // extract all-but-prefixlen msbs of intpart 
	    uint64_t a_lo[2] = {
	        extract_bits(a0, 2*precision-fracpart, 2*precision+intpart-prefixlen), 
	        extract_bits(a1, 2*precision-fracpart, 2*precision+intpart-prefixlen)
	    };

	    std::cout << " ---> " << 2*precision+intpart-prefixlen << " " << 2*precision+intpart << std::endl;
	    std::cout << " ---> " << 2*precision+intpart-prefixlen << " " << 2*precision-fracpart << std::endl;

	    printf("hi = %d <-> %d\n", a_hi[0], a_hi[1]);
		printf("lo = %d <-> %d\n\n", a_lo[0], a_lo[1]);
	    
	    std::cout << "a0 = " << a0 << std::endl;
	    std::cout << "a_lo[2] = " << a_lo[0] << std::endl;
	    std::cout << 2*precision-fracpart << " - " << 2*precision+intpart-prefixlen << std::endl;
     
     	uint64_t c0 = 0, c2 = 0;
 
	    { 
	    	// RAII scope for carry dpf
    		size_t alloc_size;
    		__m256i *query0, *query2;

    		// P1 samples the following keys, sends to P0 and P2
   			size_t carrydpf_nitems = 1ULL << (intpart+fracpart-prefixlen+1);
    		auto [carrydpf0, carrydpf2] = dpf_key<>::gen(prgkey, carrydpf_nitems, a_lo[1]);
 			if (a_lo[0] < carrydpf_nitems/2)
		    {
		        uint64_t from = carrydpf_nitems   - a_lo[0];
		        uint64_t to   = carrydpf_nitems/2 - a_lo[0];

		        auto alloc_size0 = dpf_key<>::interval_bytes(0, to);
		        auto alloc_size1 = dpf_key<>::interval_bytes(from, carrydpf_nitems-1);
		        alloc_size = alloc_size0 + alloc_size1;

		        query0 = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
		        carrydpf0.evalinterval(0, to, (bool*)query0, NULL, true);
		        carrydpf0.evalinterval(from, carrydpf_nitems-1, ((bool*)query0) + alloc_size0, NULL, true);

		        query2 = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
		        carrydpf2.evalinterval(0, to, (bool*)query2, NULL, true);
		        carrydpf2.evalinterval(from, carrydpf_nitems-1, ((bool*)query2) + alloc_size0, NULL, true);
			}   		
	   		else
	    	{
	        	uint64_t from = carrydpf_nitems - a_lo[0];
	        	uint64_t to = from + carrydpf_nitems/2;

	       		alloc_size = dpf_key<>::interval_bytes(from, to);

	       	 	query0 = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
	        	carrydpf0.evalinterval(from, to, (bool*)query0, NULL, true);

	        	query2 = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
	        	carrydpf2.evalinterval(from, to, (bool*)query2, NULL, true);
	    	}
 
		    for (size_t i = 0; i < alloc_size / sizeof(__m256i); ++i)
		    {
		        c0 += (__builtin_parityll(query0[i][0]) + __builtin_parityll(query0[i][1])  + __builtin_parityll(query0[i][2])  + __builtin_parityll(query0[i][3]));
		        c2 -= (__builtin_parityll(query2[i][0]) + __builtin_parityll(query2[i][1])  + __builtin_parityll(query2[i][2])  + __builtin_parityll(query2[i][3]));
		    }
		    if (alloc_size % sizeof(__m256i) > 0)
		    {
		        c0 += ( __builtin_parityll(query0[alloc_size/sizeof(__m256i)][0]) + __builtin_parityll(query0[alloc_size/sizeof(__m256i)][1]));
		        c2 -= ( __builtin_parityll(query2[alloc_size/sizeof(__m256i)][0]) + __builtin_parityll(query2[alloc_size/sizeof(__m256i)][1]));
		    }

    	free(query0);
    	free(query2);
    } // RAII scope for carry dpf

    // Du-Attlah values for conditionally flipping sign of carry bit
    // P1 chooses these; sends C0 to P0 and C2 to P2
    uint64_t C0, C2;
    arc4random_buf(&C0, sizeof(C0));
    arc4random_buf(&C2, sizeof(C2));
    uint64_t alpha0, alpha2;
    arc4random_buf(&alpha0, sizeof(alpha0));
    arc4random_buf(&alpha2, sizeof(alpha2));
    uint64_t d0 = C0*C2+alpha0, d2 = C0*C2+alpha2;

    auto carry = (c0*(c0+(c2+C2))-C0*(c2+C2)+d0) + (c2*(c2+(c0+C0))-C2*(c0+C0)+d2);
    assert(carry-alpha0-alpha2 == 0 || carry-alpha0-alpha2 == 1);

    bool r0 = arc4random_uniform(2), r2 = arc4random_uniform(2);
    bool prefixbit0, prefixbit1 = r0 ^ r2;


    { // RAII for a_hi dpf
	    size_t hidpf_nitems = 1ULL << prefixlen;
	    uint64_t target = -(a_hi[1] + alpha0 + alpha2) & (hidpf_nitems-1);

	    std::cout << "target  = " << target << std::endl;

	    auto [hidpf0, hidpf2] = dpf_key<>::gen(prgkey, hidpf_nitems, target);

	    auto evalat = (a_hi[0] + carry) & (hidpf_nitems-1);
	    bool b0 = r0 ^ hidpf0.eval(evalat);
	    bool b2 = r2 ^ hidpf2.eval(evalat);
	    prefixbit0 = b0 ^ b2;
	    //std::cout << "carry = " << carry << std::endl;
	    //std::cout << "hi = " << ((a_hi[0] + a_hi[1] +carry)&(hidpf_nitems-1)) << std::endl;
	    //std::cout << "target = " << target << "; evaling at = " << evalat << std::endl;
	    //std::cout << "b0 ^ b2 = " << b0 << " ^ " << b2 << " = " << (b0^b2) << std::endl;
	    //std::cout << "r0 ^ r2 = " << r0 << " ^ " << r2 << " = " << (r0^r2) << std::endl;
    } // RAII for a_hi dpf 
	

    {	//RAII for a_lo dpf
	
        auto diff0 =  extract_bits(a0 - b0, 2*precision-fracpart, 2*precision+intpart-prefixlen+1); 
        auto diff1 =  extract_bits(a1 - b1, 2*precision-fracpart, 2*precision+intpart-prefixlen+1);
	    
	    size_t lodpf_nitems = (1ULL << fracpart+intpart-prefixlen+2);
	   	
	   	auto target = diff1 | (prefixbit1 << (fracpart+intpart-prefixlen + 1));
	
		std::cout << "a_lo = " << std::bitset<64>(a_lo[1]) << std::endl;
		std::cout << "prefixbit1 = " << prefixbit1 << std::endl;
		std::cout << "prefixbit0 = " << prefixbit0 << std::endl;
		std::cout << "target = " << std::bitset<64>(target) << std::endl;

		auto [lodpf0, lodpf2] = dpf_key<>::gen(prgkey, lodpf_nitems, target);
	    
	    auto prefix0 = (prefixbit0 << (fracpart + intpart - prefixlen + 1));

	   
	   uint64_t alloc_size = 0;
	
		std::cout << "lodpf_nitems = " << lodpf_nitems << std::endl;	
	  	std::cout << "target = " << target << std::endl;

 		uint64_t offset = 0;

 		if(prefixbit1 == 1) offset = lodpf_nitems/2;
 		std::cout << "offset = " << offset << std::endl;
 		std::cout << "diff0 = " << diff0 << std::endl;
	   if(diff0 < lodpf_nitems/4)
	   {

	   		std::cout << "\n\nif (diff0 < lodpf_nitems/2)\n\n";

	   		uint64_t from = lodpf_nitems/2 - diff0;
			uint64_t to = lodpf_nitems/4 - diff0;

			// uint64_t alloc_size0 = dpf_key<>::interval_bytes(0, to);
			// uint64_t alloc_size1 = dpf_key<>::interval_bytes(from, lodpf_nitems-1);

            uint64_t alloc_size0 = dpf_key<>::interval_bytes(0+offset, to+offset);
            uint64_t alloc_size1 = dpf_key<>::interval_bytes(from+offset, lodpf_nitems/2-1+offset);

			alloc_size = alloc_size0 + alloc_size1;

			// do evalintervals

			std::cout << "first evalinterval " << 0  + offset << " -> " << to  + offset <<  std::endl;
			std::cout << "second evalinterval = " << from + offset  << " -> " << lodpf_nitems/2  + offset <<  std::endl;
			std::cout << "alloc_size = " << alloc_size << std::endl;	

			auto query0 = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
 
			lodpf0.evalinterval(0 + offset, to + offset, (bool*)query0, NULL, true);
			lodpf0.evalinterval(from + offset, lodpf_nitems/2-1 + offset, ((bool*)query0) + alloc_size0, NULL, true);

			// lodpf0.evalinterval(prefix0, prefix0|to, (bool*)query0, NULL, true);
			// lodpf0.evalinterval(from, prefix0|(lodpf_nitems-1), ((bool*)query0) + alloc_size0, NULL, true);

			auto query2 = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));

			lodpf2.evalinterval(0 + offset, to + offset, (bool*)query2, NULL, true);
		    lodpf2.evalinterval(from + offset, lodpf_nitems/2-1 + offset, ((bool*)query2) + alloc_size0, NULL, true);

			// lodpf2 .evalinterval(prefix0, prefix0|to, (bool*)query2, NULL, true);
			// lodpf2 .evalinterval(from, prefix0|(lodpf_nitems-1), ((bool*)query2) + alloc_size0, NULL, true);

			// compute the parities
			for (int i = 1; i < alloc_size / sizeof(__m256i); ++i)
			{
				query0[0] ^= query0[i];
				query2[0] ^= query2[i];
			}
			if (alloc_size % sizeof(__m256i) > 0)
			{
				query0[0][0] ^= query0[alloc_size/sizeof(__m256i)][0] ^ query0[alloc_size/sizeof(__m256i)][1];
				query2[0][0] ^= query2[alloc_size/sizeof(__m256i)][0] ^ query2[alloc_size/sizeof(__m256i)][1];
			}
			auto c = __builtin_parityll(query0[0][0]^query0[0][1]^query0[0][2]^query0[0][3]) ^ __builtin_parityll(query2[0][0]^query2[0][1]^query2[0][2]^query2[0][3]);

			auto x = __builtin_parityll(query0[0][0]^query0[0][1]^query0[0][2]^query0[0][3]);
			auto y = __builtin_parityll(query2[0][0]^query2[0][1]^query2[0][2]^query2[0][3]);



			if(a > b)	assert( (x + y)  == 1);		
			if(a < b)	assert( (x + y)%2  == 0);

			// std::cout << "x = " << x << std::endl;
			// std::cout << "y = " << y << std::endl;
			
	   }
 	   else
 	   {

 	   		std::cout <<"\n\nelse\n\n";

 	   	 	uint64_t from = lodpf_nitems/2 - diff0;
			uint64_t to = from + lodpf_nitems/4;

			uint64_t alloc_size = dpf_key<>::interval_bytes(from, to);

			// do evalintervals
			auto query0 = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
			lodpf0.evalinterval(from + offset, to + offset, (bool*)query0, NULL, true);

			auto query2 = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
			lodpf2.evalinterval(from + offset, to + offset, (bool*)query2, NULL, true);

			// compute the parities
			for (int i = 1; i < alloc_size / sizeof(__m256i); ++i)
			{
				query0[0] ^= query0[i];
				query2[0] ^= query2[i];
			}
			if (alloc_size % sizeof(__m256i) > 0)
			{
				query0[0][0] ^= query0[alloc_size/sizeof(__m256i)][0] ^ query0[alloc_size/sizeof(__m256i)][1];
				query2[0][0] ^= query2[alloc_size/sizeof(__m256i)][0] ^ query2[alloc_size/sizeof(__m256i)][1];
			}

			auto c = __builtin_parityll(query0[0][0]^query0[0][1]^query0[0][2]^query0[0][3]) ^ __builtin_parityll(query2[0][0]^query2[0][1]^query2[0][2]^query2[0][3]);

			auto x = __builtin_parityll(query0[0][0]^query0[0][1]^query0[0][2]^query0[0][3]);
			auto y = __builtin_parityll(query2[0][0]^query2[0][1]^query2[0][2]^query2[0][3]);
		    
			if(a > b)	assert( (x + y)  == 1);			
			if(a < b)	assert( (x + y)%2  == 0);

			// std::cout << "x = " << x << std::endl;
			// std::cout << "y = " << y << std::endl;

 	   }

	} //RAII for lo dpf

}

   	 auto stop_comp = high_resolution_clock::now();   	
  	 size_t duration_comp = duration_cast<milliseconds>(stop_comp - start_comp ).count();

 	 printf("gen_comp =  %zu\n", duration_comp);
 
	return 0;
}

