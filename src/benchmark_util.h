#ifndef BENCHMARK_UTIL_H

#include <chrono>
#include <functional>
#include <string>
#include <vector>

struct BenchmarkResult {
    double total_seconds;
    size_t total_reps;
    std::vector<std::pair<std::string, double>> marginal_rates;
    double goal_seconds = -1;

    BenchmarkResult &show_rate(std::string new_unit_name, double new_multiplier) {
        marginal_rates.emplace_back(new_unit_name, new_multiplier);
        return *this;
    }

    BenchmarkResult &goal_nanos(double nanos) {
        goal_seconds = nanos / 1000 / 1000 / 1000;
        return *this;
    }

    BenchmarkResult &goal_micros(double micros) {
        goal_seconds = micros / 1000 / 1000;
        return *this;
    }

    BenchmarkResult &goal_millis(double millis) {
        goal_seconds = millis / 1000;
        return *this;
    }
};

struct RegisteredBenchmark {
    std::string name;
    std::function<void(void)> func;
    std::vector<BenchmarkResult> results;
};
extern RegisteredBenchmark *running_benchmark;
extern std::vector<RegisteredBenchmark> all_registered_benchmarks;

#define BENCHMARK(name)                                                          \
    void BENCH_##name##_METHOD();                                                \
    struct BENCH_STARTUP_TYPE_##name {                                           \
        BENCH_STARTUP_TYPE_##name() {                                            \
            all_registered_benchmarks.push_back({#name, BENCH_##name##_METHOD}); \
        }                                                                        \
    };                                                                           \
    static BENCH_STARTUP_TYPE_##name BENCH_STARTUP_INSTANCE_##name;              \
    void BENCH_##name##_METHOD()

// HACK: Templating the body function type makes inlining significantly more likely.
template <typename FUNC>
BenchmarkResult &benchmark_go(FUNC body) {
    size_t total_reps = 0;
    double total_seconds = 0.0;
    double target_wait_time_seconds = 0.5;

    for (size_t rep_limit = 1; total_seconds < target_wait_time_seconds; rep_limit *= 100) {
        double remaining_time = target_wait_time_seconds - total_seconds;
        size_t reps = rep_limit;
        if (total_seconds > 0) {
            reps = (size_t)(remaining_time * total_reps / total_seconds);
            if (reps < total_reps * 0.1) {
                break;
            }
            if (reps > rep_limit) {
                reps = rep_limit;
            }
            if (reps < 1) {
                reps = 1;
            }
        }
        auto start = std::chrono::steady_clock::now();
        for (size_t rep = 0; rep < reps; rep++) {
            body();
        }
        auto end = std::chrono::steady_clock::now();
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        total_reps += reps;
        total_seconds += (double)micros / 1000.0 / 1000.0;
    }

    running_benchmark->results.push_back({total_seconds, total_reps});
    return running_benchmark->results.back();
}

#endif
