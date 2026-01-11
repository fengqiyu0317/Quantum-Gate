#ifndef QUANTUM_ARITHMETIC_H
#define QUANTUM_ARITHMETIC_H

#include "quantum_gates.h"
#include <vector>

// Quantum Ripple-Carry Adder (Cuccaro Adder)
// Computes: |a⟩|b⟩|0⟩ → |a⟩|a+b⟩|carry⟩
// Where:
// - a_start: starting qubit for first addend
// - b_start: starting qubit for second addend (also stores result)
// - carry_start: starting qubit for carry bits
// - num_bits: number of bits in each number
class QuantumAdder : public QuantumGate {
private:
    int a_start;
    int b_start;
    int carry_start;
    int num_bits;
    
public:
    QuantumAdder(int a_start, int b_start, int carry_start, int num_bits)
        : a_start(a_start), b_start(b_start), carry_start(carry_start), num_bits(num_bits) {
        
        if (a_start < 0 || b_start < 0 || carry_start < 0) {
            throw std::invalid_argument("Qubit positions must be non-negative");
        }
        if (num_bits <= 0) {
            throw std::invalid_argument("Number of bits must be positive");
        }
    }
    
    void apply(QuantumState& state) override {
        int total_qubits = state.getNumQubits();
        
        // Validate qubit positions
        if (a_start + num_bits > total_qubits || 
            b_start + num_bits > total_qubits || 
            carry_start + num_bits + 1 > total_qubits) {
            throw std::invalid_argument("Register exceeds number of qubits in state");
        }

        // Perform ripple-carry addition
        for (int i = 0; i < num_bits; i++) {
            int a_qubit = a_start + i;
            int b_qubit = b_start + i;
            int carry_in = carry_start + i;
            int carry_out = carry_start + i + 1;
            
            // Compute carry_out = MAJ(a_i, b_i, carry_in)
            // MAJ(a,b,c) = (a ∧ b) ∨ (a ∧ c) ∨ (b ∧ c)
            
            
            // Step 1: Compute a_i ∧ b_i
            ToffoliGate maj1(a_qubit, b_qubit, carry_out);
            maj1.apply(state);
            
            // Step 2: Add a_i ∧ carry_in
            ToffoliGate maj2(a_qubit, carry_in, carry_out);
            maj2.apply(state);
            
            // Step 3: Add b_i ∧ carry_in
            ToffoliGate maj3(b_qubit, carry_in, carry_out);
            maj3.apply(state);
            
            // Compute sum bit: b_i = a_i ⊕ b_i ⊕ carry_in
            // First: b_i = a_i ⊕ b_i
            CNOTGate sum1(a_qubit, b_qubit);
            sum1.apply(state);
            
            // Then: b_i = b_i ⊕ carry_in
            CNOTGate sum2(carry_in, b_qubit);
            sum2.apply(state);            
        }
    }
    
    int getAStart() const { return a_start; }
    int getBStart() const { return b_start; }
    int getCarryStart() const { return carry_start; }
    int getNumBits() const { return num_bits; }
};

// Quantum Comparator
// Computes: |a⟩|b⟩|0⟩ → |a⟩|b⟩|a=b⟩
// Where:
// - a_start: starting qubit for first number
// - b_start: starting qubit for second number
// - result_start: qubit to store comparison result (1 if a = b, 0 otherwise)
// - num_bits: number of bits in each number
// - ancilla_start: starting qubit for ancilla (temporary workspace)
class QuantumComparator : public QuantumGate {
private:
    int a_start;
    int b_start;
    int result_start;
    int num_bits;
    int ancilla_start;
    
public:
    QuantumComparator(int a_start, int b_start, int result_start, int num_bits, int ancilla_start = -1)
        : a_start(a_start), b_start(b_start), result_start(result_start), num_bits(num_bits), ancilla_start(ancilla_start){
        if (a_start < 0 || b_start < 0 || result_start < 0) {
            throw std::invalid_argument("Qubit positions must be non-negative");
        }
        if (num_bits <= 0) {
            throw std::invalid_argument("Number of bits must be positive");
        }
    }
    
    void apply(QuantumState& state) override {
        int total_qubits = state.getNumQubits();
        
        // Validate qubit positions
        if (a_start + num_bits > total_qubits ||
            b_start + num_bits > total_qubits ||
            result_start + num_bits + 1 > total_qubits) {
            throw std::invalid_argument("Register exceeds number of qubits in state");
        }

        XGate init_result(result_start);
        init_result.apply(state);
        
        // Compare from most significant bit to least significant bit
        for (int i = 0; i < num_bits; i++) {
            int a_qubit = a_start + i;
            int b_qubit = b_start + i;
            int result_qubit = result_start + 1 + i;

            // Step 1: XOR the bits to check if they're different
            // If a_i = b_i, then b_qubit becomes |0⟩, otherwise |1⟩
            CNOTGate xor_ab(a_qubit, b_qubit);
            xor_ab.apply(state);

            XGate invert_b(b_qubit);
            invert_b.apply(state);
            
            // Step 2: Use Toffoli gate to propagate inequality detection
            // If current bits are different (b_qubit = |1⟩) AND all more significant
            // bits were equal (result_qubit - 1 = |1⟩), then mark this position
            // This ensures we detect the first (most significant) difference
            ToffoliGate check_not_equal(b_qubit, result_qubit - 1, result_qubit);
            check_not_equal.apply(state);
        }
    }
    
    int getAStart() const { return a_start; }
    int getBStart() const { return b_start; }
    int getResultStart() const { return result_start; }
    int getNumBits() const { return num_bits; }
    int getAncillaStart() const { return ancilla_start; }
};

#endif // QUANTUM_ARITHMETIC_H