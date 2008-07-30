#ifndef MULTIBODY_KERNEL_H
#define MULTIBODY_KERNEL_H

#include "fastlib/fastlib.h"
#include "mlpack/series_expansion/kernel_aux.h"

template<typename TTree, typename TBound>
class AxilrodTellerForceKernel {
  
 private:

  ////////// Private Member Constants //////////

  /** @brief The "nu" constant in front of the potential.
   */
  static const double AXILROD_TELLER_COEFF = 1e-18;

  ////////// Private Member Variables //////////

  /** @brief The temporary matrix to store pairwise distances.
   */
  Matrix distmat_;
  
  /** @brief The temporary ArrayList to store the mapped indices for
   *         computing the gradient.
   */
  ArrayList<index_t> index_orders_;
  
  ////////// Private Member Functions //////////

  /** @brief Computes the L1 norm of a vector. This probably needs to
   *         be part of the LaPack library.
   */
  double L1Norm_(const Vector &v) {
    double length = 0;

    for(index_t i = 0; i < v.length(); i++) {
      length += fabs(v[i]);
    }
    return length;
  }

  /** @brief Computes error due to finite difference approximation.
   */
  void ComputeApproximationError_
  (double min_negative_gradient1, double max_negative_gradient1,
   double min_positive_gradient1, double max_positive_gradient1,
   double min_negative_gradient2, double max_negative_gradient2,
   double min_positive_gradient2, double max_positive_gradient2,
   double min_negative_gradient3, double max_negative_gradient3,
   double min_positive_gradient3, double max_positive_gradient3,
   double &negative_gradient1_error, double &positive_gradient1_error,
   double &negative_gradient2_error, double &positive_gradient2_error,
   double &negative_gradient3_error, double &positive_gradient3_error) {
    
    negative_gradient1_error = 
      (max_negative_gradient1 - min_negative_gradient1) * 0.5;
    positive_gradient1_error =
      (max_positive_gradient1 - min_positive_gradient1) * 0.5;
    negative_gradient2_error = 
      (max_negative_gradient2 - min_negative_gradient2) * 0.5;
    positive_gradient2_error =
      (max_positive_gradient2 - min_positive_gradient2) * 0.5;
    negative_gradient3_error = 
      (max_negative_gradient3 - min_negative_gradient3) * 0.5;
    positive_gradient3_error =
      (max_positive_gradient3 - min_positive_gradient3) * 0.5;    
  }

  void force_(const Matrix &data, const ArrayList<index_t> &indices, 
	      double &negative_gradient1, double &positive_gradient1, 
	      double &negative_gradient2, double &positive_gradient2,
	      double &negative_gradient3, double &positive_gradient3,
	      Vector &negative_force1_e, Vector &negative_force1_u,
	      Vector &positive_force1_l, Vector &positive_force1_e,
	      Matrix &negative_force2_e, Matrix &negative_force2_u,
	      Matrix &positive_force2_l, Matrix &positive_force2_e) {

    // Negative contribution to the first component.
    negative_force1_e[indices[index_orders_[0]]] += 
      negative_gradient1 + negative_gradient2;
    negative_force1_u[indices[index_orders_[0]]] += 
      negative_gradient1 + negative_gradient2;

    // Positive contribution to the first component.
    positive_force1_l[indices[index_orders_[0]]] += 
      positive_gradient1 + positive_gradient2;
    positive_force1_e[indices[index_orders_[0]]] += 
      positive_gradient1 + positive_gradient2;

    // Negative contribution to the second component.
    la::AddExpert(data.n_rows(), negative_gradient1, 
		  data.GetColumnPtr(indices[index_orders_[1]]),
		  negative_force2_e.GetColumnPtr(indices[index_orders_[0]]));
    la::AddExpert(data.n_rows(), negative_gradient2,
		  data.GetColumnPtr(indices[index_orders_[2]]),
		  negative_force2_e.GetColumnPtr(indices[index_orders_[0]]));
    la::AddExpert(data.n_rows(), negative_gradient1, 
		  data.GetColumnPtr(indices[index_orders_[1]]),
		  negative_force2_u.GetColumnPtr(indices[index_orders_[0]]));
    la::AddExpert(data.n_rows(), negative_gradient2,
		  data.GetColumnPtr(indices[index_orders_[2]]),
		  negative_force2_u.GetColumnPtr(indices[index_orders_[0]]));

    // Positive contribution to the second component.
    la::AddExpert(data.n_rows(), positive_gradient1, 
		  data.GetColumnPtr(indices[index_orders_[1]]),
		  positive_force2_e.GetColumnPtr(indices[index_orders_[0]]));
    la::AddExpert(data.n_rows(), positive_gradient2,
		  data.GetColumnPtr(indices[index_orders_[2]]),
		  positive_force2_e.GetColumnPtr(indices[index_orders_[0]]));
    la::AddExpert(data.n_rows(), positive_gradient1, 
		  data.GetColumnPtr(indices[index_orders_[1]]),
		  positive_force2_l.GetColumnPtr(indices[index_orders_[0]]));
    la::AddExpert(data.n_rows(), positive_gradient2,
		  data.GetColumnPtr(indices[index_orders_[2]]),
		  positive_force2_l.GetColumnPtr(indices[index_orders_[0]]));
  }

  void gradient_(const ArrayList<index_t> &index_orders, 
		 double &minimum_negative_gradient,
		 double *maximum_negative_gradient,
		 double &minimum_positive_gradient,
		 double *maximum_positive_gradient) {

    double min_dsqd1 = distmat_.get(index_orders[0], index_orders[1]);
    double min_dist1 = sqrt(min_dsqd1);
    double min_dqrt1 = math::Sqr(min_dsqd1);
    double min_dsix1 = min_dsqd1 * min_dqrt1;

    double max_dsqd1 = distmat_.get(index_orders[1], index_orders[0]);
    double max_dist1 = sqrt(max_dsqd1);
    double max_dqrt1 = math::Sqr(max_dsqd1);
    double max_dsix1 = max_dsqd1 * max_dqrt1;

    double min_dsqd2 = distmat_.get(index_orders[0], index_orders[2]);
    double min_dist2 = sqrt(min_dsqd2);
    double min_dcub2 = min_dsqd2 * min_dist2;
    double min_dqui2 = min_dsqd2 * min_dcub2;

    double max_dsqd2 = distmat_.get(index_orders[2], index_orders[0]);
    double max_dist2 = sqrt(max_dsqd2);
    double max_dcub2 = max_dsqd2 * max_dist2;
    double max_dqui2 = max_dsqd2 * max_dcub2;

    double min_dsqd3 = distmat_.get(index_orders[1], index_orders[2]);
    double min_dist3 = sqrt(min_dsqd3);
    double min_dcub3 = min_dsqd3 * min_dist3;
    double min_dqui3 = min_dsqd3 * min_dcub3;

    double max_dsqd3 = distmat_.get(index_orders[2], index_orders[1]);
    double max_dist3 = sqrt(max_dsqd3);
    double max_dcub3 = max_dsqd3 * max_dist3;
    double max_dqui3 = max_dsqd3 * max_dcub3;
    
    double min_common_factor = 3.0 * AXILROD_TELLER_COEFF / (8.0 * max_dist1);
    double max_common_factor = 3.0 * AXILROD_TELLER_COEFF / (8.0 * min_dist1);

    minimum_negative_gradient = max_common_factor *
      (-8.0 / (min_dqrt1 * min_dcub2 * min_dcub3)
       - 1.0 / (min_dqui2 * min_dqui3)
       - 1.0 / (min_dsqd1 * min_dcub2 * min_dqui3)
       - 1.0 / (min_dsqd1 * min_dqui2 * min_dcub3)
       - 3.0 / (min_dqrt1 * min_dist2 * min_dqui3)
       - 3.0 / (min_dqrt1 * min_dqui2 * min_dist3)
       - 5.0 / (min_dsix1 * min_dist2 * min_dcub3)
       - 5.0 / (min_dsix1 * min_dcub2 * min_dist3));
    
    if(maximum_negative_gradient) {
      *maximum_negative_gradient = min_common_factor *
	(-8.0 / (max_dqrt1 * max_dcub2 * max_dcub3)
	 - 1.0 / (max_dqui2 * max_dqui3)
	 - 1.0 / (max_dsqd1 * max_dcub2 * max_dqui3)
	 - 1.0 / (max_dsqd1 * max_dqui2 * max_dcub3)
	 - 3.0 / (max_dqrt1 * max_dist2 * max_dqui3)
	 - 3.0 / (max_dqrt1 * max_dqui2 * max_dist3)
	 - 5.0 / (max_dsix1 * max_dist2 * max_dcub3)
	 - 5.0 / (max_dsix1 * max_dcub2 * max_dist3));
    }

    minimum_positive_gradient = min_common_factor *
      (5 * min_dist2 / (max_dsix1 * max_dqui3) +
       5 * min_dist3 / (max_dsix1 * max_dqui2) +
       6 / (max_dqrt1 * max_dcub2 * max_dcub3));

    if(maximum_positive_gradient) {
      *maximum_positive_gradient = max_common_factor *
	(5 * max_dist2 / (min_dsix1 * min_dqui3) +
	 5 * max_dist3 / (min_dsix1 * min_dqui2) +
	 6 / (min_dqrt1 * min_dcub2 * min_dcub3));
    }
  }

 public:
  
  ////////// Constructor/Destructor //////////

  /** @brief The default constructor.
   */
  AxilrodTellerForceKernel() {
  }

  /** @brief The default destructor.
   */
  ~AxilrodTellerForceKernel() {
  }

  ////////// Getters/Setters //////////
  
  /** @brief Gets the squared distance matrix.
   */
  const Matrix &pairwise_squared_distances() const { return distmat_; }
  
  /** @brief Gets the interaction order of the kernel.
   */
  int order() {
    return 3;
  }

  ////////// User-level Functions //////////

  /** @brief Initializes the kernel.
   */
  void Init(double bandwidth_in) {
    distmat_.Init(3, 3);
    index_orders_.Init(3);
  }

  /** @brief Computes the pairwise distance among FastLib tree nodes.
   */
  void EvalMinMaxSquaredDistances(ArrayList<TTree *> &tree_nodes) {

    int num_nodes = tree_nodes.size();

    for(index_t i = 0; i < num_nodes - 1; i++) {
      const TBound &node_i_bound = tree_nodes[i]->bound();

      for(index_t j = i + 1; j < num_nodes; j++) {
	const TBound &node_j_bound = tree_nodes[j]->bound();
        double min_squared_distance = node_i_bound.MinDistanceSq(node_j_bound);
        double max_squared_distance = node_i_bound.MaxDistanceSq(node_j_bound);

        distmat_.set(i, j, min_squared_distance);
        distmat_.set(j, i, max_squared_distance);
      }
    }
  }

  /** @brief Computes the pairwise distance among three points.
   */
  void EvalMinMaxSquaredDistances(const Matrix &data, 
				  const ArrayList<index_t> &indices) {
    
    int num_order = order();
    
    for(index_t i = 0; i < num_order - 1; i++) {
      
      const double *point_i = data.GetColumnPtr(indices[i]);
      
      for(index_t j = i + 1; j < num_order; j++) {
	const double *point_j = data.GetColumnPtr(indices[j]);
        double squared_distance = la::DistanceSqEuclidean(data.n_rows(), 
							  point_i, point_j);
        distmat_.set(i, j, squared_distance);
        distmat_.set(j, i, squared_distance);
      }
    }
  }

  /** @brief Computes $\frac{\nu}{r_i - r_j} \frac{\partial
   *         u}{\partial (r_i - r_j)}$, $\frac{\nu}{r_i - r_k}
   *         \frac{\partial u}{\partial (r_i - r_k)}$ and
   *         $\frac{\nu}{r_j - r_k} \frac{\partial u}{\partial (r_j -
   *         r_k)}$.
   */
  void EvalGradients(const Matrix &dsqd_matrix,
		     double &min_negative_gradient1,
		     double *max_negative_gradient1,
		     double &min_positive_gradient1, 
		     double *max_positive_gradient1,
		     double &min_negative_gradient2, 
		     double *max_negative_gradient2,
		     double &min_positive_gradient2, 
		     double *max_positive_gradient2,
		     double &min_negative_gradient3, 
		     double *max_negative_gradient3,
		     double &min_positive_gradient3, 
		     double *max_positive_gradient3) {

    index_orders_[0] = 0;
    index_orders_[1] = 1;
    index_orders_[2] = 2;
    gradient_(index_orders_, min_negative_gradient1, max_negative_gradient1,
	      min_positive_gradient1, max_positive_gradient1);

    index_orders_[0] = 0;
    index_orders_[1] = 2;
    index_orders_[2] = 1;
    gradient_(index_orders_, min_negative_gradient2, max_negative_gradient2,
	      min_positive_gradient2, max_positive_gradient2);
    
    index_orders_[0] = 1;
    index_orders_[1] = 2;
    index_orders_[2] = 0;
    gradient_(index_orders_, min_negative_gradient3, max_negative_gradient3,
	      min_positive_gradient3, max_positive_gradient3);
  }

  void EvalContributions
  (const Matrix &data, const ArrayList<index_t> &indices,
   double &negative_gradient1, double &positive_gradient1,
   double &negative_gradient2, double &positive_gradient2,
   double &negative_gradient3, double &positive_gradient3,
   Vector &negative_force1_e, Vector &negative_force1_u,
   Vector &positive_force1_l, Vector &positive_force1_e,
   Matrix &negative_force2_e, Matrix &negative_force2_u,
   Matrix &positive_force2_l, Matrix &positive_force2_e) {
   
    index_orders_[0] = 0;
    index_orders_[1] = 1;
    index_orders_[2] = 2;
    force_(data, indices, negative_gradient1, positive_gradient1, 
	   negative_gradient2, positive_gradient2,
	   negative_gradient3, positive_gradient3,
	   negative_force1_e, negative_force1_u,
	   positive_force1_l, positive_force1_e,
	   negative_force2_e, negative_force2_u,
	   positive_force2_l, positive_force2_e);
    
    index_orders_[0] = 1;
    index_orders_[1] = 0;
    index_orders_[2] = 2;
    force_(data, indices, negative_gradient1, positive_gradient1,
	   negative_gradient2, positive_gradient2,
	   negative_gradient3, positive_gradient3,
	   negative_force1_e, negative_force1_u,
	   positive_force1_l, positive_force1_e,
	   negative_force2_e, negative_force2_u,
	   positive_force2_l, positive_force2_e);
    
    index_orders_[0] = 2;
    index_orders_[1] = 1;
    index_orders_[2] = 0;
    force_(data, indices, negative_gradient2, positive_gradient2, 
	   negative_gradient3, positive_gradient3,
	   negative_gradient1, positive_gradient1,
	   negative_force1_e, negative_force1_u,
	   positive_force1_l, positive_force1_e,
	   negative_force2_e, negative_force2_u,
	   positive_force2_l, positive_force2_e);
  }

  /** @brief Computes the first/second components of the
   *         negative/positive force components.
   */
  void Eval(const Matrix &data, const ArrayList<index_t> &indices,
	    Vector &negative_force1_e, Vector &negative_force1_u,
	    Vector &positive_force1_l, Vector &positive_force1_e,
	    Matrix &negative_force2_e, Matrix &negative_force2_u,
	    Matrix &positive_force2_l, Matrix &positive_force2_e) {

    double negative_gradient1, positive_gradient1;
    double negative_gradient2, positive_gradient2;
    double negative_gradient3, positive_gradient3;
    
    // Evaluate the pairwise distances among all points.
    EvalMinMaxSquaredDistances(data, indices);

    // Evaluate the required components of the force vector.
    EvalGradients(distmat_, negative_gradient1, NULL, positive_gradient1, NULL,
		  negative_gradient2, NULL, positive_gradient2, NULL,
		  negative_gradient3, NULL, positive_gradient3, NULL);

    // Contributions to all three particles in the list.
    EvalContributions(data, indices,
		      negative_gradient1, positive_gradient1,
		      negative_gradient2, positive_gradient2,
		      negative_gradient3, positive_gradient3,
		      negative_force1_e, negative_force1_u,
		      positive_force1_l, positive_force1_e,
		      negative_force2_e, negative_force2_u,
		      positive_force2_l, positive_force2_e);
  }

  /** @brief
   *
   *  WARNING: This function assumes that each tree node contains a
   *  bounding box in three dimensions.
   */
  bool Eval(ArrayList<TTree *> &tree_nodes, double relative_error,
	    double total_n_minus_one_num_tuples) {

    // First, compute the pairwise distance among the three nodes.
    EvalMinMaxSquaredDistances(tree_nodes);

    // Do not prune if any of the minimum distances is zero.
    for(index_t i = 0; i < tree_nodes.size() - 1; i++) {
      for(index_t j = i + 1; j < tree_nodes.size(); j++) {
	if(distmat_.get(i, j) == 0) {
	  return false;
	}
      }
    }

    // The result of pruning
    bool prunable = false;

    // Temporary variables.
    double min_negative_gradient1, max_negative_gradient1, 
      min_positive_gradient1, max_positive_gradient1, min_negative_gradient2, 
      max_negative_gradient2, min_positive_gradient2, max_positive_gradient2,
      min_negative_gradient3, max_negative_gradient3, min_positive_gradient3, 
      max_positive_gradient3;

    // Then, evaluate the gradients.
    EvalGradients(distmat_, min_negative_gradient1, &max_negative_gradient1, 
		  min_positive_gradient1, &max_positive_gradient1, 
		  min_negative_gradient2, &max_negative_gradient2, 
		  min_positive_gradient2, &max_positive_gradient2, 
		  min_negative_gradient3, &max_negative_gradient3, 
		  min_positive_gradient3, &max_positive_gradient3);

    // If any of the components is computed to be NaN's or Inf's, then
    // do not attempt to approximate.
    if(isnan(min_negative_gradient1) || isnan(max_negative_gradient1) ||
       isnan(min_positive_gradient1) || isnan(max_positive_gradient1) ||
       isnan(min_negative_gradient2) || isnan(max_negative_gradient2) ||
       isnan(min_positive_gradient2) || isnan(max_positive_gradient2) ||
       isnan(min_negative_gradient3) || isnan(max_negative_gradient3) ||
       isnan(min_positive_gradient3) || isnan(max_positive_gradient3) ||
       isinf(min_negative_gradient1) || isinf(max_negative_gradient1) ||
       isinf(min_positive_gradient1) || isinf(max_positive_gradient1) ||
       isinf(min_negative_gradient2) || isinf(max_negative_gradient2) ||
       isinf(min_positive_gradient2) || isinf(max_positive_gradient2) ||
       isinf(min_negative_gradient3) || isinf(max_negative_gradient3) ||
       isinf(min_positive_gradient3) || isinf(max_positive_gradient3)) {
      return false;
    }

    // Compute approximation error.
    double negative_gradient1_error, positive_gradient1_error,
      negative_gradient2_error, positive_gradient2_error,
      negative_gradient3_error, positive_gradient3_error;
    ComputeApproximationError_
      (min_negative_gradient1, max_negative_gradient1,
       min_positive_gradient1, max_positive_gradient1,
       min_negative_gradient2, max_negative_gradient2,
       min_positive_gradient2, max_positive_gradient2,
       min_negative_gradient3, max_negative_gradient3,
       min_positive_gradient3, max_positive_gradient3,
       negative_gradient1_error, positive_gradient1_error,
       negative_gradient2_error, positive_gradient2_error,
       negative_gradient3_error, positive_gradient3_error);

    // Now determine whether all three nodes satisfy the pruning
    // conditions.
    double num_ik_pairs = tree_nodes[0]->count() * tree_nodes[2]->count();
    double num_ij_pairs = tree_nodes[0]->count() * tree_nodes[1]->count();
    double num_jk_pairs = tree_nodes[1]->count() * tree_nodes[2]->count();

    bool first_node_prunable =
      ((negative_gradient1_error + negative_gradient2_error) <=
       (relative_error / (double) total_n_minus_one_num_tuples) *
       fabs(tree_nodes[0]->stat().negative_gradient1_u)) 
      &&
      ((positive_gradient1_error + positive_gradient2_error) <=
       (relative_error / (double) total_n_minus_one_num_tuples) *
       tree_nodes[0]->stat().positive_gradient1_l) 
      &&
      (tree_nodes[2]->count() * tree_nodes[1]->stat().l1_norm_coordinate_sum_ *
       negative_gradient1_error +
       tree_nodes[1]->count() * tree_nodes[2]->stat().l1_norm_coordinate_sum_ *
       negative_gradient2_error <=
       relative_error * num_jk_pairs / total_n_minus_one_num_tuples *
       L1Norm_(tree_nodes[0]->stat().negative_gradient2_u)) 
      &&
      (tree_nodes[2]->count() * tree_nodes[1]->stat().l1_norm_coordinate_sum_ *
       positive_gradient1_error +
       tree_nodes[1]->count() * tree_nodes[2]->stat().l1_norm_coordinate_sum_ *
       positive_gradient2_error <=
       relative_error * num_jk_pairs / total_n_minus_one_num_tuples *
       L1Norm_(tree_nodes[0]->stat().positive_gradient2_l));
    
    bool second_node_prunable =
      ((negative_gradient1_error + negative_gradient3_error) <=
       (relative_error / (double) total_n_minus_one_num_tuples) *
       fabs(tree_nodes[1]->stat().negative_gradient1_u)) 
      &&
      ((positive_gradient1_error + positive_gradient3_error) <=
       (relative_error / (double) total_n_minus_one_num_tuples) *
       tree_nodes[1]->stat().positive_gradient1_l) 
      &&
      (tree_nodes[2]->count() * tree_nodes[0]->stat().l1_norm_coordinate_sum_ *
       negative_gradient1_error +
       tree_nodes[0]->count() * tree_nodes[2]->stat().l1_norm_coordinate_sum_ *
       negative_gradient3_error <=
       relative_error * num_ik_pairs / total_n_minus_one_num_tuples *
       L1Norm_(tree_nodes[1]->stat().negative_gradient2_u)) 
      &&
      (tree_nodes[2]->count() * tree_nodes[0]->stat().l1_norm_coordinate_sum_ *
       positive_gradient1_error +
       tree_nodes[0]->count() * tree_nodes[2]->stat().l1_norm_coordinate_sum_ *
       positive_gradient3_error <=
       relative_error * num_ik_pairs / total_n_minus_one_num_tuples *
       L1Norm_(tree_nodes[1]->stat().positive_gradient2_l));
    bool third_node_prunable =
      ((negative_gradient2_error + negative_gradient3_error) <=
       (relative_error / (double) total_n_minus_one_num_tuples) *
       fabs(tree_nodes[2]->stat().negative_gradient1_u)) 
      &&
      ((positive_gradient2_error + positive_gradient3_error) <=
       (relative_error / (double) total_n_minus_one_num_tuples) *
       tree_nodes[2]->stat().positive_gradient1_l) 
      &&
      (tree_nodes[1]->count() * tree_nodes[0]->stat().l1_norm_coordinate_sum_ *
       negative_gradient2_error +
       tree_nodes[0]->count() * tree_nodes[1]->stat().l1_norm_coordinate_sum_ *
       negative_gradient3_error <=
       relative_error * num_ij_pairs / total_n_minus_one_num_tuples *
       L1Norm_(tree_nodes[2]->stat().negative_gradient2_u)) 
      &&
      (tree_nodes[1]->count() * tree_nodes[0]->stat().l1_norm_coordinate_sum_ *
       positive_gradient2_error +
       tree_nodes[0]->count() * tree_nodes[1]->stat().l1_norm_coordinate_sum_ *
       positive_gradient3_error <=
       relative_error * num_ij_pairs / total_n_minus_one_num_tuples *
       L1Norm_(tree_nodes[2]->stat().positive_gradient2_l));
    
    // Prune only if all three nodes can be approximated.
    prunable = first_node_prunable && second_node_prunable &&
      third_node_prunable;
    if(prunable) {

      // First the i-th node.
      tree_nodes[0]->stat().postponed_negative_gradient1_e += 
	num_jk_pairs * 0.5 * (min_negative_gradient1 + max_negative_gradient1 +
			      min_negative_gradient2 + max_negative_gradient2);
      tree_nodes[0]->stat().postponed_negative_gradient1_u += 
	num_jk_pairs * (max_negative_gradient1 + max_negative_gradient2);
      tree_nodes[0]->stat().postponed_positive_gradient1_l += 
	num_jk_pairs * (min_positive_gradient1 + min_positive_gradient2);
      tree_nodes[0]->stat().postponed_positive_gradient1_e += 
	num_jk_pairs * 0.5 * (min_positive_gradient1 + max_positive_gradient1 +
			      min_positive_gradient2 + max_positive_gradient2);
      la::AddExpert
	(tree_nodes[2]->count() *
	 0.5 * (min_negative_gradient1 + max_negative_gradient1),
	 tree_nodes[1]->stat().coordinate_sum_,
	 &(tree_nodes[0]->stat().postponed_negative_gradient2_e));
      la::AddExpert
	(tree_nodes[1]->count() *
	 0.5 * (min_negative_gradient2 + max_negative_gradient2),
	 tree_nodes[2]->stat().coordinate_sum_,
	 &(tree_nodes[0]->stat().postponed_negative_gradient2_e));
      la::AddExpert
	(tree_nodes[2]->count() *
	 max_negative_gradient1, tree_nodes[1]->stat().coordinate_sum_,
	 &(tree_nodes[0]->stat().postponed_negative_gradient2_u));
      la::AddExpert
	(tree_nodes[1]->count() *
	 max_negative_gradient2, tree_nodes[2]->stat().coordinate_sum_,
	 &(tree_nodes[0]->stat().postponed_negative_gradient2_u));
      la::AddExpert
	(tree_nodes[2]->count() *
	 min_positive_gradient1, tree_nodes[1]->stat().coordinate_sum_,
	 &(tree_nodes[0]->stat().postponed_positive_gradient2_l));
      la::AddExpert
	(tree_nodes[1]->count() *
	 min_positive_gradient2, tree_nodes[2]->stat().coordinate_sum_,
	 &(tree_nodes[0]->stat().postponed_positive_gradient2_l));
      la::AddExpert
	(tree_nodes[2]->count() *
	 0.5 * (min_positive_gradient1 + max_positive_gradient1),
	 tree_nodes[1]->stat().coordinate_sum_,
	 &(tree_nodes[0]->stat().postponed_positive_gradient2_e));
      la::AddExpert
	(tree_nodes[1]->count() *
	 0.5 * (min_positive_gradient2 + max_positive_gradient2),
	 tree_nodes[2]->stat().coordinate_sum_,
	 &(tree_nodes[0]->stat().postponed_positive_gradient2_e));

      // Then the j-th node.
      tree_nodes[1]->stat().postponed_negative_gradient1_e += 
	num_ik_pairs * 0.5 * (min_negative_gradient1 + max_negative_gradient1 +
			      min_negative_gradient3 + max_negative_gradient3);
      tree_nodes[1]->stat().postponed_negative_gradient1_u += 
	num_ik_pairs * (max_negative_gradient1 + max_negative_gradient3);
      tree_nodes[1]->stat().postponed_positive_gradient1_l += 
	num_ik_pairs * (min_positive_gradient1 + min_positive_gradient3);
      tree_nodes[1]->stat().postponed_positive_gradient1_e += 
	num_ik_pairs * 0.5 * (min_positive_gradient1 + max_positive_gradient1 +
			      min_positive_gradient3 + max_positive_gradient3);
      la::AddExpert
	(tree_nodes[2]->count() *
	 0.5 * (min_negative_gradient1 + max_negative_gradient1),
	 tree_nodes[0]->stat().coordinate_sum_,
	 &(tree_nodes[1]->stat().postponed_negative_gradient2_e));
      la::AddExpert
	(tree_nodes[0]->count() *
	 0.5 * (min_negative_gradient3 + max_negative_gradient3),
	 tree_nodes[2]->stat().coordinate_sum_,
	 &(tree_nodes[1]->stat().postponed_negative_gradient2_e));
      la::AddExpert
	(tree_nodes[2]->count() *
	 max_negative_gradient1, tree_nodes[0]->stat().coordinate_sum_,
	 &(tree_nodes[1]->stat().postponed_negative_gradient2_u));
      la::AddExpert
	(tree_nodes[0]->count() *
	 max_negative_gradient3, tree_nodes[2]->stat().coordinate_sum_,
	 &(tree_nodes[1]->stat().postponed_negative_gradient2_u));
      la::AddExpert
	(tree_nodes[2]->count() *
	 min_positive_gradient1, tree_nodes[0]->stat().coordinate_sum_,
	 &(tree_nodes[1]->stat().postponed_positive_gradient2_l));
      la::AddExpert
	(tree_nodes[0]->count() *
	 min_positive_gradient3, tree_nodes[2]->stat().coordinate_sum_,
	 &(tree_nodes[1]->stat().postponed_positive_gradient2_l));
      la::AddExpert
	(tree_nodes[2]->count() *
	 0.5 * (min_positive_gradient1 + max_positive_gradient1),
	 tree_nodes[0]->stat().coordinate_sum_,
	 &(tree_nodes[1]->stat().postponed_positive_gradient2_e));
      la::AddExpert
	(tree_nodes[0]->count() *
	 0.5 * (min_positive_gradient3 + max_positive_gradient3),
	 tree_nodes[2]->stat().coordinate_sum_,
	 &(tree_nodes[1]->stat().postponed_positive_gradient2_e));

      // Then the k-th node.
      tree_nodes[2]->stat().postponed_negative_gradient1_e += 
	num_ij_pairs * 0.5 * (min_negative_gradient2 + max_negative_gradient2 +
			      min_negative_gradient3 + max_negative_gradient3);
      tree_nodes[2]->stat().postponed_negative_gradient1_u += 
	num_ij_pairs * (max_negative_gradient2 + max_negative_gradient3);
      tree_nodes[2]->stat().postponed_positive_gradient1_l += 
	num_ij_pairs * (min_positive_gradient2 + min_positive_gradient3);
      tree_nodes[2]->stat().postponed_positive_gradient1_e += 
	num_ij_pairs * 0.5 * (min_positive_gradient2 + max_positive_gradient2 +
			      min_positive_gradient3 + max_positive_gradient3);
      la::AddExpert
	(tree_nodes[1]->count() *
	 0.5 * (min_negative_gradient2 + max_negative_gradient2),
	 tree_nodes[0]->stat().coordinate_sum_,
	 &(tree_nodes[2]->stat().postponed_negative_gradient2_e));
      la::AddExpert
	(tree_nodes[0]->count() *
	 0.5 * (min_negative_gradient3 + max_negative_gradient3),
	 tree_nodes[1]->stat().coordinate_sum_,
	 &(tree_nodes[2]->stat().postponed_negative_gradient2_e));
      la::AddExpert
	(tree_nodes[1]->count() *
	 max_negative_gradient2, tree_nodes[0]->stat().coordinate_sum_,
	 &(tree_nodes[2]->stat().postponed_negative_gradient2_u));
      la::AddExpert
	(tree_nodes[0]->count() *
	 max_negative_gradient3, tree_nodes[1]->stat().coordinate_sum_,
	 &(tree_nodes[2]->stat().postponed_negative_gradient2_u));
      la::AddExpert
	(tree_nodes[1]->count() *
	 min_positive_gradient2, tree_nodes[0]->stat().coordinate_sum_,
	 &(tree_nodes[2]->stat().postponed_positive_gradient2_l));
      la::AddExpert
	(tree_nodes[0]->count() *
	 min_positive_gradient3, tree_nodes[1]->stat().coordinate_sum_,
	 &(tree_nodes[2]->stat().postponed_positive_gradient2_l));
      la::AddExpert
	(tree_nodes[1]->count() *
	 0.5 * (min_positive_gradient2 + max_positive_gradient2),
	 tree_nodes[0]->stat().coordinate_sum_,
	 &(tree_nodes[2]->stat().postponed_positive_gradient2_e));
      la::AddExpert
	(tree_nodes[0]->count() *
	 0.5 * (min_positive_gradient3 + max_positive_gradient3),
	 tree_nodes[1]->stat().coordinate_sum_,
	 &(tree_nodes[2]->stat().postponed_positive_gradient2_e));
    }

    return prunable;
  }

};

#endif
