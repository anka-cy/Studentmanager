#include "MainWindow.h"
#include "StudentDialog.h"

#include "../models/UndergraduateStudent.h"
#include "../models/GraduateStudent.h"
#include "../models/PhDStudent.h"
#include "../persistence/PersistenceManager.h"
#include "../exceptions/StudentExceptions.h"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>

namespace {
QString resolveDataPath() {
    QDir dir(QCoreApplication::applicationDirPath());

    for (int i = 0; i < 8; ++i) {
        QString projectFile = dir.filePath("StudentManager.pro");
        if (QFile::exists(projectFile)) {
            return QDir::cleanPath(dir.filePath("data/students.txt"));
        }

        if (!dir.cdUp()) {
            break;
        }
    }

    QDir fallback(QCoreApplication::applicationDirPath());
    return QDir::cleanPath(fallback.filePath("data/students.txt"));
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    dataPath = resolveDataPath();

    setupUi();
    // Restore theme preference
    QSettings settings(QCoreApplication::applicationName(), QCoreApplication::applicationName());
    bool dark = settings.value("theme/dark", false).toBool();
    appliquerThemeSombre(dark);
    loadData();
    refreshTable();
}

void MainWindow::setupUi() {
    setWindowTitle("Gestion des Etudiants");
    resize(900, 600);

    QWidget* central = new QWidget();
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    titleLabel = new QLabel("<b>Systeme de Gestion des Etudiants</b>");
    subtitleLabel = new QLabel("");
    
    QVBoxLayout* titleLayout = new QVBoxLayout();
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);
    
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    QHBoxLayout* controlsLayout = new QHBoxLayout();
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Rechercher par nom, ID, type...");
    
    typeFilter = new QComboBox();
    typeFilter->addItem("Tous");
    typeFilter->addItem("Licence");
    typeFilter->addItem("Master");
    typeFilter->addItem("Doctorat");

    addButton = new QPushButton("Ajouter");
    editButton = new QPushButton("Modifier");
    deleteButton = new QPushButton("Supprimer");
    refreshButton = new QPushButton("Actualiser");
    exportButton = new QPushButton("Exporter");
    themeButton = new QPushButton("Mode sombre");

    controlsLayout->addWidget(searchEdit, 2);
    controlsLayout->addWidget(typeFilter, 1);
    controlsLayout->addWidget(addButton);
    controlsLayout->addWidget(editButton);
    controlsLayout->addWidget(deleteButton);
    controlsLayout->addWidget(refreshButton);
    controlsLayout->addWidget(exportButton);
    controlsLayout->addWidget(themeButton);

    mainLayout->addLayout(controlsLayout);

    table = new QTableWidget();
    table->setColumnCount(6);
    QStringList headers;
    headers << "ID" << "Nom" << "Type" << "Moyenne" << "Bourse" << "Details";
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSortingEnabled(true);
    
    mainLayout->addWidget(table, 1);

    statusBar()->showMessage("Pret.");

    connect(addButton, &QPushButton::clicked, this, &MainWindow::ajouterEtudiant);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::modifierEtudiant);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::supprimerEtudiant);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::actualiserDonnees);
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::exporterCsv);
    connect(themeButton, &QPushButton::clicked, this, &MainWindow::basculerModeSombre);

    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::refreshTable);
    connect(typeFilter, &QComboBox::currentTextChanged, this, &MainWindow::refreshTable);

    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) {
        modifierEtudiant();
    });
}

void MainWindow::loadData() {
    StudentManager loadedManager;
    try {
        PersistenceManager::load(loadedManager, dataPath.toStdString());
        manager = loadedManager;
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Erreur de chargement", QString("Erreur lors du chargement : ") + e.what());
    }
}

void MainWindow::saveData() {
    PersistenceManager::save(manager, dataPath.toStdString());
}

QString MainWindow::getStudentDetails(std::shared_ptr<Student> student) const {
    if (!student) return "";

    if (student->getType() == "Licence") {
        auto u = std::dynamic_pointer_cast<UndergraduateStudent>(student);
        if (u) return "Filiere : " + QString::fromStdString(u->getFiliere());
    }
    else if (student->getType() == "Master") {
        auto g = std::dynamic_pointer_cast<GraduateStudent>(student);
        if (g) return "Sujet : " + QString::fromStdString(g->getSujetRecherche());
    }
    else if (student->getType() == "Doctorat") {
        auto p = std::dynamic_pointer_cast<PhDStudent>(student);
        if (p) return "Encadrant : " + QString::fromStdString(p->getEncadrant()) + " | Annee : " + QString::number(p->getAnneeEtude());
    }
    return "";
}

bool MainWindow::matchesFilters(std::shared_ptr<Student> student) const {
    if (!student) return false;

    QString selectedType = typeFilter->currentText();
    QString search = searchEdit->text().trimmed().toLower();

    QString id = QString::number(student->getId());
    QString nom = QString::fromStdString(student->getNom()).toLower();
    QString type = QString::fromStdString(student->getType()).toLower();
    QString moyenne = QString::number(student->getMoyenne(), 'f', 2);
    QString bourse = QString::number(student->calculerBourse(), 'f', 2);
    QString details = getStudentDetails(student).toLower();

    if (selectedType != "Tous" && QString::fromStdString(student->getType()) != selectedType) {
        return false;
    }

    if (search.isEmpty()) return true;

    return id.contains(search) || nom.contains(search) || type.contains(search) ||
           moyenne.contains(search) || bourse.contains(search) || details.contains(search);
}

void MainWindow::refreshTable() {
    table->setSortingEnabled(false);
    table->setRowCount(0);

    for (const auto& pair : manager.obtenirListe()) {
        std::shared_ptr<Student> student = pair.second;
        if (!matchesFilters(student)) continue;

        int row = table->rowCount();
        table->insertRow(row);

        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(student->getId()));
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(student->getNom()));
        QTableWidgetItem* typeItem = new QTableWidgetItem(QString::fromStdString(student->getType()));
        QTableWidgetItem* gpaItem = new QTableWidgetItem(QString::number(student->getMoyenne(), 'f', 2));
        QTableWidgetItem* scholarshipItem = new QTableWidgetItem(QString::number(student->calculerBourse(), 'f', 2) + " EUR");
        QTableWidgetItem* detailsItem = new QTableWidgetItem(getStudentDetails(student));

        idItem->setData(Qt::UserRole, student->getId());

        table->setItem(row, 0, idItem);
        table->setItem(row, 1, nameItem);
        table->setItem(row, 2, typeItem);
        table->setItem(row, 3, gpaItem);
        table->setItem(row, 4, scholarshipItem);
        table->setItem(row, 5, detailsItem);
    }

    table->setSortingEnabled(true);
    updateStatus();
}

void MainWindow::updateStatus() {
    QString message = QString("Total: %1 etudiant(s) | Affiches: %2 | Moyenne generale: %3")
        .arg(manager.obtenirNombreTotal())
        .arg(table->rowCount())
        .arg(manager.calculerMoyenneGenerale(), 0, 'f', 2);
    statusBar()->showMessage(message);
}

int MainWindow::getSelectedId() const {
    int row = table->currentRow();
    if (row < 0) return -1;
    QTableWidgetItem* item = table->item(row, 0);
    if (!item) return -1;
    return item->data(Qt::UserRole).toInt();
}

void MainWindow::ajouterEtudiant() {
    StudentDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            manager.ajouterEtudiant(dialog.getStudent());
            saveData();
            refreshTable();
            statusBar()->showMessage("Etudiant ajoute avec succes.", 3000);
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Erreur", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::modifierEtudiant() {
    int id = getSelectedId();
    if (id == -1) {
        QMessageBox::information(this, "Selection requise", "Veuillez selectionner un etudiant a modifier.");
        return;
    }

    try {
        std::shared_ptr<Student> existing = manager.chercherParId(id);
        StudentDialog dialog(this, existing);
        if (dialog.exec() == QDialog::Accepted) {
            manager.modifierEtudiant(id, dialog.getStudent());
            saveData();
            refreshTable();
            statusBar()->showMessage("Etudiant modifie avec succes.", 3000);
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur", QString::fromStdString(e.what()));
    }
}

void MainWindow::supprimerEtudiant() {
    int id = getSelectedId();
    if (id == -1) {
        QMessageBox::information(this, "Selection requise", "Veuillez selectionner un etudiant a supprimer.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmation", "Etes-vous sur de vouloir supprimer cet etudiant ?", QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    try {
        manager.supprimerEtudiant(id);
        saveData();
        refreshTable();
        statusBar()->showMessage("Etudiant supprime avec succes.", 3000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur", QString::fromStdString(e.what()));
    }
}

void MainWindow::actualiserDonnees() {
    loadData();
    refreshTable();
    statusBar()->showMessage("Donnees rechargees depuis le fichier.", 3000);
}

void MainWindow::exporterCsv() {
    QString filePath = QFileDialog::getSaveFileName(this, "Exporter en CSV", "etudiants_export.csv", "Fichiers CSV (*.csv)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur d'exportation", "Impossible de creer le fichier CSV.");
        return;
    }

    QTextStream out(&file);
    out << "ID,Nom,Type,Moyenne,Bourse,Details\n";
    for (const auto& pair : manager.obtenirListe()) {
        std::shared_ptr<Student> student = pair.second;
        out << student->getId() << ",\""
            << QString::fromStdString(student->getNom()) << "\",\""
            << QString::fromStdString(student->getType()) << "\","
            << QString::number(student->getMoyenne(), 'f', 2) << ","
            << QString::number(student->calculerBourse(), 'f', 2) << ",\""
            << getStudentDetails(student) << "\"\n";
    }
    file.close();
    QMessageBox::information(this, "Exportation CSV", "Donnees exportees avec succes.");
}

void MainWindow::basculerModeSombre() {
    QSettings settings(QCoreApplication::applicationName(), QCoreApplication::applicationName());
    bool current = settings.value("theme/dark", false).toBool();
    bool next = !current;
    settings.setValue("theme/dark", next);
    appliquerThemeSombre(next);
}

void MainWindow::appliquerThemeSombre(bool activer) {
    if (activer) {
        // Simple dark stylesheet
        QString dark = R"(
            QWidget { background: #2b2b2b; color: #dcdcdc; }
            QTableWidget { background: #333333; gridline-color: #3c3c3c; }
            QHeaderView::section { background: #3c3c3c; color: #ffffff; }
            QPushButton { background-color: #444444; color: #ffffff; border: 1px solid #5a5a5a; padding: 4px; }
            QLineEdit, QComboBox { background: #3a3a3a; color: #ffffff; }
            QStatusBar { background: #2b2b2b; color: #dcdcdc; }
            QMessageBox { background: #2b2b2b; color: #dcdcdc; }
        )";
        qApp->setStyleSheet(dark);
        themeButton->setText("Mode sombre: ON");
    } else {
        qApp->setStyleSheet("");
        themeButton->setText("Mode sombre");
    }
}