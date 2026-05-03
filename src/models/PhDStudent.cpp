#include "PhDStudent.h"
#include <iostream>

PhDStudent::PhDStudent(const std::string& nom, int id, float moyenne, const std::string& encadrant, int annee)
    : Student(nom, id, moyenne), encadrant(encadrant), anneeEtude(annee) {}

std::string PhDStudent::getType() const {
    return "Doctorat";
}

double PhDStudent::calculerBourse() const {
    return 1200.0;
}

void PhDStudent::afficherDetails() const {
    std::cout << "[ " << getType() << " ] "
              << "ID: " << id
              << " | Nom: " << nom
              << " | Moyenne: " << moyenne
              << " | Encadrant: " << encadrant
              << " | Annee: " << anneeEtude
              << " | Bourse: " << calculerBourse() << " EUR/mois"
              << std::endl;
}