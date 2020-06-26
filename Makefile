CXX=g++
CXXFLAGS=--std=c++17 -march=native -O3 -g -Wno-ignored-attributes  -fopenmp -DBOOST_ERROR_CODE_HEADER_ONLY #-DDEBUG #-DVERBOSE
LIBS=-lbsd -lboost_system -lboost_thread -pthread

default: p0 p1 p2 p3 gen_items gen_users gen_queries

p0: pB.cpp pB.h dpf.h prg.h fixed.h datatypes.h sanitycheck.h connections.h 4P-mux.h common.h i-precision-reduction.h u-precision-reduction.h
	$(CXX) $(CXXFLAGS) -o p0 pB.cpp $(LIBS) -DPARTY=0 -I.

p1: pB.cpp pB.h dpf.h prg.h fixed.h datatypes.h sanitycheck.h connections.h 4P-mux.h common.h i-precision-reduction.h u-precision-reduction.h
	touch updated_uprofiles.0
	touch updated_uprofiles.1
	touch updated_iprofiles.0
	touch updated_iprofiles.1
	$(CXX) $(CXXFLAGS) -o p1 pB.cpp $(LIBS) -DPARTY=1 -I.

p2: p2.cpp network.h prg.h aes.h fixed.h datatypes.h sanitycheck.h connections.h 4P-mux.h common.h i-precision-reduction.h u-precision-reduction.h
	$(CXX) $(CXXFLAGS) -o p2 p2.cpp $(LIBS) -I.

p3: p3.cpp network.h prg.h aes.h fixed.h datatypes.h sanitycheck.h connections.h 4P-mux.h common.h
	$(CXX) $(CXXFLAGS) -o p3 p3.cpp $(LIBS) -I.

nonprivate: nonprivate.cpp datatypesnp.h fixed-np.h
	$(CXX) $(CXXFLAGS) -o nonpriv nonprivate.cpp $(LIBS) -I.

gen_items: gen_items.cpp datatypes.h fixed.h common.h
	touch iprofiles
	touch iprofiles.0
	touch iprofiles.1
	touch iprofilesA
	$(CXX) $(CXXFLAGS) -o gen_items gen_items.cpp $(LIBS)

gen_users: gen_users.cpp datatypes.h fixed.h common.h
	touch uprofiles
	touch uprofilesA
	touch uprofiles.0
	touch uprofiles.1
	$(CXX) $(CXXFLAGS) -o gen_users gen_users.cpp $(LIBS)

gen_queries: gen_queries.cpp rand_queries.cpp datatypes.h fixed.h dpf.h common.h
	touch queries
	touch queries.0
	touch queries.1
	$(CXX) $(CXXFLAGS) -o gen_queries gen_queries.cpp $(LIBS)
	$(CXX) $(CXXFLAGS) -o rand_queries rand_queries.cpp $(LIBS)

gen_queries_bad: gen_queries_bad.cpp rand_queries.cpp datatypes.h fixed.h dpf.h common.h
	$(CXX) $(CXXFLAGS) -o gen_queries_bad gen_queries_bad.cpp $(LIBS)

clean:
	rm -f p0 p1 p2 p3 gen_items gen_users gen_queries

kill:
	pkill p2 & pkill p1 & pkill p0 & pkill p3

