#ifndef GRADUATE_STUDENT_H
#define GRADUATE_STUDENT_H

#include "Student.h"
#include <string>

class GraduateStudent : public Student {
private:
    std::string sujetRecherche;

public:
    GraduateStudent(const std::string& nom, int id, float moyenne, const std::string& sujet);

    void afficherDetails() const override;
    std::string getType() const override;
    double calculerBourse() const override;

    std::string getSujetRecherche() const { return sujetRecherche; }
    void setSujetRecherche(const std::string& sujet) { sujetRecherche = sujet; }
};

#endif