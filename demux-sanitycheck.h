void demux_sanity_check(AES_KEY& prgkey)
{
  
  // dp_sanity = (uint64_t *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(uint64_t));  

  profile<3*precision>  tCW; 

  profile<3*precision>  zero_profile;
  
  for(size_t dim = 0; dim < DIM; ++dim) zero_profile.u64[dim] = 0;

  for(size_t j = 0; j < nqueries; ++j)
  {
      while (progress[step::evalfull_done_] < j + 1 || progress[step::leaf_expanded_] < j + 1 || progress[step::bvr3_in] < j + 1 || progress[step::bvr2_sanity_in] < j + 1) 
      { 
        std::this_thread::yield(); 
      }
       
      uint64_t  RRtCW = 0.0;
      
      for(size_t k = 0; k < nitems; ++k)
      { 
       
        if(T[j][k])  tCW  = queries[j].dpfkey.leaf;
        
        if(!T[j][k]) tCW  =  zero_profile; 
        
        tCW = expanded_leaf[j][k] - tCW - tCW;  
        
        for(size_t dim = 0; dim < DIM; ++dim) RRtCW += tCW.u64[dim];
      }

      #if (PARTY == 0)
        dp_sanity[j] = RRtCW + bvr2_sanity[j].RRtCW;// - p3_bvrs[j].sanity_blinds;    
      #endif

      #if (PARTY == 1)
        dp_sanity[j] = RRtCW + p3_bvrs[j].RRtCW;// - bvr2[j].sanity_blinds;  
      #endif

      progress[step::demux_sanity_gen] = j + 1;
  }
}


 void write_demux_sanity_dp(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
 {
    while(progress[step::demux_sanity_gen] < j + 1)
    {
      std::this_thread::yield();
    }

     async_write(sout, boost::asio::buffer(&dp_sanity[j], sizeof(uint64_t)),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(j + 1 < nqueries) 
                   {
                    write_demux_sanity_dp(j + 1, io_context, sout);
                   }
                }          
                else
                {                
                  write_demux_sanity_dp(j , io_context, sout);
                }
            
            }); 

 }

 void read_demux_sanity_dp(tcp::socket& sin)
 {
   // dp_sanity_recv = (uint64_t *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(uint64_t ));
   //uint64_t dp_sanity_recv;

   for(size_t j = 0; j < nqueries; ++j)
   {
      while(progress[step::demux_sanity_gen] < j + 1)
      {
       std::this_thread::yield();
      }



      boost::asio::read(sin, boost::asio::buffer(&dp_sanity_recv[j], sizeof(uint64_t)));


      progress[step::demux_sanity_read] = j + 1;

      
     
   }

 }


 void demux_verify()
 {
 

  size_t demux_verified = 0;
  for(size_t j = 0; j < nqueries; ++j)
  {


      if(uint64_t(dp_sanity_recv[j]) == uint64_t(dp_sanity[j])) 
      {
        sanity_vector_demux[j] = 1;
        ++demux_verified;
      }
      else
      {
        std::cerr << "---> " << uint64_t(dp_sanity_recv[j]) << " == " << uint64_t(dp_sanity[j]) << std::endl;
        assert(false);
      }
 

     progress[step::demux_sanity_done] = j + 1;
  }

   std::cerr << "demux_verified = " << demux_verified << std::endl;

 }