#pragma once
#include "lue/framework/benchmark/environment.hpp"
#include "lue/framework/benchmark/stopwatch.hpp"
#include "lue/framework/benchmark/task.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>


namespace lue {
namespace benchmark {

/*
Notes

Benchmark:
- Measure performance of some solution (exe, parameters, system)
- Benchmark is an exe
    - Work to benchmark must be a callable in a lib
    - Each run must be a call to this function, timed by the benchmark
        instance
- Description
- Meta-information
    - exe
    - parameters
    - system
    - ...
- Timings (multiple equivalent runs, capture spread)

Benchmark
    description
    system
    exe
        path
        build_configuration
    parameters
    timings
        timing
            start
            end

HPXBenchmark
    system
        localities
        ...
    hpx
        build_configuration
*/


// template<
//     typename Callable>
// class CallableTraits
// {
// 
// public:
// 
//     using ResultT = void;
// 
// //     using ResultT = decltype(
// //         Callable{}(
// //             Environment{0, 0, std::optional<std::size_t>{0}},
// //             Task{0, Task::Shape{}, Task::Shape{}}));
// 
// };
// 
// 
// template<
//     typename Callable>
// using ResultT = typename CallableTraits<Callable>::ResultT;


/*!
    @brief      Class for managing a performance benchmark

    A performance benchmark is an experiment to see how fast a piece of
    code executeѕ. Benchmark instances will manage the timing of a
    single piece of code. Multiple timings can be collected in order to
    obtain information about the spread in the results.
*/
template<
    typename Callable>
class BenchmarkBase
{

public:

    using Duration = Stopwatch::Duration;
    using Timing = Stopwatch;
    using Timings = std::vector<Timing>;

    /*!
        @brief      Construct an instance

        In case of weird errors, verify that the callable is moved in.
    */
    BenchmarkBase(
        Callable&& callable,
        Environment const& environment,
        Task const& task):

        _callable{std::forward<Callable>(callable)},
        _environment{environment},
        _task{task},
        _timing{},
        _timings{}

    {
        // assert(!_array_shape.empty());
        // assert(std::size(_array_shape) == std::size(_partition_shape));
    }

    BenchmarkBase(BenchmarkBase const&)=delete;

    BenchmarkBase(BenchmarkBase&&)=delete;

    virtual ~BenchmarkBase()=default;

    BenchmarkBase& operator=(BenchmarkBase const&)=delete;

    BenchmarkBase& operator=(BenchmarkBase&&)=delete;

    Environment const& environment() const
    {
        return _environment;
    }

    Task const& task() const
    {
        return _task;
    }

    Timing const& timing() const
    {
        return _timing;
    }

    /*!
        @brief      Return the timings of each run
    */
    Timings const& timings() const
    {
        return _timings;
    }

    /*!
        @brief      Run the benchmarks @a count times
        @return     EXIT_SUCCESS or EXIT_FAILURE

        You can call this function multiple times, but any previous results
        will be overwritten in that case.

        After calling this function, @a count timings will be available.
    */
    virtual int run()=0;

protected:

    Callable& callable()
    {
        return _callable;
    }

    Timing& timing()
    {
        return _timing;
    }

    /*!
        @brief      Return the timings of each run
    */
    Timings& timings()
    {
        return _timings;
    }

private:

    //! Callable representing the workload to time
    Callable _callable;

    //! Environment the benchmark runs in
    Environment const _environment;

    Task const _task;

    //! Name of the benchmark
    std::string const _name;

    Timing _timing;

    //! Timings of the @a count benchmark runs
    Timings _timings;

};


// ----------------------------------------------------------------------------
template<
    typename Callable,
    typename Result=void,
    typename Enable=void>
class Benchmark:
    public BenchmarkBase<Callable>
{
    // Use this when the callable does not return a result

public:

    Benchmark(
        Callable&& callable,
        Environment const& environment,
        Task const& task):

        BenchmarkBase<Callable>(
            std::forward<Callable>(callable), environment, task)

    {
    }

    Benchmark(Benchmark const&)=delete;

    Benchmark(Benchmark&&)=delete;

    ~Benchmark()=default;

    Benchmark& operator=(Benchmark const&)=delete;

    Benchmark& operator=(Benchmark&&)=delete;

    /*!
        @overload
    */
    int run() override
    {
        using namespace std::chrono_literals;

        this->timings().clear();
        Stopwatch stopwatch;

        this->timing().start();

        for(std::size_t i = 0; i < this->environment().count(); ++i) {
            std::this_thread::sleep_for(2s);

            stopwatch.start();

            this->callable()(this->environment(), this->task());

            stopwatch.stop();

            this->timings().push_back(stopwatch);

            // if(i < _environment.count() - 1) {
            //     std::this_thread::sleep_for(5s);
            // }
        }

        this->timing().stop();

        return EXIT_SUCCESS;
    }

};


// ----------------------------------------------------------------------------
template<
    typename Callable,
    typename Result>
class Benchmark<Callable, Result, std::enable_if_t<!std::is_void_v<Result>>>:
    public BenchmarkBase<Callable>
{
public:

    // Use this when the callable returns a result
    using Results = std::vector<Result>;

    Benchmark(
        Callable&& callable,
        Environment const& environment,
        Task const& task):

        BenchmarkBase<Callable>(
            std::forward<Callable>(callable), environment, task)

    {
    }

    Benchmark(Benchmark const&)=delete;

    Benchmark(Benchmark&&)=delete;

    ~Benchmark()=default;

    Benchmark& operator=(Benchmark const&)=delete;

    Benchmark& operator=(Benchmark&&)=delete;

    Results const& results () const
    {
        return _results;
    }

    /*!
        @overload

        After calling this function, @a count results will be available.
    */
    int run() override
    {
        using namespace std::chrono_literals;

        this->timings().clear();
        _results.clear();
        Stopwatch stopwatch;

        this->timing().start();

        for(std::size_t i = 0; i < this->environment().count(); ++i) {
            std::this_thread::sleep_for(2s);

            stopwatch.start();

            Result result = this->callable()(this->environment(), this->task());

            stopwatch.stop();

            this->timings().push_back(stopwatch);
            _results.push_back(std::move(result));
        }

        this->timing().stop();

        return EXIT_SUCCESS;
    }

protected:

    Results& results()
    {
        return _results;
    }

private:

    Results _results;

};


template<
    typename Type>
class TypeTraits
{

public:

    using ResultT = void;

};


template<
    typename Callable,
    typename Result>
class TypeTraits<Benchmark<Callable, Result>>
{

public:

    using ResultT = Result;

};


template<
    typename Type>
using ResultT = typename TypeTraits<Type>::ResultT;

}  // namespace benchmark
}  // namespace lue
