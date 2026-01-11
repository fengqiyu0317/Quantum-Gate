# Toffoli Gate AND Operation Demonstration

## Overview

This document explains the demonstration of the Toffoli gate (CCNOT) computing the AND operation and verifying its reversibility.

## The Toffoli Gate

The Toffoli gate, also known as the CCNOT (Controlled-Controlled-NOT) gate, is a 3-qubit quantum gate:
- **Control qubit 1**: q₀
- **Control qubit 2**: q₁
- **Target qubit**: q₂

### Operation
The Toffoli gate flips the target qubit **if and only if both control qubits are in state |1⟩**.

### Truth Table

| Input (q₂,q₁,q₀) | Output (q₂,q₁,q₀) | Target Changed? |
|-----------------|------------------|-----------------|
| |000⟩            | |000⟩            | No              |
| |001⟩            | |001⟩            | No              |
| |010⟩            | |010⟩            | No              |
| |011⟩            | |111⟩            | **Yes**         |
| |100⟩            | |100⟩            | No              |
| |101⟩            | |101⟩            | No              |
| |110⟩            | |110⟩            | No              |
| |111⟩            | |011⟩            | **Yes**         |

## AND Operation

The Toffoli gate can compute the logical AND of two bits:

```
target = control1 AND control2
```

### Setup
1. Initialize 3 qubits to |000⟩
2. Set q₀ to input value **a** (using X gate if a=1)
3. Set q₁ to input value **b** (using X gate if b=1)
4. Keep q₂ at |0⟩ (this will store the result)

### Operation
- Apply Toffoli gate with controls q₀, q₁ and target q₂
- After the gate, q₂ will contain **a AND b**

### Results

| a | b | Initial State | After Toffoli | q₂ (a AND b) |
|---|---|---------------|---------------|--------------|
| 0 | 0 | |000⟩         | |000⟩         | 0            |
| 0 | 1 | |010⟩         | |010⟩         | 0            |
| 1 | 0 | |001⟩         | |001⟩         | 0            |
| 1 | 1 | |011⟩         | |111⟩         | 1            |

**All test cases PASS ✓**

## Reversibility

### Key Property
The Toffoli gate is its own inverse:

```
Toffoli² = I  (Identity)
```

Applying the Toffoli gate twice returns the quantum state to its original configuration.

### Why This Matters

In classical computing, the AND gate is **not reversible**:
- Input: (1, 1) → Output: 1
- Input: (1, 0) → Output: 1
- Given output 1, you cannot determine which input produced it

In quantum computing, the Toffoli gate **preserves all information**:
- Input: |110⟩ → Output: |111⟩
- Input: |111⟩ → Output: |110⟩
- The mapping is one-to-one and reversible

### Demonstration

For each test case, the program:
1. Prepares initial state |a,b,0⟩
2. Applies Toffoli gate → computes AND
3. Applies Toffoli gate again → reverses to initial state
4. Verifies the state matches the original

### Reversibility Results

Looking at the actual quantum states:

| Initial State | After 1st Toffoli | After 2nd Toffoli | Reversible? |
|---------------|-------------------|-------------------|-------------|
| |000⟩ (0,0)    | |000⟩             | |000⟩             | ✓ Yes        |
| |010⟩ (0,1)    | |010⟩             | |010⟩             | ✓ Yes        |
| |001⟩ (1,0)    | |001⟩             | |001⟩             | ✓ Yes        |
| |011⟩ (1,1)    | |111⟩             | |011⟩             | ✓ Yes        |

**All test cases demonstrate reversibility ✓**

## Matrix Representation

The Toffoli gate is represented by an 8×8 unitary matrix:

```
|000⟩ │100⟩ │010⟩ │110⟩ │001⟩ │101⟩ │011⟩ │111⟩
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
|000⟩ │100⟩ │010⟩ │110⟩ │001⟩ │101⟩ │111⟩ │011⟩
```

Note: Only |110⟩ ↔ |111⟩ are swapped (when both controls are 1).

## Quantum vs Classical AND

### Classical AND Gate
```
Inputs: a, b
Output: a AND b

Properties:
- Irreversible (loses information)
- Not universal for computation alone
- Requires memory (output overwrites inputs)
```

### Quantum AND (Toffoli)
```
Inputs: |a⟩|b⟩|0⟩
Output: |a⟩|b⟩|a AND b⟩

Properties:
- Reversible (preserves all information)
- Universal for quantum computation
- No information loss
- Control qubits preserved
```

## Usage

### Compile
```bash
g++ -std=c++17 -O3 -o test_toffoli_and test_toffoli_and.cpp quantum_state.cpp quantum_gates.cpp
```

### Run
```bash
./test_toffoli_and
```

### Output
The program displays:
1. Initial quantum state preparation
2. State after applying Toffoli gate
3. Measurement results showing AND computation
4. Reversibility verification (applying Toffoli twice)
5. Summary with truth table and matrix representation

## Applications

The Toffoli gate is fundamental because it:
1. **Computes AND**: Essential for arithmetic operations
2. **Universal**: Can construct any reversible circuit
3. **Preserves Information**: Critical for quantum algorithms
4. **Building Block**: Used in:
   - Shor's factoring algorithm
   - Quantum arithmetic circuits
   - Error correction codes
   - Quantum adders and multipliers

## Conclusion

The Toffoli gate successfully demonstrates:
- ✓ Correct computation of AND operation
- ✓ Complete reversibility (no information loss)
- ✓ Preservation of control qubit values
- ✓ Foundation for complex quantum circuits

This makes it an essential component in quantum computing, enabling reversible computation that classical gates cannot achieve.
