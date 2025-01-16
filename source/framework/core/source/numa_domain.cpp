#include "lue/framework/core/numa_domain.hpp"
#include <hpx/synchronization/mutex.hpp>
#include <mutex>


namespace lue {
    namespace {

        using Mutex = hpx::mutex;
        using Lock = std::lock_guard<Mutex>;

        Targets _targets{};
        NUMADomainExecutors _executors{};
        bool initialized{false};
        Mutex mutex;


        void init()
        {
            Lock const lock{mutex};

            if (!initialized)
            {

                lue_hpx_assert(_targets.empty());
                lue_hpx_assert(_executors.empty());

                _targets = hpx::compute::host::numa_domains();

                _executors.reserve(_targets.size());

                for (auto& target : _targets)
                {
                    _executors.emplace_back(NUMADomainExecutor{Targets{1, target}});
                }

                initialized = true;
            }

            lue_hpx_assert(!_targets.empty());
            lue_hpx_assert(!_executors.empty());
        }


        Targets& targets()
        {
            init();

            return _targets;
        }


        NUMADomainExecutors& executors()
        {
            init();

            return _executors;
        }

    }  // Anonymous namespace


    std::size_t nr_numa_targets()
    {
        return targets().size();
    }


    Target& target(TargetIndex const idx)
    {
        lue_hpx_assert(idx < targets().size());

        return targets()[idx];
    }


    /*!
        @brief      Return executor associated with NUMA domain @a idx passed in
    */
    NUMADomainExecutor& numa_domain_executor(TargetIndex const idx)
    {
        lue_hpx_assert(idx < executors().size());

        return executors()[idx];
    }


    TargetIndex scattered_target_index()
    {
        static ScatterTargetIndex scatter{};

        {
            static bool initialized{false};
            static Mutex mutex;

            Lock const lock{mutex};

            if (!initialized)
            {
                scatter = ScatterTargetIndex{0, nr_numa_targets() - 1};
                initialized = true;
            }
        }

        TargetIndex result = scatter();

        lue_hpx_assert(result < nr_numa_targets());

        return result;
    }

}  // namespace lue
