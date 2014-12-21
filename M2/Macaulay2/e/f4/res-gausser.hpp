// Copyright 2014 Michael E. Stillman.

#ifndef _res__gausser_hpp_
#define _res__gausser_hpp_

#include "../ring.hpp"
#include "../ZZp.hpp"
#include "../coeffrings.hpp"

class ResF4Mem;

class ResGausser
{
  enum {ZZp} typ;
  const Ring *K;

  CoefficientRingZZp *Kp;
  ResF4Mem *Mem;

  ResGausser(const Z_mod *K0, ResF4Mem *Mem0);
public:
  typedef int* F4CoefficientArray;
  //  typedef void *F4CoefficientArray;

  struct dense_row {
    int len; // coeffs is an array 0..len-1
    F4CoefficientArray coeffs;
  };

  ~ResGausser() {}

  static ResGausser *newResGausser(const Ring *K, ResF4Mem *Mem0);

  const Ring * get_ring() const { return K; }

  const CoefficientRingZZp* get_coeff_ring() const { return Kp; }

  F4CoefficientArray from_ringelem_array(int len, ring_elem *elems) const;

  void to_ringelem_array(int len, F4CoefficientArray, ring_elem *result) const;

  F4CoefficientArray copy_F4CoefficientArray(int len, F4CoefficientArray F) const;

  // leading coefficient
  int lead_coeff(F4CoefficientArray coeffs) const
  {
    return coeffs[0];
  }

  // other routines:
  void deallocate_F4CCoefficientArray(F4CoefficientArray &F, int len) const;

  // reduce mod p. (QQ --> ZZ/p) (place in double's??)

  // other reductions

  // combine them (ZZ/p1, ..., ZZ/pn --> QQ)

  // Hensel lift routine?

  // evaluation of multivariate poly's or fcn's.

  void dense_row_allocate(dense_row &r, int nelems) const;
  // create a row of 0's (over K).

  void dense_row_clear(dense_row &r, int first, int last) const;

  void dense_row_deallocate(dense_row &r) const;

  int dense_row_next_nonzero(dense_row &r, int first, int last) const;

  void dense_row_fill_from_sparse(dense_row &r,
                                  int len,
                                  F4CoefficientArray sparse,
                                  int *comps) const;
  // Fills 'r' from 'sparse' (and 'comps')

  void dense_row_cancel_sparse(dense_row &r,
                               int len,
                               F4CoefficientArray sparse,
                               int *comps) const;
  // dense += c * sparse, where c is chosen to cancel column comps[0].
  // There should also be a version of this routine which records this value c
  // into a F4CoefficientArray.

  void dense_row_to_sparse_row(dense_row &r,
                               int &result_len,
                               F4CoefficientArray &result_sparse,
                               int *&result_comps,
                               int first,
                               int last) const;

  void sparse_row_make_monic(int len,
                             F4CoefficientArray sparse) const;
  int coeff_to_int(int f) const //anton
  {
    return Kp->to_int(f);
  }

  mutable long n_dense_row_cancel;
  mutable long n_subtract_multiple;
};

#endif

// Local Variables:
//  compile-command: "make -C $M2BUILDDIR/Macaulay2/e "
//  End:
