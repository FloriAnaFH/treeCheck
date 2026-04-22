# Protokoll – AVL-Baum Überprüfung

## 1. Aufgabenstellung

Implementierung eines Programms, das einen binären Suchbaum aus einer Textdatei einliest und überprüft, ob dieser ein AVL-Baum ist. 
Zusätzlich werden statistische Daten (Minimum, Maximum, Durchschnitt) ausgegeben.

---

## 2. Problemstellung

Ein binärer Suchbaum ist nur dann effizient wenn er ausgeglichen ist.
Die Struktur des Baums hängt von der Einfügereihenfolge der Schlüsselwerte
ab. Bei ungünstiger Reihenfolge, zum Beispiel aufsteigend sortierten Werten
– degeneriert der Baum zur verketteten Liste. In diesem Fall verschlechtert
sich die Suchkomplexität von O(log N) auf O(N).

Dieses Programm macht dieses Problem sichtbar: Es liest Schlüsselwerte aus
einer Textdatei ein, baut daraus einen binären Suchbaum auf, und beantwortet
folgende Fragen:

**Analysemodus:**
- Wie groß ist der Balance- Faktor jedes Knotens?
- Verletzt ein Knoten die AVL-Bedingung?
- Ist der gesamte Baum ein AVL-Baum?
- Was sind die statistischen Kennzahlen (Minimum, Maximum, Durchschnitt)?

**Suchmodus:**
- Auf welchem Pfad von der Wurzel befindet sich ein gesuchter Schlüsselwert?
- Ist eine bestimmte Baumstruktur als exakter Teilbaum im Hauptbaum enthalten?


## 3. Architektur

Das Programm ist nach dem Prinzip der **Schichtenarchitektur** aufgebaut.
Jede Schicht hat eine klar definierte Aufgabe und kommuniziert nur mit der
direkt darunter liegenden Schicht. Die Kommunikation ist einseitig – obere
Schichten rufen untere auf, nie umgekehrt. 
**Single Responsibility Principle**: jede Einheit macht genau eine Sache.

| Schicht | Datei(en) | Aufgabe |
|---|---|---|
| 1 | `main.cpp` | Einstieg & Weiterleitung |
| 2 | `modes.cpp` | Koordination & Ablaufsteuerung |
| 3 | `tree.cpp`, `stats.cpp` | Datenstruktur & Algorithmen |
| 4 | `node.h` | Rohe Daten |


### Schicht 1 – main.cpp: Einstieg und Weiterleitung

`main.cpp` ist der einzige Einstiegspunkt des Programms. Sie liest die
Kommandozeilenargumente (`argc`, `argv`) und entscheidet anhand der Anzahl
der Argumente welcher Modus ausgeführt wird:

- 2 Argumente → `analysisMode(argv[1])`
- 3 Argumente → `searchMode(argv[1], argv[2])`
- sonst → Fehlerausgabe

`main.cpp` enthält keine Logik über Bäume oder Algorithmen. Sie delegiert
ausschließlich weiter.

### Schicht 2 – modes.cpp: Koordination und Ablaufsteuerung

`modes.cpp` koordiniert den Ablauf der einzelnen Operationen in der
richtigen Reihenfolge. Sie entscheidet nicht wie ein Baum funktioniert,
sondern was in welcher Reihenfolge aufgerufen wird.

Im `analysisMode` bedeutet das konkret:
1. Baum aus Datei einlesen (`Tree::fromFile`)
2. Balance-Faktoren berechnen und ausgeben (`printBalance`)
3. AVL-Ergebnis speichern und ausgeben (`setAVL`, `printAVL`)
4. Statistiken berechnen und ausgeben (`Stats::getStats`, `printStats`)

Im `searchMode` koordiniert sie:
1. Beide Bäume einlesen (Hauptbaum und Query-Baum)
2. Fallunterscheidung: ein Key → Einzelsuche, mehrere Keys → Teilbaumsuche
3. Ergebnis ausgeben

### Schicht 3 – tree.cpp & stats.cpp: Datenstruktur und Algorithmen

Diese Schicht enthält die eigentliche Intelligenz des Programms. Hier
werden der binäre Suchbaum aufgebaut, rekursiv traversiert, Balancefaktoren
berechnet und Suchalgorithmen ausgeführt. `stats.cpp` sammelt statistische
Kennzahlen während der Traversierung.

### Schicht 4 – node.h: Rohe Daten

`Node` ist die kleinste Einheit des Programms. Sie speichert einen
Schlüsselwert und zwei Zeiger auf Kindknoten. Es gibt kein eigenes
Verhalten – nur Daten.

---

---
## 4. Datenstrukturen

### Knoten (Node)
Ein Knoten des binären Suchbaums wird durch folgende Klasse repräsentiert:

```cpp
struct Node {
    int key_;             
    std::unique_ptr<Node> left;   
    std::unique_ptr<Node> right;
    
      explicit Node(int key);
};
```

Die Kindzeiger sind als `std::unique_ptr` implementiert. Das bedeutet:
jeder Knoten besitzt seine Kinder. Wird ein Knoten gelöscht, werden
automatisch alle Kindknoten rekursiv mitgelöscht – ohne manuellen
Aufräum-Code.

### Binärer Suchbaum (BST)

Die `Tree`-Klasse verwaltet den gesamten Baum. Sie hält einen `unique_ptr`
auf die Wurzel, die aktuelle Größe und ein AVL-Flag:

```cpp
class Tree {
  private:
    std::unique_ptr<Node> root;
    std::size_t size_ = 0;
    bool avl = true;
};
```

Die BST-Eigenschaft gilt für jeden Knoten rekursiv:
- Alle Werte im linken Teilbaum sind kleiner als der aktuelle Knoten
- Alle Werte im rechten Teilbaum sind größer
- Doppelte Schlüsselwerte werden beim Einfügen verworfen

## 4. Rekursive Funktionen

### `insert_` – Einfügen in den BST

- **Abbruchbedingung:** `node == nullptr` → neuer Knoten wird angelegt
- **Parameter:** Referenz auf `unique_ptr<Node>`, einzufügender Key
- **Rückgabewert:** `bool` – `true` wenn eingefügt, `false` bei Duplikat
- **Logik:** Key kleiner → rekursiv links; Key größer → rekursiv rechts;
  Key gleich → ignorieren
- **Traversierung:** keiner, da Einfügepfad dem BST-Pfad folgt

### `printBalance_` – Balancefaktor berechnen und AVL prüfen

- **Abbruchbedingung:** `node == nullptr` → `true` zurückgeben
- **Parameter:** roher Zeiger auf aktuellen Knoten (`const Node*`)
- **Rückgabewert:** `bool` – `true` wenn gesamter Teilbaum AVL-konform
- **Logik:** Balancefaktor `bf = height(rechts) - height(links)` berechnen,
  ausgeben, auf Violation prüfen; dann rekursiv links und rechts.
  `return okHere && leftOk && rightOk` – der gesamte Baum ist nur AVL wenn
  jeder einzelne Knoten die Bedingung erfüllt.
- **Traversierung:** Pre-Order (Knoten → links → rechts)

### `height` – Höhe eines Teilbaums berechnen

- **Abbruchbedingung:** `node == nullptr` → `-1` zurückgeben
- **Parameter:** `const unique_ptr<Node>&`
- **Rückgabewert:** `int` – Höhe des Teilbaums
- **Logik:** `max(height(links), height(rechts)) + 1`
- **Traversierung:** Post-Order (erst Kinder, dann aktueller Knoten)

### `getStats` – Statistiken durch Traversierung berechnen

- **Abbruchbedingung:** `node == nullptr` → return
- **Parameter:** `const unique_ptr<Node>&`
- **Rückgabewert:** keiner – Ergebnisse werden in `Stats`-Membervariablen
  gespeichert (`min`, `max`, `sum`, `count`)
- **Logik:** `std::min` und `std::max` aktualisieren, sum und count
  hochzählen; rekursiv links und rechts
- **Traversierung:** Pre-Order

### `searchPath` – Einzelsuche mit Pfadausgabe

- **Abbruchbedingung:** `node == nullptr` → `false` zurückgeben
- **Parameter:** `const unique_ptr<Node>&`, gesuchter Key, `vector<int>&`
  für den Pfad
- **Rückgabewert:** `bool` – `true` wenn gefunden
- **Logik:** iterativ (nicht rekursiv) – Suchpfad entlang des BST;
  jeden besuchten Knoten in `path` sammeln; bei Treffer `true`
- **Traversierung:** iterativ, folgt dem BST-Pfad

### `containsSubtree` – Teilbaumsuche

- **Abbruchbedingung:** `subtree == nullptr` → `true`;
  `main == nullptr` → `false`
- **Parameter:** zwei `const unique_ptr<Node>&`
- **Rückgabewert:** `bool`
- **Logik:** prüft an jedem Knoten des Hauptbaums ob `sameTree` gilt;
  sonst rekursiv links und rechts weiter
- **Traversierung:** Pre-Order durch den Hauptbaum

### `sameTree` – struktureller Vergleich zweier Teilbäume

- **Abbruchbedingung:** beide `nullptr` → `true`; einer `nullptr` → `false`
- **Parameter:** zwei `const unique_ptr<Node>&`
- **Rückgabewert:** `bool`
- **Logik:** Keys müssen gleich sein, linke Teilbäume gleich, rechte gleich –
  alles rekursiv; Kurzschlussauswertung mit `&&`

---
## 5. Aufwandsabschätzung (O-Notation)
### Einzelne Funktionen

N = Anzahl der Knoten im Hauptbaum, M = Anzahl der Knoten im Query-Baum

| Funktion | Aufwand | Begründung |
|---|---|---|
| `insert_` | O(log N) avg, O(N) worst | Folgt dem BST-Pfad; worst case bei degeneriertem Baum |
| `height` | O(N) | Besucht jeden Knoten genau einmal |
| `printBalance_` | O(N²) | Für jeden der N Knoten wird `height` aufgerufen (O(N)) |
| `getStats` | O(N) | Einmalige Traversierung aller Knoten |
| `searchPath` | O(log N) avg, O(N) worst | Folgt dem BST-Pfad |
| `containsSubtree` | O(N · M) | An jedem der N Knoten wird `sameTree` (O(M)) aufgerufen |
| `sameTree` | O(M) | Vergleicht jeden Knoten des Subtrees |


### Gesamtaufwand pro Modus

Die Operationen innerhalb eines Modus laufen nacheinander ab – der dominante Term bestimmt den Gesamtaufwand.

| Modus | Dominante Operation | Gesamtaufwand |
|---|---|---|
| Analysemodus | `printBalance_` mit O(N²) | **O(N²)** |
| Suchmodus – Einzelsuche | Baumaufbau mit O(N log N) | **O(N log N)** |
| Suchmodus – Teilbaumsuche | `containsSubtree` mit O(N · M) | **O(N · M)** |

Der Analysemodus ist der teuerste Modus. Die Ursache für O(N²) liegt in `printBalance_`,
für jeden der N Knoten wird `height` separat aufgerufen, eas selbst O(N) benötigt. Eine mögliche Optimierung
wäre die Höhe direkt im Knoten zu speichern – dann wäre `printBalance_` nur O(N).
