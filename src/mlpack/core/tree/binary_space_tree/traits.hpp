/**
 * @file traits.hpp
 * @author Ryan Curtin
 *
 * Specialization of the TreeTraits class for the BinarySpaceTree type of tree.
 */
#ifndef __MLPACK_CORE_TREE_BINARY_SPACE_TREE_TRAITS_HPP
#define __MLPACK_CORE_TREE_BINARY_SPACE_TREE_TRAITS_HPP

#include <mlpack/core/tree/tree_traits.hpp>

namespace mlpack {
namespace tree {

/**
 * This is a specialization of the TreeType class to the BinarySpaceTree tree
 * type.  It defines characteristics of the binary space tree, and is used to
 * help write tree-independent (but still optimized) tree-based algorithms.  See
 * mlpack/core/tree/tree_traits.hpp for more information.
 */
template<typename BoundType,
         typename StatisticType,
         typename MatType>
class TreeTraits<BinarySpaceTree<BoundType, StatisticType, MatType> >
{
 public:
  /**
   * The binary space tree cannot easily calculate the distance from a node to
   * its parent; so BinarySpaceTree<...>::ParentDistance() does not exist.
   */
  static const bool HasParentDistance = false;

  /**
   * Each binary space tree node has two children which represent
   * non-overlapping subsets of the space which the node represents.  Therefore,
   * children are not overlapping.
   */
  static const bool HasOverlappingChildren = false;

  /**
   * There is no guarantee that the first point in a node is its centroid.
   */
  static const bool FirstPointIsCentroid = false;
};

}; // namespace tree
}; // namespace mlpack

#endif
