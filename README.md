# treeCheck

Liest eine Textdatei mit ganzen Zahlen (eine pro Zeile), baut daraus einen binären Suchbaum auf und analysiert ihn.

---

## Build

**Voraussetzungen:** CMake >= 3.10, g++ oder clang++

```sh
mkdir build && cd build
cmake ..
make
```

---

## Verwendung

**Analysemodus** – gibt Balancefaktoren, AVL-Status sowie Statistiken (Min, Max, Durchschnitt) aus:
```sh
./treecheck <baum.txt>
```

**Suchmodus** – enthält die Anfragedatei einen Schlüssel, wird er im Baum gesucht und der Pfad ausgegeben; bei mehreren Schlüsseln wird geprüft, ob der Anfrage-Baum ein Teilbaum des Hauptbaums ist:
```sh
./treecheck <hauptbaum.txt> <anfrage.txt>
```

---

## Projektstruktur

```
treeCheck/
├── src/          # Implementierung (Modi, Baum, Statistiken)
├── include/      # Header-Dateien
├── tests/        # Beispiel-Eingaben und Tests
└── main.cpp
```
