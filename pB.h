 #ifndef PB_H__
 #define PB_H__


#include<bitset>
template<size_t nitems> struct query { size_t user; dpf_key<nitems> dpfkey; }; // TODO


 
size_t item_update_array[n_buckets] = {0};

size_t evalfull_done[n_cores] = {0}; // for doing the evalfull in parallel
size_t leaf_expanded[n_cores] = {0}; // for doing the evalfull in parallel

enum step
{
  bvr2_in = 0,
  bvr2_sanity_in,
  p3_seed_in,
  bvr3_in,
  written,
  evalfull_done_,
  leaf_expanded_,
  profiles_in,
  profiles_gen,
  perms_in,
  norms_in,
  norms_gen,
  cws_in ,
  cws_gen,
  cws_out,
  user_update,
  buckets_complete,
  buckets_in,
  buckets_out,
  user_reconstructed,
  item_reconstructed,
  item_update, 
  mux_seed,
  eval_done,
  bvr_mux_in,
  itm_blind_in,
  itm_blind_gen,
  itm_blind_out,
  itm_mux_in,
  itm_mux_out,
  u_norm_done,
  sanity_seed,
  mux_sanity_done,
  demux_sanity_read,
  demux_sanity_gen,
  demux_sanity_done,
  precision_seed_in,  // item profile precision reduction
  precision_beaver_in, // item profile precision reduction
  i_msb_in, // item profile precision reduction
  iprofile_msbs_in, // item profile precision reduction
  i_profile_msbs_blinded, // item profile precision reduction
  msb_prod_gen, // item profile precision reduction
  blinded_i_msbs_in, // item profile precision reduction
  offset_iprofiles, // item profile precision reduction
  u_msbs_gen, // user profile precision reduction
  uprofile_msbs_in, // user profile precision reduction
  u_profile_msbs_blinded, // user profile precision reduction
  u_precision_seed_in, // user profile precision reduction
  u_precision_beaver_in, // user profile precision reduction
  u_msb_prod_gen, // user profile precision reduction
  offset_uprofiles, // user profile precision reduction
  u_msb_prod_recv, // user profile precision reduction
  blinded_u_msbs_in, // user profile precision reduction
  dp_sanity_in,
  dp_sanity_PB_out,
  dp_sanity_PB_in,
  sanity_blinds_in,
  sanity_seeds_in,
  shifts_in,
  dp_gen,
  dp_out,
  iprofile_out,
  uprofile_out,
  num_steps
};
size_t progress[step::num_steps] = { 0 };
 #endif
