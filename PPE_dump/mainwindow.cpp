#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"

#include "settingsdialog.h"
#include "selectexport.h"
#include <QtSerialPort/QSerialPort>
#include <math.h>


QTime temps;
bool SerialListen(false);
float value;
QByteArray v;
bool firstvalue = true;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
  m_settings(new SettingsDialog),
m_export(new selectexport)
{
    ui->setupUi(this);
    setWindowTitle("PPE Dump");
    setMinimumSize(size());
    setMaximumSize(size());
    ui->dpvvent->setBackgroundRole(QPalette::Base);
    ui->dpvvent->setEnabled(false);
    ui->plot->addGraph();
    ui->vplot->addGraph();
    ui->pushButton->setEnabled(false);
    ui->plot->graph(0)->setPen(QPen(Qt::blue));
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot->xAxis->setLabel("Temps (s)");
    ui->plot->yAxis->setLabel("Ecart angulaire (°)");
    ui->vplot->graph(0)->setPen(QPen(Qt::blue));
    ui->vplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->vplot->xAxis->setLabel("Temps (s)");
    ui->vplot->yAxis->setLabel("Vitesse (m/s)");
    arduino.setPortName("NA");
    connect(&arduino, SIGNAL(readyRead()), this, SLOT(serialEvent()));
    connect(ui->widget, SIGNAL(&Console::getData), this, SLOT(MainWindow::writeData()));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_settings;
    if (arduino.isOpen())
    {
            arduino.close();
    }
}

void MainWindow::clearData()
{
    qv_x.clear();
    qv_y.clear();
    bv_y.clear();
    ui->dpvvent->display(0);
}

void MainWindow::writeData(const QByteArray &data)
{
    if (arduino.isOpen())
    {
            arduino.write(data);
    }
}

void MainWindow::plot()
{
    ui->plot->graph(0)->setData(qv_x, qv_y);
    ui->plot->graph(0)->rescaleAxes();
    ui->plot->replot();
    ui->plot->update();
    ui->vplot->graph(0)->setData(qv_x, bv_y);
    ui->vplot->graph(0)->rescaleAxes();
    ui->vplot->replot();
    ui->vplot->update();
}

void MainWindow::on_actionQuitter_triggered()
{
   QApplication::quit();
}

void MainWindow::on_Start_clicked()
{
    if(!SerialListen)
    {
    if (QString::compare(arduino.portName(), "NA", Qt::CaseInsensitive) == 0)
    {
        QMessageBox::information(this, "Choisir le port...", "Merci de sélectionner le port de l'arduino et ses caractéristiques");
        MainWindow::on_actionPorts_triggered();
    }
    SerialListen = true;
    ui->Start->setText("Stop");
    if (arduino.isOpen())
    {
    ui->pushButton->setEnabled(true);
    ui->dpvvent->setEnabled(true);

    }
    }
    else
    {
        SerialListen = false;
        ui->Start->setText("Start");
         ui->pushButton->setEnabled(false);
         ui->dpvvent->setEnabled(false);

    }
}

void MainWindow::on_Clear_clicked()
{
    clearData();
    plot();
    ui->widget->clear();
    firstvalue = true;
}

void MainWindow::on_actionPorts_triggered()
{
    if (arduino.isOpen())
    {
            arduino.close();
    }

    m_settings->exec();
    const SettingsDialog::Settings p = m_settings->settings();

    arduino.setPortName(p.name);
       arduino.setBaudRate(p.baudRate);
       arduino.setDataBits(p.dataBits);
       arduino.setParity(p.parity);
       arduino.setStopBits(p.stopBits);
       arduino.setFlowControl(p.flowControl);
       if (arduino.open(QIODevice::ReadWrite)) {


            QMessageBox::information(this, tr("Connecté"), tr("Connecté à %1 : %2, %3, %4, %5, %6")
                                 .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                 .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));

           ui->widget->setLocalEchoEnabled(p.localEchoEnabled);
           } else {
               QMessageBox::critical(this, tr("Erreur"), arduino.errorString());

               QMessageBox::critical(this, tr("Erreur"), tr("Erreur d'ouverture du stream"));
               arduino.setPortName("NA");
           }
}

void MainWindow::serialEvent()
{
    if (SerialListen && arduino.isOpen())
    {
     v.append(arduino.readAll());

    if (v.contains('\n'))
    {
    QString value = QString::fromUtf8(v.data());

    if (value.startsWith("$") && (value.endsWith("\n")))
    {
        value.remove(0,1);

        ui->widget->putDataStr(value);
        value.chop(1);
        if (firstvalue)
        {
            temps.start();
            temps.restart();
            firstvalue = false;
            qv_x.append(0);
        }
        else
        {
            qv_x.append( (float) temps.elapsed()/1000);
        }
        QStringList Dump = value.split("|");
        qv_y.append(Dump[1].toDouble());
        bv_y.append(Dump[0].toDouble());

        ui->dpvvent->display(Dump[0]);
        plot();
    }

    v.clear();
    }
    }
}


void MainWindow::on_pushButton_clicked()
{
    bool ok;
   double vvent = QInputDialog::getDouble(this, "Vvent", "Entrez la vitesse de la soufflerie", 0, 0, 10, 2, &ok,0, 0.01);
  if (ok)
  {

      int tr = int((820.27)/(1+0.00023*exp(1.25*vvent)));
     QByteArray tra = QByteArray::number(tr) + "\n";
    QString showph = QString("-- Valeur transmise: %1 m/s(%2) --\n").arg(vvent).arg(tr);
    ui->widget->putDataStr(showph);
   arduino.write(tra);
  }
}



void MainWindow::on_pushButton_2_clicked()
{
    if(ui->graph->isVisible())
    {
    ui->plot->graph(0)->rescaleAxes();
    ui->plot->replot();
    ui->plot->update();
    }
    else if(ui->vvent->isVisible())
    {
        ui->vplot->graph(0)->rescaleAxes();
        ui->vplot->replot();
        ui->vplot->update();
    }
}

void MainWindow::on_actionExportation_2_triggered()
{
    if(SerialListen)
    {
        QMessageBox::warning(this, "Erreur", "Merci d'arrêter la lecture avant d'exporter");
    }
    else
    {
        m_export->exec();
        const selectexport::Expinfo pr = m_export->expinfo();
        if(pr.ok)
        {
            switch (pr.type){
        case 0:
            if(pr.data == 1)
            {
                    ui->plot->savePdf(pr.name);
            }
            else
            {
                    ui->vplot->savePdf(pr.name);
        }
                break;
        case 1:
                    if(pr.data == 1)
                    {
                            ui->plot->saveJpg(pr.name, pr.largeur, pr.hauteur);

                    }
                    else
                    {
                            ui->vplot->saveJpg(pr.name, pr.largeur, pr.hauteur);
                }
                break;
        case 2:
                if(pr.data == 1)
                {
                    QFile file(pr.name);
                    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
                    {

                    QTextStream out(&file);
                    out << "Fichier contenant les valeurs numérique de theta(t). \nFormat: Theta   |   temps (s) \n";
                     if(!firstvalue)
                     {
                     for(int i = 0; i < qv_y.size(); i++)
                     {
                         out << qv_y.value(i) << "   |   " << qv_x.value(i) << "\n";
                     }
                     }
                     file.close();
                    }
                    else
                    {
                        QMessageBox::critical(this, "Erreur", "Enregistrement du fichier impossible");

                    }
                }
                else
                {
                    QFile file(pr.name);
                    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
                    {

                    QTextStream out(&file);
                    out << "Fichier contenant les valeurs numérique de v(t). \nFormat: Vitesse (m/s)   |   temps (s) \n";
                     if(!firstvalue)
                     {
                     for(int i = 0; i < qv_y.size(); i++)
                     {
                         out << bv_y.value(i) << "   |   " << qv_x.value(i) << "\n";
                     }
                     }
                     file.close();
                    }
                    else
                    {
                        QMessageBox::critical(this, "Erreur", "Enregistrement du fichier impossible");

                    }

                }
        break;
            }
        }
}
}
