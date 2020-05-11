#pragma once
#include "lue/framework/core/define.hpp"
#include <hpx/include/compute.hpp>
#include <cassert>


namespace lue {

using TargetIndex = std::size_t;

using Target = hpx::compute::host::target;

using Targets = std::vector<Target>;

// An executor can manage multiple targets
using NUMADomainExecutor = hpx::compute::host::block_executor<>;

// For each NUMA domain target an executor managing only that single target
using NUMADomainExecutors = std::vector<NUMADomainExecutor>;

template<
    typename Element>
using NUMADomainAllocator = hpx::compute::host::block_allocator<Element>;

std::size_t        nr_numa_targets     ();

Target&            target              (TargetIndex idx);

NUMADomainExecutor& numa_domain_executor(
                                        TargetIndex idx);

TargetIndex        scattered_target_index();


class ScatterTargetIndex
{

public:

    ScatterTargetIndex()=default;

    ScatterTargetIndex(ScatterTargetIndex const& other)=default;

    ScatterTargetIndex(ScatterTargetIndex&& other)=default;

    ~ScatterTargetIndex()=default;

    ScatterTargetIndex& operator=(ScatterTargetIndex const& other)=default;

    ScatterTargetIndex& operator=(ScatterTargetIndex&& other)=default;

    ScatterTargetIndex(
        TargetIndex const first,
        TargetIndex const last):

        _first{first},
        _last{last},
        _index{last}

    {
        assert(first <= last);
    }

    TargetIndex operator()()
    {
        ++_index;

        if(_index > _last) {
            _index = _first;
        }

        return _index;
    }

private:

    TargetIndex    _first;

    TargetIndex    _last;

    TargetIndex    _index;

};

}  // namespace lue
