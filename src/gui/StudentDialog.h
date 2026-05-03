#ifndef STUDENT_DIALOG_H
#define STUDENT_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

#include <memory>
#include "../models/Student.h"

class StudentDialog : public QDialog {
    Q_OBJECT

private:
    QComboBox* typeCombo;
    QLineEdit* nameEdit;
    QLineEdit* idEdit;
    QDoubleSpinBox* gpaSpin;

    QLabel* extra1Label;
    QLineEdit* extra1Edit;

    QLabel* extra2Label;
    QSpinBox* yearSpin;

    QPushButton* validateButton;
    QPushButton* cancelButton;

    std::shared_ptr<Student> studentResult;

    void setupUi();
    void updateExtraFields();
    void fillFromStudent(std::shared_ptr<Student> student);

protected:
    void accept() override;

public:
    explicit StudentDialog(QWidget* parent = nullptr);
    StudentDialog(QWidget* parent, std::shared_ptr<Student> student);

    std::shared_ptr<Student> getStudent() const;
};

#endif