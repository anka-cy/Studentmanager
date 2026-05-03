#include "StudentDialog.h"

#include "../models/UndergraduateStudent.h"
#include "../models/GraduateStudent.h"
#include "../models/PhDStudent.h"
#include "../exceptions/StudentExceptions.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QIntValidator>

StudentDialog::StudentDialog(QWidget* parent)
    : QDialog(parent), studentResult(nullptr) {
    setupUi();
}

StudentDialog::StudentDialog(QWidget* parent, std::shared_ptr<Student> student)
    : QDialog(parent), studentResult(nullptr) {
    setupUi();
    fillFromStudent(student);
}

void StudentDialog::setupUi() {
    setWindowTitle("Formulaire Etudiant");
    setMinimumWidth(420);
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(18, 18, 18, 18);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(10);

    typeCombo = new QComboBox();
    typeCombo->addItem("Licence");
    typeCombo->addItem("Master");
    typeCombo->addItem("Doctorat");

    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Ex: Dupont Marie");

    idEdit = new QLineEdit();
    idEdit->setPlaceholderText("Ex: 1001 (>= 1000)");
    idEdit->setValidator(new QIntValidator(1000, 999999999, this));

    gpaSpin = new QDoubleSpinBox();
    gpaSpin->setRange(0.0, 4.0);
    gpaSpin->setSingleStep(0.1);
    gpaSpin->setDecimals(2);

    extra1Label = new QLabel("Filiere");
    extra1Edit = new QLineEdit();

    extra2Label = new QLabel("Annee d'etude");
    yearSpin = new QSpinBox();
    yearSpin->setRange(1, 10);

    formLayout->addRow("Type", typeCombo);
    formLayout->addRow("Nom complet", nameEdit);
    formLayout->addRow("Identifiant", idEdit);
    formLayout->addRow("Moyenne", gpaSpin);
    formLayout->addRow(extra1Label, extra1Edit);
    formLayout->addRow(extra2Label, yearSpin);

    mainLayout->addLayout(formLayout);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();

    cancelButton = new QPushButton("Annuler");
    validateButton = new QPushButton("Valider");

    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(validateButton);

    mainLayout->addLayout(buttonsLayout);

    connect(typeCombo, &QComboBox::currentTextChanged, this, &StudentDialog::updateExtraFields);
    connect(cancelButton, &QPushButton::clicked, this, &StudentDialog::reject);
    connect(validateButton, &QPushButton::clicked, this, &StudentDialog::accept);

    updateExtraFields();
}

void StudentDialog::updateExtraFields() {
    QString type = typeCombo->currentText();

    if (type == "Licence") {
        extra1Label->setText("Filiere");
        extra1Edit->setPlaceholderText("Ex: Informatique");
        extra2Label->hide();
        yearSpin->hide();
    }
    else if (type == "Master") {
        extra1Label->setText("Sujet de recherche");
        extra1Edit->setPlaceholderText("Ex: Intelligence Artificielle");
        extra2Label->hide();
        yearSpin->hide();
    }
    else {
        extra1Label->setText("Encadrant");
        extra1Edit->setPlaceholderText("Ex: Dr. Martin");
        extra2Label->show();
        yearSpin->show();
    }
}

void StudentDialog::fillFromStudent(std::shared_ptr<Student> student) {
    if (!student) return;

    typeCombo->setCurrentText(QString::fromStdString(student->getType()));
    nameEdit->setText(QString::fromStdString(student->getNom()));
    idEdit->setText(QString::number(student->getId()));
    gpaSpin->setValue(student->getMoyenne());

    if (student->getType() == "Licence") {
        auto u = std::dynamic_pointer_cast<UndergraduateStudent>(student);
        if (u) {
            extra1Edit->setText(QString::fromStdString(u->getFiliere()));
        }
    }
    else if (student->getType() == "Master") {
        auto g = std::dynamic_pointer_cast<GraduateStudent>(student);
        if (g) {
            extra1Edit->setText(QString::fromStdString(g->getSujetRecherche()));
        }
    }
    else if (student->getType() == "Doctorat") {
        auto p = std::dynamic_pointer_cast<PhDStudent>(student);
        if (p) {
            extra1Edit->setText(QString::fromStdString(p->getEncadrant()));
            yearSpin->setValue(p->getAnneeEtude());
        }
    }

    updateExtraFields();
}

void StudentDialog::accept() {
    QString type = typeCombo->currentText();
    QString nom = nameEdit->text().trimmed();
    QString idText = idEdit->text().trimmed();
    QString extra1 = extra1Edit->text().trimmed();

    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le nom ne peut pas etre vide.");
        return;
    }

    if (idText.isEmpty()) {
        QMessageBox::warning(this, "Validation", "L'identifiant ne peut pas etre vide.");
        return;
    }

    if (extra1.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le champ supplementaire ne peut pas etre vide.");
        return;
    }

    bool ok = false;
    int id = idText.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Validation", "L'identifiant doit etre un nombre valide.");
        return;
    }

    float moyenne = static_cast<float>(gpaSpin->value());

    try {
        if (type == "Licence") {
            studentResult = std::make_shared<UndergraduateStudent>(
                nom.toStdString(), id, moyenne, extra1.toStdString()
            );
        }
        else if (type == "Master") {
            studentResult = std::make_shared<GraduateStudent>(
                nom.toStdString(), id, moyenne, extra1.toStdString()
            );
        }
        else {
            studentResult = std::make_shared<PhDStudent>(
                nom.toStdString(), id, moyenne, extra1.toStdString(), yearSpin->value()
            );
        }

        QDialog::accept();
    } catch (const DuplicateIDException& e) {
        QMessageBox::critical(this, "Erreur", QString::fromStdString(e.what()));
    } catch (const InvalidGradeException& e) {
        QMessageBox::warning(this, "Erreur Moyenne", QString::fromStdString(e.what()));
    } catch (const InvalidIDException& e) {
        QMessageBox::critical(this, "Erreur ID", QString::fromStdString(e.what()));
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur", QString::fromStdString(e.what()));
    }
}

std::shared_ptr<Student> StudentDialog::getStudent() const {
    return studentResult;
}
