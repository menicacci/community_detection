## Tarjan Algorithm Implementation in C

This repository contains a C implementation of Tarjan's Algorithm with k-Core decomposition for finding communities in a graph.

**Files:**

- `graph.c` and `graph.h`: Graph data structure implementation.
- `utils.c` and `utils.h`: k-Core Tarjan Algorithm implementation.
- `graph_io.c` and `graph_io.h`: Graph Structure and Stats.

- `examples`: Input graphs examples.

**Compilation:**

```bash
gcc -o tarjan.exe main.c graph.c utils.c graph_io.c
```

**Usage:**

Run the executable with a graph file:

```bash
./<executable_name> input_graph output_file k_core
```
