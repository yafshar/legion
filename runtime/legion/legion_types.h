/* Copyright 2016 Stanford University, NVIDIA Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __LEGION_TYPES_H__
#define __LEGION_TYPES_H__

/**
 * \file legion_types.h
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <stdint.h>

#include "limits.h"

#include <map>
#include <set>
#include <list>
#include <deque>
#include <vector>

#include "legion_config.h"

// Make sure we have the appropriate defines in place for including realm
#define REALM_USE_LEGION_LAYOUT_CONSTRAINTS
#include "realm.h"

namespace BindingLib { class Utility; } // BindingLib namespace

namespace Legion {
  /**
   * \struct LegionStaticAssert
   * Help with static assertions.
   */
  template<bool> struct LegionStaticAssert;
  template<> struct LegionStaticAssert<true> { };
#define LEGION_STATIC_ASSERT(condition) \
  do { LegionStaticAssert<(condition)>(); } while (0)

  /**
   * \struct LegionTypeEquality
   * Help with checking equality of types.
   */
  template<typename T, typename U>
  struct LegionTypeInequality {
  public:
    static const bool value = true;
  };
  template<typename T>
  struct LegionTypeInequality<T,T> {
  public:
    static const bool value = false;
  };

  typedef ::legion_error_t LegionErrorType;
  typedef ::legion_privilege_mode_t PrivilegeMode;
  typedef ::legion_allocate_mode_t AllocateMode;
  typedef ::legion_coherence_property_t CoherenceProperty;
  typedef ::legion_region_flags_t RegionFlags;
  typedef ::legion_handle_type_t HandleType;
  typedef ::legion_partition_kind_t PartitionKind;
  typedef ::legion_dependence_type_t DependenceType;
  typedef ::legion_index_space_kind_t IndexSpaceKind;
  typedef ::legion_file_mode_t LegionFileMode;

  // Forward declarations for user level objects
  // legion.h
  class IndexSpace;
  class IndexPartition;
  class FieldSpace;
  class LogicalRegion;
  class LogicalPartition;
  class IndexAllocator;
  class FieldAllocator;
  class TaskArgument;
  class ArgumentMap;
  class Lock;
  struct LockRequest;
  class Grant;
  class PhaseBarrier;
  struct RegionRequirement;
  struct IndexSpaceRequirement;
  struct FieldSpaceRequirement;
  struct TaskLauncher;
  struct IndexLauncher;
  struct InlineLauncher;
  struct CopyLauncher;
  struct AcquireLauncher;
  struct ReleaseLauncher;
  struct FillLauncher;
  struct LayoutConstraintRegistrar;
  struct TaskVariantRegistrar;
  struct TaskGeneratorArguments;
  class Future;
  class FutureMap;
  class Predicate;
  class PhysicalRegion;
  class IndexIterator;
  template<typename T> struct ColoredPoints; 
  struct InputArgs;
  class ProjectionFunctor;
  class Task;
  class Copy;
  class InlineMapping;
  class Acquire;
  class Release;
  class Close;
  class Runtime;
  // For backwards compatibility
  typedef Runtime HighLevelRuntime;
  // Helper for saving instantiated template functions
  struct SerdezRedopFns;

  // Forward declarations for compiler level objects
  // legion.h
  class ColoringSerializer;
  class DomainColoringSerializer;

  // Forward declarations for wrapper tasks
  // legion.h
  class LegionTaskWrapper;
  class LegionSerialization;

  // Forward declarations for C wrapper objects
  // legion_c_util.h
  class TaskResult;
  class CObjectWrapper;

  // legion_utilities.h
  struct RegionUsage;
  class AutoLock;
  class ImmovableAutoLock;
  class ColorPoint;
  class Serializer;
  class Deserializer;
  template<typename T> class Fraction;
  template<typename T, unsigned int MAX, 
           unsigned SHIFT, unsigned MASK> class BitMask;
  template<typename T, unsigned int MAX,
           unsigned SHIFT, unsigned MASK> class TLBitMask;
#ifdef __SSE2__
  template<unsigned int MAX> class SSEBitMask;
  template<unsigned int MAX> class SSETLBitMask;
#endif
#ifdef __AVX__
  template<unsigned int MAX> class AVXBitMask;
  template<unsigned int MAX> class AVXTLBitMask;
#endif
  template<typename T, unsigned LOG2MAX> class BitPermutation;
  template<typename IT, typename DT, bool BIDIR = false> class IntegerSet;

  // legion_constraint.h
  class ISAConstraint;
  class ProcessorConstraint;
  class ResourceConstraint;
  class LaunchConstraint;
  class ColocationConstraint;
  class ExecutionConstraintSet;

  class SpecializedConstraint;
  class MemoryConstraint;
  class FieldConstraint;
  class OrderingConstraint;
  class SplittingConstraint;
  class DimensionConstraint;
  class AlignmentConstraint;
  class OffsetConstraint;
  class PointerConstraint;
  class LayoutConstraintSet;
  class TaskLayoutConstraintSet;

  namespace Mapping {
    class Mappable; 
    class PhysicalInstance;
    class MapperEvent;
    class ProfilingRequestSet;
    class Mapper;
    class DefaultMapper;
  };
  
  namespace Internal {

    enum OpenState {
      NOT_OPEN            = 0,
      OPEN_READ_ONLY      = 1,
      OPEN_READ_WRITE     = 2, // unknown dirty information below
      OPEN_SINGLE_REDUCE  = 3, // only one open child with reductions below
      OPEN_MULTI_REDUCE   = 4, // multiple open children with same reduction
    }; 

    // redop IDs - none used in HLR right now, but 0 isn't allowed
    enum {
      REDOP_ID_AVAILABLE    = 1,
    };

    // Runtime task numbering 
    enum {
      INIT_TASK_ID            = Realm::Processor::TASK_ID_PROCESSOR_INIT,
      SHUTDOWN_TASK_ID        = Realm::Processor::TASK_ID_PROCESSOR_SHUTDOWN,
      HLR_TASK_ID             = Realm::Processor::TASK_ID_FIRST_AVAILABLE,
      HLR_LEGION_PROFILING_ID = Realm::Processor::TASK_ID_FIRST_AVAILABLE+1,
      HLR_MAPPER_PROFILING_ID = Realm::Processor::TASK_ID_FIRST_AVAILABLE+2,
      HLR_LAUNCH_TOP_LEVEL_ID = Realm::Processor::TASK_ID_FIRST_AVAILABLE+3,
      TASK_ID_AVAILABLE       = Realm::Processor::TASK_ID_FIRST_AVAILABLE+4,
    };

    // Enumeration of high-level runtime tasks
    enum HLRTaskID {
      HLR_SCHEDULER_ID,
      HLR_POST_END_ID,
      HLR_DEFERRED_MAPPING_TRIGGER_ID,
      HLR_DEFERRED_RESOLUTION_TRIGGER_ID,
      HLR_DEFERRED_EXECUTION_TRIGGER_ID,
      HLR_DEFERRED_COMMIT_TRIGGER_ID,
      HLR_DEFERRED_POST_MAPPED_ID,
      HLR_DEFERRED_EXECUTE_ID,
      HLR_DEFERRED_COMPLETE_ID,
      HLR_DEFERRED_COMMIT_ID,
      HLR_RECLAIM_LOCAL_FIELD_ID,
      HLR_DEFERRED_COLLECT_ID,
      HLR_TRIGGER_DEPENDENCE_ID,
      HLR_TRIGGER_OP_ID,
      HLR_TRIGGER_TASK_ID,
      HLR_DEFERRED_RECYCLE_ID,
      HLR_DEFERRED_SLICE_ID,
      HLR_MUST_INDIV_ID,
      HLR_MUST_INDEX_ID,
      HLR_MUST_MAP_ID,
      HLR_MUST_DIST_ID,
      HLR_MUST_LAUNCH_ID,
      HLR_DEFERRED_FUTURE_SET_ID,
      HLR_DEFERRED_FUTURE_MAP_SET_ID,
      HLR_RESOLVE_FUTURE_PRED_ID,
      HLR_MPI_RANK_ID,
      HLR_CONTRIBUTE_COLLECTIVE_ID,
      HLR_STATE_ANALYSIS_ID,
      HLR_MAPPER_TASK_ID,
      HLR_DISJOINTNESS_TASK_ID,
      HLR_PART_INDEPENDENCE_TASK_ID,
      HLR_SPACE_INDEPENDENCE_TASK_ID,
      HLR_PENDING_CHILD_TASK_ID,
      HLR_DECREMENT_PENDING_TASK_ID,
      HLR_SEND_VERSION_STATE_TASK_ID,
      HLR_ADD_TO_DEP_QUEUE_TASK_ID,
      HLR_WINDOW_WAIT_TASK_ID,
      HLR_ISSUE_FRAME_TASK_ID,
      HLR_CONTINUATION_TASK_ID,
      HLR_MAPPER_CONTINUATION_TASK_ID,
      HLR_TASK_IMPL_SEMANTIC_INFO_REQ_TASK_ID,
      HLR_INDEX_SPACE_SEMANTIC_INFO_REQ_TASK_ID,
      HLR_INDEX_PART_SEMANTIC_INFO_REQ_TASK_ID,
      HLR_FIELD_SPACE_SEMANTIC_INFO_REQ_TASK_ID,
      HLR_FIELD_SEMANTIC_INFO_REQ_TASK_ID,
      HLR_REGION_SEMANTIC_INFO_REQ_TASK_ID,
      HLR_PARTITION_SEMANTIC_INFO_REQ_TASK_ID,
      HLR_SELECT_TUNABLE_TASK_ID,
      HLR_DEFERRED_ENQUEUE_TASK_ID,
      HLR_MESSAGE_ID, // These four must be last (see issue_runtime_meta_task)
      HLR_SHUTDOWN_ATTEMPT_TASK_ID,
      HLR_SHUTDOWN_NOTIFICATION_TASK_ID,
      HLR_SHUTDOWN_RESPONSE_TASK_ID,
      HLR_LAST_TASK_ID, // This one should always be last
    };

    // Make this a macro so we can keep it close to 
    // declaration of the task IDs themselves
#define HLR_TASK_DESCRIPTIONS(name)                               \
      const char *name[HLR_LAST_TASK_ID] = {                      \
        "Scheduler",                                              \
        "Post-Task Execution",                                    \
        "Deferred Mapping Trigger",                               \
        "Deferred Resolution Trigger",                            \
        "Deferred Execution Trigger",                             \
        "Deferred Commit Trigger",                                \
        "Deferred Post Mapped",                                   \
        "Deferred Execute",                                       \
        "Deferred Complete",                                      \
        "Deferred Commit",                                        \
        "Reclaim Local Field",                                    \
        "Garbage Collection",                                     \
        "Logical Dependence Analysis",                            \
        "Operation Physical Dependence Analysis",                 \
        "Task Physical Dependence Analysis",                      \
        "Deferred Recycle",                                       \
        "Deferred Slice",                                         \
        "Must Individual Task Dependence Analysis",               \
        "Must Index Task Dependence Analysis",                    \
        "Must Task Physical Dependence Analysis",                 \
        "Must Task Distribution",                                 \
        "Must Task Launch",                                       \
        "Deferred Future Set",                                    \
        "Deferred Future Map Set",                                \
        "Resolve Future Predicate",                               \
        "Update MPI Rank Info",                                   \
        "Contribute Collective",                                  \
        "State Analaysis",                                        \
        "Mapper Task",                                            \
        "Disjointness Test",                                      \
        "Partition Independence Test",                            \
        "Index Space Independence Test",                          \
        "Remove Pending Child",                                   \
        "Decrement Pending Task",                                 \
        "Send Version State",                                     \
        "Add to Dependence Queue",                                \
        "Window Wait",                                            \
        "Issue Frame",                                            \
        "Legion Continuation",                                    \
        "Mapper Continuation",                                    \
        "Task Impl Semantic Request",                             \
        "Index Space Semantic Request",                           \
        "Index Partition Semantic Request",                       \
        "Field Space Semantic Request",                           \
        "Field Semantic Request",                                 \
        "Region Semantic Request",                                \
        "Partition Semantic Request",                             \
        "Select Tunable",                                         \
        "Deferred Task Enqueue",                                  \
        "Remote Message",                                         \
        "Shutdown Attempt",                                       \
        "Shutdown Notification",                                  \
        "Shutdown Response",                                      \
      };

    enum MappingCallKind {
      GET_MAPPER_NAME_CALL,
      GET_MAPER_SYNC_MODEL_CALL,
      SELECT_TASK_OPTIONS_CALL,
      PREMAP_TASK_CALL,
      SLICE_TASK_CALL,
      MAP_TASK_CALL,
      SELECT_VARIANT_CALL,
      POSTMAP_TASK_CALL,
      TASK_SELECT_SOURCES_CALL,
      TASK_SPECULATE_CALL,
      TASK_REPORT_PROFILING_CALL,
      MAP_INLINE_CALL,
      INLINE_SELECT_SOURCES_CALL,
      INLINE_REPORT_PROFILING_CALL,
      MAP_COPY_CALL,
      COPY_SELECT_SOURCES_CALL,
      COPY_SPECULATE_CALL,
      COPY_REPORT_PROFILING_CALL,
      MAP_CLOSE_CALL,
      CLOSE_SELECT_SOURCES_CALL,
      CLOSE_REPORT_PROFILING_CALL,
      MAP_ACQUIRE_CALL,
      ACQUIRE_SPECULATE_CALL,
      ACQUIRE_REPORT_PROFILING_CALL,
      MAP_RELEASE_CALL,
      RELEASE_SELECT_SOURCES_CALL,
      RELEASE_SPECULATE_CALL,
      RELEASE_REPORT_PROFILING_CALL,
      CONFIGURE_CONTEXT_CALL,
      SELECT_TUNABLE_VALUE_CALL,
      MAP_MUST_EPOCH_CALL,
      MAP_DATAFLOW_GRAPH_CALL,
      SELECT_TASKS_TO_MAP_CALL,
      SELECT_STEAL_TARGETS_CALL,
      PERMIT_STEAL_REQUEST_CALL,
      HANDLE_MESSAGE_CALL,
      HANDLE_TASK_RESULT_CALL,
      LAST_MAPPER_CALL,
    };

#define MAPPER_CALL_NAMES(name)                     \
    const char *name[LAST_MAPPER_CALL] = {          \
      "get_mapper_name",                            \
      "get_mapper_sync_model",                      \
      "select_task_options",                        \
      "premap_task",                                \
      "slice_task",                                 \
      "map_task",                                   \
      "select_task_variant",                        \
      "postmap_task",                               \
      "select_task_sources",                        \
      "speculate (for task)",                       \
      "report profiling (for task)",                \
      "map_inline",                                 \
      "select_inline_sources",                      \
      "report profiling (for inline)",              \
      "map_copy",                                   \
      "select_copy_sources",                        \
      "speculate (for copy)",                       \
      "report_profiling (for copy)",                \
      "map_close",                                  \
      "select_close_sources",                       \
      "report_profiling (for close)",               \
      "map_acquire",                                \
      "speculate (for acquire)",                    \
      "report_profiling (for acquire)",             \
      "map_release",                                \
      "select_release_sources",                     \
      "speculate (for release)",                    \
      "report_profiling (for release)",             \
      "configure_context",                          \
      "select_tunable_value",                       \
      "map_must_epoch",                             \
      "map_dataflow_graph",                         \
      "select_tasks_to_map",                        \
      "select_steal_targets",                       \
      "permit_steal_request",                       \
      "handle_message",                             \
      "handle_task_result",                         \
    }

    enum HLRPriority {
      HLR_THROUGHPUT_PRIORITY = 0, // don't care so much
      HLR_LATENCY_PRIORITY = 1, // care some but not too much
      HLR_RESOURCE_PRIORITY = 2, // this needs to be first
    };

    enum VirtualChannelKind {
      DEFAULT_VIRTUAL_CHANNEL = 0,
      INDEX_AND_FIELD_VIRTUAL_CHANNEL = 1,
      LOGICAL_TREE_VIRTUAL_CHANNEL = 2,
      DISTRIBUTED_VIRTUAL_CHANNEL = 3,
      MAPPER_VIRTUAL_CHANNEL = 4,
      SEMANTIC_INFO_VIRTUAL_CHANNEL = 5,
      LAYOUT_CONSTRAINT_VIRTUAL_CHANNEL = 6,
      CONTEXT_VIRTUAL_CHANNEL = 7,
      MANAGER_VIRTUAL_CHANNEL = 8,
      VIEW_VIRTUAL_CHANNEL = 9,
      VARIANT_VIRTUAL_CHANNEL = 10,
      MAX_NUM_VIRTUAL_CHANNELS = 11, // this one must be last
    };

    enum MessageKind {
      TASK_MESSAGE,
      STEAL_MESSAGE,
      ADVERTISEMENT_MESSAGE,
      SEND_INDEX_SPACE_NODE,
      SEND_INDEX_SPACE_REQUEST,
      SEND_INDEX_SPACE_RETURN,
      SEND_INDEX_SPACE_CHILD_REQUEST,
      SEND_INDEX_PARTITION_NODE,
      SEND_INDEX_PARTITION_REQUEST,
      SEND_INDEX_PARTITION_RETURN,
      SEND_INDEX_PARTITION_CHILD_REQUEST,
      SEND_FIELD_SPACE_NODE,
      SEND_FIELD_SPACE_REQUEST,
      SEND_FIELD_SPACE_RETURN,
      SEND_FIELD_ALLOC_REQUEST,
      SEND_FIELD_ALLOC_NOTIFICATION,
      SEND_FIELD_SPACE_TOP_ALLOC,
      SEND_FIELD_FREE,
      SEND_TOP_LEVEL_REGION_REQUEST,
      SEND_TOP_LEVEL_REGION_RETURN,
      SEND_LOGICAL_REGION_NODE,
      INDEX_SPACE_DESTRUCTION_MESSAGE,
      INDEX_PARTITION_DESTRUCTION_MESSAGE,
      FIELD_SPACE_DESTRUCTION_MESSAGE,
      LOGICAL_REGION_DESTRUCTION_MESSAGE,
      LOGICAL_PARTITION_DESTRUCTION_MESSAGE,
      INDIVIDUAL_REMOTE_MAPPED,
      INDIVIDUAL_REMOTE_COMPLETE,
      INDIVIDUAL_REMOTE_COMMIT,
      SLICE_REMOTE_MAPPED,
      SLICE_REMOTE_COMPLETE,
      SLICE_REMOTE_COMMIT,
      DISTRIBUTED_REMOTE_REGISTRATION,
      DISTRIBUTED_VALID_UPDATE,
      DISTRIBUTED_GC_UPDATE,
      DISTRIBUTED_RESOURCE_UPDATE,
      DISTRIBUTED_CREATE_ADD,
      DISTRIBUTED_CREATE_REMOVE,
      SEND_ATOMIC_RESERVATION_REQUEST,
      SEND_ATOMIC_RESERVATION_RESPONSE,
      SEND_MATERIALIZED_VIEW,
      SEND_MATERIALIZED_UPDATE,
      SEND_COMPOSITE_VIEW,
      SEND_FILL_VIEW,
      SEND_REDUCTION_VIEW,
      SEND_REDUCTION_UPDATE,
      SEND_INSTANCE_MANAGER,
      SEND_REDUCTION_MANAGER,
      SEND_CREATE_TOP_VIEW_REQUEST,
      SEND_CREATE_TOP_VIEW_RESPONSE,
      SEND_SUBVIEW_DID_REQUEST,
      SEND_SUBVIEW_DID_RESPONSE,
      SEND_VIEW_REQUEST,
      SEND_MANAGER_REQUEST,
      SEND_FUTURE_RESULT,
      SEND_FUTURE_SUBSCRIPTION,
      SEND_MAPPER_MESSAGE,
      SEND_MAPPER_BROADCAST,
      SEND_TASK_IMPL_SEMANTIC_REQ,
      SEND_INDEX_SPACE_SEMANTIC_REQ,
      SEND_INDEX_PARTITION_SEMANTIC_REQ,
      SEND_FIELD_SPACE_SEMANTIC_REQ,
      SEND_FIELD_SEMANTIC_REQ,
      SEND_LOGICAL_REGION_SEMANTIC_REQ,
      SEND_LOGICAL_PARTITION_SEMANTIC_REQ,
      SEND_TASK_IMPL_SEMANTIC_INFO,
      SEND_INDEX_SPACE_SEMANTIC_INFO,
      SEND_INDEX_PARTITION_SEMANTIC_INFO,
      SEND_FIELD_SPACE_SEMANTIC_INFO,
      SEND_FIELD_SEMANTIC_INFO,
      SEND_LOGICAL_REGION_SEMANTIC_INFO,
      SEND_LOGICAL_PARTITION_SEMANTIC_INFO,
      SEND_REMOTE_CONTEXT_REQUEST,
      SEND_REMOTE_CONTEXT_RESPONSE,
      SEND_REMOTE_CONTEXT_FREE,
      SEND_REMOTE_CONVERT_VIRTUAL,
      SEND_VERSION_STATE_PATH,
      SEND_VERSION_STATE_INIT,
      SEND_VERSION_STATE_REQUEST,
      SEND_VERSION_STATE_RESPONSE,
      SEND_INSTANCE_REQUEST,
      SEND_INSTANCE_RESPONSE,
      SEND_GC_PRIORITY_UPDATE,
      SEND_NEVER_GC_RESPONSE,
      SEND_ACQUIRE_REQUEST,
      SEND_ACQUIRE_RESPONSE,
      SEND_BACK_LOGICAL_STATE,
      SEND_VARIANT_REQUEST,
      SEND_VARIANT_RESPONSE,
      SEND_CONSTRAINT_REQUEST,
      SEND_CONSTRAINT_RESPONSE,
      SEND_CONSTRAINT_RELEASE,
      SEND_CONSTRAINT_REMOVAL,
      SEND_TOP_LEVEL_TASK_REQUEST,
      SEND_TOP_LEVEL_TASK_COMPLETE,
      SEND_SHUTDOWN_NOTIFICATION,
      SEND_SHUTDOWN_RESPONSE,
      LAST_SEND_KIND, // This one must be last
    };

#define HLR_MESSAGE_DESCRIPTIONS(name)                                \
      const char *name[LAST_SEND_KIND] = {                            \
        "Task Message",                                               \
        "Steal Message",                                              \
        "Advertisement Message",                                      \
        "Send Index Space Node",                                      \
        "Send Index Space Request",                                   \
        "Send Index Space Return",                                    \
        "Send Index Space Child Request",                             \
        "Send Index Partition Node",                                  \
        "Send Index Partition Request",                               \
        "Send Index Partition Return",                                \
        "Send Index Partition Child Request",                         \
        "Send Field Space Node",                                      \
        "Send Field Space Request",                                   \
        "Send Field Space Return",                                    \
        "Send Field Alloc Request",                                   \
        "Send Field Alloc Notification",                              \
        "Send Field Space Top Alloc",                                 \
        "Send Field Free",                                            \
        "Send Top Level Region Request",                              \
        "Send Top Level Region Return",                               \
        "Send Logical Region Node",                                   \
        "Index Space Destruction",                                    \
        "Index Partition Destruction",                                \
        "Field Space Destruction",                                    \
        "Logical Region Destruction",                                 \
        "Logical Partition Destruction",                              \
        "Individual Remote Mapped",                                   \
        "Individual Remote Complete",                                 \
        "Individual Remote Commit",                                   \
        "Slice Remote Mapped",                                        \
        "Slice Remote Complete",                                      \
        "Slice Remote Commit",                                        \
        "Distributed Remote Registration",                            \
        "Distributed Valid Update",                                   \
        "Distributed GC Update",                                      \
        "Distributed Resource Update",                                \
        "Distributed Create Add",                                     \
        "Distributed Create Remove",                                  \
        "Send Atomic Reservation Request",                            \
        "Send Atomic Reservation Response",                           \
        "Send Materialized View",                                     \
        "Send Materialized Update",                                   \
        "Send Composite View",                                        \
        "Send Fill View",                                             \
        "Send Reduction View",                                        \
        "Send Reduction Update",                                      \
        "Send Instance Manager",                                      \
        "Send Reduction Manager",                                     \
        "Send Create Top View Request",                               \
        "Send Create Top View Response",                              \
        "Send Subview DID Request",                                   \
        "Send Subview DID Response",                                  \
        "Send View Request",                                          \
        "Send Manager Request",                                       \
        "Send Future Result",                                         \
        "Send Future Subscription",                                   \
        "Send Mapper Message",                                        \
        "Send Mapper Broadcast",                                      \
        "Send Task Impl Semantic Req",                                \
        "Send Index Space Semantic Req",                              \
        "Send Index Partition Semantic Req",                          \
        "Send Field Space Semantic Req",                              \
        "Send Field Semantic Req",                                    \
        "Send Logical Region Semantic Req",                           \
        "Send Logical Partition Semantic Req",                        \
        "Send Task Impl Semantic Info",                               \
        "Send Index Space Semantic Info",                             \
        "Send Index Partition Semantic Info",                         \
        "Send Field Space Semantic Info",                             \
        "Send Field Semantic Info",                                   \
        "Send Logical Region Semantic Info",                          \
        "Send Logical Partition Semantic Info",                       \
        "Send Remote Context Request",                                \
        "Send Remote Context Response",                               \
        "Send Remote Context Free",                                   \
        "Send Remote Convert Virtual Instances",                      \
        "Send Version State Path",                                    \
        "Send Version State Init",                                    \
        "Send Version State Request",                                 \
        "Send Version State Response",                                \
        "Send Instance Request",                                      \
        "Send Instance Response",                                     \
        "Send GC Priority Update",                                    \
        "Send Never GC Response",                                     \
        "Send Acquire Request",                                       \
        "Send Acquire Response",                                      \
        "Send Back Logical State",                                    \
        "Send Task Variant Request",                                  \
        "Send Task Variant Response",                                 \
        "Send Constraint Request",                                    \
        "Send Constraint Response",                                   \
        "Send Constraint Release",                                    \
        "Send Constraint Removal",                                    \
        "Top Level Task Request",                                     \
        "Top Level Task Complete",                                    \
        "Send Shutdown Notification",                                 \
        "Send Shutdown Response",                                     \
      };

    enum SemanticInfoKind {
      INDEX_SPACE_SEMANTIC,
      INDEX_PARTITION_SEMANTIC,
      FIELD_SPACE_SEMANTIC,
      FIELD_SEMANTIC,
      LOGICAL_REGION_SEMANTIC,
      LOGICAL_PARTITION_SEMANTIC,
      TASK_SEMANTIC,
    };

    // Forward declarations for runtime level objects
    // runtime.h
    class Collectable;
    class ArgumentMapImpl;
    class ArgumentMapStore;
    class FutureImpl;
    class FutureMapImpl;
    class PhysicalRegionImpl;
    class GrantImpl;
    class PredicateImpl;
    class MPILegionHandshakeImpl;
    class ProcessorManager;
    class MemoryManager;
    class MessageManager;
    class GarbageCollectionEpoch;
    class TaskImpl;
    class VariantImpl;
    class LayoutConstraints;
    class GeneratorImpl;
    class Runtime;

    // legion_ops.h
    class Operation;
    class SpeculativeOp;
    class MapOp;
    class CopyOp;
    class FenceOp;
    class FrameOp;
    class DeletionOp;
    class CloseOp;
    class TraceCloseOp;
    class InterCloseOp;
    class ReadCloseOp;
    class PostCloseOp;
    class VirtualCloseOp;
    class AcquireOp;
    class ReleaseOp;
    class DynamicCollectiveOp;
    class FuturePredOp;
    class NotPredOp;
    class AndPredOp;
    class OrPredOp;
    class MustEpochOp;
    class PendingPartitionOp;
    class DependentPartitionOp;
    class FillOp;
    class AttachOp;
    class DetachOp;
    class TimingOp;
    class TaskOp;

    // legion_tasks.h
    class SingleTask;
    class MultiTask;
    class IndividualTask;
    class PointTask;
    class WrapperTask;
    class RemoteTask;
    class InlineTask;
    class IndexTask;
    class SliceTask;
    class RemoteTask;
    class MinimalPoint;
    
    // legion_trace.h
    class LegionTrace;
    class TraceCaptureOp;
    class TraceCompleteOp;

    // region_tree.h
    class RegionTreeForest;
    class IndexTreeNode;
    class IndexSpaceNode;
    class IndexPartNode;
    class FieldSpaceNode;
    class RegionTreeNode;
    class RegionNode;
    class PartitionNode;

    class RegionTreeContext;
    class RegionTreePath;
    class FatTreePath;
    class PathTraverser;
    class NodeTraverser;
    class PhysicalTraverser;
    class PremapTraverser;
    class MappingTraverser;
    class RestrictInfo;

    class CurrentState;
    class PhysicalState;
    class VersionState;
    class VersionInfo;
    class RestrictInfo;

    class DistributedCollectable;
    class LayoutDescription;
    class PhysicalManager; // base class for instance and reduction
    class CopyAcrossHelper;
    class LogicalView; // base class for instance and reduction
    class InstanceManager;
    class InstanceKey;
    class InstanceView;
    class DeferredView;
    class MaterializedView;
    class CompositeView;
    class CompositeVersionInfo;
    class CompositeNode;
    class FillView;
    class MappingRef;
    class InstanceRef;
    class InstanceSet;
    class InnerTaskView;
    class ReductionManager;
    class ListReductionManager;
    class FoldReductionManager;
    class VirtualManager;
    class ReductionView;
    class InstanceBuilder;

    class RegionAnalyzer;
    class RegionMapper;

    struct EscapedUser;
    struct EscapedCopy;
    struct GenericUser;
    struct LogicalUser;
    struct PhysicalUser;
    struct TraceInfo;
    class LogicalCloser;
    class PhysicalCloser;
    class CompositeCloser;
    class ReductionCloser;
    class TreeCloseImpl;
    class TreeClose;
    struct CloseInfo; 

    // legion_spy.h
    class TreeStateLogger;

    // legion_profiling.h
    class LegionProfiler;
    class LegionProfInstance;

    // mapper_manager.h
    class MappingCallInfo;
    class MapperManager;
    class SerializingManager;
    class ConcurrentManager;
    typedef Mapping::MapperEvent MapperEvent;

#define FRIEND_ALL_RUNTIME_CLASSES                          \
    friend class Legion::Runtime;                           \
    friend class Internal::Runtime;                         \
    friend class Internal::PhysicalRegionImpl;              \
    friend class Internal::TaskImpl;                        \
    friend class Internal::ProcessorManager;                \
    friend class Internal::MemoryManager;                   \
    friend class Internal::Operation;                       \
    friend class Internal::SpeculativeOp;                   \
    friend class Internal::MapOp;                           \
    friend class Internal::CopyOp;                          \
    friend class Internal::FenceOp;                         \
    friend class Internal::DynamicCollectiveOp;             \
    friend class Internal::FuturePredOp;                    \
    friend class Internal::DeletionOp;                      \
    friend class Internal::CloseOp;                         \
    friend class Internal::TraceCloseOp;                    \
    friend class Internal::InterCloseOp;                    \
    friend class Internal::ReadCloseOp;                     \
    friend class Internal::PostCloseOp;                     \
    friend class Internal::VirtualCloseOp;                  \
    friend class Internal::AcquireOp;                       \
    friend class Internal::ReleaseOp;                       \
    friend class Internal::NotPredOp;                       \
    friend class Internal::AndPredOp;                       \
    friend class Internal::OrPredOp;                        \
    friend class Internal::MustEpochOp;                     \
    friend class Internal::PendingPartitionOp;              \
    friend class Internal::DependentPartitionOp;            \
    friend class Internal::FillOp;                          \
    friend class Internal::AttachOp;                        \
    friend class Internal::DetachOp;                        \
    friend class Internal::TimingOp;                        \
    friend class Internal::TaskOp;                          \
    friend class Internal::SingleTask;                      \
    friend class Internal::MultiTask;                       \
    friend class Internal::IndividualTask;                  \
    friend class Internal::PointTask;                       \
    friend class Internal::IndexTask;                       \
    friend class Internal::SliceTask;                       \
    friend class Internal::RegionTreeForest;                \
    friend class Internal::IndexSpaceNode;                  \
    friend class Internal::IndexPartNode;                   \
    friend class Internal::FieldSpaceNode;                  \
    friend class Internal::RegionTreeNode;                  \
    friend class Internal::RegionNode;                      \
    friend class Internal::PartitionNode;                   \
    friend class Internal::LogicalView;                     \
    friend class Internal::InstanceView;                    \
    friend class Internal::DeferredView;                    \
    friend class Internal::ReductionView;                   \
    friend class Internal::MaterializedView;                \
    friend class Internal::CompositeView;                   \
    friend class Internal::CompositeNode;                   \
    friend class Internal::FillView;                        \
    friend class Internal::LayoutDescription;               \
    friend class Internal::PhysicalManager;                 \
    friend class Internal::InstanceManager;                 \
    friend class Internal::ReductionManager;                \
    friend class Internal::ListReductionManager;            \
    friend class Internal::FoldReductionManager;            \
    friend class Internal::TreeStateLogger;                 \
    friend class Internal::MapperManager;                   \
    friend class Internal::InstanceRef;                     \
    friend class BindingLib::Utility;                       \
    friend class CObjectWrapper;                  

#define LEGION_EXTERN_LOGGER_DECLARATIONS                        \
    extern LegionRuntime::Logger::Category log_run;              \
    extern LegionRuntime::Logger::Category log_task;             \
    extern LegionRuntime::Logger::Category log_index;            \
    extern LegionRuntime::Logger::Category log_field;            \
    extern LegionRuntime::Logger::Category log_region;           \
    extern LegionRuntime::Logger::Category log_inst;             \
    extern LegionRuntime::Logger::Category log_leak;             \
    extern LegionRuntime::Logger::Category log_variant;          \
    extern LegionRuntime::Logger::Category log_allocation;       \
    extern LegionRuntime::Logger::Category log_prof;             \
    extern LegionRuntime::Logger::Category log_garbage;          \
    extern LegionRuntime::Logger::Category log_spy;              \
    extern LegionRuntime::Logger::Category log_shutdown;

  }; // Internal namespace

  // Typedefs that are needed everywhere
  typedef Realm::Runtime RealmRuntime;
  typedef Realm::Machine Machine;
  typedef Realm::Domain Domain;
  typedef Realm::DomainPoint DomainPoint;
  typedef Realm::IndexSpaceAllocator IndexSpaceAllocator;
  typedef Realm::RegionInstance PhysicalInstance;
  typedef Realm::Memory Memory;
  typedef Realm::Processor Processor;
  typedef Realm::CodeDescriptor CodeDescriptor;
  typedef Realm::Event Event;
  typedef Realm::UserEvent UserEvent;
  typedef Realm::Reservation Reservation;
  typedef Realm::Barrier Barrier;
  typedef ::legion_reduction_op_id_t ReductionOpID;
  typedef Realm::ReductionOpUntyped ReductionOp;
  typedef ::legion_custom_serdez_id_t CustomSerdezID;
  typedef Realm::CustomSerdezUntyped SerdezOp;
  typedef Realm::Machine::ProcessorMemoryAffinity ProcessorMemoryAffinity;
  typedef Realm::Machine::MemoryMemoryAffinity MemoryMemoryAffinity;
  typedef Realm::ElementMask::Enumerator Enumerator;
  typedef Realm::IndexSpace::FieldDataDescriptor FieldDataDescriptor;
  typedef std::map<CustomSerdezID, 
                   const Realm::CustomSerdezUntyped *> SerdezOpTable;
  typedef std::map<Realm::ReductionOpID, 
          const Realm::ReductionOpUntyped *> ReductionOpTable;
  typedef void (*SerdezInitFnptr)(const ReductionOp*, void *&, size_t&);
  typedef void (*SerdezFoldFnptr)(const ReductionOp*, void *&, 
                                  size_t&, const void*);
  typedef std::map<Realm::ReductionOpID, SerdezRedopFns> SerdezRedopTable;
  typedef ::legion_address_space_t AddressSpace;
  typedef ::legion_task_priority_t TaskPriority;
  typedef ::legion_garbage_collection_priority_t GCPriority;
  typedef ::legion_color_t Color;
  typedef ::legion_field_id_t FieldID;
  typedef ::legion_trace_id_t TraceID;
  typedef ::legion_mapper_id_t MapperID;
  typedef ::legion_context_id_t ContextID;
  typedef ::legion_instance_id_t InstanceID;
  typedef ::legion_index_space_id_t IndexSpaceID;
  typedef ::legion_index_partition_id_t IndexPartitionID;
  typedef ::legion_index_tree_id_t IndexTreeID;
  typedef ::legion_field_space_id_t FieldSpaceID;
  typedef ::legion_generation_id_t GenerationID;
  typedef ::legion_type_handle TypeHandle;
  typedef ::legion_projection_id_t ProjectionID;
  typedef ::legion_region_tree_id_t RegionTreeID;
  typedef ::legion_distributed_id_t DistributedID;
  typedef ::legion_address_space_id_t AddressSpaceID;
  typedef ::legion_tunable_id_t TunableID;
  typedef ::legion_generator_id_t GeneratorID;
  typedef ::legion_mapping_tag_id_t MappingTagID;
  typedef ::legion_semantic_tag_t SemanticTag;
  typedef ::legion_variant_id_t VariantID;
  typedef ::legion_unique_id_t UniqueID;
  typedef ::legion_version_id_t VersionID;
  typedef ::legion_task_id_t TaskID;
  typedef ::legion_layout_constraint_id_t LayoutConstraintID;
  typedef std::map<Color,ColoredPoints<ptr_t> > Coloring;
  typedef std::map<Color,Domain> DomainColoring;
  typedef std::map<Color,std::set<Domain> > MultiDomainColoring;
  typedef std::map<DomainPoint,ColoredPoints<ptr_t> > PointColoring;
  typedef std::map<DomainPoint,Domain> DomainPointColoring;
  typedef std::map<DomainPoint,std::set<Domain> > MultiDomainPointColoring;
  typedef void (*RegistrationCallbackFnptr)(Machine machine, 
      Runtime *rt, const std::set<Processor> &local_procs);
  typedef LogicalRegion (*RegionProjectionFnptr)(LogicalRegion parent, 
      const DomainPoint&, Runtime *rt);
  typedef LogicalRegion (*PartitionProjectionFnptr)(LogicalPartition parent, 
      const DomainPoint&, Runtime *rt);
  typedef bool (*PredicateFnptr)(const void*, size_t, 
      const std::vector<Future> futures);
  typedef std::map<ProjectionID,RegionProjectionFnptr> 
    RegionProjectionTable;
  typedef std::map<ProjectionID,PartitionProjectionFnptr> 
    PartitionProjectionTable;
  typedef void (*RealmFnptr)(const void*,size_t,
                             const void*,size_t,Processor);
  // The most magical of typedefs
  typedef Internal::SingleTask* Context;
  typedef Internal::GeneratorImpl* GeneratorContext;
  typedef void (*GeneratorFnptr)(GeneratorContext,
                                 const TaskGeneratorArguments&, Runtime*);
  // Anothing magical typedef
  namespace Mapping {
    typedef Internal::MappingCallInfo* MapperContext;
    typedef Internal::PhysicalManager* PhysicalInstanceImpl;
  };

  namespace Internal { 

    // Pull some of the mapper types into the internal space
    typedef Mapping::Mapper Mapper;
    typedef Mapping::PhysicalInstance MappingInstance;
    // A little bit of logic here to figure out the 
    // kind of bit mask to use for FieldMask

// The folowing macros are used in the FieldMask instantiation of BitMask
// If you change one you probably have to change the others too
#define LEGION_FIELD_MASK_FIELD_TYPE          uint64_t 
#define LEGION_FIELD_MASK_FIELD_SHIFT         6
#define LEGION_FIELD_MASK_FIELD_MASK          0x3F
#define LEGION_FIELD_MASK_FIELD_ALL_ONES      0xFFFFFFFFFFFFFFFF

#if defined(__AVX__)
#if (MAX_FIELDS > 256)
    typedef AVXTLBitMask<MAX_FIELDS> FieldMask;
#elif (MAX_FIELDS > 128)
    typedef AVXBitMask<MAX_FIELDS> FieldMask;
#elif (MAX_FIELDS > 64)
    typedef SSEBitMask<MAX_FIELDS> FieldMask;
#else
    typedef BitMask<LEGION_FIELD_MASK_FIELD_TYPE,MAX_FIELDS,
                    LEGION_FIELD_MASK_FIELD_SHIFT,
                    LEGION_FIELD_MASK_FIELD_MASK> FieldMask;
#endif
#elif defined(__SSE2__)
#if (MAX_FIELDS > 128)
    typedef SSETLBitMask<MAX_FIELDS> FieldMask;
#elif (MAX_FIELDS > 64)
    typedef SSEBitMask<MAX_FIELDS> FieldMask;
#else
    typedef BitMask<LEGION_FIELD_MASK_FIELD_TYPE,MAX_FIELDS,
                    LEGION_FIELD_MASK_FIELD_SHIFT,
                    LEGION_FIELD_MASK_FIELD_MASK> FieldMask;
#endif
#else
#if (MAX_FIELDS > 64)
    typedef TLBitMask<LEGION_FIELD_MASK_FIELD_TYPE,MAX_FIELDS,
                      LEGION_FIELD_MASK_FIELD_SHIFT,
                      LEGION_FIELD_MASK_FIELD_MASK> FieldMask;
#else
    typedef BitMask<LEGION_FIELD_MASK_FIELD_TYPE,MAX_FIELDS,
                    LEGION_FIELD_MASK_FIELD_SHIFT,
                    LEGION_FIELD_MASK_FIELD_MASK> FieldMask;
#endif
#endif
    typedef BitPermutation<FieldMask,LEGION_FIELD_LOG2> FieldPermutation;
    typedef Fraction<unsigned long> InstFrac;
#undef LEGION_FIELD_MASK_FIELD_SHIFT
#undef LEGION_FIELD_MASK_FIELD_MASK

    // Similar logic as field masks for node masks

// The following macros are used in the NodeMask instantiation of BitMask
// If you change one you probably have to change the others too
#define LEGION_NODE_MASK_NODE_TYPE           uint64_t
#define LEGION_NODE_MASK_NODE_SHIFT          6
#define LEGION_NODE_MASK_NODE_MASK           0x3F
#define LEGION_NODE_MASK_NODE_ALL_ONES       0xFFFFFFFFFFFFFFFF

#if defined(__AVX__)
#if (MAX_NUM_NODES > 256)
    typedef AVXTLBitMask<MAX_NUM_NODES> NodeMask;
#elif (MAX_NUM_NODES > 128)
    typedef AVXBitMask<MAX_NUM_NODES> NodeMask;
#elif (MAX_NUM_NODES > 64)
    typedef SSEBitMask<MAX_NUM_NODES> NodeMask;
#else
    typedef BitMask<LEGION_NODE_MASK_NODE_TYPE,MAX_NUM_NODES,
                    LEGION_NODE_MASK_NODE_SHIFT,
                    LEGION_NODE_MASK_NODE_MASK> NodeMask;
#endif
#elif defined(__SSE2__)
#if (MAX_NUM_NODES > 128)
    typedef SSETLBitMask<MAX_NUM_NODES> NodeMask;
#elif (MAX_NUM_NODES > 64)
    typedef SSEBitMask<MAX_NUM_NODES> NodeMask;
#else
    typedef BitMask<LEGION_NODE_MASK_NODE_TYPE,MAX_NUM_NODES,
                    LEGION_NODE_MASK_NODE_SHIFT,
                    LEGION_NODE_MASK_NODE_MASK> NodeMask;
#endif
#else
#if (MAX_NUM_NODES > 64)
    typedef TLBitMask<LEGION_NODE_MASK_NODE_TYPE,MAX_NUM_NODES,
                      LEGION_NODE_MASK_NODE_SHIFT,
                      LEGION_NODE_MASK_NODE_MASK> NodeMask;
#else
    typedef BitMask<LEGION_NODE_MASK_NODE_TYPE,MAX_NUM_NODES,
                    LEGION_NODE_MASK_NODE_SHIFT,
                    LEGION_NODE_MASK_NODE_MASK> NodeMask;
#endif
#endif
    typedef IntegerSet<AddressSpaceID,NodeMask> NodeSet;

#undef LEGION_NODE_MASK_NODE_SHIFT
#undef LEGION_NODE_MASK_NODE_MASK

// The following macros are used in the ProcessorMask instantiation of BitMask
// If you change one you probably have to change the others too
#define LEGION_PROC_MASK_PROC_TYPE           uint64_t
#define LEGION_PROC_MASK_PROC_SHIFT          6
#define LEGION_PROC_MASK_PROC_MASK           0x3F
#define LEGION_PROC_MASK_PROC_ALL_ONES       0xFFFFFFFFFFFFFFFF

#if defined(__AVX__)
#if (MAX_NUM_PROCS > 256)
    typedef AVXTLBitMask<MAX_NUM_PROCS> ProcessorMask;
#elif (MAX_NUM_PROCS > 128)
    typedef AVXBitMask<MAX_NUM_PROCS> ProcessorMask;
#elif (MAX_NUM_PROCS > 64)
    typedef SSEBitMask<MAX_NUM_PROCS> ProcessorMask;
#else
    typedef BitMask<LEGION_PROC_MASK_PROC_TYPE,MAX_NUM_PROCS,
                    LEGION_PROC_MASK_PROC_SHIFT,
                    LEGION_PROC_MASK_PROC_MASK> ProcessorMask;
#endif
#elif defined(__SSE2__)
#if (MAX_NUM_PROCS > 128)
    typedef SSETLBitMask<MAX_NUM_PROCS> ProcessorMask;
#elif (MAX_NUM_PROCS > 64)
    typedef SSEBitMask<MAX_NUM_PROCS> ProcessorMask;
#else
    typedef BitMask<LEGION_PROC_MASK_PROC_TYPE,MAX_NUM_PROCS,
                    LEGION_PROC_MASK_PROC_SHIFT,
                    LEGION_PROC_MASK_PROC_MASK> ProcessorMask;
#endif
#else
#if (MAX_NUM_PROCS > 64)
    typedef TLBitMask<LEGION_PROC_MASK_PROC_TYPE,MAX_NUM_PROCS,
                      LEGION_PROC_MASK_PROC_SHIFT,
                      LEGION_PROC_MASK_PROC_MASK> ProcessorMask;
#else
    typedef BitMask<LEGION_PROC_MASK_PROC_TYPE,MAX_NUM_PROCS,
                    LEGION_PROC_MASK_PROC_SHIFT,
                    LEGION_PROC_MASK_PROC_MASK> ProcessorMask;
#endif
#endif

#undef PROC_SHIFT
#undef PROC_MASK
  }; // namespace Internal
}; // Legion namespace

#endif // __LEGION_TYPES_H__
