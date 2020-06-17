void read_sanity_seeds(AES_KEY& prgkey, tcp::socket& sin)
{
  read(sin, boost::asio::buffer(sanity_seeds, 2 * sizeof(__m128i)));

  auto [rnd0, rnd1] = gen_rands(prgkey, sanity_seeds, nitems);

  rnd0_[0] = rnd0; rnd0_[1] = rot(rnd0,1); rnd0_[2] = rot(rnd0,2); rnd0_[3] = rot(rnd0,3);
  rnd1_[0] = rnd1; rnd1_[1] = rot(rnd1,1); rnd1_[2] = rot(rnd1,2); rnd1_[3] = rot(rnd1,3); 

  progress[step::sanity_seeds_in] = 1;
}

void read_shifts(tcp::socket& sin)
{
  for(size_t j = 0; j < nqueries; ++j)
  {
    boost::asio::read(sin, boost::asio::buffer(shift[j], 2 * sizeof(uint64_t)));
    progress[step::shifts_in] = j + 1;
  }
}


void read_mux_verification(tcp::socket& sin)
{
  for(size_t j = 0; j < nqueries; ++j)
  {
    boost::asio::read(sin, boost::asio::buffer(&sanity_vector_mux[j], sizeof(int8_t)));
    
    if(sanity_vector_mux[j] == 0)
    {
     std::cerr << "sanity_vector_mux[" << j << "] = " << sanity_vector_mux[j] << std::endl;
    }

    progress[step::mux_sanity_done] = j + 1;
  }
}




 void generate_dotproducts()
 {    

    dotproducts = (uint64_t *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(uint64_t));

    for(size_t j = 0; j < nqueries; ++j)
    { 
      while(progress[step::sanity_seeds_in] < 1 || progress[step::evalfull_done_] < j + 1 || progress[step::shifts_in] < j + 1)
      {
       std::this_thread::yield();
      }
  
      if(sanity_vector_demux[j] != 0)
       {
        dotproducts[j] = dot_prod(rnd0_, shift[j][0] , rnd1_, shift[j][1], T[j]);
       }
       else{
        std::cerr << "sanity_vector_demux[" << j << "] = " << sanity_vector_demux[j] << "\n\n";
        dotproducts[j] = 0;
       }
      progress[step::dp_gen] = j + 1;
    }
 }

 
 void send_dp_to_P2(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
 {
    while(progress[step::dp_gen] < j + 1)
    {
      std::this_thread::yield();
    }
    
   
   async_write(sout, boost::asio::buffer(&dotproducts[j], sizeof(uint64_t)),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(j + 1 < nqueries) 
                   {
                    send_dp_to_P2(j + 1, io_context, sout);
                   }
                }          
                else
                {                
                  send_dp_to_P2(j , io_context, sout);
                }
            
            });  
 }