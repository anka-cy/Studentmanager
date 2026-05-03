#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include "../src/models/UndergraduateStudent.h"
#include "../src/models/GraduateStudent.h"
#include "../src/models/PhDStudent.h"
#include "../src/services/StudentManager.h"
#include "../src/persistence/PersistenceManager.h"
#include "../src/exceptions/StudentExceptions.h"

template<typename Ex, typename F>
std::string expectThrows(F&& fn) {
    try {
        fn();
    } catch (const Ex& e) {
        return e.what();
    } catch (const std::exception& e) {
        return std::string("Unexpected std::exception: ") + e.what();
    } catch (...) {
        return "Unknown exception";
    }
    return "";
}

void testClassesIndividuelles() {
    UndergraduateStudent u("Amira", 1001, 3.60f, "Informatique");
    GraduateStudent g("Amina", 2001, 3.80f, "Réseaux");
    PhDStudent p("Martin", 3001, 4.00f, "Leclerc", 2);

    assert(u.getNom() == "Amira");
    assert(u.getId() == 1001);
    assert(u.getMoyenne() == 3.60f);
    assert(u.getType() == "Licence");
    assert(u.calculerBourse() == 500.0);

    assert(g.getType() == "Master");
    assert(g.calculerBourse() == (double)g.getMoyenne() * 300.0);

    assert(p.getType() == "Doctorat");
    assert(p.calculerBourse() == 1200.0);

    // afficherDetails() should run without throwing
    u.afficherDetails();
    g.afficherDetails();
    p.afficherDetails();

    std::cout << "testClassesIndividuelles ok\n";
}

void testCRUDTriEtRecherche() {
    StudentManager mgr;

    auto s1 = std::make_shared<UndergraduateStudent>("Amira", 1001, 3.60f, "Info");
    auto s2 = std::make_shared<GraduateStudent>("Cherif", 2001, 3.50f, "IA");
    auto s3 = std::make_shared<PhDStudent>("Martin", 3001, 4.00f, "Leclerc", 2);
    auto s4 = std::make_shared<UndergraduateStudent>("Ziad", 1002, 2.90f, "Physique");
    auto s5 = std::make_shared<GraduateStudent>("Amina", 2002, 3.80f, "Reseau");

    mgr.ajouterEtudiant(s1);
    mgr.ajouterEtudiant(s2);
    mgr.ajouterEtudiant(s3);
    mgr.ajouterEtudiant(s4);
    mgr.ajouterEtudiant(s5);

    assert(mgr.obtenirNombreTotal() == 5);

    // chercherParId
    auto found = mgr.chercherParId(2002);
    assert(found->getNom() == "Amina");

    // chercherParNom (partial)
    auto matches = mgr.chercherParNom("mi");
    bool foundAny = false;
    for (auto &m : matches) if (m->getNom().find("mi") != std::string::npos || m->getNom().find("Mi") != std::string::npos) foundAny = true;
    assert(!matches.empty() || foundAny);

    // trierParMoyenne -> descending
    auto sortedByGpa = mgr.trierParMoyenne();
    for (size_t i = 1; i < sortedByGpa.size(); ++i) {
        assert(sortedByGpa[i-1]->getMoyenne() >= sortedByGpa[i]->getMoyenne());
    }

    // trierParNom -> ascending (case-insensitive)
    auto sortedByName = mgr.trierParNom();
    for (size_t i = 1; i < sortedByName.size(); ++i) {
        std::string a = sortedByName[i-1]->getNom();
        std::string b = sortedByName[i]->getNom();
        std::transform(a.begin(), a.end(), a.begin(), ::tolower);
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
        assert(a <= b);
    }

    // modifierEtudiant: change id of s1 from 1001 -> 1010
    auto s1_mod = std::make_shared<UndergraduateStudent>("AmiraRenommee", 1010, 3.60f, "Info");
    mgr.modifierEtudiant(1001, s1_mod);
    assert(mgr.obtenirNombreTotal() == 5);
    auto checkNew = mgr.chercherParId(1010);
    assert(checkNew->getNom() == "AmiraRenommee");

    // supprimerEtudiant
    mgr.supprimerEtudiant(1002); // remove Ziad
    assert(mgr.obtenirNombreTotal() == 4);

    std::cout << "testCRUDTriEtRecherche ok\n";
}

void testPersistenceRoundTrip() {
    StudentManager mgr;
    auto s1 = std::make_shared<UndergraduateStudent>("A", 1100, 3.0f, "X");
    auto s2 = std::make_shared<GraduateStudent>("B", 2100, 3.5f, "Y");
    auto s3 = std::make_shared<PhDStudent>("C", 3100, 4.0f, "Z", 1);

    mgr.ajouterEtudiant(s1);
    mgr.ajouterEtudiant(s2);
    mgr.ajouterEtudiant(s3);

    const std::string path = "data/test_roundtrip.txt";

    PersistenceManager::save(mgr, path);

    StudentManager loaded;
    PersistenceManager::load(loaded, path);

    assert(loaded.obtenirNombreTotal() == mgr.obtenirNombreTotal());

    // optional: compare IDs
    for (const auto &pair : mgr.obtenirListe()) {
        int id = pair.first;
        auto other = loaded.chercherParId(id);
        assert(other->getNom() == pair.second->getNom());
    }

    std::cout << "testPersistenceRoundTrip ok\n";
}

void testExceptions() {
    // Invalid ID
    std::string msg1 = expectThrows<InvalidIDException>([]() {
        UndergraduateStudent bad("Bad", 999, 3.0f, "X");
    });
    assert(!msg1.empty());

    // Invalid grade
    std::string msg2 = expectThrows<InvalidGradeException>([]() {
        UndergraduateStudent bad2("Bad2", 1500, -1.0f, "X");
    });
    assert(!msg2.empty());

    // Duplicate ID
    StudentManager mgr;
    mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("P", 5000, 3.0f, "X"));
    std::string msg3 = expectThrows<DuplicateIDException>([&mgr]() {
        mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("Q", 5000, 2.0f, "Y"));
    });
    assert(!msg3.empty());

    // Student not found (supprimer)
    std::string msg4 = expectThrows<StudentNotFoundException>([&mgr]() {
        mgr.supprimerEtudiant(99999);
    });
    assert(!msg4.empty());

    std::cout << "testExceptions ok\n";
}

int main() {
    std::cout << "Lancement des tests..." << std::endl;
    testClassesIndividuelles();
    testCRUDTriEtRecherche();
    testPersistenceRoundTrip();
    testExceptions();
    // Nouveau: tableau de résultats obligatoires demandé
    auto printRow = [](const std::string &cas, const std::string &entree, const std::string &attendu, const std::string &result){
        std::cout << cas << " | " << entree << " | " << attendu << " | " << result << std::endl;
    };

    std::cout << "\n--- Tableau de resultats obligatoires ---" << std::endl;

    // 1) Ajout etudiant valide
    {
        StudentManager mgr;
        std::string entree = "ID=1001, GPA=3.8";
        try {
            mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("Valide", 1001, 3.8f, "X"));
            printRow("Ajout etudiant valide", entree, "Succes", "OK");
        } catch (const std::exception &e) {
            printRow("Ajout etudiant valide", entree, "Succes", std::string("ECHEC: ") + e.what());
            assert(false);
        }
    }

    // 2) GPA invalide
    {
        std::string entree = "GPA=5.5";
        std::string msg = expectThrows<InvalidGradeException>([](){ UndergraduateStudent bad("BadGPA", 1101, 5.5f, "X"); });
        printRow("GPA invalide", entree, "InvalidGradeException", msg.empty() ? "ECHEC" : "OK");
        assert(!msg.empty());
    }

    // 3) ID invalide
    {
        std::string entree = "ID=42";
        std::string msg = expectThrows<InvalidIDException>([](){ UndergraduateStudent bad("BadID", 42, 3.0f, "X"); });
        printRow("ID invalide", entree, "InvalidIDException", msg.empty() ? "ECHEC" : "OK");
        assert(!msg.empty());
    }

    // 4) ID duplique
    {
        StudentManager mgr;
        std::string entree = "ID=1001 (x2)";
        mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("D1", 1001, 3.0f, "X"));
        std::string msg = expectThrows<DuplicateIDException>([&mgr](){ mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("D2", 1001, 2.5f, "Y")); });
        printRow("ID duplique", entree, "DuplicateIDException", msg.empty() ? "ECHEC" : "OK");
        assert(!msg.empty());
    }

    // 5) Etudiant introuvable (remove)
    {
        StudentManager mgr;
        std::string entree = "remove(9999)";
        std::string msg = expectThrows<StudentNotFoundException>([&mgr](){ mgr.supprimerEtudiant(9999); });
        printRow("Etudiant introuvable", entree, "StudentNotFoundException", msg.empty() ? "ECHEC" : "OK");
        assert(!msg.empty());
    }

    // 6) Tri GPA decroissant
    {
        StudentManager mgr;
        mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("A", 2101, 3.0f, "X"));
        mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("B", 2102, 4.0f, "X"));
        mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("C", 2103, 2.5f, "X"));
        mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("D", 2104, 3.7f, "X"));
        mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("E", 2105, 3.2f, "X"));
        auto sorted = mgr.trierParMoyenne();
        bool ok = true;
        for (size_t i = 1; i < sorted.size(); ++i) if (sorted[i-1]->getMoyenne() < sorted[i]->getMoyenne()) ok = false;
        printRow("Tri GPA decroissant", "5 etudiants", "Liste triee", ok ? "OK" : "ECHEC");
        assert(ok);
    }

    // 7) Persistance Save + reload
    {
        StudentManager mgr;
        mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>("P1", 3101, 3.1f, "F1"));
        mgr.ajouterEtudiant(std::make_shared<GraduateStudent>("P2", 3201, 3.6f, "S1"));
        const std::string path = "data/test_persistence_table.txt";
        PersistenceManager::save(mgr, path);
        StudentManager loaded;
        PersistenceManager::load(loaded, path);
        bool same = (loaded.obtenirNombreTotal() == mgr.obtenirNombreTotal());
        if (same) {
            for (const auto &pair : mgr.obtenirListe()) {
                int id = pair.first;
                try {
                    auto other = loaded.chercherParId(id);
                    if (other->getNom() != pair.second->getNom()) { same = false; break; }
                } catch (...) { same = false; break; }
            }
        }
        printRow("Persistance Save+reload", "Save + reload", "Donnees identiques", same ? "OK" : "ECHEC");
        assert(same);
    }
    std::cout << "Tous les tests sont passes." << std::endl;
    return 0;
}