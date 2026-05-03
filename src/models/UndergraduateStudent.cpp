#include "UndergraduateStudent.h"
#include <iostream>

UndergraduateStudent::UndergraduateStudent(const std::string& nom, int id, float moyenne, const std::string& filiere)
    : Student(nom, id, moyenne), filiere(filiere) {}

std::string UndergraduateStudent::getType() const {
    return "Licence";
}

double UndergraduateStudent::calculerBourse() const {
    if (moyenne >= 3.5f) {
        return 500.0;
    }
    return 0.0;
}

void UndergraduateStudent::afficherDetails() const {
    std::cout << "[ " << getType() << " ] "
              << "ID: " << id
              << " | Nom: " << nom
              << " | Moyenne: " << moyenne
              << " | Filiere: " << filiere
              << " | Bourse: " << calculerBourse() << " EUR"
              << std::endl;
}