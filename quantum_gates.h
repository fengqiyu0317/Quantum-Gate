#ifndef QUANTUM_GATES_H
#define QUANTUM_GATES_H

#include "quantum_state.h"
#include <cmath>
#include <vector>

// Abstract base class for quantum gates
class QuantumGate {
public:
    virtual void apply(QuantumState& state) = 0;
    virtual ~QuantumGate() = default;
};

// Hadamard Gate
// Creates superposition by transforming: H|0⟩ = (|0⟩ + |1⟩)/√2, H|1⟩ = (|0⟩ - |1⟩)/√2
class HadamardGate : public QuantumGate {
private:
    int target_qubit;
    static constexpr double INV_SQRT2 = 0.70710678118654752440; // 1/√2

public:
    HadamardGate(int target) : target_qubit(target) {
        if (target < 0) {
            throw std::invalid_argument("Target qubit must be non-negative");
        }
    }

    void apply(QuantumState& state) override {
        int num_qubits = state.getNumQubits();
        int state_size = state.getStateSize();

        // Validate target qubit
        if (target_qubit >= num_qubits) {
            throw std::invalid_argument("Target qubit exceeds number of qubits in state");
        }

        // Get current amplitudes
        const std::vector<Complex>& old_amplitudes = state.getAmplitudes();
        std::vector<Complex> new_amplitudes(state_size);

        // Apply Hadamard gate to target qubit
        // For each basis state, we need to find its pair (differing only in target qubit)
        int mask = 1 << target_qubit;

        for (int i = 0; i < state_size; i++) {
            int j = i ^ mask; // Flip the target qubit bit

            // Only process each pair once (when i has 0 at target bit)
            if ((i & mask) == 0) {
                Complex amp0 = old_amplitudes[i];
                Complex amp1 = old_amplitudes[j];

                // Apply Hadamard transformation
                // H|0⟩ = (|0⟩ + |1⟩)/√2
                // H|1⟩ = (|0⟩ - |1⟩)/√2
                new_amplitudes[i] = (amp0 + amp1) * INV_SQRT2;
                new_amplitudes[j] = (amp0 - amp1) * INV_SQRT2;
            }
        }

        // Update the quantum state with new amplitudes
        for (int i = 0; i < state_size; i++) {
            state.setAmplitude(i, new_amplitudes[i]);
        }
    }

    int getTarget() const { return target_qubit; }
};


// CNOT Gate (Controlled-NOT)
// Flips target qubit if control qubit is |1⟩
class CNOTGate : public QuantumGate {
private:
    int control_qubit;
    int target_qubit;

public:
    CNOTGate(int control, int target) : control_qubit(control), target_qubit(target) {
        if (control < 0 || target < 0) {
            throw std::invalid_argument("Control and target qubits must be non-negative");
        }
        if (control == target) {
            throw std::invalid_argument("Control and target qubits must be different");
        }
    }

    void apply(QuantumState& state) override {
        int num_qubits = state.getNumQubits();
        int state_size = state.getStateSize();

        // Validate qubits
        if (control_qubit >= num_qubits || target_qubit >= num_qubits) {
            throw std::invalid_argument("Control or target qubit exceeds number of qubits in state");
        }

        // Get current amplitudes
        const std::vector<Complex>& old_amplitudes = state.getAmplitudes();
        std::vector<Complex> new_amplitudes = old_amplitudes;

        // Apply CNOT gate
        // For each basis state where control qubit is 1, flip the target qubit
        int control_mask = 1 << control_qubit;
        int target_mask = 1 << target_qubit;

        for (int i = 0; i < state_size; i++) {
            // Check if control qubit is 1
            if ((i & control_mask) != 0) {
                // Flip target qubit to get the destination index
                int j = i ^ target_mask;
                // Swap amplitudes (CNOT is its own inverse)
                new_amplitudes[j] = old_amplitudes[i];
            }
        }

        // Update the quantum state with new amplitudes
        for (int i = 0; i < state_size; i++) {
            state.setAmplitude(i, new_amplitudes[i]);
        }
    }

    int getControl() const { return control_qubit; }
    int getTarget() const { return target_qubit; }
};

// SWAP Gate
// Exchanges the quantum states of two qubits
class SWAPGate : public QuantumGate {
private:
    int qubit1;
    int qubit2;

public:
    SWAPGate(int q1, int q2) : qubit1(q1), qubit2(q2) {
        if (q1 < 0 || q2 < 0) {
            throw std::invalid_argument("Qubits must be non-negative");
        }
        if (q1 == q2) {
            throw std::invalid_argument("SWAP qubits must be different");
        }
    }

    void apply(QuantumState& state) override {
        int num_qubits = state.getNumQubits();
        int state_size = state.getStateSize();

        // Validate qubits
        if (qubit1 >= num_qubits || qubit2 >= num_qubits) {
            throw std::invalid_argument("Qubit exceeds number of qubits in state");
        }

        // Get current amplitudes
        const std::vector<Complex>& old_amplitudes = state.getAmplitudes();
        std::vector<Complex> new_amplitudes = old_amplitudes;

        // Apply SWAP gate
        // For each basis state where qubit1 and qubit2 have different values,
        // swap with the state where those two bits are flipped
        int mask1 = 1 << qubit1;
        int mask2 = 1 << qubit2;

        for (int i = 0; i < state_size; i++) {
            // Check if the two qubits have different values
            bool bit1 = (i & mask1) != 0;
            bool bit2 = (i & mask2) != 0;

            if (bit1 != bit2) {
                // Swap: flip both bits to get the paired state
                int j = i ^ mask1 ^ mask2;
                // Swap amplitudes
                new_amplitudes[i] = old_amplitudes[j];
            }
        }

        // Update the quantum state with new amplitudes
        for (int i = 0; i < state_size; i++) {
            state.setAmplitude(i, new_amplitudes[i]);
        }
    }

    int getQubit1() const { return qubit1; }
    int getQubit2() const { return qubit2; }
};

// Toffoli Gate (CCNOT - Controlled-Controlled-NOT)
// Flips target qubit if both control qubits are |1⟩
class ToffoliGate : public QuantumGate {
private:
    int control1_qubit;
    int control2_qubit;
    int target_qubit;

public:
    ToffoliGate(int control1, int control2, int target)
        : control1_qubit(control1), control2_qubit(control2), target_qubit(target) {
        if (control1 < 0 || control2 < 0 || target < 0) {
            throw std::invalid_argument("All qubits must be non-negative");
        }
        if (control1 == target || control2 == target) {
            throw std::invalid_argument("Target qubit must be different from control qubits");
        }
        if (control1 == control2) {
            throw std::invalid_argument("Control qubits must be different");
        }
    }

    void apply(QuantumState& state) override {
        int num_qubits = state.getNumQubits();
        int state_size = state.getStateSize();

        // Validate qubits
        if (control1_qubit >= num_qubits || control2_qubit >= num_qubits || target_qubit >= num_qubits) {
            throw std::invalid_argument("Qubit exceeds number of qubits in state");
        }

        // Get current amplitudes
        const std::vector<Complex>& old_amplitudes = state.getAmplitudes();
        std::vector<Complex> new_amplitudes = old_amplitudes;

        // Apply Toffoli gate
        // For each basis state where both control qubits are 1, flip the target qubit
        int control1_mask = 1 << control1_qubit;
        int control2_mask = 1 << control2_qubit;
        int target_mask = 1 << target_qubit;

        for (int i = 0; i < state_size; i++) {
            // Check if both control qubits are 1
            if (((i & control1_mask) != 0) && ((i & control2_mask) != 0)) {
                // Flip target qubit to get the destination index
                int j = i ^ target_mask;
                // Swap amplitudes (Toffoli is its own inverse)
                new_amplitudes[j] = old_amplitudes[i];
            }
        }

        // Update the quantum state with new amplitudes
        for (int i = 0; i < state_size; i++) {
            state.setAmplitude(i, new_amplitudes[i]);
        }
    }

    int getControl1() const { return control1_qubit; }
    int getControl2() const { return control2_qubit; }
    int getTarget() const { return target_qubit; }
};

// Phase Shift Gate (Rz gate)
// Applies a phase shift to the |1⟩ state: |0⟩ → |0⟩, |1⟩ → e^(iθ)|1⟩
// Can be used to implement:
// - S gate: phase = π/2
// - T gate: phase = π/4
// - Z gate: phase = π
// - Any arbitrary phase shift
class PhaseShiftGate : public QuantumGate {
private:
    int target_qubit;
    double phase_angle;
    Complex phase_factor;

public:
    PhaseShiftGate(int target, double angle) : target_qubit(target), phase_angle(angle) {
        if (target < 0) {
            throw std::invalid_argument("Target qubit must be non-negative");
        }
        // Pre-compute the phase factor: e^(iθ) = cos(θ) + i·sin(θ)
        phase_factor = Complex(std::cos(angle), std::sin(angle));
    }

    void apply(QuantumState& state) override {
        int num_qubits = state.getNumQubits();
        int state_size = state.getStateSize();

        // Validate target qubit
        if (target_qubit >= num_qubits) {
            throw std::invalid_argument("Target qubit exceeds number of qubits in state");
        }

        // Apply phase shift to target qubit
        // For basis states where target qubit is 1, multiply amplitude by e^(iθ)
        int mask = 1 << target_qubit;

        for (int i = 0; i < state_size; i++) {
            // Check if target qubit is 1
            if ((i & mask) != 0) {
                Complex current_amp = state.getAmplitude(i);
                state.setAmplitude(i, current_amp * phase_factor);
            }
            // If target qubit is 0, amplitude remains unchanged
        }
    }

    int getTarget() const { return target_qubit; }
    double getPhase() const { return phase_angle; }
};

// Pauli-X Gate (NOT Gate)
// Flips the qubit state: |0⟩ → |1⟩, |1⟩ → |0⟩
class XGate : public QuantumGate {
private:
    int target_qubit;

public:
    XGate(int target) : target_qubit(target) {
        if (target < 0) {
            throw std::invalid_argument("Target qubit must be non-negative");
        }
    }

    void apply(QuantumState& state) override {
        int num_qubits = state.getNumQubits();
        int state_size = state.getStateSize();

        // Validate target qubit
        if (target_qubit >= num_qubits) {
            throw std::invalid_argument("Target qubit exceeds number of qubits in state");
        }

        // Get current amplitudes
        const std::vector<Complex>& old_amplitudes = state.getAmplitudes();
        std::vector<Complex> new_amplitudes = old_amplitudes;

        // Apply X gate by swapping amplitudes of basis states that differ only in target qubit
        int mask = 1 << target_qubit;

        for (int i = 0; i < state_size; i++) {
            // Only process each pair once (when i has 0 at target bit)
            if ((i & mask) == 0) {
                int j = i ^ mask; // Flip the target qubit bit
                // Swap amplitudes
                new_amplitudes[i] = old_amplitudes[j];
                new_amplitudes[j] = old_amplitudes[i];
            }
        }

        // Update the quantum state with new amplitudes
        for (int i = 0; i < state_size; i++) {
            state.setAmplitude(i, new_amplitudes[i]);
        }
    }

    int getTarget() const { return target_qubit; }
};

// Controlled Modular Multiplication Gate
// Performs: |control, y⟩ → |control, (multiplier * y) mod N⟩ if control is |1⟩
// NOTE: This gate is ONLY reversible/unitary when gcd(multiplier, N) = 1
// The input validation ensures this condition is met
class ControlledModMultGate : public QuantumGate {
private:
    int control_qubit;
    int target_qubits_start;  // Starting position of target register
    int target_qubits_count;  // Number of qubits in target register
    uint64_t multiplier;
    uint64_t modulus;

    // Helper: Extract target register value from state index
    uint64_t extractTarget(int state_index) const {
        return (state_index >> target_qubits_start) & ((1ULL << target_qubits_count) - 1);
    }

    // Helper: Replace target register bits in state index
    int replaceTarget(int state_index, uint64_t new_target_value) const {
        // Clear the target bits
        int mask = ((1ULL << target_qubits_count) - 1) << target_qubits_start;
        int cleared = state_index & ~mask;

        // Set new target value
        return cleared | (new_target_value << target_qubits_start);
    }

public:
    ControlledModMultGate(int control, int target_start, int target_count,
                         uint64_t mult, uint64_t mod)
        : control_qubit(control), target_qubits_start(target_start),
          target_qubits_count(target_count), multiplier(mult), modulus(mod) {

        if (control < 0 || target_start < 0 || target_count <= 0) {
            throw std::invalid_argument("Invalid qubit parameters");
        }
        if (mult == 0 || mod == 0) {
            throw std::invalid_argument("Multiplier and modulus must be positive");
        }
        if (control >= target_start && control < target_start + target_count) {
            throw std::invalid_argument("Control qubit must not be in target register");
        }
    }

    void apply(QuantumState& state) override {
        int num_qubits = state.getNumQubits();
        int state_size = state.getStateSize();

        // Validate
        if (control_qubit >= num_qubits) {
            throw std::invalid_argument("Control qubit exceeds number of qubits");
        }
        if (target_qubits_start + target_qubits_count > num_qubits) {
            throw std::invalid_argument("Target register exceeds number of qubits");
        }

        // Get current amplitudes
        const std::vector<Complex>& old_amplitudes = state.getAmplitudes();
        std::vector<Complex> new_amplitudes = old_amplitudes;

        int control_mask = 1 << control_qubit;

        // Apply controlled modular multiplication
        for (int i = 0; i < state_size; i++) {
            // Only act if control qubit is |1⟩
            if ((i & control_mask) != 0) {
                // Extract target register value
                uint64_t y = extractTarget(i);

                // Apply modular multiplication
                uint64_t new_y = (multiplier * y) % modulus;

                // Calculate new state index
                int j = replaceTarget(i, new_y);

                // Move amplitude
                new_amplitudes[j] = old_amplitudes[i];
            }
        }

        // Update state
        for (int i = 0; i < state_size; i++) {
            state.setAmplitude(i, new_amplitudes[i]);
        }
    }

    int getControl() const { return control_qubit; }
    int getTargetStart() const { return target_qubits_start; }
    int getTargetCount() const { return target_qubits_count; }
    uint64_t getMultiplier() const { return multiplier; }
    uint64_t getModulus() const { return modulus; }
};

#endif // QUANTUM_GATES_H
