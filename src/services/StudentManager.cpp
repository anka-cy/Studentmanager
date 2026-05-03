#include "StudentManager.h"
#include "../exceptions/StudentExceptions.h"

#include <algorithm>
#include <cctype>

static std::string toLowerCase(const std::string& text) {
    std::string result = text;

    std::transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) {
                  return std::tolower(c);
              });

    return result;
}

void StudentManager::ajouterEtudiant(std::shared_ptr<Student> etudiant) {
    int id = etudiant->getId();

    if (listeEtudiants.find(id) != listeEtudiants.end()) {
        throw DuplicateIDException(id);
    }

    listeEtudiants[id] = etudiant;
}

void StudentManager::supprimerEtudiant(int id) {
    auto it = listeEtudiants.find(id);

    if (it == listeEtudiants.end()) {
        throw StudentNotFoundException(id);
    }

    listeEtudiants.erase(it);
}

void StudentManager::modifierEtudiant(int id, std::shared_ptr<Student> etudiantModifie) {
    auto it = listeEtudiants.find(id);

    if (it == listeEtudiants.end()) {
        throw StudentNotFoundException(id);
    }

    int newId = etudiantModifie->getId();

    if (newId != id && listeEtudiants.find(newId) != listeEtudiants.end()) {
        throw DuplicateIDException(newId);
    }

    listeEtudiants.erase(it);
    listeEtudiants[newId] = etudiantModifie;
}

std::shared_ptr<Student> StudentManager::chercherParId(int id) const {
    auto it = listeEtudiants.find(id);

    if (it == listeEtudiants.end()) {
        throw StudentNotFoundException(id);
    }

    return it->second;
}

std::vector<std::shared_ptr<Student>> StudentManager::chercherParNom(const std::string& nom) const {
    std::vector<std::shared_ptr<Student>> result;
    std::string searchNom = toLowerCase(nom);

    for (const auto& pair : listeEtudiants) {
        std::string nomEtudiant = toLowerCase(pair.second->getNom());

        if (nomEtudiant.find(searchNom) != std::string::npos) {
            result.push_back(pair.second);
        }
    }

    return result;
}

std::vector<std::shared_ptr<Student>> StudentManager::trierParMoyenne() const {
    std::vector<std::shared_ptr<Student>> result;

    for (const auto& pair : listeEtudiants) {
        result.push_back(pair.second);
    }

    std::sort(result.begin(), result.end(),
         [](const std::shared_ptr<Student>& a, const std::shared_ptr<Student>& b) {
             return a->getMoyenne() > b->getMoyenne();
         });

    return result;
}

std::vector<std::shared_ptr<Student>> StudentManager::trierParNom() const {
    std::vector<std::shared_ptr<Student>> result;

    for (const auto& pair : listeEtudiants) {
        result.push_back(pair.second);
    }

    std::sort(result.begin(), result.end(),
         [](const std::shared_ptr<Student>& a, const std::shared_ptr<Student>& b) {
             return toLowerCase(a->getNom()) < toLowerCase(b->getNom());
         });

    return result;
}

double StudentManager::calculerMoyenneGenerale() const {
    if (listeEtudiants.empty()) {
        return 0.0;
    }

    double sum = 0.0;

    for (const auto& pair : listeEtudiants) {
        sum += pair.second->getMoyenne();
    }

    return sum / listeEtudiants.size();
}

int StudentManager::obtenirNombreParType(const std::string& type) const {
    return std::count_if(listeEtudiants.begin(), listeEtudiants.end(),
                    [&type](const auto& pair) {
                        return pair.second->getType() == type;
                    });
}