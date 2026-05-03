#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <memory>

#include "../services/StudentManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    StudentManager manager;
    QString dataPath;

    QTableWidget* table;
    QLineEdit* searchEdit;
    QComboBox* typeFilter;

    QLabel* titleLabel;
    QLabel* subtitleLabel;

    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deleteButton;
    QPushButton* refreshButton;
    QPushButton* exportButton;
    QPushButton* themeButton;

    void setupUi();
    void loadData();
    void saveData();
    void refreshTable();
    void updateStatus();
    int getSelectedId() const;
    QString getStudentDetails(std::shared_ptr<Student> student) const;
    bool matchesFilters(std::shared_ptr<Student> student) const;

private slots:
    void ajouterEtudiant();
    void modifierEtudiant();
    void supprimerEtudiant();
    void actualiserDonnees();
    void exporterCsv();
    void basculerModeSombre();
    void appliquerThemeSombre(bool activer);

public:
    explicit MainWindow(QWidget* parent = nullptr);
};

#endif