#include "tableau_simulator.h"

#include "../benchmark_util.h"
#include "../common_circuits.h"
#include "../simulators/gate_data.h"

BENCHMARK(TableauSimulator_sample_unrotated_surface_code_d5) {
    size_t distance = 5;
    auto circuit = Circuit::from_text(unrotated_surface_code_program_text(distance, distance, 0));
    std::mt19937_64 rng(0);
    benchmark_go([&]() {
        TableauSimulator::sample_circuit(circuit, rng);
    })
        .goal_micros(150)
        .show_rate("Layers", distance)
        .show_rate("OutBits", circuit.num_measurements);
}

BENCHMARK(TableauSimulator_sample_unrotated_surface_code_d41) {
    size_t distance = 41;
    auto circuit = Circuit::from_text(unrotated_surface_code_program_text(distance, distance, 0));
    std::mt19937_64 rng(0);
    benchmark_go([&]() {
        TableauSimulator::sample_circuit(circuit, rng);
    })
        .goal_millis(650)
        .show_rate("Layers", distance)
        .show_rate("OutBits", circuit.num_measurements);
}
