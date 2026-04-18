# Protokoll – AVL-Baum Überprüfung

## 1. Aufgabenstellung

Implementierung eines Programms, das einen binären Suchbaum aus einer Textdatei einliest und überprüft, ob dieser ein AVL-Baum ist. 
Zusätzlich werden statistische Daten (Minimum, Maximum, Durchschnitt) ausgegeben.


---

## 2. Datenstruktur

Ein Knoten des binären Suchbaums wird durch folgende Klasse repräsentiert:

```cpp
class Node {
public:
    int key;
    Node *left;
    Node *right;
    Node(int key);
};
```
### Rekursive Funktionen
Alle Durchläufe des Baumes wurden rekursiv implementiert. Für jede Funktion wurden folgende Punkte überlegt:

#### `height(Node* node)`
- **Abbruchbedingung:** `node == nullptr` → Rückgabe `-1` (leerer Teilbaum hat Höhe −1)
- **Parameter:** Zeiger auf aktuellen Knoten
- **Rückgabewert:** Höhe des Teilbaums als `int`
- **Logik:** Höhe = 1 + Maximum aus linker und rechter Teilbaumhöhe

#### `insert(Node* node, int key)`
- **Abbruchbedingung:** `node == nullptr` → neuer Knoten wird erstellt
- **Parameter:** Zeiger auf aktuellen Knoten, einzufügender Schlüssel
- **Rückgabewert:** Zeiger auf (möglicherweise neuen) Knoten
- **Logik:** Wenn key < node->key → links einfügen, sonst rechts. Doppelte Schlüssel werden verworfen.

#### `printBalance(Node* node, bool& avl)`
- **Abbruchbedingung:** `node == nullptr` → return
- **Parameter:** Zeiger auf aktuellen Knoten, Referenz auf AVL-Flag
- **Rückgabewert:** keiner (void)
- **Logik:** Traversierung rechts → links → Knoten (damit Ausgabe der Referenz entspricht). Bei |BF| > 1 wird AVL-Flag auf false gesetzt.

#### `isAVL(Node* node)`
- **Abbruchbedingung:** `node == nullptr` → return `true`
- **Parameter:** Zeiger auf aktuellen Knoten
- **Rückgabewert:** `bool`
- **Logik:** Prüft ob |BF| ≤ 1 für jeden Knoten, rekursiv für linken und rechten Teilbaum.

#### `getStats(Node* node, int& count, int& sum)`
- **Abbruchbedingung:** `node == nullptr` → return
- **Parameter:** Zeiger auf aktuellen Knoten, Referenzen auf Zähler und Summe
- **Rückgabewert:** keiner (void)
- **Logik:** Zählt alle Knoten und summiert alle Schlüsselwerte auf.

#### `getMin(Node* node)` / `getMax(Node* node)`
- **Abbruchbedingung:** kein linkes/rechtes Kind mehr vorhanden
- **Parameter:** Zeiger auf aktuellen Knoten
- **Rückgabewert:** `int` (kleinster/größter Schlüsselwert)
- **Logik:** Im BST ist das Minimum immer ganz links, das Maximum immer ganz rechts.

---
## 4. Aufwand (O-Notation)

| Funktion | Aufwand | Begründung |
|---|---|---|
| `insert` | O(N) | Im schlechtesten Fall (unbalancierter Baum) alle N Knoten durchlaufen |
| `height` | O(N) | Besucht jeden Knoten genau einmal |
| `balanceFactor` | O(N) | Ruft `height` zweimal auf → O(N) |
| `printBalance` | O(N²) | Für jeden Knoten wird `balanceFactor` aufgerufen, der O(N) braucht |
| `isAVL` | O(N²) | Gleiche Begründung wie `printBalance` |
| `getStats` | O(N) | Besucht jeden Knoten genau einmal |
| `getMin` / `getMax` | O(N) | Im schlechtesten Fall alle Knoten durchlaufen |

---

## 5. Fazit

Das Programm liest einen binären Suchbaum aus einer Textdatei ein, berechnet für jeden Knoten den Balance-Faktor und überprüft die AVL-Bedingung. Die statistischen Daten werden durch eine Traversierung des Baumes berechnet. Der Baum mit den Testdaten ist kein AVL-Baum, da drei Knoten (5, 9, 23) die AVL-Bedingung verletzen.