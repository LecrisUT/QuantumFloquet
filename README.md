<h1 align="center">QuanFloq</h1>
<h3 align="center">Extensible library for quantum floquet calculations and more</h3>

# This project is still a work in progress. Expect breaking changes until v1.0

## Philosophy

- **Computationally efficient:**
    - Directly access efficient low-level libraries
    - Cache minimal wrapper of the low-level function calls
- **Extensible:**
    - Easily creatable C++ plugins with cookie-cutter templates that are automatically discovered and loaded
    - Simple and structured human-readable top-level interface
    - Modularized design of the quantum objects, calculation steps, etc. that can be interchanged, patched, and extended
- **Built for scientists:**
    - Assume the user does not have coding expertise. Provide a wide range of top-level interface
    - Primary users are scientist. Naming conventions and documentations (in and out of code) are built upon familiar
      physical and chemical language

## Roadmap:

- [x] Extensible framework for the quantum objects
    - [x] Save/Load variables `IExposable`, `Scriber`, etc.
    - [x] Auto-Registration of the C++ class `TypeInfo`, `TypeRegistrar`, etc.
    - [x] Registration interface
    - [x] Extensible `Thing` interface
        - [x] `ThingCache`: Cache object containing a link to the called object and cached function interfaces. Linked
          to `ThingDriver` and `IThing`
        - [x] `ThingDriver`: Flexible driver class of `IThing`, which are implementation dependent
        - [x] `IThing`: High-level interface
        - [x] `ThingDef`: Definition and constructor of `Thing` defining the Thing's class, driver and components
          (if applicable). Human-readable interface via `JSON`
        - [x] `ThingComp`: Extendable components beyond the `IThing` interface and `Thing_t` data structure. Contains
          similar framework as `Thing` (`CompCache`, `CompDef`, `CompDriver`, `IComp`)
        - [x] `ThingData`: Variable data structure
        - [x] `Thing`: Top level class implementing all of the above
    - [x] File interface `JSON` only for now
    - [ ] CLI interface
- [ ] Hamiltonian `Thing` implementation
    - [ ] Basic wave function interface, e.g. `$H\ket{\Psi}$`
    - [ ] Primary components
        - [ ] Floquet component calculating the quasi-energy and average energy
        - [ ] Hartree-Fock component calculating the basic Fock operator
        - [ ] Floquet Hartree-Fock component calculating the orbital's quasi-energy and total average energy
    - [ ] Basic implementation using `mkl`
- [ ] Wave function `Thing`
- [ ] Density matrix `Thing` using Fock-space representation
- [ ] Master equation `Thing`
- [ ] CalculationNode `Thing`: Main high-level interface, performs
    - [ ] Connectable to eachother and outside via ports
    - [ ] Main interfaces:
        - [ ] Eigensolvers
        - [ ] Self-consistent solvers
        - [ ] Propagators (numeric and exact)
        - [ ] Steady-state solvers
- [ ] CalculationTree `Thing`: Graph of the various calculation nodes