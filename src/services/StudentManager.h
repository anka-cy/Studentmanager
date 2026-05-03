#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include <map>
#include <vector>
#include <memory>
#include <string>

#include "../models/Student.h"

class StudentManager {
private:
    std::map<int, std::shared_ptr<Student>> listeEtudiants;

public:
    void ajouterEtudiant(std::shared_ptr<Student> etudiant);
    void supprimerEtudiant(int id);
    void modifierEtudiant(int id, std::shared_ptr<Student> etudiantModifie);

    std::shared_ptr<Student> chercherParId(int id) const;
    std::vector<std::shared_ptr<Student>> chercherParNom(const std::string& nom) const;

    std::vector<std::shared_ptr<Student>> trierParMoyenne() const;
    std::vector<std::shared_ptr<Student>> trierParNom() const;

    double calculerMoyenneGenerale() const;
    int obtenirNombreTotal() const { return listeEtudiants.size(); }

    int obtenirNombreParType(const std::string& type) const;

    const std::map<int, std::shared_ptr<Student>>& obtenirListe() const {
        return listeEtudiants;
    }
};

#endif