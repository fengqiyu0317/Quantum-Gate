# Modular Exponentiation with Quantum Gates

## Overview
This project aims to implement modular exponentiation using basic quantum gates. Modular exponentiation is a fundamental component of Shor's algorithm for integer factorization.

## Mathematical Foundation
The computation we want to perform is:
$$f(x) = a^x \mod N$$

Where:
- $a$ is the base
- $x$ is the exponent (input in quantum superposition)
- $N$ is the modulus

## Quantum Gates Required

### 1. **Hadamard Gate (H)**
- **Purpose**: Create superposition of all possible exponents
- **Application**: Applied to each qubit in the input register to create uniform superposition
- **Formula**: $H|0\rangle = \frac{1}{\sqrt{2}}(|0\rangle + |1\rangle)$

### 2. **CNOT Gate (Controlled-NOT)**
- **Purpose**: Entanglement and basic conditional operations
- **Application**: Used in building controlled modular multiplication circuits
- **Operation**: Flips target qubit if control qubit is $|1\rangle$

### 3. **Toffoli Gate (CCNOT)**
- **Purpose**: Universal reversible logic for controlled-controlled operations
- **Application**: Essential for implementing classical logic (AND, OR) reversibly
- **Operation**: Flips target qubit if both control qubits are $|1\rangle$

### 4. **Controlled-U Gate**
- **Purpose**: Apply modular multiplication conditioned on control qubits
- **Application**: Core of modular exponentiation - performs $U|y\rangle = |a \cdot y \mod N\rangle$
- **Construction**: Built from CNOT and Toffoli gates for specific implementations

### 5. **SWAP Gate**
- **Purpose**: Exchange quantum states between qubits
- **Application**: Used for reordering qubits during computation

### 6. **Phase Gate (S, T)**
- **Purpose**: Add phase factors to quantum states
- **Application**: Required for constructing arbitrary controlled operations
- **S Gate**: $\sqrt{Z}$ gate (phase shift of $\pi/2$)
- **T Gate**: $\sqrt[4]{Z}$ gate (phase shift of $\pi/4$)

## Algorithm Structure

### Phase 1: Input Preparation
1. Initialize two quantum registers:
   - Control register (n qubits for exponent)
   - Target register (m qubits for modular values)
2. Apply Hadamard gates to control register for superposition

### Phase 2: Modular Exponentiation Circuit
For each qubit $i$ in the control register:
- If qubit $i$ is $|1\rangle$, apply $U^{2^i}$ where $U$ performs $a^{2^i} \mod N$
- This requires decomposing modular multiplication into basic quantum gates

### Phase 3: Modular Multiplication Implementation
The core challenge is implementing $U|y\rangle = |a \cdot y \mod N\rangle$ using:
- Reversible adders (using CNOT and Toffoli)
- Controlled modular addition
- Repeated addition-based multiplication

## Key Challenges
1. **Reversibility**: All operations must be reversible (unitary)
2. **Decomposition**: Complex operations must break down into H, CNOT, and Toffoli gates
3. **Ancilla Qubits**: Additional qubits needed for intermediate computations
4. **Circuit Depth**: Balancing between qubit count and gate complexity

## Project Specifications

### Input Format
- **File-based input**: Configuration file containing:
  - Base `a`: Integer
  - Modulus `N`: Integer (must be < 1024)
  - Number of qubits `n`: Integer (must be ≤ 10)

Example input file:
```
# Modular Exponentiation Input Configuration

# Base for modular exponentiation
base: 7

# Modulus N (must be < 1024)
modulus: 15

# Number of qubits for exponent register (must be ≤ 10)
qubits: 4
```

### Implementation Details
- **Language**: C++
- **Constants**:
  - `MAX_QUBITS = 10`: Maximum number of qubits for exponent register (easily modifiable)
  - `MAX_MODULUS = 1024`: Maximum modulus value (2^10)
- **Scale**: Support for up to 10 qubits (1024 states, ~16 KB memory)
- **Simulation**: Classical state-vector simulation of quantum computation

### Input Validation
The program performs the following validations:

1. **Qubit limit check**:
   - If `qubits > MAX_QUBITS`: Display warning with memory calculation
   - Memory formula: `2^n × 16 bytes` (complex<double>)
   - Example for 15 qubits: 2^15 × 16 = 524,288 bytes ≈ 512 KB

2. **Modulus limit check**:
   - If `modulus >= MAX_MODULUS`: Display error and exit
   - Ensures modulus fits within 10-bit representation

3. **Positive integer check**:
   - Base and modulus must be positive integers
   - Number of qubits must be positive

4. **File format check**:
   - Validates file can be opened and parsed
   - Reports specific line/field if parsing fails

### Output Format
The program will:
1. Execute the modular exponentiation quantum circuit
2. Verify results against classical computation
3. Output whether the quantum computation is correct
4. Provide probability distribution of measurement outcomes

## Implementation Plan

### Phase 1: Basic Infrastructure
1. Create file parser for input configuration
2. Implement basic quantum state representation
3. Implement quantum gate primitives (H, CNOT, Toffoli, etc.)

### Phase 2: Core Quantum Components
1. Build quantum register class
2. Implement gate application to quantum states
3. Create measurement functionality

### Phase 3: Modular Exponentiation Circuit
1. Implement controlled modular multiplication
2. Build the exponentiation circuit structure
3. Optimize for memory and performance

### Phase 4: Verification
1. Implement classical modular exponentiation for comparison
2. Add result verification logic
3. Generate output with correctness confirmation

## Next Steps
1. Implement basic quantum gate primitives in C++
2. Create file input parser
3. Construct controlled modular multiplier
4. Build the exponentiation circuit
5. Add verification and output functionality
