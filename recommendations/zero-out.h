void read_sq_blinds_from_P2(tcp::socket& sin)
 {
    for(size_t j = 0; j < nrecords; ++j)
    {
      read(sin, boost::asio::buffer(&sq_alpha[j], sizeof(sq_alpha[j])));
    }
 }

 void read_zero_blinds_from_P2(tcp::socket& sin)
 {
    for(size_t j = 0; j < nrecords; ++j)
    {
      read(sin, boost::asio::buffer(&zero_alpha[j], sizeof(zero_alpha[j])));
    }
 }



void blinds_for_squaring_and_zeroing()
{
  for(size_t j = 0; j < nrecords; ++j)
  {
    blinded_zero_sent0[j] = uv[j] + zero_blinds[j];
    blinded_zero_sent1[j] = squared[j] + sq_blinds[2][j];
    blinded_sq_sent0[j]   =  eval_vector[j] + sq_blinds[0][j];
    blinded_sq_sent1[j]   = -eval_vector[j] + sq_blinds[1][j];
  }
}


void write_sq_blinds0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{

     async_write(sout, boost::asio::buffer(&blinded_sq_sent0[j], sizeof(blinded_sq_sent0[j])),
      [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
      
      {       
          if(!ec)
          {

             if(j + 1 < nrecords) 
             {
              //std::cout << "write_sq_blinds - " << j << std::endl;                   
              write_sq_blinds0(j + 1, io_context, sout);
             }
          }          
          else
          {               
            write_sq_blinds0(j , io_context, sout);
          }
      
      }); 
}

void read_sq_blinds0(tcp::socket& sin)
{
  for(size_t j = 0; j < nrecords; ++j)
   {
      read(sin, boost::asio::buffer(&blinded_sq_recv0[j], sizeof(blinded_sq_recv0[j])));

      comparison_progress[comparison_step::sq_blinded_0_in] = j + 1;
   }
}

void write_sq_blinds1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{

     async_write(sout, boost::asio::buffer(&blinded_sq_sent1[j], sizeof(blinded_sq_sent1[j])),
      [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
      
      {       
          if(!ec)
          {

             if(j + 1 < nrecords) 
             {
              //std::cout << "write_sq_blinds - " << j << std::endl;                   
              write_sq_blinds1(j + 1, io_context, sout);
             }
          }          
          else
          {               
            write_sq_blinds1(j , io_context, sout);
          }
      
      }); 
}

void read_sq_blinds1(tcp::socket& sin)
{
  for(size_t j = 0; j < nrecords; ++j)
   {
      read(sin, boost::asio::buffer(&blinded_sq_recv1[j], sizeof(blinded_sq_recv1[j])));

      comparison_progress[comparison_step::sq_blinded_1_in] = j + 1;
   }
}

void  write_zero_blinds0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
     async_write(sout, boost::asio::buffer(&blinded_zero_sent0[j], sizeof(blinded_zero_sent0[j])),
      [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
      
      {       
          if(!ec)
          {

             if(j + 1 < nrecords) 
             {
              //std::cout << "write_zero_blinds - " << j << std::endl;                   
              write_zero_blinds0(j + 1, io_context, sout);
             }
          }          
          else
          {               
            write_zero_blinds0(j , io_context, sout);
          }
      
      }); 
}

void read_zero_blinds0(tcp::socket& sin)
{
     for(size_t j = 0; j < nrecords; ++j)
   {


      read(sin, boost::asio::buffer(&blinded_zero_recv0[j], sizeof(blinded_zero_recv0[j])));
        comparison_progress[comparison_step::zero_blinded_in] = j + 1;
   }
}
void  write_zero_blinds1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
     async_write(sout, boost::asio::buffer(&blinded_zero_sent1[j], sizeof(blinded_zero_sent1[j])),
      [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
      
      {       
          if(!ec)
          {

             if(j + 1 < nrecords) 
             {
              //std::cout << "write_zero_blinds - " << j << std::endl;                   
              write_zero_blinds1(j + 1, io_context, sout);
             }
          }          
          else
          {               
            write_zero_blinds1(j , io_context, sout);
          }
      
      }); 
}

void read_zero_blinds1(tcp::socket& sin)
{
     for(size_t j = 0; j < nrecords; ++j)
   {


      read(sin, boost::asio::buffer(&blinded_zero_recv1[j], sizeof(blinded_zero_recv1[j])));
        comparison_progress[comparison_step::zero_blinded_in] = j + 1;
   }
}
 void squaring()
 {
   // for(size_t j = 0; j < nrecords; ++j)
   // { 

   //     while(comparison_progress[comparison_step::sq_blinded_in] < j + 1)
   //     {
   //      std::this_thread::yield();
   //     }
   //    x0 =  eval_vector[j];
   //    x1 = -eval_vector[j];

   //    y0 =  eval_vector[j];
   //    y1 = -eval_vector[j];

   //     int8_t x0, x1, X0, X1; 
      

   //    // int8_t z0 = x0 * (y0 + (y1 + Y1)) - (Y0 * (x1 + X1)) + sq_alpha0[j];
   //     int8_t z = x1 * (y1 + (y0 + Y0)) - (Y1 * (x0 + X0)) + sq_alpha[j];

   // }
 }

 void zeroing()
 {
   for(size_t j = 0; j < nrecords; ++j)
   {

   }
 }