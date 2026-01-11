#include "quantum_arithmetic.h"
#include "quantum_state.h"
#include <iostream>
#include <cassert>

// Helper function to initialize a quantum state with a specific value in a register
void initializeRegister(QuantumState& state, int start, int count, int value) {
    int total_qubits = state.getNumQubits();
    int state_size = state.getStateSize();
    
    // First, find which position has amplitude 1
    int one_position = -1;
    for (int i = 0; i < state_size; i++) {
        Complex amp = state.getAmplitude(i);
        if (amp.real() > 0.99 && amp.real() < 1.01 && amp.imag() > -0.01 && amp.imag() < 0.01) {
            one_position = i;
            break;
        }
    }
    
    // If no position has amplitude 1, start with position 0
    if (one_position == -1) {
        one_position = 0;
        // Clear all amplitudes
        for (int i = 0; i < state_size; i++) {
            state.setAmplitude(i, Complex(0, 0));
        }
        state.setAmplitude(0, Complex(1, 0));
    }
    
    // Create masks for the register
    int register_mask = ((1 << count) - 1) << start;
    int other_mask = ~register_mask;
    
    // Extract the bits outside the register we're setting
    int other_bits = one_position & other_mask;
    
    // Create the new state index with the desired register value
    int target_state = other_bits | (value << start);
    
    // Move the amplitude 1 from the current position to the target position
    state.setAmplitude(one_position, Complex(0, 0));
    state.setAmplitude(target_state, Complex(1, 0));
    
    // Verify the new position
    int new_one_position = -1;
    for (int i = 0; i < state_size; i++) {
        Complex amp = state.getAmplitude(i);
        if (amp.real() > 0.99 && amp.real() < 1.01 && amp.imag() > -0.01 && amp.imag() < 0.01) {
            new_one_position = i;
            break;
        }
    }
}

// Helper function to measure a register value
int measureRegister(QuantumState& state, int start, int count) {
    int state_size = state.getStateSize();
    double max_prob = 0.0;
    int most_likely_value = 0;
    
    // Find the basis state with highest probability
    for (int i = 0; i < state_size; i++) {
        double prob = state.getProbability(i);
        if (prob > max_prob) {
            max_prob = prob;
            // Extract register value from state index
            most_likely_value = (i >> start) & ((1 << count) - 1);
        }
    }
    
    return most_likely_value;
}

// Test basic comparison functionality
void testBasicComparison() {
    std::cout << "Testing basic comparison functionality (equality check)..." << std::endl;
    
    // Test case 1: a = 5, b = 3, expect result = 0 (5 ≠ 3)
    {
        int num_bits = 3;
        int total_qubits = 3 + 3 + 4; // a + b + result (need num_bits+1 qubits for result)
        QuantumState state(total_qubits);
        
        // Initialize a = 5 (101), b = 3 (011)
        initializeRegister(state, 0, 3, 5);  // a register at qubits 0-2
        initializeRegister(state, 3, 3, 3);  // b register at qubits 3-5
        
        // Apply comparator (checks for equality)
        QuantumComparator comparator(0, 3, 6, 3);  // a_start=0, b_start=3, result=6, num_bits=3
        comparator.apply(state);
        
        // Measure result (check if all bits in result register are 1)
        int result = measureRegister(state, 6, 4);  // Measure all result qubits
        bool equal = (result == 0xF);  // All bits should be 1 if equal
        std::cout << "Test 1: a=5, b=3, result=" << (equal ? "EQUAL" : "NOT EQUAL") << " (expected NOT EQUAL)" << std::endl;
        assert(!equal);
    }
    
    // Test case 2: a = 2, b = 7, expect result = 0 (2 ≠ 7)
    {
        int num_bits = 3;
        int total_qubits = 3 + 3 + 4; // a + b + result
        QuantumState state(total_qubits);
        
        // Initialize a = 2 (010), b = 7 (111)
        initializeRegister(state, 0, 3, 2);  // a register at qubits 0-2
        initializeRegister(state, 3, 3, 7);  // b register at qubits 3-5
        
        // Apply comparator
        QuantumComparator comparator(0, 3, 6, 3);  // a_start=0, b_start=3, result=6, num_bits=3
        comparator.apply(state);
        
        // Measure result
        int result = measureRegister(state, 6, 4);
        bool equal = (result == 0xF);
        std::cout << "Test 2: a=2, b=7, result=" << (equal ? "EQUAL" : "NOT EQUAL") << " (expected NOT EQUAL)" << std::endl;
        assert(!equal);
    }
    
    // Test case 3: a = 4, b = 4, expect result = 1 (4 = 4)
    {
        int num_bits = 3;
        int total_qubits = 3 + 3 + 4; // a + b + result
        QuantumState state(total_qubits);
        
        // Initialize a = 4 (100), b = 4 (100)
        initializeRegister(state, 0, 3, 4);  // a register at qubits 0-2
        initializeRegister(state, 3, 3, 4);  // b register at qubits 3-5
        
        // Apply comparator
        QuantumComparator comparator(0, 3, 6, 3);  // a_start=0, b_start=3, result=6, num_bits=3
        comparator.apply(state);
        
        // Measure result
        int result = measureRegister(state, 6, 4);
        bool equal = (result == 0xF);
        std::cout << "Test 3: a=4, b=4, result=" << (equal ? "EQUAL" : "NOT EQUAL") << " (expected EQUAL)" << std::endl;
        assert(equal);
    }
    
    std::cout << "Basic comparison tests passed!" << std::endl;
}

// Test edge cases
void testEdgeCases() {
    std::cout << "Testing edge cases..." << std::endl;
    
    // Test case 1: a = 0, b = 0, expect result = 1 (0 = 0)
    {
        int num_bits = 3;
        int total_qubits = 3 + 3 + 4; // a + b + result
        QuantumState state(total_qubits);
        
        // Initialize a = 0, b = 0
        initializeRegister(state, 0, 3, 0);  // a register at qubits 0-2
        initializeRegister(state, 3, 3, 0);  // b register at qubits 3-5
        
        // Apply comparator
        QuantumComparator comparator(0, 3, 6, 3);  // a_start=0, b_start=3, result=6, num_bits=3
        comparator.apply(state);
        
        // Measure result
        int result = measureRegister(state, 6, 4);
        bool equal = (result == 0xF);
        std::cout << "Edge Test 1: a=0, b=0, result=" << (equal ? "EQUAL" : "NOT EQUAL") << " (expected EQUAL)" << std::endl;
        assert(equal);
    }
    
    // Test case 2: a = 7, b = 0, expect result = 0 (7 ≠ 0)
    {
        int num_bits = 3;
        int total_qubits = 3 + 3 + 4; // a + b + result
        QuantumState state(total_qubits);
        
        // Initialize a = 7 (111), b = 0
        initializeRegister(state, 0, 3, 7);  // a register at qubits 0-2
        initializeRegister(state, 3, 3, 0);  // b register at qubits 3-5
        
        // Apply comparator
        QuantumComparator comparator(0, 3, 6, 3);  // a_start=0, b_start=3, result=6, num_bits=3
        comparator.apply(state);
        
        // Measure result
        int result = measureRegister(state, 6, 4);
        bool equal = (result == 0xF);
        std::cout << "Edge Test 2: a=7, b=0, result=" << (equal ? "EQUAL" : "NOT EQUAL") << " (expected NOT EQUAL)" << std::endl;
        assert(!equal);
    }
    
    std::cout << "Edge case tests passed!" << std::endl;
}

int main() {
    std::cout << "=== Quantum Comparator Tests ===" << std::endl;
    
    try {
        testBasicComparison();
        testEdgeCases();
        
        std::cout << "\nAll tests completed!" << std::endl;
        std::cout << "\nNote: The current implementation is a simplified demonstration of equality comparison." << std::endl;
        std::cout << "A production implementation would require:" << std::endl;
        std::cout << "1. More efficient equality computation" << std::endl;
        std::cout << "2. Complete ancilla cleanup for reversibility" << std::endl;
        std::cout << "3. Optimization for fewer ancilla qubits" << std::endl;
        std::cout << "4. Support for greater than/less than comparisons" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}