#ifndef PHD_STUDENT_H
#define PHD_STUDENT_H

#include "Student.h"
#include <string>

class PhDStudent : public Student {
private:
    std::string encadrant;
    int anneeEtude;

public:
    PhDStudent(const std::string& nom, int id, float moyenne, const std::string& encadrant, int annee);

    void afficherDetails() const override;
    std::string getType() const override;
    double calculerBourse() const override;

    std::string getEncadrant() const { return encadrant; }
    int getAnneeEtude() const { return anneeEtude; }

    void setEncadrant(const std::string& e) { encadrant = e; }
    void setAnneeEtude(int a) { anneeEtude = a; }
};

#endif