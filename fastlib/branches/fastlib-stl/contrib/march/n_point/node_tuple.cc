/*
 *  node_tuple.cc
 *  
 *
 *  Created by William March on 2/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "node_tuple.h"


bool npt::NodeTuple::CheckSymmetry(const std::vector<NptNode*>& nodes, 
                                   index_t split_ind, bool is_left) {
  
  // only check the new node for symmetry with respect to the others
  if (is_left) {
    for (index_t i = 0; i < split_ind; i++) {
      
      if (nodes[split_ind]->left()->end() <= nodes[i]->begin()) {
        return false;
      }
      
    } // for i
    
    for (index_t i = split_ind + 1; i < tuple_size_; i++) {
      
      if (nodes[i]->end() <= nodes[split_ind]->left()->begin()) {
        return false;
      } 
      
    } // for i
  }
  else {
    for (index_t i = 0; i < split_ind; i++) {
      
      if (nodes[split_ind]->right()->end() <= nodes[i]->begin()) {
        return false;
      }
      
    } // for i
    
    for (index_t i = split_ind + 1; i < tuple_size_; i++) {
      
      if (nodes[i]->end() <= nodes[split_ind]->right()->begin()) {
        return false;
      } 
      
    } // for i    
  }
  return true;
  
} // CheckSymmetry_



void npt::NodeTuple::FindInvalidIndices_(std::vector<index_t>& inds) {

  //std::vector<index_t> inds;
  
  // simple case
  if (tuple_size_ == 2) {
    inds.push_back(0);
  }
  else {
    
    index_t bad_ind = ind_to_split_ - 1;
    index_t bad_ind2 = 0;
    
    // the values are in a lower triangular matrix, stored in a straight array
    // this loop goes horizontally along the row ind_to_split_ until
    // we reach the diagonal
    for (index_t i = 0; i < ind_to_split_; i++) {
      
      inds.push_back(bad_ind);
      bad_ind += tuple_size_ - 1 - (i+1);
      bad_ind2 += tuple_size_ - i - 1;
      
    } // horizontal
    
    // after the diagonal, we need to go down the column
    for (index_t i = ind_to_split_+1; i < tuple_size_; i++) {
      
      inds.push_back(bad_ind2);
      bad_ind2++;
      
    }
    
  }    
    
} // FindInvalidIndices


void npt::NodeTuple::UpdateIndices_(index_t split_ind, 
                                    std::vector<index_t>& invalid_indices) {
  
  /*
  std::cout << "\n";
  for (index_t i = 0; i < invalid_indices.size(); i++) {
    std::cout << "Invalid inds[" << i << "]: " << invalid_indices[i] << "\n";
  }
  
  std::cout << "\n";
  for (index_t i = 0; i < input_to_upper_.size(); i++) {
    std::cout << "input_to_upper[" << i << "]: " << input_to_upper_[i] << "\n";
  }

  std::cout << "\n";
  for (index_t i = 0; i < input_to_lower_.size(); i++) {
    std::cout << "input_to_lower[" << i << "]: " << input_to_lower_[i] << "\n";
  }
   */
  
  NptNode* new_node = node_list_[split_ind];
  
  int split_size = -1;
  ind_to_split_ = -1;
  
  // iterate over the invalid indices with this
  index_t invalid_ind = 0;
  
  all_leaves_ = true;
  
  // iterate over the entire tuple
  for (index_t i = 0; i < tuple_size_; i++) {
    
    NptNode* node_i = node_list_[i];
    
    if (!(node_i->is_leaf())) {
      
      all_leaves_ = false;
      
      if (node_i->count() > split_size) {
        split_size = node_i->count();
        ind_to_split_ = i;
      }
      
    } // not leaf
    
    // don't compute the distance of the new node to itself
    if (i != split_ind) {
      
      double max_dist_sq = node_i->bound().MaxDistanceSq(new_node->bound());
      double min_dist_sq = node_i->bound().MinDistanceSq(new_node->bound());
      
      DRange this_range;
      this_range.Init(min_dist_sq, max_dist_sq);
      
      index_t bad_ind = invalid_indices[invalid_ind];
      invalid_ind++;
      
      ranges_[bad_ind] = this_range;
      
      index_t upper_bad_ind = input_to_upper_[bad_ind];
      index_t lower_bad_ind = input_to_lower_[bad_ind];
      
      
      sorted_upper_[upper_bad_ind] = std::pair<double, index_t> (max_dist_sq, 
                                                                 bad_ind);
      sorted_lower_[lower_bad_ind] = std::pair<double, index_t> (min_dist_sq, 
                                                                 bad_ind);
      
      /*
      if (sorted_lower_[lower_bad_ind].second != lower_bad_ind) {
        std::cout << "problem!\n";
      }
      sorted_upper_[upper_bad_ind] = std::pair<double, index_t> (max_dist_sq, 
                                                                 upper_bad_ind);
      sorted_lower_[lower_bad_ind] = std::pair<double, index_t> (min_dist_sq, 
                                                                 lower_bad_ind);
      */
      
    } // computing distances
    
  } // for i
  
  
  // now, resort the ordered lists
  // TODO: can I do better than sorting the entire thing?  
  
  std::sort(sorted_upper_.begin(), sorted_upper_.end());
  std::sort(sorted_lower_.begin(), sorted_lower_.end());
 
  /*
  for (index_t i = 0; i < sorted_upper_.size(); i++) {
    std::cout << "sorted_upper[" << i << "]: (";
    std::cout << sorted_upper_[i].first << ", " << sorted_upper_[i].second;
    std::cout << ")\n";
  } 
  
  std::cout <<"\n";
  
  for (index_t i = 0; i < sorted_lower_.size(); i++) {
    std::cout << "sorted_lower[" << i << "]: (";
    std::cout << sorted_lower_[i].first << ", " << sorted_lower_[i].second;
    std::cout << ")\n";
  } 
  */
  
  // get the new mapping to sorted indices
  for (index_t i = 0; i < sorted_upper_.size(); i++) {
   
    input_to_upper_[sorted_upper_[i].second] = i;
    input_to_lower_[sorted_lower_[i].second] = i;
    
  } // for i
  
} // UpdateIndices()




