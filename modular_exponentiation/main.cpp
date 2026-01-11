#include "quantum_state.h"
#include "quantum_gates.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

// Euclidean algorithm for GCD
uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int main(int argc, char* argv[]) {
    std::string filename = "input.txt";

    // Allow command line argument for input file
    if (argc > 1) {
        filename = argv[1];
    }

    // Read configuration
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
        return 1;
    }

    uint64_t base, modulus;
    int num_qubits;

    file >> base >> modulus >> num_qubits;
    file.close();

    // Simple validation
    if (base == 0 || modulus == 0 || num_qubits <= 0) {
        std::cerr << "Error: All values must be positive" << std::endl;
        return 1;
    }

    if (num_qubits > 10) {
        std::cerr << "Error: Number of qubits cannot exceed 10" << std::endl;
        return 1;
    }

    if (modulus >= 1024) {
        std::cerr << "Error: Modulus must be < 1024" << std::endl;
        return 1;
    }

    std::cout << "Configuration loaded:" << std::endl;
    std::cout << "  Base: " << base << std::endl;
    std::cout << "  Modulus: " << modulus << std::endl;
    std::cout << "  Qubits: " << num_qubits << std::endl;

    // Check if base and modulus are coprime
    uint64_t g = gcd(base, modulus);
    if (g != 1) {
        std::cout << "\n=== NOTE ===" << std::endl;
        std::cout << "gcd(" << base << ", " << modulus << ") = " << g << " ≠ 1" << std::endl;
        std::cout << "WARNING: The modular multiplication gate is NOT reversible!" << std::endl;
        std::cout << std::endl;
        std::cout << "For Shor's algorithm, this means you've already found a non-trivial factor:" << std::endl;
        std::cout << "  Factor of " << modulus << ": " << g << std::endl;
        std::cout << "\nThe quantum circuit would not work correctly in this case." << std::endl;
        std::cout << "Please choose a base coprime to the modulus." << std::endl;
        std::cout << "========================================" << std::endl;
        return 1;
    }

    std::cout << "  gcd(" << base << ", " << modulus << ") = 1 ✓ (reversible)" << std::endl;
    std::cout << std::endl;

    // ========================================
    // Step 1: Calculate target register size
    // ========================================
    // Need enough qubits to represent values 0 to modulus-1
    int target_qubits = 0;
    uint64_t temp = modulus - 1;
    while (temp > 0) {
        temp >>= 1;
        target_qubits++;
    }
    if (target_qubits == 0) target_qubits = 1;

    std::cout << "Target register size: " << target_qubits << " qubits" << std::endl;
    std::cout << "Total qubits: " << (num_qubits + target_qubits) << std::endl;
    std::cout << std::endl;

    // ========================================
    // Step 2: Initialize quantum state
    // ========================================
    int total_qubits = num_qubits + target_qubits;
    QuantumState state(total_qubits);

    // Initialize target register to |1⟩ (since a^0 = 1)
    // Target register starts at qubit 'num_qubits'
    // To set it to |1⟩, we set amplitude at index (1 << num_qubits)
    state.setAmplitude(0, Complex(0, 0));
    state.setAmplitude(1 << num_qubits, Complex(1, 0));

    std::cout << "Initial state: |0⟩^" << num_qubits << " ⊗ |1⟩" << std::endl;
    std::cout << std::endl;

    // ========================================
    // Step 3: Apply Hadamard gates to control register
    // ========================================
    std::cout << "Applying Hadamard gates to control register..." << std::endl;
    for (int i = 0; i < num_qubits; i++) {
        HadamardGate H(i);
        H.apply(state);
    }

    std::cout << "Control register now in superposition of all exponents 0 to "
              << ((1 << num_qubits) - 1) << std::endl;
    std::cout << std::endl;

    // ========================================
    // Step 4: Precompute powers of base
    // ========================================
    std::cout << "Precomputing powers of " << base << " mod " << modulus << ":" << std::endl;
    std::vector<uint64_t> powers;
    uint64_t current_power = base % modulus;

    for (int i = 0; i < num_qubits; i++) {
        powers.push_back(current_power);
        std::cout << "  " << base << "^(2^" << i << ") mod " << modulus << " = " << current_power << std::endl;
        current_power = (current_power * current_power) % modulus;
    }
    std::cout << std::endl;

    // ========================================
    // Step 5: Apply controlled modular multiplications
    // ========================================
    std::cout << "Applying controlled modular multiplication gates..." << std::endl;

    for (int i = 0; i < num_qubits; i++) {
        // For each control qubit i, apply U^(2^i)
        // where U multiplies by powers[i]
        ControlledModMultGate gate(i, num_qubits, target_qubits, powers[i], modulus);
        gate.apply(state);
        std::cout << "  Applied U^(2^" << i << ") on control qubit " << i
                  << " (multiplier: " << powers[i] << ")" << std::endl;
    }
    std::cout << std::endl;

    // ========================================
    // Step 6: Verify results
    // ========================================
    std::cout << "========================================" << std::endl;
    std::cout << "Results Verification" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Check each basis state
    int num_tests = 0;
    int num_passed = 0;

    // Debug: Print all non-zero probability states
    std::cout << "Debug: All quantum states with non-zero probability:" << std::endl;
    int non_zero_count = 0;
    for (int idx = 0; idx < state.getStateSize(); idx++) {
        double prob = state.getProbability(idx);
        if (prob > 0.001) {
            // State index layout: bits 0 to num_qubits-1 are control
            // bits num_qubits to total are target
            int control = idx & ((1 << num_qubits) - 1);  // Lower bits: control
            int target = idx >> num_qubits;  // Upper bits: target
            std::cout << "  |" << control << "⟩⊗|" << target << "⟩: P = " << prob << std::endl;
            non_zero_count++;
        }
    }
    std::cout << "Total non-zero states: " << non_zero_count << std::endl;
    std::cout << std::endl;

    for (int x = 0; x < (1 << num_qubits); x++) {
        // Find the actual state after computation
        // State index = (target << num_qubits) | control
        double max_prob = 0;
        int most_likely_target = 0;

        for (int y = 0; y < (1 << target_qubits); y++) {
            int idx = (y << num_qubits) | x;  // target in upper bits, control in lower
            double prob = state.getProbability(idx);
            if (prob > max_prob) {
                max_prob = prob;
                most_likely_target = y;
            }
        }

        // Classical computation
        uint64_t classical_result = 1;
        uint64_t base_pow = base % modulus;
        uint64_t exponent = x;

        for (int bit = 0; bit < num_qubits; bit++) {
            if ((exponent >> bit) & 1) {
                classical_result = (classical_result * base_pow) % modulus;
            }
            base_pow = (base_pow * base_pow) % modulus;
        }

        // Check if quantum result matches classical
        // In uniform superposition of n qubits, each state has probability 1/2^n
        double expected_prob = 1.0 / (1 << num_qubits);
        double relative_error = std::abs(max_prob - expected_prob) / expected_prob;
        bool passed = (most_likely_target == classical_result) && (relative_error < 0.01);

        if (passed) {
            num_passed++;
        }

        num_tests++;
    }

    // Summary
    std::cout << "========================================" << std::endl;
    std::cout << "Summary: " << num_passed << "/" << num_tests << " tests passed" << std::endl;
    if (num_passed == num_tests) {
        std::cout << "✓ All tests passed!" << std::endl;
    } else {
        std::cout << "✗ Some tests failed" << std::endl;
    }
    std::cout << "========================================" << std::endl;

    return 0;
}
