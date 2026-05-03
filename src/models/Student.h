#ifndef STUDENT_H
#define STUDENT_H

#include <string>

class Student {
protected:
    std::string nom;
    int id;
    float moyenne;

public:
    Student(const std::string& nom, int id, float moyenne);
    virtual ~Student() = default;

    // Methodes virtuelles pures
    virtual void afficherDetails() const = 0;
    virtual std::string getType() const = 0;
    virtual double calculerBourse() const = 0;

    // Getters
    std::string getNom() const { return nom; }
    int getId() const { return id; }
    float getMoyenne() const { return moyenne; }

    // Setters
    void setNom(const std::string& n);
    void setMoyenne(float m);
};

#endif