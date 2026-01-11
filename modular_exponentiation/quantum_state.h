#ifndef QUANTUM_STATE_H
#define QUANTUM_STATE_H

#include <vector>
#include <complex>
#include <iostream>
#include <stdexcept>

// Type definitions for quantum simulation
typedef std::complex<double> Complex;

class QuantumState {
private:
    std::vector<Complex> amplitudes;
    int num_qubits;
    int state_size;

public:
    // Constructor: Initialize quantum state with n qubits
    // All qubits start in |0⟩ state, so the state is |00...0⟩
    QuantumState(int n) : num_qubits(n) {
        if (n <= 0) {
            throw std::invalid_argument("Number of qubits must be positive");
        }

        state_size = 1 << n;  // 2^n
        amplitudes.resize(state_size, 0.0);

        // Initialize to |00...0⟩ state
        amplitudes[0] = 1.0;
    }

    // Get the number of qubits
    int getNumQubits() const {
        return num_qubits;
    }

    // Get the total number of basis states
    int getStateSize() const {
        return state_size;
    }

    // Get amplitude at a specific basis state index
    Complex getAmplitude(int index) const {
        if (index < 0 || index >= state_size) {
            throw std::out_of_range("Index out of range");
        }
        return amplitudes[index];
    }

    // Set amplitude at a specific basis state index
    void setAmplitude(int index, Complex amplitude) {
        if (index < 0 || index >= state_size) {
            throw std::out_of_range("Index out of range");
        }
        amplitudes[index] = amplitude;
    }

    // Get the probability of measuring a specific basis state
    // Probability = |amplitude|^2
    double getProbability(int index) const {
        if (index < 0 || index >= state_size) {
            throw std::out_of_range("Index out of range");
        }
        return std::norm(amplitudes[index]);
    }

    // Get all amplitudes
    const std::vector<Complex>& getAmplitudes() const {
        return amplitudes;
    }

    // Print the quantum state in a readable format
    void printState() const {
        std::cout << "Quantum State (" << num_qubits << " qubits):" << std::endl;
        std::cout << "Total basis states: " << state_size << std::endl;
        std::cout << std::endl;

        for (int i = 0; i < state_size; i++) {
            double prob = getProbability(i);
            if (prob > 1e-10) {  // Only print non-zero states
                std::cout << "|" << i << "> (binary: ";
                // Print binary representation
                for (int q = num_qubits - 1; q >= 0; q--) {
                    std::cout << ((i >> q) & 1);
                }
                std::cout << "): " << amplitudes[i];
                std::cout << " (probability: " << prob << ")" << std::endl;
            }
        }
        std::cout << std::endl;
    }

    // Verify that the state is normalized (total probability = 1)
    bool isNormalized() const {
        double total_prob = 0.0;
        for (int i = 0; i < state_size; i++) {
            total_prob += getProbability(i);
        }
        return std::abs(total_prob - 1.0) < 1e-10;
    }

    // Get memory usage in bytes
    size_t getMemoryUsage() const {
        return state_size * sizeof(Complex);
    }
};

#endif // QUANTUM_STATE_H
