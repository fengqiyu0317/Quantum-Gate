#include "quantum_gates.h"
#include <iostream>
#include <cassert>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Helper function to print test header
void printTestHeader(const std::string& test_name) {
    std::cout << "\n=== " << test_name << " ===" << std::endl;
}

// Test 1: Hadamard Gate
void test_hadamard() {
    printTestHeader("Hadamard Gate Test");

    // Test H on |0⟩: should create (|0⟩ + |1⟩)/√2
    QuantumState state1(1);
    HadamardGate H(0);
    H.apply(state1);

    double prob0 = state1.getProbability(0);
    double prob1 = state1.getProbability(1);

    std::cout << "H|0⟩: P(|0⟩) = " << prob0 << ", P(|1⟩) = " << prob1 << std::endl;
    assert(std::abs(prob0 - 0.5) < 1e-10 && "H|0⟩: P(|0⟩) should be 0.5");
    assert(std::abs(prob1 - 0.5) < 1e-10 && "H|0⟩: P(|1⟩) should be 0.5");
    assert(state1.isNormalized() && "State should be normalized");
    std::cout << "✓ H|0⟩ test passed" << std::endl;

    // Test H on |1⟩: should create (|0⟩ - |1⟩)/√2
    QuantumState state2(1);
    state2.setAmplitude(0, Complex(0, 0));
    state2.setAmplitude(1, Complex(1, 0));
    H.apply(state2);

    Complex amp1 = state2.getAmplitude(1);
    std::cout << "H|1⟩: amplitude of |1⟩ = " << amp1 << " (should have negative real part)" << std::endl;
    assert(amp1.real() < 0 && "H|1⟩: amplitude should be negative");
    std::cout << "✓ H|1⟩ test passed" << std::endl;
}

// Test 2: CNOT Gate
void test_cnot() {
    printTestHeader("CNOT Gate Test");

    int num_qubits = 2;
    int control = 0;
    int target = 1;

    // Test CNOT|00⟩: control=0, target unchanged
    QuantumState state1(num_qubits);
    CNOTGate CNOT(control, target);
    CNOT.apply(state1);
    std::cout << "CNOT|00⟩: should stay |00⟩" << std::endl;
    assert(std::abs(state1.getProbability(0) - 1.0) < 1e-10 && "CNOT|00⟩ failed");
    std::cout << "✓ CNOT|00⟩ test passed" << std::endl;

    // Test CNOT|01⟩: control=1, target flips
    // |01⟩ means qubit 0=1 (control), qubit 1=0 (target)
    QuantumState state2(num_qubits);
    state2.setAmplitude(0, Complex(0, 0));
    state2.setAmplitude(1, Complex(1, 0)); // |01⟩
    CNOT.apply(state2);
    std::cout << "CNOT|01⟩: should become |11⟩" << std::endl;
    assert(std::abs(state2.getProbability(3) - 1.0) < 1e-10 && "CNOT|01⟩ failed");
    std::cout << "✓ CNOT|01⟩ test passed" << std::endl;

    // Test CNOT|11⟩: control=1, target flips
    // |11⟩ means qubit 0=1 (control), qubit 1=1 (target)
    QuantumState state3(num_qubits);
    state3.setAmplitude(0, Complex(0, 0));
    state3.setAmplitude(3, Complex(1, 0)); // |11⟩
    CNOT.apply(state3);
    std::cout << "CNOT|11⟩: should become |01⟩" << std::endl;
    assert(std::abs(state3.getProbability(1) - 1.0) < 1e-10 && "CNOT|11⟩ failed");
    std::cout << "✓ CNOT|11⟩ test passed" << std::endl;
}

// Test 3: SWAP Gate
void test_swap() {
    printTestHeader("SWAP Gate Test");

    int num_qubits = 2;
    int q1 = 0, q2 = 1;

    // Test SWAP|01⟩: should become |10⟩
    QuantumState state1(num_qubits);
    state1.setAmplitude(0, Complex(0, 0));
    state1.setAmplitude(1, Complex(1, 0)); // |01⟩
    SWAPGate SWAP(q1, q2);
    SWAP.apply(state1);
    std::cout << "SWAP|01⟩: should become |10⟩" << std::endl;
    assert(std::abs(state1.getProbability(2) - 1.0) < 1e-10 && "SWAP|01⟩ failed");
    std::cout << "✓ SWAP|01⟩ test passed" << std::endl;

    // Test SWAP|10⟩: should become |01⟩
    QuantumState state2(num_qubits);
    state2.setAmplitude(0, Complex(0, 0));
    state2.setAmplitude(2, Complex(1, 0)); // |10⟩
    SWAP.apply(state2);
    std::cout << "SWAP|10⟩: should become |01⟩" << std::endl;
    assert(std::abs(state2.getProbability(1) - 1.0) < 1e-10 && "SWAP|10⟩ failed");
    std::cout << "✓ SWAP|10⟩ test passed" << std::endl;

    // Test SWAP|00⟩: should stay |00⟩
    QuantumState state3(num_qubits);
    SWAP.apply(state3);
    std::cout << "SWAP|00⟩: should stay |00⟩" << std::endl;
    assert(std::abs(state3.getProbability(0) - 1.0) < 1e-10 && "SWAP|00⟩ failed");
    std::cout << "✓ SWAP|00⟩ test passed" << std::endl;
}

// Test 4: Toffoli Gate
void test_toffoli() {
    printTestHeader("Toffoli Gate Test");

    int num_qubits = 3;
    int ctrl1 = 0, ctrl2 = 1, target = 2;

    ToffoliGate Toffoli(ctrl1, ctrl2, target);

    // Test Toffoli|011⟩: both controls=1, target flips
    // |011⟩ means: qubit 0=1 (ctrl1), qubit 1=1 (ctrl2), qubit 2=0 (target)
    QuantumState state1(num_qubits);
    state1.setAmplitude(0, Complex(0, 0));
    state1.setAmplitude(3, Complex(1, 0)); // |011⟩
    Toffoli.apply(state1);
    std::cout << "Toffoli|011⟩: should become |111⟩" << std::endl;
    assert(std::abs(state1.getProbability(7) - 1.0) < 1e-10 && "Toffoli|011⟩ failed");
    std::cout << "✓ Toffoli|011⟩ test passed" << std::endl;

    // Test Toffoli|100⟩: only one control=1, target unchanged
    QuantumState state2(num_qubits);
    state2.setAmplitude(0, Complex(0, 0));
    state2.setAmplitude(4, Complex(1, 0)); // |100⟩
    Toffoli.apply(state2);
    std::cout << "Toffoli|100⟩: should stay |100⟩" << std::endl;
    assert(std::abs(state2.getProbability(4) - 1.0) < 1e-10 && "Toffoli|100⟩ failed");
    std::cout << "✓ Toffoli|100⟩ test passed" << std::endl;

    // Test Toffoli|111⟩: both controls=1, target flips
    // |111⟩ means: qubit 0=1 (ctrl1), qubit 1=1 (ctrl2), qubit 2=1 (target)
    QuantumState state3(num_qubits);
    state3.setAmplitude(0, Complex(0, 0));
    state3.setAmplitude(7, Complex(1, 0)); // |111⟩
    Toffoli.apply(state3);
    std::cout << "Toffoli|111⟩: should become |011⟩" << std::endl;
    assert(std::abs(state3.getProbability(3) - 1.0) < 1e-10 && "Toffoli|111⟩ failed");
    std::cout << "✓ Toffoli|111⟩ test passed" << std::endl;
}

// Test 5: Phase Shift Gate
void test_phase_shift() {
    printTestHeader("Phase Shift Gate Test");

    // Test S gate (π/2 phase shift)
    QuantumState state1(1);
    state1.setAmplitude(0, Complex(0, 0));
    state1.setAmplitude(1, Complex(1, 0)); // |1⟩
    PhaseShiftGate S_gate(0, M_PI / 2);
    S_gate.apply(state1);
    Complex amp_s = state1.getAmplitude(1);
    std::cout << "S|1⟩: amplitude = " << amp_s << " (should be i)" << std::endl;
    assert(std::abs(amp_s.real()) < 1e-10 && "S gate: real part should be ~0");
    assert(std::abs(amp_s.imag() - 1.0) < 1e-10 && "S gate: imag part should be 1.0");
    std::cout << "✓ S gate test passed" << std::endl;

    // Test T gate (π/4 phase shift)
    QuantumState state2(1);
    state2.setAmplitude(0, Complex(0, 0));
    state2.setAmplitude(1, Complex(1, 0)); // |1⟩
    PhaseShiftGate T_gate(0, M_PI / 4);
    T_gate.apply(state2);
    Complex amp_t = state2.getAmplitude(1);
    std::cout << "T|1⟩: amplitude = " << amp_t << " (should be e^(iπ/4))" << std::endl;
    double expected_t = std::sqrt(2.0) / 2.0;
    assert(std::abs(amp_t.real() - expected_t) < 1e-10 && "T gate: real part incorrect");
    assert(std::abs(amp_t.imag() - expected_t) < 1e-10 && "T gate: imag part incorrect");
    std::cout << "✓ T gate test passed" << std::endl;

    // Test Z gate (π phase shift)
    QuantumState state3(1);
    state3.setAmplitude(0, Complex(0, 0));
    state3.setAmplitude(1, Complex(1, 0)); // |1⟩
    PhaseShiftGate Z_gate(0, M_PI);
    Z_gate.apply(state3);
    Complex amp_z = state3.getAmplitude(1);
    std::cout << "Z|1⟩: amplitude = " << amp_z << " (should be -1)" << std::endl;
    assert(std::abs(amp_z.real() + 1.0) < 1e-10 && "Z gate: should flip sign");
    assert(std::abs(amp_z.imag()) < 1e-10 && "Z gate: no imaginary part");
    std::cout << "✓ Z gate test passed" << std::endl;

    // Test that |0⟩ is unchanged
    QuantumState state4(1);
    PhaseShiftGate S_gate2(0, M_PI / 2);
    S_gate2.apply(state4);
    Complex amp_0 = state4.getAmplitude(0);
    std::cout << "S|0⟩: amplitude = " << amp_0 << " (should be 1)" << std::endl;
    assert(std::abs(amp_0.real() - 1.0) < 1e-10 && "Phase gate: |0⟩ should be unchanged");
    std::cout << "✓ Phase gate leaves |0⟩ unchanged" << std::endl;
}

// Test 6: Gate Composition (Bell State)
void test_bell_state() {
    printTestHeader("Bell State Creation Test");

    // Create Bell state: (|00⟩ + |11⟩)/√2
    // H|0⟩ then CNOT
    QuantumState state(2);

    // Apply H to qubit 0
    HadamardGate H(0);
    H.apply(state);

    // Apply CNOT with control=0, target=1
    CNOTGate CNOT(0, 1);
    CNOT.apply(state);

    double prob_00 = state.getProbability(0); // |00⟩
    double prob_11 = state.getProbability(3); // |11⟩

    std::cout << "Bell state probabilities:" << std::endl;
    std::cout << "  P(|00⟩) = " << prob_00 << " (expected: 0.5)" << std::endl;
    std::cout << "  P(|11⟩) = " << prob_11 << " (expected: 0.5)" << std::endl;

    assert(std::abs(prob_00 - 0.5) < 1e-10 && "Bell state: P(|00⟩) should be 0.5");
    assert(std::abs(prob_11 - 0.5) < 1e-10 && "Bell state: P(|11⟩) should be 0.5");
    assert(state.isNormalized() && "Bell state should be normalized");

    std::cout << "✓ Bell state creation test passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Quantum Gates Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        test_hadamard();
        test_cnot();
        test_swap();
        test_toffoli();
        test_phase_shift();
        test_bell_state();

        std::cout << "\n========================================" << std::endl;
        std::cout << "   All tests passed successfully! ✓" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
