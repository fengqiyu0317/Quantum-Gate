# Real Modular Exponentiation Design Document

## Executive Summary

This document outlines the design for implementing genuine quantum modular exponentiation, a critical component of Shor's algorithm. The current implementation uses a simplified approach with a lookup table for modular multiplication. This design proposes a proper quantum circuit decomposition using only fundamental quantum gates (Hadamard, CNOT, Toffoli, and Phase gates).

## 1. Current Implementation Analysis

### 1.1 Strengths
- Well-structured quantum state representation
- Comprehensive basic quantum gates (H, CNOT, Toffoli, SWAP, Phase)
- Proper file input handling
- Good testing framework
- Correct mathematical foundation

### 1.2 Limitations
- **ControlledModMultGate** uses a classical lookup table approach rather than quantum circuit decomposition
- Not truly quantum - performs classical computation within the gate
- Doesn't demonstrate the actual quantum circuit complexity
- Limited educational value for understanding quantum algorithms

### 1.3 What Needs Improvement
1. Decompose modular multiplication into reversible quantum operations
2. Implement quantum arithmetic circuits (adders, multipliers)
3. Ensure all operations are unitary and reversible
4. Maintain proper quantum entanglement throughout the computation

## 2. Mathematical Foundation

### 2.1 Problem Statement
Implement the unitary operation:
$$U_a|x\rangle|y\rangle = |x\rangle|a^x \cdot y \mod N\rangle$$

For modular exponentiation, we initialize y=1:
$$U_a|x\rangle|1\rangle = |x\rangle|a^x \mod N\rangle$$

### 2.2 Binary Exponentiation
$$a^x = a^{\sum_{i=0}^{n-1} x_i 2^i} = \prod_{i=0}^{n-1} (a^{2^i})^{x_i}$$

Where $x_i$ is the i-th bit of exponent x.

### 2.3 Quantum Circuit Structure
```
|x⟩ ──H──■────■────■────■───
         │    │    │    │
|1⟩ ─────U────U²───U⁴───U⁸───
```

## 3. Quantum Circuit Design

### 3.1 High-Level Architecture

```
┌─────────────────────────────────────────────────────────┐
│  Modular Exponentiation Circuit: f(x) = a^x mod N     │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  Input: |0⟩^n ⊗ |1⟩^m                                  │
│  Output: |x⟩ ⊗ |a^x mod N⟩                             │
│                                                         │
│  ┌───┐       ┌─────────────────────────────────────┐   │
│  │ H │       │  Controlled Modular Multiplication │   │
│  │   │       │  (U, U², U⁴, ..., U^(2^(n-1)))     │   │
│  └───┘       └─────────────────────────────────────┘   │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### 3.2 Core Components

#### 3.2.1 Quantum Adder
- **Cuccaro Adder**: Ripple-carry adder using only CNOT and Toffoli gates
- **Input**: |a⟩|b⟩|0⟩ → |a⟩|a+b⟩|carry⟩
- **Ancilla**: O(n) qubits for carry bits

#### 3.2.2 Quantum Multiplier
- **Repeated Addition**: Multiply by repeated addition
- **Controlled Addition**: Add only when control bit is 1
- **Ancilla**: O(n²) qubits for intermediate results

#### 3.2.3 Quantum Modulo Reduction
- **Comparison**: Compare result with modulus
- **Conditional Subtraction**: Subtract N if result ≥ N
- **Reversible**: All operations must be reversible

### 3.3 Detailed Circuit for U (Controlled Modular Multiplication)

```
Control: ──■───────────────────────────────────────
           │
Target: ────┼───[Quantum Multiplier]───[Modulo Reducer]───
           │
Ancilla: ───┼───[Computation Workspace]──────────────────
           │
Result: ────┘
```

## 4. Implementation Strategy

### 4.1 Phase 1: Quantum Arithmetic Primitives

#### 4.1.1 Quantum Ripple-Carry Adder
```cpp
class QuantumAdder : public QuantumGate {
private:
    int a_start, b_start, carry_start;
    int num_bits;
    
public:
    // Computes: |a⟩|b⟩|0⟩ → |a⟩|a+b⟩|carry⟩
    void apply(QuantumState& state) override;
};
```

**Implementation using Toffoli gates:**
- For each bit position i:
  - Compute sum bit: `sum_i = a_i XOR b_i XOR carry_in`
  - Compute carry out: `carry_out = MAJ(a_i, b_i, carry_in)`
  - Use Toffoli for MAJ (majority) function
  - Use CNOT for XOR operations

#### 4.1.2 Quantum Comparator
```cpp
class QuantumComparator : public QuantumGate {
private:
    int a_start, b_start, result_start;
    int num_bits;
    
public:
    // Computes: |a⟩|b⟩|0⟩ → |a⟩|b⟩|a≥b⟩
    void apply(QuantumState& state) override;
};
```

#### 4.1.3 Quantum Conditional Subtractor
```cpp
class QuantumConditionalSubtractor : public QuantumGate {
private:
    int a_start, b_start, control_start;
    int num_bits;
    
public:
    // If control=1: |a⟩|b⟩ → |a⟩|b-a⟩
    // If control=0: |a⟩|b⟩ → |a⟩|b⟩
    void apply(QuantumState& state) override;
};
```

### 4.2 Phase 2: Modular Multiplication Circuit

#### 4.2.1 Controlled Multiplier
```cpp
class ControlledMultiplier : public QuantumGate {
private:
    int control, a_start, b_start, result_start;
    int num_bits;
    
public:
    // If control=1: |c⟩|a⟩|b⟩|0⟩ → |c⟩|a⟩|b⟩|a·b⟩
    // If control=0: unchanged
    void apply(QuantumState& state) override;
};
```

**Algorithm:**
1. Initialize result to 0
2. For each bit i of multiplier a:
   - If a_i = 1: add b shifted by i to result
   - Use controlled addition for each bit

#### 4.2.2 Modular Reduction
```cpp
class ModularReducer : public QuantumGate {
private:
    int value_start, modulus_start, result_start;
    int num_bits;
    
public:
    // |value⟩|modulus⟩|0⟩ → |value⟩|modulus⟩|value mod modulus⟩
    void apply(QuantumState& state) override;
};
```

**Algorithm:**
1. Compare value with modulus
2. If value ≥ modulus: subtract modulus
3. Repeat until value < modulus
4. Use repeated subtraction or optimized division

### 4.3 Phase 3: Full Modular Exponentiation

```cpp
class ModularExponentiation : public QuantumGate {
private:
    int control_start, control_count;
    int target_start, target_count;
    uint64_t base, modulus;
    
public:
    // |x⟩|1⟩ → |x⟩|base^x mod modulus⟩
    void apply(QuantumState& state) override;
};
```

## 5. Ancilla Management Strategy

### 5.1 Qubit Allocation
```
Register Layout:
┌─────────────────────────────────────────────────────────┐
│ Control Register (n qubits)                             │
│   Qubits 0 to n-1                                       │
├─────────────────────────────────────────────────────────┤
│ Target Register (m qubits)                              │
│   Qubits n to n+m-1                                     │
├─────────────────────────────────────────────────────────┤
│ Ancilla Register (k qubits)                             │
│   Qubits n+m to n+m+k-1                                 │
│   - Carry bits for addition                             │
│   - Intermediate multiplication results                 │
│   - Comparison results                                  │
└─────────────────────────────────────────────────────────┘
```

### 5.2 Ancilla Cleanup
- All ancilla qubits must be returned to |0⟩ state
- Use uncomputation: reverse computation after use
- Ensures no garbage accumulation

## 6. Optimization Strategies

### 6.1 Circuit Depth Optimization
- **Parallel Addition**: Add multiple bits simultaneously where possible
- **Carry Lookahead**: Replace ripple-carry with faster adders
- **Windowed Multiplication**: Process multiple bits at once

### 6.2 Qubit Count Optimization
- **In-place Computation**: Reuse ancilla qubits
- **Trade-off**: More gates vs fewer qubits
- **Adaptive Allocation**: Dynamic ancilla based on input size

### 6.3 Specialized Optimizations
- **Constant Multiplier**: Optimize for known base
- **Power of 2 Modulus**: Simplified reduction
- **Small Modulus**: Precomputed lookup tables

## 7. Verification and Testing

### 7.1 Unit Tests
```cpp
// Test each component independently
void test_quantum_adder();
void test_quantum_multiplier();
void test_modular_reducer();
void test_controlled_mod_mult();
```

### 7.2 Integration Tests
```cpp
// Test full modular exponentiation
void test_modular_exponentiation_small();
void test_modular_exponentiation_medium();
```

### 7.3 Test Cases
- **Small moduli**: N = 3, 5, 7, 15
- **Various bases**: Coprime and non-coprime with N
- **Edge cases**: N=2, a=1, a=N-1
- **Random testing**: Monte Carlo verification

### 7.4 Verification Strategy
1. **Classical Comparison**: Compare with classical computation
2. **Unitary Verification**: Ensure U†U = I
3. **Reversibility Check**: Verify ancilla cleanup
4. **Probability Distribution**: Check measurement statistics

## 8. Performance Analysis

### 8.1 Complexity Metrics
| Component | Qubits | Gate Count | Circuit Depth |
|-----------|--------|------------|---------------|
| Adder | O(n) | O(n) | O(n) |
| Multiplier | O(n²) | O(n²) | O(n) |
| Modulo | O(n) | O(n²) | O(n) |
| Full ModExp | O(n²) | O(n³) | O(n²) |

### 8.2 Resource Requirements
- **For N < 1024** (10 bits):
  - Control: 10 qubits
  - Target: 10 qubits
  - Ancilla: ~100-200 qubits
  - Total: ~120-220 qubits

### 8.3 Scalability
- **Current limit**: 10 qubits per register (due to memory)
- **Expansion path**: Sparse state representation
- **Optimization**: Reduce ancilla for practical sizes

## 9. Implementation Roadmap

### Phase 1: Foundation (Week 1-2)
- [ ] Implement quantum adder
- [ ] Implement quantum comparator
- [ ] Test and verify basic arithmetic

### Phase 2: Multiplication (Week 3-4)
- [ ] Implement controlled multiplier
- [ ] Implement modular reducer
- [ ] Test modular multiplication

### Phase 3: Integration (Week 5-6)
- [ ] Assemble full modular exponentiation
- [ ] Optimize ancilla management
- [ ] Comprehensive testing

### Phase 4: Optimization (Week 7-8)
- [ ] Circuit depth optimization
- [ ] Qubit count reduction
- [ ] Performance benchmarking

## 10. Risk Assessment

### 10.1 Technical Risks
- **Ancilla explosion**: Too many ancilla qubits required
- **Circuit depth**: Too deep for practical simulation
- **Numerical precision**: Floating-point errors accumulation

### 10.2 Mitigation Strategies
- **Modular design**: Implement and test components separately
- **Incremental complexity**: Start with small moduli
- **Precision monitoring**: Track normalization throughout

### 10.3 Alternative Approaches
- **Simplified circuits**: For educational purposes
- **Hybrid approach**: Classical precomputation where possible
- **Approximate methods**: For large-scale simulation

## 11. Conclusion

This design provides a roadmap for implementing genuine quantum modular exponentiation using only fundamental quantum gates. The approach emphasizes:

1. **Correctness**: All operations are unitary and reversible
2. **Educational Value**: Clear decomposition into quantum primitives
3. **Scalability**: Designed to work with increasing problem sizes
4. **Verifiability**: Comprehensive testing and validation

The implementation will demonstrate the true complexity of quantum algorithms while maintaining clarity and educational value. This approach provides a solid foundation for understanding Shor's algorithm and other quantum algorithms requiring modular exponentiation.

## References

1. Nielsen, M. A., & Chuang, I. L. (2010). *Quantum Computation and Quantum Information*
2. Cuccaro, S. A., et al. (2004). *A New Quantum Ripple-Carry Addition Circuit*
3. Vedral, V., Barenco, A., & Ekert, A. (1996). *Quantum Networks for Elementary Arithmetic Operations*
4. Shor, P. W. (1994). *Algorithms for Quantum Computation: Discrete Log and Factoring*
5. Beauregard, S. (2002). *Circuit for Shor's Algorithm Using 2n+3 Qubits*