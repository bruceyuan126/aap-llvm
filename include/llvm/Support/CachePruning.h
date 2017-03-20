//=- CachePruning.h - Helper to manage the pruning of a cache dir -*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements pruning of a directory intended for cache storage, using
// various policies.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_CACHE_PRUNING_H
#define LLVM_SUPPORT_CACHE_PRUNING_H

#include "llvm/ADT/StringRef.h"
#include <chrono>

namespace llvm {

template <typename T> class Expected;

/// Policy for the pruneCache() function. A default constructed
/// CachePruningPolicy provides a reasonable default policy.
struct CachePruningPolicy {
  /// The pruning interval. This is intended to be used to avoid scanning the
  /// directory too often. It does not impact the decision of which file to
  /// prune. A value of 0 forces the scan to occur.
  std::chrono::seconds Interval = std::chrono::seconds(1200);

  /// The expiration for a file. When a file hasn't been accessed for Expiration
  /// seconds, it is removed from the cache. A value of 0 disables the
  /// expiration-based pruning.
  std::chrono::seconds Expiration = std::chrono::hours(7 * 24); // 1w

  /// The maximum size for the cache directory, in terms of percentage of the
  /// available space on the the disk. Set to 100 to indicate no limit, 50 to
  /// indicate that the cache size will not be left over half the available disk
  /// space. A value over 100 will be reduced to 100. A value of 0 disables the
  /// size-based pruning.
  unsigned PercentageOfAvailableSpace = 75;
};

/// Parse the given string as a cache pruning policy. Defaults are taken from a
/// default constructed CachePruningPolicy object.
/// For example: "prune_interval=30s:prune_after=24h:cache_size=50%"
/// which means a pruning interval of 30 seconds, expiration time of 24 hours
/// and maximum cache size of 50% of available disk space.
Expected<CachePruningPolicy> parseCachePruningPolicy(StringRef PolicyStr);

/// Peform pruning using the supplied policy, returns true if pruning
/// occured, i.e. if Policy.Interval was expired.
bool pruneCache(StringRef Path, CachePruningPolicy Policy);

} // namespace llvm

#endif
