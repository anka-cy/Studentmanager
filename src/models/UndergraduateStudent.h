#ifndef UNDERGRADUATE_STUDENT_H
#define UNDERGRADUATE_STUDENT_H

#include "Student.h"
#include <string>

class UndergraduateStudent : public Student {
private:
    std::string filiere;

public:
    UndergraduateStudent(const std::string& nom, int id, float moyenne, const std::string& filiere);

    void afficherDetails() const override;
    std::string getType() const override;
    double calculerBourse() const override;

    std::string getFiliere() const { return filiere; }
    void setFiliere(const std::string& f) { filiere = f; }
};

#endif