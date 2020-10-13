
   #if (PARTY==0)
    std::cout << "P_ZERO: " << PORT_P0_P1 << std::endl;

    boost::asio::io_context io_context;
    boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);
    ssl_context.load_verify_file("server.crt");
    boost::asio::ssl::context ssl_contextb(boost::asio::ssl::context::sslv23);
    ssl_contextb.load_verify_file("server.crt");
    tcp::resolver resolver(io_context);


    
        ssl_socket s2(io_context, ssl_context);
    s2.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2 = resolver.resolve(host1, std::to_string(PORT_P0_P2));
    boost::asio::connect(s2.lowest_layer(), endpoints2);
    s2.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2(io_context);
    // boost::asio::connect(s2, resolver.resolve({host2,
    // std::to_string(PORT_P0_P2)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);


    ssl_socket s2_a(io_context, ssl_context);
    s2_a.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_a = resolver.resolve(host1, std::to_string(PORT_P0_P2_a));
    boost::asio::connect(s2_a.lowest_layer(), endpoints2_a);
    s2_a.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_a(io_context);
    // boost::asio::connect(s2_a, resolver.resolve({host2,
    // std::to_string(PORT_P0_P2_a)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);


    ssl_socket s2_b(io_context, ssl_context);
    s2_b.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_b = resolver.resolve(host1, std::to_string(PORT_P0_P2_b));
    boost::asio::connect(s2_b.lowest_layer(), endpoints2_b);
    s2_b.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_b(io_context);
    // boost::asio::connect(s2_b, resolver.resolve({host2,
    // std::to_string(PORT_P0_P2_b)}));
    // std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);

    ssl_socket s2_c(io_context, ssl_context);
    s2_c.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_c = resolver.resolve(host1, std::to_string(PORT_P0_P2_c));
    boost::asio::connect(s2_c.lowest_layer(), endpoints2_c);
    s2_c.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_c(io_context);
    // boost::asio::connect(s2_c, resolver.resolve({host2,
    // std::to_string(PORT_P0_P2_c)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);
    

    ssl_socket s2_d(io_context, ssl_context);
    s2_d.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_d = resolver.resolve(host1, std::to_string(PORT_P0_P2_d));
    boost::asio::connect(s2_d.lowest_layer(), endpoints2_d);
    s2_d.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_d(io_context);
    // boost::asio::connect(s2_d, resolver.resolve({host2,
    // std::to_string(PORT_P0_P2_d)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);


    ssl_socket s2_e(io_context, ssl_context);
    s2_e.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_e = resolver.resolve(host1, std::to_string(PORT_P0_P2_e));
    boost::asio::connect(s2_e.lowest_layer(), endpoints2_e);
    s2_e.handshake(boost::asio::ssl::stream_base::client);


    // tcp::socket s2_e(io_context);
    // boost::asio::connect(s2_e, resolver.resolve({host2,
    // std::to_string(PORT_P0_P2_e)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);

    ssl_socket s2_f(io_context, ssl_context);
    s2_f.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_f = resolver.resolve(host1, std::to_string(PORT_P0_P2_f));
    boost::asio::connect(s2_f.lowest_layer(), endpoints2_f);
    s2_f.handshake(boost::asio::ssl::stream_base::client);

    //  tcp::socket s2_f(io_context);
    // boost::asio::connect(s2_f, resolver.resolve({host2,
    // std::to_string(PORT_P0_P2_f)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);


    ssl_socket s2_g(io_context, ssl_context);
    s2_g.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_g = resolver.resolve(host1, std::to_string(PORT_P0_P2_g));
    boost::asio::connect(s2_g.lowest_layer(), endpoints2_g);
    s2_g.handshake(boost::asio::ssl::stream_base::client);

    //  tcp::socket s2_g(io_context);
    // boost::asio::connect(s2_g, resolver.resolve({host2,
    // std::to_string(PORT_P0_P2_g)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);

    ssl_socket s2_h(io_context, ssl_context);
    s2_h.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_h = resolver.resolve(host1, std::to_string(PORT_P0_P2_h));
    boost::asio::connect(s2_h.lowest_layer(), endpoints2_h);
    s2_h.handshake(boost::asio::ssl::stream_base::client);


    //      tcp::socket s2_h(io_context);
    // boost::asio::connect(s2_h, resolver.resolve({host2,
    // std::to_string(PORT_P0_P2_h)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;
    usleep(20000);



    ssl_socket s3(io_context, ssl_context);
    s3.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints3 = resolver.resolve(host1, std::to_string(PORT_P0_P3));
    boost::asio::connect(s3.lowest_layer(), endpoints3);
    s3.handshake(boost::asio::ssl::stream_base::client);

   //  ssl_socket s3(io_context, ssl_context);
   //  auto endpoints3 = resolver.resolve(host1, std::to_string(PORT_P0_P3));
   //  boost::asio::connect(s3.next_layer(), endpoints3);
   // s3.async_handshake(boost::asio::ssl::stream_base::client, [&] (boost::system::error_code error)
   //  {
   //  // Handshake is done
   //  });
    // tcp::socket s3(io_context);
    // boost::asio::connect(s3, resolver.resolve({host3,
    // std::to_string(PORT_P0_P3)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000);


    ssl_socket s3_a(io_context, ssl_context);
    s3_a.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints3_a = resolver.resolve(host1, std::to_string(PORT_P0_P3_a));
    boost::asio::connect(s3_a.lowest_layer(), endpoints3_a);
    s3_a.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s3_a(io_context);
    // boost::asio::connect(s3_a, resolver.resolve({host3,
    // std::to_string(PORT_P0_P3_a)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000);


    ssl_socket s3_b(io_context, ssl_context);
    s3_b.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints3_b = resolver.resolve(host1, std::to_string(PORT_P0_P3_b));
    boost::asio::connect(s3_b.lowest_layer(), endpoints3_b);
    s3_b.handshake(boost::asio::ssl::stream_base::client);


    // tcp::socket s3_b(io_context);
    // boost::asio::connect(s3_b, resolver.resolve({host3,
    // std::to_string(PORT_P0_P3_b)}));
    std::cout << "P0: [Established connection P3]" << std::endl;
    usleep(20000);
    

    ssl_socket sb(io_context, ssl_contextb);
    sb.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb = resolver.resolve(host1, std::to_string(PORT_P1_P0));
    boost::asio::connect(sb.lowest_layer(), endpointsb);
    sb.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket sb(io_context);
    // boost::asio::connect(sb, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0)}));
    //  std::cout << "P0: [Established connection P1 (1)]" << std::endl;
     usleep(20000);

    ssl_socket sb_a(io_context, ssl_contextb);
    sb_a.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_a = resolver.resolve(host1, std::to_string(PORT_P1_P0_a));
    boost::asio::connect(sb_a.lowest_layer(), endpointsb_a);
    sb_a.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket sb_a(io_context);
    // boost::asio::connect(sb_a, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_a)}));
    std::cout << "P0: [Established connection P1 (2)]" << std::endl;
    usleep(20000);
    
    tcp::socket sb_b(io_context);
    boost::asio::connect(sb_b, resolver.resolve({host1,
    std::to_string(PORT_P1_P0_b)}));
    std::cout << "P0: [Established connection P1 (3)]" << std::endl;
    usleep(20000);



    ssl_socket sb_c(io_context, ssl_contextb);
    sb_c.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_c = resolver.resolve(host1, std::to_string(PORT_P1_P0_c));
    boost::asio::connect(sb_c.lowest_layer(), endpointsb_c);
    sb_c.handshake(boost::asio::ssl::stream_base::client);

 
    // tcp::socket sb_c(io_context);
    // boost::asio::connect(sb_c, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_c)}));
    
    std::cout << "P0: [Established connection P1 (4)]" << std::endl;
    usleep(20000);



    ssl_socket sb_d(io_context, ssl_contextb);
    sb_d.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_d = resolver.resolve(host1, std::to_string(PORT_P1_P0_d));
    boost::asio::connect(sb_d.lowest_layer(), endpointsb_d);
    sb_d.handshake(boost::asio::ssl::stream_base::client);

    // ssl_socket sb_d(io_context, ssl_context);
    // auto endpoints_d = resolver.resolve(host1, std::to_string(PORT_P1_P0_d));
    // boost::asio::connect(sb_d.next_layer(), endpoints_d);
    // sb_d.async_handshake(boost::asio::ssl::stream_base::client, [&] (boost::system::error_code error)
    // {
    // // Handshake is done
    // });
    
    // tcp::socket sb_d(io_context);
    // boost::asio::connect(sb_d, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_d)}));
    // std::cout << "P0: [Established connection P1 (5)]" << std::endl;
    usleep(20000);


    // ssl_socket sb_e(io_context, ssl_context);
    // auto endpoints_e = resolver.resolve(host1, std::to_string(PORT_P1_P0_e));
    // boost::asio::connect(sb_e.next_layer(), endpoints_e);
    //sb_e.handshake(boost::asio::ssl::stream_base::client);
    // sb_e.async_handshake(boost::asio::ssl::stream_base::client, [&] (boost::system::error_code error)
    // {
    // // Handshake is done
    // });

    ssl_socket sb_e(io_context, ssl_contextb);
    sb_e.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_e = resolver.resolve(host1, std::to_string(PORT_P1_P0_e));
    boost::asio::connect(sb_e.lowest_layer(), endpointsb_e);
    sb_e.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket sb_e(io_context);
    // boost::asio::connect(sb_e, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_e)}));
    std::cout << "P0: [Established connection P1 (6)]" << std::endl;
    usleep(20000);

    ssl_socket sb_f(io_context, ssl_contextb);
    sb_f.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_f = resolver.resolve(host1, std::to_string(PORT_P1_P0_f));
    boost::asio::connect(sb_f.lowest_layer(), endpointsb_f);
    sb_f.handshake(boost::asio::ssl::stream_base::client);


    // tcp::socket sb_f(io_context);
    // boost::asio::connect(sb_f, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_f)}));
    std::cout << "P0: [Established connection P1 (7)]" << std::endl;
    usleep(20000);
    ssl_socket sb_g(io_context, ssl_contextb);
    sb_g.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_g = resolver.resolve(host1, std::to_string(PORT_P1_P0_g));
    boost::asio::connect(sb_g.lowest_layer(), endpointsb_g);
    sb_g.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket sb_g(io_context);
    // boost::asio::connect(sb_g, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_g)}));
    std::cout << "P0: [Established connection P1 (8)]" << std::endl;
    usleep(20000);

    ssl_socket sb_h(io_context, ssl_contextb);
    sb_h.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_h = resolver.resolve(host1, std::to_string(PORT_P1_P0_h));
    boost::asio::connect(sb_h.lowest_layer(), endpointsb_h);
    sb_h.handshake(boost::asio::ssl::stream_base::client);


    // tcp::socket sb_h(io_context);
    // boost::asio::connect(sb_h, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_h)}));
    std::cout << "P0: [Established connection P1 (9)]" << std::endl;
    usleep(20000);


    ssl_socket sb_i(io_context, ssl_contextb);
    sb_i.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_i = resolver.resolve(host1, std::to_string(PORT_P1_P0_i));
    boost::asio::connect(sb_i.lowest_layer(), endpointsb_i);
    sb_i.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket sb_i(io_context);
    // boost::asio::connect(sb_i, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_i)}));
    std::cout << "P0: [Established connection P1 (9)]" << std::endl;
    usleep(20000);

 
    ssl_socket sb_j(io_context, ssl_contextb);
    sb_j.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_j = resolver.resolve(host1, std::to_string(PORT_P1_P0_j));
    boost::asio::connect(sb_j.lowest_layer(), endpointsb_j);
    sb_j.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket sb_j(io_context);
    // boost::asio::connect(sb_j, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_j)}));
    std::cout << "P0: [Established connection P1 (9)]" << std::endl;
    usleep(20000);

    ssl_socket sb_k(io_context, ssl_contextb);
    sb_k.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_k = resolver.resolve(host1, std::to_string(PORT_P1_P0_k));
    boost::asio::connect(sb_k.lowest_layer(), endpointsb_k);
    sb_k.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket sb_k(io_context);
    // boost::asio::connect(sb_k, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_k)}));
    std::cout << "P0: [Established connection P1 (9)]" << std::endl;
    usleep(20000);

    ssl_socket sb_l(io_context, ssl_contextb);
    sb_l.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_l = resolver.resolve(host1, std::to_string(PORT_P1_P0_l));
    boost::asio::connect(sb_l.lowest_layer(), endpointsb_l);
    sb_l.handshake(boost::asio::ssl::stream_base::client);
    // tcp::socket sb_l(io_context);
    // boost::asio::connect(sb_l, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_l)}));
    std::cout << "P0: [Established connection P1 (9)]" << std::endl;
    usleep(20000);

    ssl_socket sb_m(io_context, ssl_contextb);
    sb_m.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_m = resolver.resolve(host1, std::to_string(PORT_P1_P0_m));
    boost::asio::connect(sb_m.lowest_layer(), endpointsb_m);
    sb_m.handshake(boost::asio::ssl::stream_base::client);
    // tcp::socket sb_m(io_context);
    // boost::asio::connect(sb_m, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_m)}));
    std::cout << "P0: [Established connection P1 (9)]" << std::endl;
    usleep(20000);


    ssl_socket sb_n(io_context, ssl_contextb);
    sb_n.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_n = resolver.resolve(host1, std::to_string(PORT_P1_P0_n));
    boost::asio::connect(sb_n.lowest_layer(), endpointsb_n);
    sb_n.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket sb_n(io_context);
    // boost::asio::connect(sb_n, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_n)}));
    std::cout << "P0: [Established connection P1 (9)]" << std::endl;
    usleep(20000);

    ssl_socket sb_o(io_context, ssl_contextb);
    sb_o.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpointsb_o = resolver.resolve(host1, std::to_string(PORT_P1_P0_o));
    boost::asio::connect(sb_o.lowest_layer(), endpointsb_o);
    sb_o.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket sb_o(io_context);
    // boost::asio::connect(sb_o, resolver.resolve({host1,
    // std::to_string(PORT_P1_P0_o)}));
    std::cout << "P0: [Established connection P1 (9)]" << std::endl;
    usleep(20000);

    tcp::socket sb_p(io_context);
    boost::asio::connect(sb_p, resolver.resolve({host1,
    std::to_string(PORT_P1_P0_p)}));
    std::cout << "P0: [Established connection P1 (9)]" << std::endl;
    usleep(20000);
  #endif



  #if (PARTY==1)
    
    std::cout << "P_ONE: " << PORT_P1_P0 << std::endl;
    boost::asio::io_context io_context;
    boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);
    ssl_context.load_verify_file("server.crt");

    boost::asio::ssl::context ssl_contextb(boost::asio::ssl::context::sslv23);
    ssl_contextb.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
   //context_.set_password_callback(boost::bind(&server::get_password, this));
    ssl_contextb.use_certificate_chain_file("server.crt"); 
    ssl_contextb.use_private_key_file("server.key", boost::asio::ssl::context::pem);
    ssl_contextb.use_tmp_dh_file("dh512.pem");



    usleep(20000);
    tcp::resolver resolver(io_context);
    
        ssl_socket s2(io_context, ssl_context);
    s2.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2 = resolver.resolve(host1, std::to_string(PORT_P1_P2));
    boost::asio::connect(s2.lowest_layer(), endpoints2);
    s2.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2(io_context);
    // boost::asio::connect(s2, resolver.resolve({host2,
    // std::to_string(PORT_P1_P2)}));


    // ssl_socket s2(io_context, ssl_context);
    // auto endpoints = resolver.resolve(host2, std::to_string(PORT_P1_P2));
    // boost::asio::connect(s2.next_layer(), endpoints);

    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000); 


    ssl_socket s2_a(io_context, ssl_context);
    s2_a.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_a = resolver.resolve(host1, std::to_string(PORT_P1_P2_a));
    boost::asio::connect(s2_a.lowest_layer(), endpoints2_a);
    s2_a.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_a(io_context);
    // boost::asio::connect(s2_a, resolver.resolve({host2,
    // std::to_string(PORT_P1_P2_a)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000);

    ssl_socket s2_b(io_context, ssl_context);
    s2_b.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_b = resolver.resolve(host1, std::to_string(PORT_P1_P2_b));
    boost::asio::connect(s2_b.lowest_layer(), endpoints2_b);
    s2_b.handshake(boost::asio::ssl::stream_base::client);
    // tcp::socket s2_b(io_context);
    // boost::asio::connect(s2_b, resolver.resolve({host2,
    // std::to_string(PORT_P1_P2_b)}));
    // std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000);

    ssl_socket s2_c(io_context, ssl_context);
    s2_c.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_c = resolver.resolve(host1, std::to_string(PORT_P1_P2_c));
    boost::asio::connect(s2_c.lowest_layer(), endpoints2_c);
    s2_c.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_c(io_context);
    // boost::asio::connect(s2_c, resolver.resolve({host2,
    // std::to_string(PORT_P1_P2_c)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000);


    ssl_socket s2_d(io_context, ssl_context);
    s2_d.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_d = resolver.resolve(host1, std::to_string(PORT_P1_P2_d));
    boost::asio::connect(s2_d.lowest_layer(), endpoints2_d);
    s2_d.handshake(boost::asio::ssl::stream_base::client);


    // tcp::socket s2_d(io_context);
    // boost::asio::connect(s2_d, resolver.resolve({host2,
    // std::to_string(PORT_P1_P2_d)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000);

    ssl_socket s2_e(io_context, ssl_context);
    s2_e.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_e = resolver.resolve(host1, std::to_string(PORT_P1_P2_e));
    boost::asio::connect(s2_e.lowest_layer(), endpoints2_e);
    s2_e.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_e(io_context);
    // boost::asio::connect(s2_e, resolver.resolve({host2,
    // std::to_string(PORT_P1_P2_e)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000);
    
    ssl_socket s2_f(io_context, ssl_context);
    s2_f.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_f = resolver.resolve(host1, std::to_string(PORT_P1_P2_f));
    boost::asio::connect(s2_f.lowest_layer(), endpoints2_f);
    s2_f.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_f(io_context);
    // boost::asio::connect(s2_f, resolver.resolve({host2,
    // std::to_string(PORT_P1_P2_f)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000);

    ssl_socket s2_g(io_context, ssl_context);
    s2_g.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_g = resolver.resolve(host1, std::to_string(PORT_P1_P2_g));
    boost::asio::connect(s2_g.lowest_layer(), endpoints2_g);
    s2_g.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_g(io_context);
    // boost::asio::connect(s2_g, resolver.resolve({host2,
    // std::to_string(PORT_P1_P2_g)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000);

    ssl_socket s2_h(io_context, ssl_context);
    s2_h.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints2_h = resolver.resolve(host1, std::to_string(PORT_P1_P2_h));
    boost::asio::connect(s2_h.lowest_layer(), endpoints2_h);
    s2_h.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s2_h(io_context);
    // boost::asio::connect(s2_h, resolver.resolve({host2,
    // std::to_string(PORT_P1_P2_h)}));
    std::cerr << "P1: [Established connection to P2]" << std::endl;    
    usleep(20000);

    ssl_socket s3(io_context, ssl_context);
    s3.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints3 = resolver.resolve(host1, std::to_string(PORT_P1_P3));
    boost::asio::connect(s3.lowest_layer(), endpoints3);
    s3.handshake(boost::asio::ssl::stream_base::client);
    // tcp::socket s3(io_context);
    // boost::asio::connect(s3, resolver.resolve({host3,
    // std::to_string(PORT_P1_P3)}));
    std::cout << "P1: [Established connection P3]" << std::endl;
    usleep(20000); 
    

    ssl_socket s3_a(io_context, ssl_context);
    s3_a.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints3_a = resolver.resolve(host1, std::to_string(PORT_P1_P3_a));
    boost::asio::connect(s3_a.lowest_layer(), endpoints3_a);
    s3_a.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s3_a(io_context);
    // boost::asio::connect(s3_a, resolver.resolve({host3,
    // std::to_string(PORT_P1_P3_a)}));
    std::cout << "P1: [Established connection P3]" << std::endl;
    usleep(20000); 
    
    ssl_socket s3_b(io_context, ssl_context);
    s3_b.set_verify_mode(boost::asio::ssl::verify_peer);
    auto endpoints3_b = resolver.resolve(host1, std::to_string(PORT_P1_P3_b));
    boost::asio::connect(s3_b.lowest_layer(), endpoints3_b);
    s3_b.handshake(boost::asio::ssl::stream_base::client);

    // tcp::socket s3_b(io_context);
    // boost::asio::connect(s3_b, resolver.resolve({host3,
    // std::to_string(PORT_P1_P3_b)}));
    // std::cout << "P1: [Established connection P3]" << std::endl;
    usleep(20000); 
    


    

     ssl_socket sb(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb(boost::asio::ip::tcp::v4(), PORT_P1_P0);
    boost::asio::ip::tcp::acceptor acceptorb(io_context, endpointb);
    acceptorb.accept(sb.next_layer());
    sb.handshake(boost::asio::ssl::stream_base::server);


    // tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb(acceptor.accept());

    std::cerr << "Listenting on port: " << PORT_P1_P0 << std::endl;


     ssl_socket sb_a(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_a(boost::asio::ip::tcp::v4(), PORT_P1_P0_a);
    boost::asio::ip::tcp::acceptor acceptorb_a(io_context, endpointb_a);
    acceptorb_a.accept(sb_a.next_layer());
    sb_a.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_a(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_a));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_a(acceptor_a.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_a << std::endl;    

    tcp::acceptor acceptor_b(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_b));
    std::cout << " --- > " << std::endl;
    tcp::socket sb_b(acceptor_b.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_b << std::endl;

 
     ssl_socket sb_c(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_c(boost::asio::ip::tcp::v4(), PORT_P1_P0_c);
    boost::asio::ip::tcp::acceptor acceptorb_c(io_context, endpointb_c);
    acceptorb_c.accept(sb_c.next_layer());
    sb_c.handshake(boost::asio::ssl::stream_base::server);

    //sb_c.handshake(boost::asio::ssl::stream_base::server);
    // sb_c.async_handshake(boost::asio::ssl::stream_base::server, [&] (boost::system::error_code error)
    // {
    //     std::cerr << "handshake done\n";
    // // Handshake is done
    // });
    // tcp::acceptor acceptor_c(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_c));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_c(acceptor_c.accept());
    // std::cerr << "Listenting on port: " << PORT_P1_P0_c << std::endl;


    // boost::asio::ip::tcp::endpoint endpoint_d(boost::asio::ip::tcp::v4(), PORT_P1_P0_d);
    // boost::asio::ip::tcp::acceptor acceptor_d(io_context, endpoint_d);
    // ssl_socket sb_d(io_context, ssl_context);
    // acceptor_d.accept(sb_d.next_layer());
    // sb_d.async_handshake(boost::asio::ssl::stream_base::server, [&] (boost::system::error_code error)
    // {
    //     std::cerr << "handshake done\n";
    // // Handshake is done
    // });

         ssl_socket sb_d(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_d(boost::asio::ip::tcp::v4(), PORT_P1_P0_d);
    boost::asio::ip::tcp::acceptor acceptorb_d(io_context, endpointb_d);
    acceptorb_d.accept(sb_d.next_layer());
    sb_d.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_d(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_d));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_d(acceptor_d.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_d << std::endl;


          ssl_socket sb_e(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_e(boost::asio::ip::tcp::v4(), PORT_P1_P0_e);
    boost::asio::ip::tcp::acceptor acceptorb_e(io_context, endpointb_e);
    acceptorb_e.accept(sb_e.next_layer());
    sb_e.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_e(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_e));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_e(acceptor_e.accept());
    // std::cerr << "Listenting on port: " << PORT_P1_P0_e << std::endl;

    ssl_socket sb_f(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_f(boost::asio::ip::tcp::v4(), PORT_P1_P0_f);
    boost::asio::ip::tcp::acceptor acceptorb_f(io_context, endpointb_f);
    acceptorb_f.accept(sb_f.next_layer());
    sb_f.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_f(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_f));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_f(acceptor_f.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_f << std::endl;

    ssl_socket sb_g(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_g(boost::asio::ip::tcp::v4(), PORT_P1_P0_g);
    boost::asio::ip::tcp::acceptor acceptorb_g(io_context, endpointb_g);
    acceptorb_g.accept(sb_g.next_layer());
    sb_g.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_g(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_g));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_g(acceptor_g.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_g << std::endl;


    ssl_socket sb_h(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_h(boost::asio::ip::tcp::v4(), PORT_P1_P0_h);
    boost::asio::ip::tcp::acceptor acceptorb_h(io_context, endpointb_h);
    acceptorb_h.accept(sb_h.next_layer());
    sb_h.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_h(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_h));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_h(acceptor_h.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_h << std::endl;


    ssl_socket sb_i(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_i(boost::asio::ip::tcp::v4(), PORT_P1_P0_i);
    boost::asio::ip::tcp::acceptor acceptorb_i(io_context, endpointb_i);
    acceptorb_i.accept(sb_i.next_layer());
    sb_i.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_i(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_i));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_i(acceptor_i.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_i << std::endl;

    ssl_socket sb_j(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_j(boost::asio::ip::tcp::v4(), PORT_P1_P0_j);
    boost::asio::ip::tcp::acceptor acceptorb_j(io_context, endpointb_j);
    acceptorb_j.accept(sb_j.next_layer());
    sb_j.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_j(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_j));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_j(acceptor_j.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_j << std::endl;

    ssl_socket sb_k(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_k(boost::asio::ip::tcp::v4(), PORT_P1_P0_k);
    boost::asio::ip::tcp::acceptor acceptorb_k(io_context, endpointb_k);
    acceptorb_k.accept(sb_k.next_layer());
    sb_k.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_k(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_k));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_k(acceptor_k.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_k << std::endl;

    ssl_socket sb_l(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_l(boost::asio::ip::tcp::v4(), PORT_P1_P0_l);
    boost::asio::ip::tcp::acceptor acceptorb_l(io_context, endpointb_l);
    acceptorb_l.accept(sb_l.next_layer());
    sb_l.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_l(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_l));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_l(acceptor_l.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_l << std::endl;
    
    ssl_socket sb_m(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_m(boost::asio::ip::tcp::v4(), PORT_P1_P0_m);
    boost::asio::ip::tcp::acceptor acceptorb_m(io_context, endpointb_m);
    acceptorb_m.accept(sb_m.next_layer());
    sb_m.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_m(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_m));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_m(acceptor_m.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_m << std::endl;
    
    ssl_socket sb_n(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_n(boost::asio::ip::tcp::v4(), PORT_P1_P0_n);
    boost::asio::ip::tcp::acceptor acceptorb_n(io_context, endpointb_n);
    acceptorb_n.accept(sb_n.next_layer());
    sb_n.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_n(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_n));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_n(acceptor_n.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_n << std::endl;

    ssl_socket sb_o(io_context, ssl_contextb);
    boost::asio::ip::tcp::endpoint endpointb_o(boost::asio::ip::tcp::v4(), PORT_P1_P0_o);
    boost::asio::ip::tcp::acceptor acceptorb_o(io_context, endpointb_o);
    acceptorb_o.accept(sb_o.next_layer());
    sb_o.handshake(boost::asio::ssl::stream_base::server);

    // tcp::acceptor acceptor_o(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_o));
    // std::cout << " --- > " << std::endl;
    // tcp::socket sb_o(acceptor_o.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_o << std::endl;

    tcp::acceptor acceptor_p(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P0_p));
    std::cout << " --- > " << std::endl;
    tcp::socket sb_p(acceptor_p.accept());
    std::cerr << "Listenting on port: " << PORT_P1_P0_p << std::endl;


   #endif
