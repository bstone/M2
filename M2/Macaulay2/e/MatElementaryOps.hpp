// Copyright 2013  Michael E. Stillman

#ifndef _mat_elementary_ops_hpp_
#define _mat_elementary_ops_hpp_

template <typename MT> class MatElementaryOps;
template <typename RT> class DMat;
template <typename RT> class SMat;

template <typename RT>
class MatElementaryOps< DMat<RT> >
{
public:
  typedef DMat<RT> Mat;
  typedef typename Mat::ElementType ElementType;
  typedef typename Mat::Iterator Iterator;
  typedef typename Mat::ConstIterator ConstIterator;

private:
  template <typename It1, typename It2, typename It3>
  static void copy_from_iter(const RT& R, It1 loc1, It2 end1, It3 loc2)
  {
    for ( ; loc1 != end1; ++loc1, ++loc2)
      R.set(*loc1, *loc2);
  }

public:

  static size_t lead_row(const Mat& mat, size_t col)
  /* returns the largest index row which has a non-zero value in column 'col'.
     returns -1 if the column has no non-zero entries */
  {
    size_t row = mat.numRows();
    while (row != 0)
      {
        --row;
        if (!mat.ring().is_zero(mat.entry(row,col)))
            return row;
      }
    return static_cast<size_t>(-1);
  }

  static size_t lead_row(const Mat& mat, size_t col, ElementType &result)
  /* returns the largest index row which has a non-zero value in column 'col'.
     Also sets result to be the entry at this index.
     returns -1 if the column is 0, or if col is out of range
     No error is flagged. */
  {
    size_t row = mat.numRows();
    while (row != 0)
      {
        --row;
        if (!mat.ring().is_zero(mat.entry(row,col)))
          {
            mat.ring().set(result, mat.entry(row,col));
            return row;
          }
      }
    return static_cast<size_t>(-1);
  }

  ///////////////////////////////
  // Row and column operations //
  ///////////////////////////////
  // The following routines return false if one of the row or columns given
  // is out of range.

  static void interchange_rows(Mat& mat, size_t i, size_t j)
  /* swap rows: row(i) <--> row(j) */
  {
    M2_ASSERT(i < mat.numRows());
    M2_ASSERT(j < mat.numRows());
    if (i == j) return;

    auto ii = mat.rowBegin(i);
    auto jj = mat.rowBegin(j);
    auto end = mat.rowEnd(i);
    for ( ; ii != end; ++ii, ++jj)
      mat.ring().swap(*ii, *jj);
  }

  static void interchange_columns(Mat& mat, size_t i, size_t j)
  /* swap columns: column(i) <--> column(j) */
  {
    M2_ASSERT(i < mat.numColumns());
    M2_ASSERT(j < mat.numColumns());
    if (i == j) return;

    auto ii = mat.columnBegin(i);
    auto jj = mat.columnBegin(j);
    auto end = mat.columnEnd(i);
    for ( ; ii != end; ++ii, ++jj)
      mat.ring().swap(*ii, *jj);
  }

  static void scale_row(Mat& mat, size_t i, const ElementType &r)
  /* row(i) <- r * row(i) */
  {
    M2_ASSERT(i < mat.numRows());

    auto loc = mat.rowBegin(i);
    auto end = mat.rowEnd(i);
    for ( ; loc != end; ++loc)
      mat.ring().mult(*loc, r, *loc); // *loc = r * *loc
  }

  static void scale_column(Mat& mat, size_t i, const ElementType &r)
  /* column(i) <- r * column(i) */
  {
    M2_ASSERT(i < mat.numColumns());

    auto loc = mat.columnBegin(i);
    auto end = mat.columnEnd(i);
    for ( ; loc != end; ++loc)
      mat.ring().mult(*loc, r, *loc); // *loc = r * *loc
  }

  static void divide_row(Mat& mat, size_t i, const ElementType &r)
  /* row(i) <- row(i) / r */
  {
    M2_ASSERT(i < mat.numRows());

    auto loc = mat.rowBegin(i);
    auto end = mat.rowEnd(i);
    for ( ; loc != end; ++loc)
      mat.ring().divide(*loc, *loc, r); // *loc = *loc / r
  }

  static void divide_column(Mat& mat, size_t i, const ElementType &r)
  /* column(i) <- column(i) / r */
  {
    M2_ASSERT(i < mat.numColumns());

    auto loc = mat.columnBegin(i);
    auto end = mat.columnEnd(i);
    for ( ; loc != end; ++loc)
      mat.ring().divide(*loc, *loc, r); // *loc = *loc / r
  }

  static void row_op(Mat& mat, size_t i, const ElementType &r, size_t j)
  /* row(i) <- row(i) + r * row(j) */
  {
    M2_ASSERT(i < mat.numRows());
    M2_ASSERT(j < mat.numRows());
    M2_ASSERT(i != j);

    ElementType f;
    mat.ring().init(f);
    mat.ring().set_zero(f);

    auto loc1 = mat.rowBegin(i);
    auto loc2 = mat.rowBegin(j);
    auto end = mat.rowEnd(i);

    for ( ; loc1 != end; ++loc1, ++loc2)
      {
        mat.ring().mult(f,r,*loc2);
        mat.ring().add(*loc1, f, *loc1);
      }
    mat.ring().clear(f);
  }

  static void column_op(Mat& mat, size_t i, const ElementType &r, size_t j)
  /* column(i) <- column(i) + r * column(j) */
  {
    M2_ASSERT(i < mat.numColumns());
    M2_ASSERT(j < mat.numColumns());
    M2_ASSERT(i != j);

    ElementType f;
    mat.ring().init(f);
    mat.ring().set_zero(f);

    auto loc1 = mat.columnBegin(i);
    auto loc2 = mat.columnBegin(j);
    auto end = mat.columnEnd(i);

    for ( ; loc1 != end; ++loc1, ++loc2)
      {
        mat.ring().mult(f,r,*loc2);
        mat.ring().add(*loc1, f, *loc1);
      }
    mat.ring().clear(f);
  }

private:
  static void op2by2(const RT& ring,
                     Iterator& loc1, ConstIterator& end, Iterator& loc2,
                     const ElementType& a1, const ElementType& a2,
                     const ElementType& b1, const ElementType& b2
                     )
  {
    ElementType f1,f2,g1,g2;
    ring.init(f1);
    ring.init(f2);
    ring.init(g1);
    ring.init(g2);
    ring.set_zero(f1);
    ring.set_zero(f2);
    ring.set_zero(g1);
    ring.set_zero(g2);
    for ( ; loc1 != end; ++loc1, ++loc2)
      {
        ring.mult(f1,a1,*loc1);
        ring.mult(f2,a2,*loc2);
        ring.mult(g1,b1,*loc1);
        ring.mult(g2,b2,*loc2);
        
        ring.add(f1,f1,f2);
        ring.add(g1,g1,g2);
        ring.set(*loc1, f1);
        ring.set(*loc2, g1);
      }
    ring.clear(f1);
    ring.clear(f2);
    ring.clear(g1);
    ring.clear(g2);
   }
public: 
  static void row2by2(Mat& mat, 
               size_t r1, size_t r2,
               const ElementType &a1, const ElementType &a2,
               const ElementType &b1, const ElementType &b2)
  /* row(r1) <- a1 * row(r1) + a2 * row(r2),
     row(r2) <- b1 * row(r1) + b2 * row(r2)
  */
  {
    M2_ASSERT(r1 < mat.numRows());
    M2_ASSERT(r2 < mat.numRows());
    M2_ASSERT(r1 != r2);

    auto loc1 = mat.rowBegin(r1);
    auto loc2 = mat.rowBegin(r2);
    auto end = mat.rowEnd(r1);

    op2by2(mat.ring(), 
           loc1, end, loc2,
           a1, a2,
           b1, b2);
  }

  static void column2by2(Mat& mat, 
                  size_t c1, size_t c2,
                  const ElementType &a1, const ElementType &a2,
                  const ElementType &b1, const ElementType &b2)
  /* column(c1) <- a1 * column(c1) + a2 * column(c2),
     column(c2) <- b1 * column(c1) + b2 * column(c2)
  */
  {
    M2_ASSERT(c1 < mat.numColumns());
    M2_ASSERT(c2 < mat.numColumns());
    M2_ASSERT(c1 != c2);

    auto loc1 = mat.columnBegin(c1);
    auto loc2 = mat.columnBegin(c2);
    auto end = mat.columnEnd(c1);

    op2by2(mat.ring(), 
           loc1, end, loc2,
           a1, a2,
           b1, b2);
  }

  static void dot_product(const Mat& mat, size_t i, size_t j, ElementType &result)
  {
    M2_ASSERT(i < mat.numColumns());
    M2_ASSERT(j < mat.numColumns());

    auto loc1 = mat.columnBegin(i);
    auto loc2 = mat.columnBegin(j);
    auto end = mat.columnEnd(i);

    ElementType f;
    mat.ring().init(f);
    mat.ring().set_zero(f);
    mat.ring().set_zero(result);
    for ( ; loc1 != end; ++loc1, ++loc2)
      {
        mat.ring().mult(f,*loc1,*loc2);
        mat.ring().add(result,result, f);
      }
    mat.ring().clear(f);
  }

  static bool row_permute(Mat& mat, size_t start_row, M2_arrayint perm)
  {
    // We copy one row to another location for each cycle in 'perm' of length > 1.
    size_t nrows_to_permute = perm->len;
    bool *done = newarray_atomic(bool,nrows_to_permute);
    for (size_t i=0; i<nrows_to_permute; i++)
      done[i] = true;
    for (size_t i=0; i<nrows_to_permute; i++)
      {
        size_t j = perm->array[i];
        if (!done[j])
          {
            ERROR("expected permutation");
            deletearray(done);
            return false;
          }
        done[j] = false;
      }
    ElementType *tmp = newarray_clear(ElementType,mat.numColumns());
    for (size_t c=0; c<mat.numColumns(); c++)
      mat.ring().init(tmp[c]);
    size_t next = 0;
    
    while (next < nrows_to_permute)
      {
        if (done[next] || perm->array[next] == next)
          {
            next++;
          }
        else
          {
            // store row 'next' into tmp
            auto loc1 = mat.rowBegin(start_row + next);
            auto end1 = mat.rowEnd(start_row + next);
            copy_from_iter(mat.ring(), tmp, tmp + mat.numColumns(), loc1);
            
            size_t r = next;
            for (;;)
              {
                // copy row perm[r] to row r
                auto loc2 = mat.rowBegin(start_row + perm->array[r]);
                loc1 = mat.rowBegin(start_row + r);
                end1 = mat.rowEnd(start_row + r);

                copy_from_iter(mat.ring(), loc1, end1, loc2);
                done[r] = true;
                size_t next_r = perm->array[r];
                if (next_r == next) break; // and so r is the previous one
                r = perm->array[r];
              }
            // Now copy tmp back
            copy_from_iter(mat.ring(), loc1, end1, tmp);
            done[r] = true;
          }
      }
    for (size_t c=0; c<mat.numColumns(); c++)
      mat.ring().clear(tmp[c]);
    deletearray(tmp);
    deletearray(done);
    return true;
  }

  static bool column_permute(Mat& mat, size_t start_col, M2_arrayint perm)
  {
    // We copy one column to another location for each cycle in 'perm' of length > 1.
    size_t ncols_to_permute = perm->len;
    bool *done = newarray_atomic(bool,ncols_to_permute);
    for (size_t i=0; i<ncols_to_permute; i++)
      done[i] = true;
    for (size_t i=0; i<ncols_to_permute; i++)
      {
        size_t j = perm->array[i];
        if (!done[j])
          {
            ERROR("expected permutation");
            deletearray(done);
            return false;
          }
        done[j] = false;
      }
    ElementType *tmp = newarray_clear(ElementType,mat.numRows());
    for (size_t r=0; r<mat.numRows(); r++)
      mat.ring().init(tmp[r]);
    size_t next = 0;
    
    while (next < ncols_to_permute)
      {
        if (done[next] || perm->array[next] == next)
          {
            next++;
          }
        else
          {
            auto loc1 = mat.columnBegin(start_col + next);
            auto end1 = mat.columnEnd(start_col + next);
            // store col 'next' into tmp
            copy_from_iter(mat.ring(), tmp, tmp + mat.numRows(), loc1);

            size_t r = next;
            for (;;)
              {
                auto loc2 = mat.columnBegin(start_col + perm->array[r]);
                loc1 = mat.columnBegin(start_col + r);
                end1 = mat.columnEnd(start_col + r);

                // copy col perm[r] to col r
                copy_from_iter(mat.ring(), loc1, end1, loc2);

                done[r] = true;
                size_t next_r = perm->array[r];
                if (next_r == next) break; // and so r is the previous one
                r = perm->array[r];
              }
            // Now copy tmp back
            copy_from_iter(mat.ring(), loc1, end1, tmp);
            done[r] = true;
          }
      }
    for (size_t r=0; r<mat.numRows(); r++)
      mat.ring().clear(tmp[r]);
    deletearray(tmp);
    deletearray(done);
    return true;
  }

  static void insert_columns(Mat& mat, size_t i, size_t n_to_add)
  /* Insert n_to_add columns directly BEFORE column i. */
  {
    size_t new_ncols = mat.numColumns() + n_to_add;
    Mat newMatrix(mat.ring(), mat.numRows(), new_ncols);
    
    for (size_t r=0; r<mat.numRows(); r++)
      {
        for (size_t c=0; c<i; c++)
          mat.ring().swap(mat.entry(r,c), newMatrix.entry(r,c));
        for (size_t c=i; c<mat.numColumns(); c++)
          mat.ring().swap(mat.entry(r,c), newMatrix.entry(r,c+n_to_add));
      }
    mat.swap(newMatrix);
  }

  static void insert_rows(Mat& mat, size_t i, size_t n_to_add)
  /* Insert n_to_add rows directly BEFORE row i. */
  {
    size_t new_nrows = mat.numRows() + n_to_add;
    Mat newMatrix(mat.ring(), new_nrows, mat.numColumns());
    
    for (size_t c=0; c<mat.numColumns(); c++)
      {
        for (size_t r=0; r<i; r++)
          mat.ring().swap(mat.entry(r,c), newMatrix.entry(r,c));
        for (size_t r=i; r<mat.numRows(); r++)
          mat.ring().swap(mat.entry(r,c), newMatrix.entry(r+n_to_add,c));
      }
    mat.swap(newMatrix);
  }

  static void delete_columns(Mat& mat, size_t i, size_t j)
  /* Delete columns i .. j from M */
  {
    M2_ASSERT(i < mat.numColumns());
    M2_ASSERT(j < mat.numColumns());
    M2_ASSERT(i <= j);
    size_t n_to_delete = j-i+1;
    size_t new_ncols = mat.numColumns() - n_to_delete;
    Mat newMatrix(mat.ring(), mat.numRows(), new_ncols);
    
    for (size_t r=0; r<mat.numRows(); r++)
      {
        for (size_t c=0; c<i; c++)
          mat.ring().swap(mat.entry(r,c), newMatrix.entry(r,c));
        for (size_t c=j+1; c<mat.numColumns(); c++)
          mat.ring().swap(mat.entry(r,c), newMatrix.entry(r,c-n_to_delete));
      }
    mat.swap(newMatrix);
  }

  static void delete_rows(Mat& mat, size_t i, size_t j)
  /* Delete rows i .. j from M */
  {
    M2_ASSERT(i < mat.numRows());
    M2_ASSERT(j < mat.numRows());
    M2_ASSERT(i <= j);
    size_t n_to_delete = j-i+1;
    size_t new_nrows = mat.numRows() - n_to_delete;
    Mat newMatrix(mat.ring(), new_nrows, mat.numColumns());
    
    for (size_t c=0; c<mat.numColumns(); c++)
      {
        for (size_t r=0; r<i; r++)
          mat.ring().swap(mat.entry(r,c), newMatrix.entry(r,c));
        for (size_t r=j+1; r<mat.numRows(); r++)
          mat.ring().swap(mat.entry(r,c), newMatrix.entry(r-n_to_delete,c));
      }
    mat.swap(newMatrix);
  }

  static void setFromSubmatrix(const Mat& mat, M2_arrayint rows, M2_arrayint cols, Mat& result)
  /* Set 'result' with the given submatrix of 'mat' */
  {
    result.resize(rows->len, cols->len); // resets to a zero matrix
    for (size_t r = 0; r < rows->len; r++)
      for (size_t c = 0; c < cols->len; c++)
        mat.ring().set(result.entry(r,c), mat.entry(rows->array[r],cols->array[c]));
  }

  static void setFromSubmatrix(const Mat& mat, M2_arrayint cols, Mat& result)
  /* Set 'result' with the given submatrix of 'mat' */
  {
    result.resize(mat.numRows(), cols->len); // resets to a zero matrix
    for (size_t r = 0; r < mat.numRows(); r++)
      for (size_t c = 0; c < cols->len; c++)
        mat.ring().set(result.entry(r,c), mat.entry(r,cols->array[c]));
  }

  static void getEntry(const Mat& mat, size_t r, size_t c, ElementType& result)
  {
    mat.ring().set(result, mat.entry(r,c));
  }

  static void setEntry(Mat& mat, size_t r, size_t c, const ElementType& a)
  {
    mat.ring().set(mat.entry(r,c), a);
  }

};

template <typename RT>
class MatElementaryOps< SMat<RT> >
{
public:
  typedef SMat<RT> Mat;
  typedef typename Mat::ElementType ElementType;

  static size_t lead_row(const Mat& mat, size_t col)
  /* returns the largest index row which has a non-zero value in column 'col'.
     returns -1 if the column is 0 */
  {
    return mat.lead_row(col);
  }  

  static size_t lead_row(const Mat& mat, size_t col, ElementType &result)
  /* returns the largest index row which has a non-zero value in column 'col'.
     Also sets result to be the entry at this index.
     returns -1 if the column is 0, or if col is out of range
     No error is flagged. */
  {
    return mat.lead_row(col, result);
  }
  ///////////////////////////////
  // Row and column operations //
  ///////////////////////////////
  // The following routines return false if one of the row or columns given
  // is out of range.

  static void interchange_rows(Mat& mat, size_t i, size_t j)
  /* swap rows: row(i) <--> row(j) */
  {
    mat.interchange_rows(i,j);
  }

  static void interchange_columns(Mat& mat, size_t i, size_t j)
  /* swap columns: column(i) <--> column(j) */
  {
    mat.interchange_columns(i,j);
  }
  static void scale_row(Mat& mat, size_t i, const ElementType &r)
  /* row(i) <- r * row(i) */
  {
    mat.scale_row(i,r);
  }

  static void scale_column(Mat& mat, size_t i, const ElementType &r)
  /* column(i) <- r * column(i) */
  {
    mat.scale_column(i,r);
  }

  static void divide_row(Mat& mat, size_t i, const ElementType &r)
  /* row(i) <- row(i) / r */
  {
    mat.divide_row(i,r);
  }

  static void divide_column(Mat& mat, size_t i, const ElementType &r)
  /* column(i) <- column(i) / r */
  {
    mat.divide_column(i,r);
  }

  static void row_op(Mat& mat, size_t i, const ElementType &r, size_t j)
  /* row(i) <- row(i) + r * row(j) */
  {
    mat.row_op(i,r,j);
  }

  static void column_op(Mat& mat, size_t i, const ElementType &r, size_t j)
  /* column(i) <- column(i) + r * column(j) */
  {
    mat.column_op(i,r,j);
  }

  static void column2by2(Mat& mat, 
                  size_t c1, size_t c2,
                  const ElementType &a1, const ElementType &a2,
                  const ElementType &b1, const ElementType &b2)
  /* column(c1) <- a1 * column(c1) + a2 * column(c2),
     column(c2) <- b1 * column(c1) + b2 * column(c2)
  */
  {
    mat.column2by2(c1,c2,a1,a2,b1,b2);
  }

  static void row2by2(Mat& mat, 
               size_t r1, size_t r2,
               const ElementType &a1, const ElementType &a2,
               const ElementType &b1, const ElementType &b2)
  /* row(r1) <- a1 * row(r1) + a2 * row(r2),
     row(r2) <- b1 * row(r1) + b2 * row(r2)
  */
  {
    mat.row2by2(r1,r2,a1,a2,b1,b2);
  }

  static void dot_product(const Mat& mat, size_t i, size_t j, ElementType &result)
  {
    mat.dot_product(i,j,result);
  }

  static bool row_permute(Mat& mat, size_t start_row, M2_arrayint perm)
  {
    return mat.row_permute(start_row, perm);
  }

  static bool column_permute(Mat& mat, size_t start_col, M2_arrayint perm)
  {
    return mat.column_permute(start_col, perm);
  }

  static void insert_columns(Mat& mat, size_t i, size_t n_to_add)
  /* Insert n_to_add columns directly BEFORE column i. */
  {
    mat.insert_columns(i, n_to_add);
  }

  static void insert_rows(Mat& mat, size_t i, size_t n_to_add)
  /* Insert n_to_add rows directly BEFORE row i. */
  {
    mat.insert_rows(i, n_to_add);
  }

  static void delete_columns(Mat& mat, size_t i, size_t j)
  /* Delete columns i .. j from M */
  {
    mat.delete_columns(i, j);
  }

  static void delete_rows(Mat& mat, size_t i, size_t j)
  /* Delete rows i .. j from M */
  {
    mat.delete_rows(i,j);
  }

  static void setFromSubmatrix(const Mat& mat, M2_arrayint rows, M2_arrayint cols, Mat& result)
  /* Set 'result' with the given submatrix of 'mat' */
  {
    result.setFromSubmatrix(mat, rows, cols);
  }

  static void setFromSubmatrix(const Mat& mat, M2_arrayint cols, Mat& result)
  /* Set 'result' with the given submatrix of 'mat' */
  {
    result.setFromSubmatrix(mat,cols);
  }
  
  static void getEntry(const Mat& mat, size_t r, size_t c, ElementType& result)
  {
    mat.get_entry(r,c,result);
  }

  static void setEntry(Mat& mat, size_t r, size_t c, const ElementType& a)
  {
    mat.set_entry(r,c,a);
  }

};


#endif

// Local Variables:
// compile-command: "make -C $M2BUILDDIR/Macaulay2/e "
// indent-tabs-mode: nil
// End: