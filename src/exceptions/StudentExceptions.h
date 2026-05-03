#ifndef ACADEMIC_EXCEPTIONS_H
#define ACADEMIC_EXCEPTIONS_H

#include <exception>
#include <string>

class InvalidIDException : public std::exception {
private:
    std::string message;

public:
    explicit InvalidIDException(int id)
        : message("Erreur : ID invalide (" + std::to_string(id) + "). Doit etre >= 1000.") {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class InvalidGradeException : public std::exception {
private:
    std::string message;

public:
    explicit InvalidGradeException(float moyenne)
        : message("Erreur : Moyenne invalide [0.0 - 4.0]. Donnee : " + std::to_string(moyenne)) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class StudentNotFoundException : public std::exception {
private:
    std::string message;

public:
    explicit StudentNotFoundException(int id)
        : message("Erreur : Etudiant introuvable pour l'ID : " + std::to_string(id)) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class DuplicateIDException : public std::exception {
private:
    std::string message;

public:
    explicit DuplicateIDException(int id)
        : message("Erreur : ID deja utilise : " + std::to_string(id)) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif