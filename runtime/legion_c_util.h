/* Copyright 2014 Stanford University
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


#ifndef __LEGION_C_UTIL_H__
#define __LEGION_C_UTIL_H__

/**
 * \file legion_c_util.h
 * Legion C API: C++ Conversion Utilities
 */

#include "legion.h"
#include "legion_c.h"

namespace LegionRuntime {
  namespace HighLevel {
    class CObjectWrapper {
    public:

#define NEW_OPAQUE_WRAPPER(T_, T)                                       \
      static T_ wrap(T t) {                                             \
        T_ t_ = { .impl = static_cast<void *>(t) };                     \
        return t_;                                                      \
      }                                                                 \
      static const T_ wrap_const(const T t) {                           \
        const T_ t_ = { .impl = const_cast<void *>(static_cast<const void *>(t)) }; \
        return t_;                                                      \
      }                                                                 \
      static T unwrap(T_ t_) {                                          \
        return static_cast<T>(t_.impl);                                 \
      }                                                                 \
      static const T unwrap_const(const T_ t_) {                        \
        return static_cast<const T>(t_.impl);                           \
      }

      NEW_OPAQUE_WRAPPER(legion_runtime_t, HighLevelRuntime *);
      NEW_OPAQUE_WRAPPER(legion_context_t, Context);
      NEW_OPAQUE_WRAPPER(legion_domain_t, Domain *);
      NEW_OPAQUE_WRAPPER(legion_coloring_t, Coloring *);
      NEW_OPAQUE_WRAPPER(legion_domain_coloring_t, DomainColoring *);
      NEW_OPAQUE_WRAPPER(legion_index_space_allocator_t, IndexSpaceAllocator *);
      NEW_OPAQUE_WRAPPER(legion_argument_map_t, ArgumentMap *);
      NEW_OPAQUE_WRAPPER(legion_predicate_t, Predicate *);
      NEW_OPAQUE_WRAPPER(legion_future_t, Future *);
      NEW_OPAQUE_WRAPPER(legion_future_map_t, FutureMap *);
      NEW_OPAQUE_WRAPPER(legion_task_launcher_t, TaskLauncher *);
      NEW_OPAQUE_WRAPPER(legion_index_launcher_t, IndexLauncher *);
      NEW_OPAQUE_WRAPPER(legion_task_t, Task *);

#undef NEW_OPAQUE_WRAPPER

      static legion_ptr_t
      wrap(ptr_t ptr)
      {
        legion_ptr_t ptr_;
        ptr_.value = ptr.value;
        return ptr_;
      }

      static ptr_t
      unwrap(legion_ptr_t ptr_)
      {
        ptr_t ptr;
        ptr.value = ptr_.value;
        return ptr;
      }

      static legion_index_space_t
      wrap(IndexSpace is)
      {
        legion_index_space_t is_;
        is_.id = is.id;
        return is_;
      }

      static IndexSpace
      unwrap(legion_index_space_t is_)
      {
        IndexSpace is;
        is.id = is_.id;
        return is;
      }

      static legion_index_allocator_t
      wrap(IndexAllocator allocator)
      {
        legion_index_allocator_t allocator_;
        allocator_.index_space = wrap(allocator.index_space);
        allocator_.allocator = wrap(allocator.allocator);
        return allocator_;
      }

      static IndexAllocator
      unwrap(legion_index_allocator_t allocator_)
      {
        IndexAllocator allocator(unwrap(allocator_.index_space),
                                 unwrap(allocator_.allocator));
        return allocator;
      }

      static legion_field_space_t
      wrap(FieldSpace fs)
      {
        legion_field_space_t fs_;
        fs_.id = fs.id;
        return fs_;
      }

      static FieldSpace
      unwrap(legion_field_space_t fs_)
      {
        FieldSpace fs(fs_.id);
        return fs;
      }

      static legion_logical_region_t
      wrap(LogicalRegion r)
      {
        legion_logical_region_t r_;
        r_.tree_id = r.tree_id;
        r_.index_space = wrap(r.index_space);
        r_.field_space = wrap(r.field_space);
        return r_;
      }

      static LogicalRegion
      unwrap(legion_logical_region_t r_)
      {
        LogicalRegion r(r_.tree_id,
                        unwrap(r_.index_space),
                        unwrap(r_.field_space));
        return r;
      }

      static legion_logical_partition_t
      wrap(LogicalPartition r)
      {
        legion_logical_partition_t r_;
        r_.tree_id = r.tree_id;
        r_.index_partition = r.index_partition;
        r_.field_space = wrap(r.field_space);
        return r_;
      }

      static LogicalPartition
      unwrap(legion_logical_partition_t r_)
      {
        LogicalPartition r(r_.tree_id,
                           r_.index_partition,
                           unwrap(r_.field_space));
        return r;
      }

      static legion_field_allocator_t
      wrap(FieldAllocator allocator)
      {
        legion_field_allocator_t allocator_;
        allocator_.field_space = wrap(allocator.field_space);
        allocator_.parent = wrap(allocator.parent);
        allocator_.runtime = wrap(allocator.runtime);
        return allocator_;
      }

      static FieldAllocator
      unwrap(legion_field_allocator_t allocator_)
      {
        FieldAllocator allocator(unwrap(allocator_.field_space),
                                 unwrap(allocator_.parent),
                                 unwrap(allocator_.runtime));
        return allocator;
      }

      static legion_task_argument_t
      wrap(TaskArgument arg)
      {
        legion_task_argument_t arg_;
        arg_.args = arg.get_ptr();
        arg_.arglen = arg.get_size();
        return arg_;
      }

      static TaskArgument
      unwrap(legion_task_argument_t arg_)
      {
        return TaskArgument(arg_.args, arg_.arglen);
      }

      static const legion_input_args_t
      wrap_const(const InputArgs arg)
      {
        const legion_input_args_t arg_ = { .argv = arg.argv, .argc = arg.argc };
        return arg_;
      }

      static const InputArgs
      unwrap_const(const legion_input_args_t args_)
      {
        const InputArgs args = { .argv = args_.argv, .argc = args_.argc };
        return args;
      }

      static legion_task_config_options_t
      wrap(TaskConfigOptions options)
      {
        legion_task_config_options_t options_ = {
          .leaf = options.leaf,
          .inner = options.inner,
          .idempotent = options.idempotent,
        };
        return options_;
      }

      static TaskConfigOptions
      unwrap(legion_task_config_options_t options_)
      {
        TaskConfigOptions options(options_.leaf,
                                  options_.inner,
                                  options_.idempotent);
        return options;
      }

      static legion_processor_kind_t
      wrap(Processor::Kind options)
      {
        return static_cast<legion_processor_kind_t>(options);
      }

      static Processor::Kind
      unwrap(legion_processor_kind_t options_)
      {
        return static_cast<Processor::Kind>(options_);
      }

    };
  }
}

#endif // __LEGION_C_UTIL_H__
