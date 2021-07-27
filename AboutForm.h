#pragma once

#include <QDialog>
#include "ui_AboutForm.h"

class AboutForm : public QDialog, public Ui::AboutForm
{
	Q_OBJECT

public:
	AboutForm(QWidget *parent = Q_NULLPTR);
	~AboutForm() = default;
};
