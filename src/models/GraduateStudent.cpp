#include "GraduateStudent.h"
#include <iostream>

GraduateStudent::GraduateStudent(const std::string& nom, int id, float moyenne, const std::string& sujet)
    : Student(nom, id, moyenne), sujetRecherche(sujet) {}

std::string GraduateStudent::getType() const {
    return "Master";
}

double GraduateStudent::calculerBourse() const {
    return moyenne * 300.0;
}

void GraduateStudent::afficherDetails() const {
    std::cout << "[ " << getType() << " ] "
              << "ID: " << id
              << " | Nom: " << nom
              << " | Moyenne: " << moyenne
              << " | Sujet: " << sujetRecherche
              << " | Bourse: " << calculerBourse() << " EUR"
              << std::endl;
}