#include "selectexport.h"
#include "ui_selectexport.h"
#include <QFileDialog>

selectexport::selectexport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectexport)
{
    ui->setupUi(this);
    ui->largeur->setValidator( new QIntValidator(0, 1920, this) );;
    ui->largeur->setText("1920");
    m_info.typelt << "pdf" << "jpg" << "txt";

    ui->hauteur->setValidator( new QIntValidator(0, 1080, this) );
    ui->hauteur->setText("1080");
}

selectexport::~selectexport()
{
    delete ui;
}

selectexport::Expinfo selectexport::expinfo() const
{
    return m_info;
}

void selectexport::on_buttonBox_accepted()
{
    m_info.type = ui->type->currentIndex();
    QString tp = m_info.typelt.at(m_info.type);
   QString file = QFileDialog::getSaveFileName(this, "Exporter en " + tp + "...", QString(), "Fichiers " + tp + " (*."+ tp + ")");
   m_info.name = file;
   m_info.data = ui->data->currentIndex();
   m_info.hauteur = ui->hauteur->text().toInt();
   m_info.largeur = ui->largeur->text().toInt();
   m_info.ok = true;
   hide();
}

void selectexport::on_type_currentIndexChanged(int index)
{
    if(index == 1)
    {
        ui->largeur->setEnabled(true);
        ui->hauteur->setEnabled(true);
    }
    else
    {
        ui->largeur->setEnabled(false);
        ui->hauteur->setEnabled(false);
    }
}
