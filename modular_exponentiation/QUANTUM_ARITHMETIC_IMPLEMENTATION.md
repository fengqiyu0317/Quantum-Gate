# Quantum Arithmetic Implementation Details

## Overview

This document provides detailed implementation guidance for the quantum arithmetic circuits needed for modular exponentiation. Each component is broken down into fundamental quantum gates (Hadamard, CNOT, Toffoli, Phase).

## 1. Quantum Ripple-Carry Adder (Cuccaro Adder)

### 1.1 Mathematical Foundation
The adder computes: |a⟩|b⟩|0⟩ → |a⟩|a+b⟩|carry⟩

For each bit position i:
- Sum bit: $s_i = a_i \oplus b_i \oplus c_{i-1}$
- Carry bit: $c_i = \text{MAJ}(a_i, b_i, c_{i-1})$

Where MAJ (majority) returns 1 if at least two inputs are 1.

### 1.2 Gate Decomposition

#### 1.2.1 Majority Function (MAJ)
Using Toffoli gates:
```
MAJ(a,b,c) = (a ∧ b) ∨ (a ∧ c) ∨ (b ∧ c)

Implementation:
1. Toffoli(a,b,temp)     // temp = a ∧ b
2. Toffoli(a,c,temp)     // temp = temp ∨ (a ∧ c)
3. Toffoli(b,c,temp)     // temp = temp ∨ (b ∧ c)
4. CNOT(temp,carry)      // carry = temp
5. // Uncompute temp
6. Toffoli(b,c,temp)
7. Toffoli(a,c,temp)
8. Toffoli(a,b,temp)
```

#### 1.2.2 Sum Calculation
Using CNOT gates:
```
sum = a ⊕ b ⊕ carry_in

Implementation:
1. CNOT(a,sum)
2. CNOT(b,sum)
3. CNOT(carry_in,sum)
```

### 1.3 Complete Adder Circuit

```cpp
class QuantumAdder : public QuantumGate {
private:
    int a_start, b_start, carry_start;
    int num_bits;
    
public:
    QuantumAdder(int a_start, int b_start, int carry_start, int num_bits)
        : a_start(a_start), b_start(b_start), carry_start(carry_start), num_bits(num_bits) {}
    
    void apply(QuantumState& state) override {
        // For each bit from LSB to MSB
        for (int i = 0; i < num_bits; i++) {
            int a_qubit = a_start + i;
            int b_qubit = b_start + i;
            int carry_qubit = carry_start + i;
            int next_carry = carry_start + i + 1;
            
            // Compute carry for next position
            if (i < num_bits - 1) {
                // MAJ(a_i, b_i, carry_i) → carry_{i+1}
                ToffoliGate maj1(a_qubit, b_qubit, next_carry);
                maj1.apply(state);
                ToffoliGate maj2(a_qubit, carry_qubit, next_carry);
                maj2.apply(state);
                ToffoliGate maj3(b_qubit, carry_qubit, next_carry);
                maj3.apply(state);
            }
            
            // Compute sum bit
            CNOTGate sum1(a_qubit, b_qubit);
            sum1.apply(state);
            CNOTGate sum2(carry_qubit, b_qubit);
            sum2.apply(state);
        }
    }
};
```

## 2. Quantum Comparator

### 2.1 Mathematical Foundation
Computes: |a⟩|b⟩|0⟩ → |a⟩|b⟩|a≥b⟩

Algorithm:
1. Compute a - b using quantum subtraction
2. Check if result is non-negative
3. Clean up ancilla

### 2.2 Implementation

```cpp
class QuantumComparator : public QuantumGate {
private:
    int a_start, b_start, result_start;
    int num_bits;
    int ancilla_start;
    
public:
    QuantumComparator(int a_start, int b_start, int result_start, 
                     int num_bits, int ancilla_start)
        : a_start(a_start), b_start(b_start), result_start(result_start),
          num_bits(num_bits), ancilla_start(ancilla_start) {}
    
    void apply(QuantumState& state) override {
        // Initialize result to |0⟩
        state.setAmplitude(result_start, Complex(0, 0));
        
        // Subtract b from a with borrow
        int borrow = ancilla_start;
        for (int i = 0; i < num_bits; i++) {
            int a_qubit = a_start + i;
            int b_qubit = b_start + i;
            int diff_qubit = ancilla_start + 1 + i;
            
            // Compute difference bit
            CNOTGate diff1(a_qubit, diff_qubit);
            diff1.apply(state);
            
            // Subtract b bit with borrow
            ToffoliGate sub1(b_qubit, borrow, diff_qubit);
            sub1.apply(state);
            
            // Update borrow for next bit
            ToffoliGate borrow_update1(a_qubit, b_qubit, borrow);
            borrow_update1.apply(state);
            ToffoliGate borrow_update2(a_qubit, borrow, borrow);
            borrow_update2.apply(state);
        }
        
        // If no borrow at the end, a ≥ b
        CNOTGate result_check(borrow, result_start);
        result_check.apply(state);
        
        // Uncompute to clean ancilla
        // (Reverse the subtraction)
    }
};
```

## 3. Quantum Conditional Subtractor

### 3.1 Mathematical Foundation
If control=1: |a⟩|b⟩ → |a⟩|b-a⟩
If control=0: |a⟩|b⟩ → |a⟩|b⟩

### 3.2 Implementation

```cpp
class QuantumConditionalSubtractor : public QuantumGate {
private:
    int a_start, b_start, control_start;
    int num_bits;
    int ancilla_start;
    
public:
    QuantumConditionalSubtractor(int a_start, int b_start, int control_start,
                                 int num_bits, int ancilla_start)
        : a_start(a_start), b_start(b_start), control_start(control_start),
          num_bits(num_bits), ancilla_start(ancilla_start) {}
    
    void apply(QuantumState& state) override {
        int borrow = ancilla_start;
        
        // Initialize borrow to 0
        state.setAmplitude(borrow, Complex(0, 0));
        
        for (int i = 0; i < num_bits; i++) {
            int a_qubit = a_start + i;
            int b_qubit = b_start + i;
            int diff_qubit = ancilla_start + 1 + i;
            
            // Copy b to diff
            CNOTGate copy(b_qubit, diff_qubit);
            copy.apply(state);
            
            // Conditional subtract: if control=1, subtract a bit
            ToffoliGate sub(control_start, a_qubit, diff_qubit);
            sub.apply(state);
            
            // Handle borrow
            ToffoliGate borrow1(control_start, borrow, diff_qubit);
            borrow1.apply(state);
            
            // Update borrow for next bit
            ToffoliGate borrow_update1(control_start, a_qubit, b_qubit, borrow);
            borrow_update1.apply(state);
            ToffoliGate borrow_update2(control_start, a_qubit, borrow, borrow);
            borrow_update2.apply(state);
        }
        
        // Copy result back to b register
        for (int i = 0; i < num_bits; i++) {
            int b_qubit = b_start + i;
            int diff_qubit = ancilla_start + 1 + i;
            
            CNOTGate copy_back(diff_qubit, b_qubit);
            copy_back.apply(state);
        }
        
        // Clean ancilla (uncompute)
        // ... reverse operations ...
    }
};
```

## 4. Controlled Multiplier

### 4.1 Mathematical Foundation
If control=1: |c⟩|a⟩|b⟩|0⟩ → |c⟩|a⟩|b⟩|a·b⟩
If control=0: unchanged

Algorithm: Repeated addition
- For each bit a_i = 1, add b shifted by i to result

### 4.2 Implementation

```cpp
class ControlledMultiplier : public QuantumGate {
private:
    int control, a_start, b_start, result_start;
    int num_bits;
    int ancilla_start;
    
public:
    ControlledMultiplier(int control, int a_start, int b_start, int result_start,
                       int num_bits, int ancilla_start)
        : control(control), a_start(a_start), b_start(b_start), 
          result_start(result_start), num_bits(num_bits),
          ancilla_start(ancilla_start) {}
    
    void apply(QuantumState& state) override {
        // Initialize result to 0
        for (int i = 0; i < num_bits; i++) {
            state.setAmplitude(result_start + i, Complex(0, 0));
        }
        
        // For each bit of a
        for (int i = 0; i < num_bits; i++) {
            int a_qubit = a_start + i;
            
            // If a_i = 1 and control = 1, add b shifted by i
            for (int j = 0; j < num_bits - i; j++) {
                int b_qubit = b_start + j;
                int result_qubit = result_start + i + j;
                
                // Controlled addition: add b_j to result_{i+j}
                ToffoliGate add(control, a_qubit);
                ToffoliGate add2(control, a_qubit, b_qubit, result_qubit);
                add2.apply(state);
            }
        }
    }
};
```

## 5. Modular Reducer

### 5.1 Mathematical Foundation
Computes: |value⟩|modulus⟩|0⟩ → |value⟩|modulus⟩|value mod modulus⟩

Algorithm: Repeated subtraction
- While value ≥ modulus: subtract modulus
- Use comparison to check condition

### 5.2 Implementation

```cpp
class ModularReducer : public QuantumGate {
private:
    int value_start, modulus_start, result_start;
    int num_bits;
    int ancilla_start;
    
public:
    ModularReducer(int value_start, int modulus_start, int result_start,
                  int num_bits, int ancilla_start)
        : value_start(value_start), modulus_start(modulus_start),
          result_start(result_start), num_bits(num_bits),
          ancilla_start(ancilla_start) {}
    
    void apply(QuantumState& state) override {
        // Copy value to result
        for (int i = 0; i < num_bits; i++) {
            CNOTGate copy(value_start + i, result_start + i);
            copy.apply(state);
        }
        
        // Repeated subtraction
        bool subtracted = true;
        while (subtracted) {
            subtracted = false;
            
            // Compare result with modulus
            QuantumComparator comp(result_start, modulus_start, 
                                   ancilla_start, num_bits, ancilla_start + 1);
            comp.apply(state);
            
            // Check if result ≥ modulus
            double prob_ge = state.getProbability(ancilla_start);
            if (prob_ge > 0.5) {  // Simplified check
                subtracted = true;
                
                // Subtract modulus from result
                QuantumConditionalSubtractor sub(modulus_start, result_start,
                                                ancilla_start, num_bits,
                                                ancilla_start + 1);
                sub.apply(state);
            }
        }
    }
};
```

## 6. Controlled Modular Multiplication

### 6.1 Complete Implementation

```cpp
class ControlledModMultGate : public QuantumGate {
private:
    int control_qubit;
    int target_start, target_count;
    uint64_t multiplier, modulus;
    int ancilla_start;
    
public:
    ControlledModMultGate(int control, int target_start, int target_count,
                         uint64_t multiplier, uint64_t modulus, int ancilla_start)
        : control_qubit(control), target_start(target_start), 
          target_count(target_count), multiplier(multiplier), 
          modulus(modulus), ancilla_start(ancilla_start) {}
    
    void apply(QuantumState& state) override {
        // Step 1: Multiply target by multiplier
        int temp_start = ancilla_start;
        ControlledMultiplier mult(control_qubit, ancilla_start, target_start,
                                  temp_start, target_count, 
                                  temp_start + target_count);
        mult.apply(state);
        
        // Step 2: Apply modulo reduction
        ModularReducer mod_reducer(temp_start, ancilla_start + target_count,
                                   target_start, target_count,
                                   ancilla_start + 2 * target_count);
        mod_reducer.apply(state);
        
        // Step 3: Clean up ancilla
        // (Uncompute multiplication)
        // ... reverse operations ...
    }
};
```

## 7. Optimization Techniques

### 7.1 Carry Lookahead Adder
Replace ripple-carry with faster adders:
- Precompute carry signals
- Reduce circuit depth from O(n) to O(log n)

### 7.2 Booth Multiplication
Optimize multiplication:
- Handle signed multiplication
- Reduce number of additions

### 7.3 Montgomery Reduction
Optimize modulo reduction:
- Avoid division operations
- Use shift and add operations

## 8. Testing Strategy

### 8.1 Unit Test Framework
```cpp
void test_quantum_adder() {
    // Test |3⟩|4⟩|0⟩ → |3⟩|7⟩|carry⟩
    QuantumState state(3 + 3 + 1);
    // Initialize to |3⟩|4⟩|0⟩
    // Apply adder
    // Verify result
}

void test_controlled_multiplier() {
    // Test multiplication with and without control
    QuantumState state(1 + 4 + 4 + 4);
    // Initialize registers
    // Apply multiplier
    // Verify result
}
```

### 8.2 Integration Testing
```cpp
void test_modular_multiplication() {
    // Test various cases:
    // - (7 * 3) mod 15 = 6
    // - (4 * 4) mod 15 = 1
    // - Edge cases
}

void test_modular_exponentiation() {
    // Test complete circuit
    // Compare with classical results
    // Verify unitary property
}
```

## 9. Performance Considerations

### 9.1 Gate Count Analysis
- Adder: O(n) Toffoli + O(n) CNOT
- Multiplier: O(n²) controlled operations
- Modulo: O(n) comparisons + O(n) subtractions
- Total: O(n³) gates for full modular exponentiation

### 9.2 Ancilla Optimization
- Reuse ancilla across operations
- In-place computation where possible
- Uncompute to clean up garbage

### 9.3 Parallelization
- Parallel independent operations
- Pipeline sequential dependencies
- Optimize for quantum hardware constraints

## 10. Debugging Techniques

### 10.1 State Inspection
```cpp
void print_register_state(QuantumState& state, int start, int count) {
    for (int i = 0; i < (1 << count); i++) {
        double prob = state.getProbability(i);
        if (prob > 1e-10) {
            std::cout << "|" << i << ">: " << prob << std::endl;
        }
    }
}
```

### 10.2 Intermediate Verification
- Check normalization after each gate
- Verify ancilla cleanup
- Trace amplitude flow through circuit

### 10.3 Classical Simulation
- Compare with classical computation
- Verify edge cases
- Validate mathematical properties

This implementation guide provides the foundation for building genuine quantum modular exponentiation circuits using only fundamental quantum gates.