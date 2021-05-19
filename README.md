# B-Tree
B-Tree implementare in C++ (C++11)

Un B-Tree are un ordin (grad minim) T:
  * Fiecare nod are cel putin T-1 chei si cel mult 2*T-1 chei
  * Radacina poate avea si mai putin de T-1 chei dar macar 1 cheie (daca arborele nu e gol)
  * Daca un nod are k chei are fix k+1 fii
  * Toate frunzele sunt pe acelasi nivel
  * Toate cheile dintr-un nod sunt sortate

---

Clasa BTree e template, trebuie sa fie definiti toti operatorii de comparatie pentru tipul de chei (<, <=, >, >=, ==, !=).

Constructorul primeste ca parametru ordinul (default este 2 => 234-Tree)

Implementare e bazata pe descrierea BTrees din [Introduction to Algorithms](https://edutechlearners.com/download/Introduction_to_algorithms-3rd%20Edition.pdf) pagina 484.
Am optimizat unele parti, de exemplu cautarea unei chei intr-un nod se face cu cautare binara, stergerea si inserarea se fac cu o singura miscare in jos in arbore.
Totusi inserarea si stergerea nu este foarte rapida pentru ca cheile sunt stocate intr-un vector.

Nu am testat daca este mai rapid cu std::list in loc de std::vector pentru retinerea cheilor si fiilor dintr-un nod dar pentru valori mici al ordinului (< 100) se misca destul de repede. 


In main.cpp e rezolvarea la [abce](https://www.infoarena.ro/problema/abce) de pe infoarena
