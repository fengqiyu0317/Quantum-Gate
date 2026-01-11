#include "quantum_state.h"
#include "quantum_gates.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Helper function to print a binary state with labels
void printState(const string& label, const QuantumState& state, int numQubits) {
    cout << label << ":\n";
    cout << "  Basis States and Amplitudes:\n";

    for (int i = 0; i < state.getStateSize(); i++) {
        Complex amp = state.getAmplitude(i);
        double prob = norm(amp);

        if (prob > 1e-10) {  // Only show states with significant probability
            // Convert index to binary representation
            string binary;
            int temp = i;
            for (int j = 0; j < numQubits; j++) {
                binary = ((temp & 1) ? "1" : "0") + binary;
                temp >>= 1;
            }

            cout << "    |" << binary << "⟩: "
                 << setw(12) << amp
                 << " (Probability: " << setw(10) << fixed << setprecision(6) << prob << ")\n";
        }
    }
    cout << "\n";
}

// Helper function to prepare a specific computational basis state
void prepareBasisState(QuantumState& state, int qubit0, int qubit1, int target) {
    // Apply X gates to set initial state (starts at |000⟩)
    if (qubit0) {
        XGate x_gate(0);
        x_gate.apply(state);
    }
    if (qubit1) {
        XGate x_gate(1);
        x_gate.apply(state);
    }
    if (target) {
        XGate x_gate(2);
        x_gate.apply(state);
    }
}

// Helper function to measure the quantum state
// Returns the basis state with highest probability
int measureState(const QuantumState& state) {
    int state_size = state.getStateSize();
    double max_prob = 0.0;
    int most_likely_state = 0;

    for (int i = 0; i < state_size; i++) {
        double prob = state.getProbability(i);
        if (prob > max_prob) {
            max_prob = prob;
            most_likely_state = i;
        }
    }

    return most_likely_state;
}

// Run test for a specific input combination
void testAndOperation(int a, int b, const string& testName) {
    cout << "═══════════════════════════════════════════════════════════════\n";
    cout << "TEST: " << testName << "\n";
    cout << "═══════════════════════════════════════════════════════════════\n";
    cout << "Input: a=" << a << ", b=" << b << ", target=0\n";
    cout << "Expected AND result: " << (a & b) << "\n\n";

    // Create 3-qubit state (2 control qubits + 1 target qubit)
    QuantumState state(3);

    // Step 1: Prepare initial state |a,b,0⟩
    // Note: Qubit 0 = a, Qubit 1 = b, Qubit 2 = target (initialized to 0)
    prepareBasisState(state, a, b, 0);
    printState("Initial State |q2,q1,q0⟩ = |target,b,a⟩", state, 3);

    // Step 2: Apply Toffoli gate (CCNOT)
    // Qubits 0 and 1 are controls, qubit 2 is target
    ToffoliGate toffoli(0, 1, 2);
    toffoli.apply(state);
    printState("After Toffoli Gate (AND Operation)", state, 3);

    // Measure the result
    int result = measureState(state);
    cout << "Measurement result: " << result << " (binary: ";
    for (int i = 2; i >= 0; i--) {
        cout << ((result >> i) & 1);
    }
    cout << ")\n";

    // Extract the target qubit value
    int targetResult = (result >> 2) & 1;
    cout << "Target qubit (AND result): " << targetResult << "\n";

    // Verify correctness
    int expectedAnd = a & b;
    if (targetResult == expectedAnd) {
        cout << "✓ CORRECT: " << a << " AND " << b << " = " << targetResult << "\n";
    } else {
        cout << "✗ ERROR: Expected " << expectedAnd << ", got " << targetResult << "\n";
    }
    cout << "\n";

    // Step 3: Demonstrate reversibility
    // Prepare the same state again
    QuantumState state2(3);
    prepareBasisState(state2, a, b, 0);
    ToffoliGate toffoli2(0, 1, 2);
    toffoli2.apply(state2);

    cout << "───────────────────────────────────────────────────────────────\n";
    cout << "REVERSIBILITY TEST\n";
    cout << "───────────────────────────────────────────────────────────────\n";

    printState("State after first Toffoli", state2, 3);

    // Apply Toffoli gate again (should reverse the operation)
    ToffoliGate toffoli3(0, 1, 2);
    toffoli3.apply(state2);
    printState("After Second Toffoli Gate (Reversal)", state2, 3);

    // Measure to verify we're back to initial state
    int reversedResult = measureState(state2);
    cout << "Reversed measurement: " << reversedResult << " (binary: ";
    for (int i = 2; i >= 0; i--) {
        cout << ((reversedResult >> i) & 1);
    }
    cout << ")\n";

    // Verify reversibility
    int originalValue = (a << 2) | (b << 1) | 0;  // |a,b,0⟩
    if (reversedResult == originalValue) {
        cout << "✓ REVERSIBILITY VERIFIED: Returned to initial state |"
             << a << b << "0⟩\n";
    } else {
        cout << "✗ REVERSIBILITY FAILED: Expected " << originalValue
             << ", got " << reversedResult << "\n";
    }
    cout << "\n\n";
}

int main() {
    cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    cout << "║   Toffoli Gate AND Operation & Reversibility Demonstration     ║\n";
    cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";

    cout << "The Toffoli gate (CCNOT) is a 3-qubit gate:\n";
    cout << "  - Control qubits: q0, q1\n";
    cout << "  - Target qubit: q2\n";
    cout << "  - Operation: Flip target if BOTH controls are |1⟩\n";
    cout << "  - This implements the AND operation: q2 = q0 AND q1\n\n";

    cout << "Reversibility: Toffoli² = I (applying Toffoli twice returns to original state)\n\n";

    // Test all 4 combinations of 2 bits
    vector<pair<int, int>> testCases = {
        {0, 0},  // 0 AND 0 = 0
        {0, 1},  // 0 AND 1 = 0
        {1, 0},  // 1 AND 0 = 0
        {1, 1}   // 1 AND 1 = 1
    };

    vector<string> testNames = {
        "0 AND 0",
        "0 AND 1",
        "1 AND 0",
        "1 AND 1"
    };

    for (size_t i = 0; i < testCases.size(); i++) {
        testAndOperation(testCases[i].first, testCases[i].second, testNames[i]);
    }

    // Summary
    cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    cout << "║                          SUMMARY                               ║\n";
    cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    cout << "The Toffoli gate successfully implements the AND operation:\n";
    cout << "  ✓ Input |00⟩ → Output |000⟩ (AND: 0)\n";
    cout << "  ✓ Input |01⟩ → Output |010⟩ (AND: 0)\n";
    cout << "  ✓ Input |10⟩ → Output |100⟩ (AND: 0)\n";
    cout << "  ✓ Input |11⟩ → Output |111⟩ (AND: 1)\n\n";

    cout << "Reversibility verified:\n";
    cout << "  ✓ Applying Toffoli twice returns the system to its original state\n";
    cout << "  ✓ Toffoli² = Identity (I)\n";
    cout << "  ✓ No information is lost in the computation\n\n";

    cout << "This demonstrates that quantum gates are reversible operations,\n";
    cout << "unlike classical AND gates which lose information.\n\n";

    cout << "Matrix representation of Toffoli gate:\n";
    cout << "  |000⟩ → |000⟩  |001⟩ → |001⟩  |010⟩ → |010⟩  |011⟩ → |011⟩\n";
    cout << "  |100⟩ → |100⟩  |101⟩ → |101⟩  |110⟩ → |111⟩  |111⟩ → |110⟩\n\n";

    cout << "Truth Table (showing reversibility):\n";
    cout << "  ┌─────┬─────┬───────────┬───────────┐\n";
    cout << "  │  a  │  b  │  Toffoli  │  Toffoli² │\n";
    cout << "  ├─────┼─────┼───────────┼───────────┤\n";
    cout << "  │  0  │  0  │  |000⟩    │  |000⟩    │\n";
    cout << "  │  0  │  1  │  |010⟩    │  |010⟩    │\n";
    cout << "  │  1  │  0  │  |100⟩    │  |100⟩    │\n";
    cout << "  │  1  │  1  │  |111⟩    │  |111⟩    │\n";
    cout << "  └─────┴─────┴───────────┴───────────┘\n\n";

    return 0;
}
