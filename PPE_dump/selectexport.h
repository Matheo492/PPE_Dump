#ifndef SELECTEXPORT_H
#define SELECTEXPORT_H

#include <QDialog>

namespace Ui {
class selectexport;
}

class selectexport : public QDialog
{
    Q_OBJECT

public:
    struct Expinfo {
        QString name;
        QStringList typelt;
        int largeur, hauteur;
        int type;
        int data;
        bool ok;
    };
    explicit selectexport(QWidget *parent = 0);
    ~selectexport();

    Expinfo expinfo() const;

private slots:

    void on_buttonBox_accepted();

    void on_type_currentIndexChanged(int index);

private:
    Ui::selectexport *ui;
    Expinfo m_info;
};

#endif // SELECTEXPORT_H
