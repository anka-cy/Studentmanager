#include "PersistenceManager.h"

#include "../models/UndergraduateStudent.h"
#include "../models/GraduateStudent.h"
#include "../models/PhDStudent.h"
#include "../exceptions/StudentExceptions.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <filesystem>

void PersistenceManager::save(const StudentManager& mgr, const std::string& path) {
    try {
        std::filesystem::path p(path);
        std::filesystem::path dir = p.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de la creation du repertoire : " << e.what() << std::endl;
        // continue; we'll try to open the file and report if it fails
    }

    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier pour l'ecriture : " << path << std::endl;
        return;
    }

    for (const auto& pair : mgr.obtenirListe()) {
        std::shared_ptr<Student> student = pair.second;

        if (student->getType() == "Licence") {
            std::shared_ptr<UndergraduateStudent> u =
                std::dynamic_pointer_cast<UndergraduateStudent>(student);

            if (u != nullptr) {
                file << u->getType() << "|"
                     << u->getId() << "|"
                     << u->getNom() << "|"
                     << u->getMoyenne() << "|"
                     << u->getFiliere()
                     << std::endl;
            }
        }
        else if (student->getType() == "Master") {
            std::shared_ptr<GraduateStudent> g =
                std::dynamic_pointer_cast<GraduateStudent>(student);

            if (g != nullptr) {
                file << g->getType() << "|"
                     << g->getId() << "|"
                     << g->getNom() << "|"
                     << g->getMoyenne() << "|"
                     << g->getSujetRecherche()
                     << std::endl;
            }
        }
        else if (student->getType() == "Doctorat") {
            std::shared_ptr<PhDStudent> p =
                std::dynamic_pointer_cast<PhDStudent>(student);

            if (p != nullptr) {
                file << p->getType() << "|"
                     << p->getId() << "|"
                     << p->getNom() << "|"
                     << p->getMoyenne() << "|"
                     << p->getEncadrant() << "|"
                     << p->getAnneeEtude()
                     << std::endl;
            }
        }
    }

    file.close();
}

void PersistenceManager::load(StudentManager& mgr, const std::string& path) {
    try {
        std::filesystem::path p(path);
        std::filesystem::path dir = p.parent_path();

        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        if (!std::filesystem::exists(p)) {
            std::ofstream createFile(path);
            createFile.close();
            std::cout << "Fichier absent. Creation automatique : " << path << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de la preparation du fichier de donnees : " << e.what() << std::endl;
    }

    std::ifstream file(path);

    if (!file.is_open()) {
        std::cout << "Impossible d'ouvrir le fichier. Demarrage avec une liste vide." << std::endl;
        return;
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        try {
            std::stringstream ss(line);

            std::string type;
            std::string idText;
            std::string nom;
            std::string moyenneText;
            std::string extra1;
            std::string extra2;

            std::getline(ss, type, '|');
            std::getline(ss, idText, '|');
            std::getline(ss, nom, '|');
            std::getline(ss, moyenneText, '|');
            std::getline(ss, extra1, '|');
            std::getline(ss, extra2, '|');

            int id = std::stoi(idText);
            float moyenne = std::stof(moyenneText);

            if (type == "Licence") {
                mgr.ajouterEtudiant(std::make_shared<UndergraduateStudent>(nom, id, moyenne, extra1));
            }
            else if (type == "Master") {
                mgr.ajouterEtudiant(std::make_shared<GraduateStudent>(nom, id, moyenne, extra1));
            }
            else if (type == "Doctorat") {
                int annee = std::stoi(extra2);
                mgr.ajouterEtudiant(std::make_shared<PhDStudent>(nom, id, moyenne, extra1, annee));
            }
        } catch (const std::exception& e) {
            std::cerr << "Ligne corrompue ignoree : " << line << std::endl;
        }
    }

    file.close();
}