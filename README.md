# StudentManager


Description
- Application de gestion d'étudiants écrite en C++ (Qt pour l'interface). Le projet implémente :
  - Modèles d'étudiants (`Licence`, `Master`, `Doctorat`) héritant d'une classe `Student` abstraite.
  - Validation et exceptions (IDs, moyennes, duplications, introuvable).
  - `StudentManager` : CRUD, recherche et tris.
  - Persistance simple en texte dans `data/students.txt` (format pipe-separated).
  - Interface graphique (Qt) et une suite de tests console complète.

Fichiers importants
- Tests : [tests/test_main.cpp](tests/test_main.cpp)
- Script tests (Windows PowerShell) : [run_tests.ps1](run_tests.ps1)
- Persistance : [src/persistence/PersistenceManager.cpp](src/persistence/PersistenceManager.cpp)
- Données exemples : [data/students.txt](data/students.txt)

Prérequis
- Windows avec `g++` (MinGW) dans le PATH pour compiler les tests.
- PowerShell (pour le script `run_tests.ps1`).
- Qt Creator / Qt (kit MinGW ou MSVC) pour construire et exécuter l'interface graphique.

Compilation et exécution

1) Exécuter l'interface (Qt Creator)
- Ouvrez `StudentManager.pro` dans Qt Creator, choisissez un kit (MinGW/MSVC + Qt), puis `Build` et `Run`.

2) Lancer la suite de tests console (PowerShell)
- Depuis le répertoire racine du projet, lancez :


- Le script compile les sources nécessaires, exécute le binaire de test et **mettra la fenêtre en pause** après l'exécution afin que vous puissiez lire les résultats. Appuyez sur Entrée pour fermer la fenêtre.


```

Informations sur la persistance
- Emplacement par défaut : `data/students.txt`.
- Format par ligne : `Type|ID|Nom|Moyenne|Extra1|Extra2`
  - `Licence` → `Extra1` = filière
  - `Master` → `Extra1` = sujet de recherche
  - `Doctorat` → `Extra1` = encadrant, `Extra2` = année d'études

Suite de tests
- Le fichier `tests/test_main.cpp` contient une suite complète couvrant :
  - Création des 3 types (`getType()`, `calculerBourse()`, `afficherDetails()`),
  - CRUD complet (ajout, modification, suppression, recherche par ID/nom),
  - Tris (GPA décroissant, ordre alphabétique),
  - Déclenchement et vérification des exceptions (`InvalidIDException`, `InvalidGradeException`, `DuplicateIDException`, `StudentNotFoundException`),
  - Persistance : sauvegarde, vidage, rechargement et comparaison des données.

