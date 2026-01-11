# Modular Exponentiation Implementation Guide

## Overview

This guide explains how to implement modular exponentiation `f(x) = a^x mod N` using the quantum gates we've built.

## Table of Contents
1. [Algorithm Structure](#algorithm-structure)
2. [Gate Decomposition Strategy](#gate-decomposition-strategy)
3. [Implementation Steps](#implementation-steps)
4. [Example Walkthrough](#example-walkthrough)

---

## Algorithm Structure

### High-Level Circuit Design

```
Input: |0⟩^n ⊗ |1⟩ (control register in |0⟩, target initialized to |1⟩)
Output: |x⟩ ⊗ |a^x mod N⟩

Step 1: Apply Hadamard gates to control register
  ┌─────────┐
┌─┤ H^×n   ├─┐
│ └─────────┘ │
│             │
│  Control    │  Target
│  Register   │  Register
│  (n qubits) │  (m qubits)
└─────────────┴────────────

Step 2: Apply controlled modular multiplications
For each qubit i in control register:
  if control[i] = |1⟩:
      apply U^(2^i) where U performs y → (a·y) mod N

Step 3: Measure target register (optional for verification)
```

---

## Gate Decomposition Strategy

### Key Insight: Modular Multiplication as Reversible Circuits

The core challenge is implementing the **Controlled-U** gate where:
```
U|y⟩ = |(a·y) mod N⟩
```

This requires decomposing modular multiplication into:
1. **Basic arithmetic operations** (addition, multiplication)
2. **Reversible logic** (using CNOT and Toffoli)
3. **Modular reduction** (reversible modulo operation)

---

## Implementation Steps

### Phase 1: Understand the Mathematical Structure

Given we want to compute `a^x mod N`:

**Binary exponentiation method:**
```
a^x = a^(Σᵢ xᵢ·2ⁱ) = Πᵢ (a^(2ⁱ))^xᵢ
```

For each bit `xᵢ` of the exponent `x`:
- If `xᵢ = 1`: multiply by `a^(2ⁱ) mod N`
- If `xᵢ = 0`: do nothing

**Example:** To compute `7^5 mod 15`:
```
5 = 101₂ (binary)
7^5 = 7^(4+1) = 7^4 · 7^1
     = 7^(2²) · 7^(2⁰)
```

### Phase 2: Implement Classical Modular Multiplication (for reference)

First, implement the classical version to understand the algorithm:

```cpp
// Classical modular multiplication
uint64_t classical_mod_mult(uint64_t a, uint64_t y, uint64_t N) {
    return (a * y) % N;
}

// Classical modular exponentiation
uint64_t classical_mod_exp(uint64_t a, uint64_t x, uint64_t N) {
    uint64_t result = 1;
    uint64_t base = a % N;

    while (x > 0) {
        if (x & 1) {
            result = (result * base) % N;
        }
        x >>= 1;
        base = (base * base) % N;
    }

    return result;
}
```

### Phase 3: Build Quantum Circuit Components

#### Step 3.1: Create Quantum Registers

```cpp
// Control register: holds the exponent in superposition
// Target register: holds the result of modular exponentiation

// Example: for N=15, we need 4 qubits for target (since 2^4 = 16 > 15)
int control_qubits = 4;  // Can represent exponents 0-15
int target_qubits = 4;   // Can represent values 0-15

// Total system
int total_qubits = control_qubits + target_qubits;
QuantumState state(total_qubits);

// Initialize target register to |1⟩
// (since a^0 = 1 for any a)
```

#### Step 3.2: Initialize Superposition

```cpp
// Apply Hadamard to each control qubit
for (int i = 0; i < control_qubits; i++) {
    HadamardGate H(i);
    H.apply(state);
}

// Now control register is in: (|0⟩ + |1⟩)/√2 ⊗ ... ⊗ (|0⟩ + |1⟩)/√2
// This creates superposition of all possible exponents 0 to 2^n - 1
```

#### Step 3.3: Precompute Powers of `a`

```cpp
// Precompute: a^(2⁰) mod N, a^(2¹) mod N, a^(2²) mod N, ...
std::vector<uint64_t> powers;
uint64_t current_power = a % N;

for (int i = 0; i < control_qubits; i++) {
    powers.push_back(current_power);
    current_power = (current_power * current_power) % N;  // Square for next power
}

// Example for a=7, N=15, control_qubits=4:
// powers[0] = 7^1 mod 15 = 7
// powers[1] = 7^2 mod 15 = 49 mod 15 = 4
// powers[2] = 7^4 mod 15 = 16 mod 15 = 1
// powers[3] = 7^8 mod 15 = 1 mod 15 = 1
```

#### Step 3.4: Implement Controlled Modular Multiplication

For each control qubit `i`, implement a **Controlled-U^(2ⁱ)** gate where `U` performs multiplication by `powers[i]`:

**Challenge:** How to implement `U|y⟩ = |(k·y) mod N⟩` using basic gates?

**Approach 1: Lookup Table (for small N)**

For small moduli (N ≤ 1024), we can implement each U^(2ⁱ) as a permutation:

```cpp
void apply_controlled_mod_mult(QuantumState& state,
                                int control_qubit,
                                int target_qubits_start,
                                uint64_t multiplier,
                                uint64_t N) {
    // This is a simplified conceptual implementation
    // In practice, you'd decompose this into basic gates

    int state_size = state.getStateSize();
    const auto& old_amps = state.getAmplitudes();
    std::vector<Complex> new_amps = old_amps;

    int control_mask = 1 << control_qubit;

    for (int i = 0; i < state_size; i++) {
        // Only act if control qubit is 1
        if ((i & control_mask) != 0) {
            // Extract target register value
            uint64_t y = extract_bits(i, target_qubits_start, target_qubits);

            // Apply modular multiplication
            uint64_t new_y = (multiplier * y) % N;

            // Find the new basis state index
            int j = replace_bits(i, target_qubits_start, target_qubits, new_y);

            // Move amplitude
            new_amps[j] = old_amps[i];
        }
    }

    // Update state
    for (int i = 0; i < state_size; i++) {
        state.setAmplitude(i, new_amps[i]);
    }
}
```

**Approach 2: Decompose into Arithmetic Gates**

For a proper quantum implementation using only H, CNOT, Toffoli, and Phase gates:

1. **Implement Quantum Adder**
   - Use CNOT and Toffoli gates for ripple-carry addition
   - Reference: Cuccaro adder, VBE adder, or Draper adder

2. **Implement Quantum Multiplier**
   - Decompose multiplication into repeated addition
   - Use controlled additions

3. **Implement Quantum Modulo Reduction**
   - Use comparison and conditional subtraction
   - All operations must be reversible

This is complex and requires many ancilla qubits.

### Phase 4: Assemble the Full Circuit

```cpp
void modular_exponentiation(QuantumState& state,
                            int control_qubits,
                            int target_qubits_start,
                            uint64_t a,
                            uint64_t N) {
    // Step 1: Apply Hadamard gates to control register
    for (int i = 0; i < control_qubits; i++) {
        HadamardGate H(i);
        H.apply(state);
    }

    // Step 2: Precompute powers of a
    std::vector<uint64_t> powers;
    uint64_t current_power = a % N;
    for (int i = 0; i < control_qubits; i++) {
        powers.push_back(current_power);
        current_power = (current_power * current_power) % N;
    }

    // Step 3: Apply controlled modular multiplications
    for (int i = 0; i < control_qubits; i++) {
        apply_controlled_mod_mult(state, i, target_qubits_start, powers[i], N);
    }
}
```

---

## Example Walkthrough

### Example: Compute 7^x mod 15 with 4 control qubits

**Setup:**
- Control qubits: 4 (qubits 0, 1, 2, 3)
- Target qubits: 4 (qubits 4, 5, 6, 7)
- a = 7, N = 15

**Initial state:** `|0000⟩ ⊗ |0001⟩`
- Control register: |0000⟩ (exponent = 0)
- Target register: |0001⟩ (initialized to 1, since a^0 = 1)

**Step 1: Apply Hadamard to control qubits**
State becomes:
```
1/4 · Σₓ₌₀¹⁵ |x⟩ ⊗ |1⟩
```
This is a superposition of all 16 possible exponents.

**Step 2: Precompute powers**
- powers[0] = 7^1 mod 15 = 7
- powers[1] = 7^2 mod 15 = 4
- powers[2] = 7^4 mod 15 = 1
- powers[3] = 7^8 mod 15 = 1

**Step 3: Apply controlled multiplications**

For each control qubit i, apply U^(2ⁱ) where U multiplies by powers[i]:

```
After all controlled-U gates:
State = 1/4 · Σₓ₌₀¹⁵ |x⟩ ⊗ |7^x mod 15⟩
```

**Verification:**
- |0000⟩ ⊗ |0001⟩ (7^0 = 1)
- |0001⟩ ⊗ |0111⟩ (7^1 = 7)
- |0010⟩ ⊗ |0100⟩ (7^2 = 4)
- |0011⟩ ⊗ |0011⟩ (7^3 = 13, which is 3 mod 15 but needs 4 bits)
- etc.

---

## Implementation Complexity Analysis

### Space Complexity
- **Control register:** n qubits (for exponent)
- **Target register:** ⌈log₂(N+1)⌉ qubits (for modular values)
- **Ancilla qubits:** O(n) for intermediate computations
- **Total:** O(n) to O(n log N) qubits

### Time Complexity (Circuit Depth)
- **Hadamard gates:** O(n)
- **Each controlled-U:** O(poly(log N)) gates
- **Total controlled-U gates:** O(n)
- **Total circuit depth:** O(n · poly(log N))

---

## Next Steps for Implementation

### Immediate Goals:
1. ✓ Implement basic quantum gates (H, CNOT, Toffoli, SWAP, Phase)
2. ✓ Test gate implementations
3. → **Implement file parser for input configuration**
4. → **Implement Controlled-U using lookup table approach (simpler)**
5. → **Add measurement and verification**
6. → **Build complete modular exponentiation circuit**

### Future Optimizations:
1. Decompose Controlled-U into basic gates (H, CNOT, Toffoli, Phase)
2. Implement quantum adders and multipliers
3. Optimize circuit depth and qubit count
4. Add support for larger moduli

---

## Testing Strategy

### Unit Tests:
- Test each controlled-U gate independently
- Verify against classical modular multiplication

### Integration Tests:
- Test full circuit with small moduli (N = 3, 5, 7, 15)
- Verify measurement probabilities

### Example Test Case:
```cpp
// Test: 7^x mod 15
a = 7, N = 15, n = 4

// Expected results:
x=0: 7^0 mod 15 = 1
x=1: 7^1 mod 15 = 7
x=2: 7^2 mod 15 = 4
x=3: 7^3 mod 15 = 13
x=4: 7^4 mod 15 = 1
x=5: 7^5 mod 15 = 7
...
```

---

## References

1. **Shor's Algorithm** - Peter Shor (1994)
2. **Quantum Computation and Quantum Information** - Nielsen & Chuang
3. **A New Quantum Ripple-Carry Addition Circuit** - Cuccaro et al.
4. **Quantum Networks for Elementary Arithmetic Operations** - Vedral et al.
