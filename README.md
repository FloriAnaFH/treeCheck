# TreeCheck

Ein binärer Suchbaum wird aus einer Textdatei aufgebaut und auf AVL-Eigenschaften geprüft.

---
## Build
**Voraussetzungen:**
- CMake >= 3.10
- C++ Compiler (g++ oder clang++)

**VSCode Einrichtung:**
1. `Cmd + ,` → suche nach `cmake.cmakePath`
2. Pfad eintragen (z.B. `/opt/homebrew/bin/cmake`)
3. Pfad herausfinden mit: `which cmake`

**Schritte:**
```bash
mkdir build
cd build
cmake ..
make
```

**Programm ausführen:**
```bash
./treecheck ../tests/input1.txt
```


## Projektstruktur

```
treecheck/
├── src/
├── tests/
├── docs/
└── README.md
```
