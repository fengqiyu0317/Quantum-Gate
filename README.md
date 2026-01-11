# Quantum Gate Implementation for Modular Exponentiation

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Quantum Computing](https://img.shields.io/badge/Quantum-Computing-purple.svg)](https://en.wikipedia.org/wiki/Quantum_computing)

A comprehensive C++ implementation of quantum gates and modular exponentiation algorithms, forming a critical component of Shor's algorithm for integer factorization. This project provides a classical simulator of quantum circuits with a focus on modular arithmetic operations.

## 📋 Table of Contents

- [Features](#features)
- [Mathematical Background](#mathematical-background)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Algorithm Overview](#algorithm-overview)
- [Examples](#examples)
- [Testing](#testing)
- [Documentation](#documentation)
- [Performance Considerations](#performance-considerations)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## ✨ Features

- **Complete Quantum Gate Library**: Implementation of fundamental quantum gates (Hadamard, CNOT, Toffoli, Pauli gates, Phase gates)
- **Quantum Arithmetic Operations**: Reversible adders, subtractors, comparators, and multipliers
- **Modular Exponentiation**: Full implementation of quantum modular exponentiation circuit
- **Classical Simulation**: State-vector simulation of quantum circuits using complex numbers
- **Input Validation**: Comprehensive validation for parameters and file inputs
- **Verification**: Automatic verification against classical computation
- **Flexible Input**: File-based configuration with command-line override support
- **Test Suite**: Extensive test programs for individual components
 **Educational Demos**: Interactive demonstrations including Toffoli gate AND operation with reversibility verification

## 🧮 Mathematical Background

### Modular Exponentiation

The core computation performed is:

$$f(x) = a^x \mod N$$

Where:
- $a$ is the base
- $x$ is the exponent (encoded in quantum superposition)
- $N$ is the modulus

This operation is fundamental to **Shor's Algorithm**, which can factor large integers exponentially faster than the best-known classical algorithms, posing a threat to RSA encryption.

### Quantum Circuit Design

The implementation uses:
- **Superposition**: Hadamard gates create uniform superposition of all possible exponents
- **Entanglement**: Controlled operations create quantum correlations
- **Interference**: Quantum gates manipulate probability amplitudes
- **Measurement**: Final measurement yields computational results with probabilistic outcomes

## 📦 Requirements

### Prerequisites

- **C++ Compiler**: GCC 7.0+ or Clang 5.0+ with C++17 support
- **CMake** (optional, for build management): Version 3.10+
- **Git** (for cloning): Version 2.0+

### System Requirements

- **RAM**: Minimum 16 MB (for 10 qubits), scales as $O(2^n)$
- **Storage**: ~5 MB for source code and binaries
- **OS**: Linux, macOS, or Windows (with WSL or MinGW)

### Supported Compilers

```bash
# Check your C++ version
g++ --version  # GCC 7.0+ recommended
clang++ --version  # Clang 5.0+ recommended
```

## 🚀 Installation

### Quick Start

```bash
# Clone the repository
git clone https://github.com/fengqiyu0317/Quantum-Gate.git
cd Quantum-Gate/modular_exponentiation

# Compile the main program
g++ -std=c++17 -O3 -o main main.cpp quantum_state.cpp quantum_gates.cpp

# Compile test programs (optional)
g++ -std=c++17 -O3 -o test_gates test_gates.cpp quantum_state.cpp quantum_gates.cpp
g++ -std=c++17 -O3 -o test_quantum_adder test_quantum_adder.cpp quantum_state.cpp quantum_gates.cpp
g++ -std=c++17 -O3 -o test_quantum_comparator test_quantum_comparator.cpp quantum_state.cpp quantum_gates.cpp
g++ -std=c++17 -O3 -o test_toffoli_and test_toffoli_and.cpp quantum_state.cpp quantum_gates.cpp
```

### Build with Optimization

For better performance with larger quantum circuits:

```bash
# Full optimization with native architecture support
g++ -std=c++17 -O3 -march=native -o main main.cpp quantum_state.cpp quantum_gates.cpp
```

### Compilation Flags

| Flag | Purpose |
|------|---------|
| `-std=c++17` | Use C++17 standard |
| `-O3` | Maximum optimization |
| `-march=native` | Optimize for your CPU architecture |
| `-g` | Include debug symbols (for development) |
| `-Wall -Wextra` | Enable all warnings (recommended) |

## 🎯 Usage

### Basic Usage

```bash
# Run with default input file (input.txt)
./main

# Run with custom input file
./main custom_input.txt
```

### Input File Format

Create a text file with three space-separated values:

```txt
# Format: base modulus num_qubits
7 15 4
```

**Parameters:**
- `base`: Integer base for modular exponentiation (a)
- `modulus`: Modulus N, must be < 1024
- `num_qubits`: Number of qubits for exponent register, ≤ 10

**Example Input Files:**

```txt
# Simple example
input.txt:
7 15 4

# Larger example
large_input.txt:
2 997 10
```

### Output Format

The program provides:

1. **Configuration Summary**: Display of input parameters
2. **Computation Results**: Quantum circuit execution results
3. **Verification**: Comparison with classical computation
4. **Probability Distribution**: Measurement outcome probabilities

**Example Output:**

```
Quantum Modular Exponentiation Simulator
==========================================
Base: 7
Modulus: 15
Number of qubits: 4
gcd(7, 15) = 1

Running quantum circuit...
Measurement result: 13
Classical verification: 7^13 mod 15 = 7
✓ Quantum computation CORRECT

Top measurement probabilities:
|13⟩: 12.5%
|7⟩: 12.5%
|1⟩: 12.5%
...
```

## 📁 Project Structure

```
modular_exponentiation/
├── README.md                          # This file
├── IMPLEMENTATION_GUIDE.md            # Detailed implementation guide
├── MODULAR_EXPONENTIATION_DESIGN.md   # Algorithm design document
├── QUANTUM_ARITHMETIC_IMPLEMENTATION.md # Arithmetic operations details
├── draft.tex                          # LaTeX documentation draft
│
├── main.cpp                           # Main program entry point
├── quantum_state.h                    # Quantum state representation
├── quantum_state.cpp                  # Quantum state implementation
├── quantum_gates.h                    # Quantum gate library
├── quantum_gates.cpp                  # Gate implementations
├── quantum_arithmetic.h               # Quantum arithmetic operations
│
├── test_gates.cpp                     # Basic gate tests
├── test_quantum_adder.cpp             # Adder tests
├── test_quantum_comparator.cpp        # Comparator tests
├── test_toffoli_and.cpp               # Toffoli AND demonstration
│
├── input.txt                          # Default input configuration
├── simple_input.txt                   # Simple test input
├── test_non_coprime.txt               # Edge case test input
│
├── TOFFOLI_AND_DEMO.md                # Toffoli gate demonstration guide
│
└── .vscode/
    └── settings.json                  # IDE configuration
```

## 🔬 Algorithm Overview

### Quantum Gates Implemented

| Gate | Symbol | Purpose | Matrix |
|------|--------|---------|--------|
| Hadamard | H | Create superposition | $\frac{1}{\sqrt{2}}\begin{bmatrix}1&1\\1&-1\end{bmatrix}$ |
| Pauli-X | X | Bit flip | $\begin{bmatrix}0&1\\1&0\end{bmatrix}$ |
| Pauli-Y | Y | Y-rotation | $\begin{bmatrix}0&-i\\i&0\end{bmatrix}$ |
| Pauli-Z | Z | Phase flip | $\begin{bmatrix}1&0\\0&-1\end{bmatrix}$ |
| CNOT | CX | Controlled-NOT | 2-qubit entangling gate |
| Toffoli | CCX | Controlled-controlled-NOT | 3-qubit universal gate |
| Phase S | S | $\sqrt{Z}$ gate | $\pi/2$ phase shift |
| Phase T | T | $\sqrt[4]{Z}$ gate | $\pi/4$ phase shift |
| SWAP | SWAP | Exchange qubits | 2-qubit swap |

### Modular Exponentiation Circuit

```
Input: |x⟩|0⟩
Step 1: Apply Hadamard gates to first register
        → Σ|x⟩|0⟩

Step 2: For each qubit i:
        If qubit i = |1⟩, apply U^(2^i)
        where U|y⟩ = |a·y mod N⟩

Step 3: Measure first register
        → Outcome with probability |amplitude|²
```

### Key Components

1. **Reversible Adders**: Quantum addition without information loss
2. **Controlled Multipliers**: Modular multiplication conditioned on control qubits
3. **Exponentiation by Squaring**: Efficient decomposition of a^x
4. **Ancilla Management**: Temporary workspace qubits

## 📊 Examples

### Example 1: Small Numbers

```bash
# Input: 7^x mod 15 with 4 qubits
echo "7 15 4" > example1.txt
./main example1.txt
```

**Expected**: Correct results for small modulus, fast execution (< 1 second)

### Example 2: Larger Computation

```bash
# Input: 2^x mod 997 with 10 qubits
echo "2 997 10" > example2.txt
./main example2.txt
```

**Expected**: Handles larger modulus, demonstrates scaling behavior

### Example 3: Edge Case Testing

```bash
# Test non-coprime base and modulus
./main test_non_coprime.txt
```

**Expected**: Graceful handling of special cases

### Example 4: Toffoli Gate AND Demonstration

```bash
# Compile and run the Toffoli gate demonstration
g++ -std=c++17 -O3 -o test_toffoli_and test_toffoli_and.cpp quantum_state.cpp quantum_gates.cpp
./test_toffoli_and
```

**Expected**: Demonstrates Toffoli gate computing AND operation and verifies reversibility
- Shows all 4 input combinations (00, 01, 10, 11)
- Verifies AND computation correctness
- Proves Toffoli² = Identity (reversibility)
- Displays quantum states before/after operations

**See also**: [TOFFOLI_AND_DEMO.md](TOFFOLI_AND_DEMO.md) for detailed explanation

## 🧪 Testing

### Running Test Suites

```bash
# Test basic quantum gates
./test_gates

# Test quantum adder
./test_quantum_adder

# Test quantum comparator
./test_quantum_comparator

# Test Toffoli gate AND operation and reversibility
./test_toffoli_and
```

### Test Coverage

- ✅ Single-qubit gates (X, Y, Z, H, S, T)
- ✅ Multi-qubit gates (CNOT, Toffoli, SWAP)
- ✅ Quantum state initialization and manipulation
- ✅ Quantum addition circuits
- ✅ Quantum comparison circuits
- ✅ Edge cases and error handling

### Validation

Each test includes:
1. **Setup**: Initialize quantum states
2. **Operation**: Apply quantum gates/circuits
3. **Verification**: Compare with expected classical results
4. **Output**: Detailed pass/fail information

## 📚 Documentation

### Core Documents

- **[README.md](README.md)**: Project overview and quick start (this file)
- **[IMPLEMENTATION_GUIDE.md](IMPLEMENTATION_GUIDE.md)**: Step-by-step implementation details
- **[MODULAR_EXPONENTIATION_DESIGN.md](MODULAR_EXPONENTIATION_DESIGN.md)**: Algorithm design and architecture
- **[QUANTUM_ARITHMETIC_IMPLEMENTATION.md](QUANTUM_ARITHMETIC_IMPLEMENTATION.md)**: Arithmetic circuit implementations
- **[draft.tex](draft.tex)**: Academic paper draft (LaTeX)
 **[TOFFOLI_AND_DEMO.md](TOFFOLI_AND_DEMO.md)**: Toffoli gate AND operation and reversibility demonstration

### Code Documentation

The codebase includes:
- **Inline comments**: Algorithm explanations in source files
- **Header files**: Interface documentation and usage notes
- **Function documentation**: Parameter descriptions and return values

## ⚡ Performance Considerations

### Memory Usage

Memory scales exponentially with number of qubits:

| Qubits | States | Memory (approx.) |
|--------|--------|------------------|
| 5      | 32     | 0.5 KB           |
| 10     | 1,024  | 16 KB            |
| 15     | 32,768 | 512 KB           |
| 20     | 1,048,576 | 16 MB       |

**Current Limit**: 10 qubits (configurable via `MAX_QUBITS` constant)

### Optimization Tips

1. **Use `-O3` flag** for production builds
2. **Limit qubit count** to what's necessary for your problem
3. **Profile** larger circuits to identify bottlenecks
4. **Consider specialized libraries** (e.g., Intel MKL) for complex number operations

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

### How to Contribute

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/your-feature`
3. **Make your changes** with clear commit messages
4. **Add tests** for new functionality
5. **Ensure all tests pass**
6. **Submit a pull request**

### Development Guidelines

- **Code Style**: Follow existing C++ conventions
- **Comments**: Document non-obvious logic
- **Tests**: Include test cases for new features
- **Documentation**: Update relevant documentation files

### Areas for Contribution

- Performance optimization
- Additional quantum gates
- Better error handling
- Visualization tools
- Integration with quantum computing frameworks

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Shor's Algorithm**: Inspired by Peter Shor's groundbreaking work in quantum factorization
- **Quantum Computing Community**: Valuable resources and documentation from the quantum computing community
- **Open Source Contributors**: Thanks to all contributors to quantum computing libraries and tools

## 🔗 References

### Academic Papers

1. Shor, P. W. (1994). "Algorithms for quantum computation: discrete logarithms and factoring". Proceedings of the 35th Annual Symposium on Foundations of Computer Science.
2. Nielsen, M. A., & Chuang, I. L. (2010). "Quantum Computation and Quantum Information". Cambridge University Press.

### Online Resources

- [IBM Quantum Experience](https://quantum-computing.ibm.com/)
- [Qiskit Textbook](https://qiskit.org/textbook/)
- [Quantum Computing Stack Exchange](https://quantumcomputing.stackexchange.com/)

---

**Note**: This is a classical simulator of quantum circuits. For actual quantum hardware, consider platforms like IBM Quantum, Rigetti, or IonQ.

## 📧 Contact

For questions, issues, or suggestions:
- Open an issue on GitHub
- Contact: [fengqiyu0317](https://github.com/fengqiyu0317)

---

⭐ **Star this repository if you find it useful!**
