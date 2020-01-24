#include "lue/framework/core/numa_domain.hpp"
#include <memory>
#include <mutex>


namespace lue {
namespace {

Targets& targets()
{
    static Targets targets{};
    static bool initialized{false};
    static std::mutex mutex;

    {
        std::lock_guard<std::mutex> const lock{mutex};

        if(!initialized) {
            targets = hpx::compute::host::numa_domains();
            initialized = true;
        }
    }

    return targets;
}


NUMADomainExecutors& executors()
{
    static NUMADomainExecutors executors{};
    static bool initialized{false};
    static std::mutex mutex;

    {
        std::lock_guard<std::mutex> const lock{mutex};

        if(!initialized) {
            for(auto& target: targets()) {
                executors.push_back(NUMADomainExecutor{Targets{1, target}});
            }
        }
    }

    return executors;
}

}  // Anonymous namespace


std::size_t nr_numa_targets()
{
    return targets().size();
}


Target& target(
    TargetIndex const idx)
{
    assert(idx < targets().size());

    return targets()[idx];
}


/*!
    @brief      Return executor associated with NUMA domain @a idx passed in
*/
NUMADomainExecutor& numa_domain_executor(
    TargetIndex const idx)
{
    assert(idx < executors().size());

    return executors()[idx];
}


TargetIndex scattered_target_index()
{
    static std::unique_ptr<ScatterTargetIndex> scatter;
    static std::mutex mutex;

    TargetIndex result;

    {
        std::lock_guard<std::mutex> const lock{mutex};

        if(!scatter) {
            scatter = std::make_unique<ScatterTargetIndex>(
                0, nr_numa_targets() - 1);
        }

        result = (*scatter)();

        assert(result < nr_numa_targets());
    }

    return result;
}

}  // namespace lue
