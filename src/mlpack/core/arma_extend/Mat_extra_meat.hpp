#if ARMA_VERSION_MAJOR < 4 || \
    (ARMA_VERSION_MAJOR == 4 && ARMA_VERSION_MINOR < 349)
///////////////////////////////////////////////////////////////////////////////
// Mat::const_row_col_iterator implementation                                //
///////////////////////////////////////////////////////////////////////////////

template<typename eT>
inline
Mat<eT>::const_row_col_iterator::const_row_col_iterator()
    : M(NULL), current_pos(NULL), internal_col(0), internal_row(0)
  {
  // Technically this iterator is invalid (it may not point to a real element)
  }



template<typename eT>
inline
Mat<eT>::const_row_col_iterator::const_row_col_iterator(const row_col_iterator& it)
    : M(it.M), current_pos(it.current_pos), internal_col(it.col()), internal_row(it.row())
  {
  // Nothing to do.
  }



template<typename eT>
inline
Mat<eT>::const_row_col_iterator::const_row_col_iterator(const const_row_iterator& it)
    : M(&it.M), current_pos(&it.M(it.row, it.col)), internal_col(it.col), internal_row(it.row)
  {
  // Nothing to do.
  }



template<typename eT>
inline
Mat<eT>::const_row_col_iterator::const_row_col_iterator(const row_iterator& it)
    : M(&it.M), current_pos(&it.M(it.row, it.col)), internal_col(it.col), internal_row(it.row)
  {
  // Nothing to do.
  }



template<typename eT>
inline
Mat<eT>::const_row_col_iterator::const_row_col_iterator(const Mat<eT>& in_M, const uword row, const uword col)
    : M(&in_M), current_pos(&in_M(row,col)), internal_col(col), internal_row(row)
  {
  // Nothing to do.
  }



template<typename eT>
inline typename Mat<eT>::const_row_col_iterator&
Mat<eT>::const_row_col_iterator::operator++()
  {
  current_pos++;
  internal_row++;

  // Check to see if we moved a column.
  if(internal_row == M->n_rows)
    {
    internal_col++;
    internal_row = 0;
    }

  return *this;
  }



template<typename eT>
inline typename Mat<eT>::const_row_col_iterator
Mat<eT>::const_row_col_iterator::operator++(int)
  {
  typename Mat<eT>::const_row_col_iterator temp(*this);

  ++(*this);

  return temp;
  }



template<typename eT>
inline typename Mat<eT>::const_row_col_iterator&
Mat<eT>::const_row_col_iterator::operator--()
  {
  if(internal_row > 0)
    {
    current_pos--;
    internal_row--;
    }
  else if(internal_col > 0)
    {
    current_pos--;
    internal_col--;
    internal_row = M->n_rows - 1;
    }

  return *this;
  }



template<typename eT>
inline typename Mat<eT>::const_row_col_iterator
Mat<eT>::const_row_col_iterator::operator--(int)
  {
  typename Mat<eT>::const_row_col_iterator temp(*this);

  --(*this);

  return temp;
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator==(const const_row_col_iterator& rhs) const
  {
  return (rhs.current_pos == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator!=(const const_row_col_iterator& rhs) const
  {
  return (rhs.current_pos != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator==(const row_col_iterator& rhs) const
  {
  return (rhs.current_pos == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator!=(const row_col_iterator& rhs) const
  {
  return (rhs.current_pos != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator==(const const_iterator& rhs) const
  {
  return (rhs == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator!=(const const_iterator& rhs) const
  {
  return (rhs != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator==(const iterator& rhs) const
  {
  return (rhs == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator!=(const iterator& rhs) const
  {
  return (rhs != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator==(const const_row_iterator& rhs) const
  {
  return (&rhs.M(rhs.row, rhs.col) == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator!=(const const_row_iterator& rhs) const
  {
  return (&rhs.M(rhs.row, rhs.col) != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator==(const row_iterator& rhs) const
  {
  return (&rhs.M(rhs.row, rhs.col) == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::const_row_col_iterator::operator!=(const row_iterator& rhs) const
  {
  return (&rhs.M(rhs.row, rhs.col) != current_pos);
  }



///////////////////////////////////////////////////////////////////////////////
// Mat::row_col_iterator implementation                                //
///////////////////////////////////////////////////////////////////////////////

template<typename eT>
inline
Mat<eT>::row_col_iterator::row_col_iterator()
    : M(NULL), current_pos(NULL), internal_col(0), internal_row(0)
  {
  // Technically this iterator is invalid (it may not point to a real element)
  }



template<typename eT>
inline
Mat<eT>::row_col_iterator::row_col_iterator(const row_iterator& it)
    : M(&it.M), current_pos(&it.M(it.row, it.col)), internal_col(it.col), internal_row(it.row)
  {
  // Nothing to do.
  }



template<typename eT>
inline
Mat<eT>::row_col_iterator::row_col_iterator(Mat<eT>& in_M, const uword row, const uword col)
    : M(&in_M), current_pos(&in_M(row,col)), internal_col(col), internal_row(row)
  {
  // Nothing to do.
  }



template<typename eT>
inline typename Mat<eT>::row_col_iterator&
Mat<eT>::row_col_iterator::operator++()
  {
  current_pos++;
  internal_row++;

  // Check to see if we moved a column.
  if(internal_row == M->n_rows)
    {
    internal_col++;
    internal_row = 0;
    }

  return *this;
  }



template<typename eT>
inline typename Mat<eT>::row_col_iterator
Mat<eT>::row_col_iterator::operator++(int)
  {
  typename Mat<eT>::row_col_iterator temp(*this);

  ++(*this);

  return temp;
  }



template<typename eT>
inline typename Mat<eT>::row_col_iterator&
Mat<eT>::row_col_iterator::operator--()
  {
  if(internal_row != 0)
    {
    current_pos--;
    internal_row--;
    }
  else if(internal_col != 0)
    {
    current_pos--;
    internal_col--;
    internal_row = M->n_rows - 1;
    }

  return *this;
  }



template<typename eT>
inline typename Mat<eT>::row_col_iterator
Mat<eT>::row_col_iterator::operator--(int)
  {
  typename Mat<eT>::row_col_iterator temp(*this);

  --(*this);

  return temp;
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator==(const const_row_col_iterator& rhs) const
  {
  return (rhs.current_pos == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator!=(const const_row_col_iterator& rhs) const
  {
  return (rhs.current_pos != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator==(const row_col_iterator& rhs) const
  {
  return (rhs.current_pos == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator!=(const row_col_iterator& rhs) const
  {
  return (rhs.current_pos != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator==(const const_iterator& rhs) const
  {
  return (rhs == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator!=(const const_iterator& rhs) const
  {
  return (rhs != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator==(const iterator& rhs) const
  {
  return (rhs == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator!=(const iterator& rhs) const
  {
  return (rhs != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator==(const const_row_iterator& rhs) const
  {
  return (&rhs.M(rhs.row, rhs.col) == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator!=(const const_row_iterator& rhs) const
  {
  return (&rhs.M(rhs.row, rhs.col) != current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator==(const row_iterator& rhs) const
  {
  return (&rhs.M(rhs.row, rhs.col) == current_pos);
  }



template<typename eT>
inline bool
Mat<eT>::row_col_iterator::operator!=(const row_iterator& rhs) const
  {
  return (&rhs.M(rhs.row, rhs.col) != current_pos);
  }



///////////////////////////////////////////////////////////////////////////////
// extended Mat functionality implementation                                 //
///////////////////////////////////////////////////////////////////////////////

template<typename eT>
inline typename Mat<eT>::const_row_col_iterator
Mat<eT>::begin_row_col() const
  {
  return const_row_col_iterator(*this);
  }



template<typename eT>
inline typename Mat<eT>::row_col_iterator
Mat<eT>::begin_row_col()
  {
  return row_col_iterator(*this);
  }



template<typename eT>
inline typename Mat<eT>::const_row_col_iterator
Mat<eT>::end_row_col() const
  {
  return ++const_row_col_iterator(*this, n_rows - 1, n_cols - 1);
  }



template<typename eT>
inline typename Mat<eT>::row_col_iterator
Mat<eT>::end_row_col()
  {
  return ++row_col_iterator(*this, n_rows - 1, n_cols - 1);
  }

#endif
