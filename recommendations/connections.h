
   #if (PARTY==0)
    std::cout << "P_ZERO: " << PORT_P0_P1 << std::endl;

    boost::asio::io_context io_context;
    
    tcp::resolver resolver(io_context);
    tcp::socket s2(io_context);
    boost::asio::connect(s2, resolver.resolve({host2,
    std::to_string(PORT_P0_P2)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);
    
    tcp::socket s2_a(io_context);
    boost::asio::connect(s2_a, resolver.resolve({host2,
    std::to_string(PORT_P0_P2_a)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);

    tcp::socket s2_b(io_context);
    boost::asio::connect(s2_b, resolver.resolve({host2,
    std::to_string(PORT_P0_P2_b)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);

    tcp::socket s2_c(io_context);
    boost::asio::connect(s2_c, resolver.resolve({host2,
    std::to_string(PORT_P0_P2_c)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);

    tcp::socket s2_d(io_context);
    boost::asio::connect(s2_d, resolver.resolve({host2,
    std::to_string(PORT_P0_P2_d)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);

    tcp::socket s2_e(io_context);
    boost::asio::connect(s2_e, resolver.resolve({host2,
    std::to_string(PORT_P0_P2_e)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);

    tcp::socket s2_f(io_context);
    boost::asio::connect(s2_f, resolver.resolve({host2,
    std::to_string(PORT_P0_P2_f)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);


    tcp::socket s3(io_context);
    boost::asio::connect(s3, resolver.resolve({host3,
    std::to_string(PORT_P0_P3)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000);

    tcp::socket s3_a(io_context);
    boost::asio::connect(s3_a, resolver.resolve({host3,
    std::to_string(PORT_P0_P3_a)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000);

    tcp::socket s3_b(io_context);
    boost::asio::connect(s3_b, resolver.resolve({host3,
    std::to_string(PORT_P0_P3_b)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000);

        tcp::socket s3_c(io_context);
    boost::asio::connect(s3_c, resolver.resolve({host3,
    std::to_string(PORT_P0_P3_c)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000);

    tcp::socket sb(io_context);
    boost::asio::connect(sb, resolver.resolve({host1,
    std::to_string(PORT_P1_P0)}));
    std::cout << "P0: [Established connection P1 (1)]" << std::endl;
    usleep(20000);

    tcp::socket sb_a(io_context);
    boost::asio::connect(sb_a, resolver.resolve({host1,
    std::to_string(PORT_P1_P0_a)}));
    std::cout << "P0: [Established connection P1 (2)]" << std::endl;
    usleep(20000);
    
    tcp::socket sb_b(io_context);
    boost::asio::connect(sb_b, resolver.resolve({host1,
    std::to_string(PORT_P1_P0_b)}));
    std::cout << "P0: [Established connection P1 (2)]" << std::endl;
    usleep(20000);

    tcp::socket sb_c(io_context);
    boost::asio::connect(sb_c, resolver.resolve({host1,
    std::to_string(PORT_P1_P0_c)}));
    std::cout << "P0: [Established connection P1 (2)]" << std::endl;
    usleep(20000);

    tcp::socket sb_d(io_context);
    boost::asio::connect(sb_d, resolver.resolve({host1,
    std::to_string(PORT_P1_P0_d)}));
    std::cout << "P0: [Established connection P1 (2)]" << std::endl;
    usleep(20000);

    tcp::socket sb_e(io_context);
    boost::asio::connect(sb_e, resolver.resolve({host1,
    std::to_string(PORT_P1_P0_e)}));
    std::cout << "P0: [Established connection P1 (2)]" << std::endl;
    usleep(20000);

    tcp::socket sb_f(io_context);
    boost::asio::connect(sb_f, resolver.resolve({host1,
    std::to_string(PORT_P1_P0_f)}));
    std::cout << "P0: [Established connection P1 (2)]" << std::endl;
    usleep(20000);

    tcp::socket sb_g(io_context);
    boost::asio::connect(sb_g, resolver.resolve({host1,
    std::to_string(PORT_P1_P0_g)}));
    std::cout << "P0: [Established connection P1 (2)]" << std::endl;
    usleep(20000);
 
  
  #endif



  #if (PARTY==1)  
    std::cout << "P_ONE: " << PORT_P1_P0 << std::endl;
    boost::asio::io_context io_context;
     ssl::context ssl_context(ssl::context::tls);

    usleep(20000);
    tcp::resolver resolver(io_context);
    

    // ssl_socket s2(io_context, ssl_context);
    // auto endpoints = resolver.resolve(host2, std::to_string(PORT_P1_P2));
    // boost::asio::connect(s2.next_layer(), endpoints);

    tcp::socket s2(io_context);
    boost::asio::connect(s2, resolver.resolve({host2,
    std::to_string(PORT_P1_P2)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
     usleep(20000); 

    tcp::socket s2_a(io_context);
    boost::asio::connect(s2_a, resolver.resolve({host2,
    std::to_string(PORT_P1_P2_a)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000); 

    tcp::socket s2_b(io_context);
    boost::asio::connect(s2_b, resolver.resolve({host2,
    std::to_string(PORT_P1_P2_b)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000); 
    
    tcp::socket s2_c(io_context);
    boost::asio::connect(s2_c, resolver.resolve({host2,
    std::to_string(PORT_P1_P2_c)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000); 

    tcp::socket s2_d(io_context);
    boost::asio::connect(s2_d, resolver.resolve({host2,
    std::to_string(PORT_P1_P2_d)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000); 

    tcp::socket s2_e(io_context);
    boost::asio::connect(s2_e, resolver.resolve({host2,
    std::to_string(PORT_P1_P2_e)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000); 

    tcp::socket s2_f(io_context);
    boost::asio::connect(s2_f, resolver.resolve({host2,
    std::to_string(PORT_P1_P2_f)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000); 

    tcp::socket s3(io_context);
    boost::asio::connect(s3, resolver.resolve({host3,
    std::to_string(PORT_P1_P3)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000); 
    
    tcp::socket s3_a(io_context);
    boost::asio::connect(s3_a, resolver.resolve({host3,
    std::to_string(PORT_P1_P3_a)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000); 
    
    tcp::socket s3_b(io_context);
    boost::asio::connect(s3_b, resolver.resolve({host3,
    std::to_string(PORT_P1_P3_b)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000); 
    
    tcp::socket s3_c(io_context);
    boost::asio::connect(s3_c, resolver.resolve({host3,
    std::to_string(PORT_P1_P3_c)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000); 




    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0));
    std::cout << " --- > " << std::endl;
    tcp::socket sb(acceptor.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0 << std::endl;

    tcp::acceptor acceptor_a(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_a));
    std::cout << " --- > " << std::endl;
    tcp::socket sb_a(acceptor_a.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_a << std::endl;
    

    tcp::acceptor acceptor_b(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_b));
    std::cout << " --- > " << std::endl;
    tcp::socket sb_b(acceptor_b.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_b << std::endl;

    tcp::acceptor acceptor_c(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_c));
    std::cout << " --- > " << std::endl;
    tcp::socket sb_c(acceptor_c.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_c << std::endl;

    tcp::acceptor acceptor_d(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_d));
    std::cout << " --- > " << std::endl;
    tcp::socket sb_d(acceptor_d.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_d << std::endl;

    tcp::acceptor acceptor_e(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_e));
    std::cout << " --- > " << std::endl;
    tcp::socket sb_e(acceptor_e.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_e << std::endl;

    tcp::acceptor acceptor_f(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_f));
    std::cout << " --- > " << std::endl;
    tcp::socket sb_f(acceptor_f.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_f << std::endl;

    tcp::acceptor acceptor_g(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_g));
    std::cout << " --- > " << std::endl;
    tcp::socket sb_g(acceptor_g.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_g << std::endl;

   #endif
