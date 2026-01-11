#include "quantum_arithmetic.h"
#include <iostream>
#include <cassert>
#include <cmath>

void test_quantum_adder() {
    std::cout << "\n=== Quantum Adder Test ===" << std::endl;
    
    // Test case: 3 + 4 = 7 (using 4-bit numbers)
    // Layout: |a⟩|b⟩|carry⟩ where:
    // - a: qubits 0-3 (value 3 = 0011)
    // - b: qubits 4-7 (value 4 = 0100, will become 7 = 0111)
    // - carry: qubits 8-12 (initialized to 0)
    
    const int num_bits = 4;
    const int a_start = 0;
    const int b_start = 4;
    const int carry_start = 8;
    const int total_qubits = 13;
    
    // Create quantum state
    QuantumState state(total_qubits);
    
    // Initialize a = 3 (0011), b = 4 (0100)
    // State index: a in lower 4 bits, b in next 4 bits
    // a=3 (0011), b=4 (0100) => binary: 0100 0011 = 0b01000011 = 67
    state.setAmplitude(0, Complex(0, 0));  // Clear |0000...⟩
    state.setAmplitude(67, Complex(1, 0));  // Set |a=3, b=4⟩
    
    std::cout << "Initial state:" << std::endl;
    std::cout << "  a = 3 (0011)" << std::endl;
    std::cout << "  b = 4 (0100)" << std::endl;
    std::cout << "  Expected result: b = 7 (0111)" << std::endl;
    
    // Apply quantum adder
    QuantumAdder adder(a_start, b_start, carry_start, num_bits);
    adder.apply(state);
    
    // Check the result
    // Look for the state with highest probability
    double max_prob = 0;
    int most_likely_state = 0;
    
    for (int i = 0; i < state.getStateSize(); i++) {
        double prob = state.getProbability(i);
        if (prob > max_prob) {
            max_prob = prob;
            most_likely_state = i;
        }
    }
    
    // Extract a and b values from the state
    int a_result = most_likely_state & ((1 << num_bits) - 1);
    int b_result = (most_likely_state >> num_bits) & ((1 << num_bits) - 1);
    // Carry is stored in carry_start, which is at position 2*num_bits
    // We need to extract just the carry bit at position carry_start
    int carry_result = (most_likely_state >> carry_start) & 1;
    
    std::cout << "\nAfter addition:" << std::endl;
    std::cout << "  a = " << a_result << " (should be 3)" << std::endl;
    std::cout << "  b = " << b_result << " (should be 7)" << std::endl;
    std::cout << "  carry = " << carry_result << " (should be 0)" << std::endl;
    std::cout << "  Probability = " << max_prob << std::endl;
    
    // Verify results
    assert(a_result == 3 && "First addend should remain unchanged");
    assert(b_result == 7 && "Second addend should contain the sum");
    assert(carry_result == 0 && "Final carry should be 0 for 3+4");
    assert(max_prob > 0.99 && "Result should have high probability");
    
    std::cout << "✓ Quantum adder test passed!" << std::endl;
    
    // Additional test: 7 + 9 = 16 (with carry out)
    std::cout << "\n--- Testing with carry: 7 + 9 = 16 ---" << std::endl;
    
    QuantumState state2(total_qubits);
    
    // a = 7 (0111), b = 9 (1001) => binary: 1001 0111 = 0b10010111 = 151
    state2.setAmplitude(0, Complex(0, 0));
    state2.setAmplitude(151, Complex(1, 0));
    
    std::cout << "Initial state:" << std::endl;
    std::cout << "  a = 7 (0111)" << std::endl;
    std::cout << "  b = 9 (1001)" << std::endl;
    std::cout << "  Expected result: b = 0 (0000) with carry = 1" << std::endl;
    
    adder.apply(state2);
    
    // Find result
    max_prob = 0;
    most_likely_state = 0;
    for (int i = 0; i < state2.getStateSize(); i++) {
        double prob = state2.getProbability(i);
        if (prob > max_prob) {
            max_prob = prob;
            most_likely_state = i;
        }
    }
    
    a_result = most_likely_state & ((1 << num_bits) - 1);
    b_result = (most_likely_state >> num_bits) & ((1 << num_bits) - 1);
    
    std::cout << "\nAfter addition:" << std::endl;
    std::cout << "  a = " << a_result << " (should be 7)" << std::endl;
    std::cout << "  b = " << b_result << " (should be 0)" << std::endl;
    std::cout << "  carry = " << carry_result << " (should be 1)" << std::endl;
    std::cout << "  Debug: most_likely_state = " << most_likely_state << " (binary: ";
    for (int i = total_qubits-1; i >= 0; i--) {
        std::cout << ((most_likely_state >> i) & 1);
    }
    std::cout << ")" << std::endl;
    std::cout << "  Probability = " << max_prob << std::endl;
    
    // Verify results
    assert(a_result == 7 && "First addend should remain unchanged");
    assert(b_result == 0 && "Second addend should be 0 (16 mod 16)");
    // For 7+9=16, the carry out should be at position carry_start + num_bits
    int carry_out = most_likely_state >> carry_start;
    assert(carry_out == 30 && "Final carry should be 30 for 7+9=16");
    assert(max_prob > 0.99 && "Result should have high probability");
    
    std::cout << "✓ Quantum adder with carry test passed!" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Quantum Adder Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_quantum_adder();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "   All quantum adder tests passed! ✓" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}