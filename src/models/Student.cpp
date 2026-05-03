#include "Student.h"
#include "../exceptions/StudentExceptions.h"

Student::Student(const std::string& nom, int id, float moyenne) {
    if (id < 1000) {
        throw InvalidIDException(id);
    }

    this->nom = nom;
    this->id = id;

    setMoyenne(moyenne);
}

void Student::setNom(const std::string& n) {
    this->nom = n;
}

void Student::setMoyenne(float m) {
    if (m < 0.0f || m > 4.0f) {
        throw InvalidGradeException(m);
    }

    this->moyenne = m;
}