#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QPainter>

#include "cmap.h"
#include "cserialwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&m_serial, SIGNAL(readyRead()), this, SLOT(onSerialReadData()));
    connect(ui->map, SIGNAL(sigAverageChange(QString)), ui->label_Average, SLOT(setText(QString)));
    connect(ui->map, SIGNAL(sigMaxChange(QString)), ui->label_Max, SLOT(setText(QString)));
    connect(ui->map, SIGNAL(sigMinChange(QString)), ui->label_Min, SLOT(setText(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushBtnScan_clicked(){
    qInfo("scan...");
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    QStringList portList;

    ui->comboBox_TTY->clear();
    for (int i = 0; i < list.size(); ++i){
        portList.append(list.at(i).portName());
    }
    ui->comboBox_TTY->addItems(portList);

    if (portList.isEmpty()){
        ui->pushBtn_Open->setEnabled(false);
    } else {
        ui->pushBtn_Open->setEnabled(true);
    }
}

QSerialPort::BaudRate MainWindow::getUISerialBaud(void){
    QString baud = ui->comboBox_Baud->currentText();
    QSerialPort::BaudRate rate = QSerialPort::Baud115200;
    if (baud == "115200"){
        rate = QSerialPort::Baud115200;
    } else if (baud == "1200"){
        rate = QSerialPort::Baud1200;
    } else if (baud == "2400"){
        rate = QSerialPort::Baud2400;
    } else if (baud == "2400"){
        rate = QSerialPort::Baud4800;
    } else if (baud == "2400"){
        rate = QSerialPort::Baud9600;
    } else if (baud == "2400"){
        rate = QSerialPort::Baud19200;
    } else if (baud == "2400"){
        rate = QSerialPort::Baud38400;
    } else if (baud == "2400"){
        rate = QSerialPort::Baud57600;
    } else {
        rate = QSerialPort::Baud1200;
    }

    return rate;
}

void MainWindow::on_pushBtn_Open_clicked(){
    QString portName = ui->comboBox_TTY->currentText();

    if (m_serial.isOpen()){
        m_serial.clear();
        m_serial.close();
    }

    m_serial.setPortName(portName);
    m_serial.setBaudRate(getUISerialBaud());

    m_serial.setDataBits(QSerialPort::Data8);
//    m_serial.setParity(QSerialPort::OddParity);
  //  m_serial.setStopBits(QSerialPort::OneStop);

    if (!m_serial.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,
                             "ERROR",
                             "Open " + portName + " failed!"
                             );
        return ;
    } else {
        qInfo("open %s success!", portName.toStdString().c_str());
        ui->pushBtn_Open->setEnabled(false);
        ui->pushBtn_Close->setEnabled(true);
    }


   // m_serial.close();
}

void MainWindow::on_pushBtn_Close_clicked(){
    if(m_serial.isOpen()){
        qInfo("close %s ...", m_serial.portName().toStdString().c_str());
        m_serial.clear();
        m_serial.close();

        if (ui->comboBox_TTY->count() != 0){
            ui->pushBtn_Open->setEnabled(true);
        } else {
            ui->pushBtn_Open->setEnabled(false);
        }
    }

    ui->pushBtn_Close->setEnabled(false);
}

void MainWindow::onSerialReadData(void){
    QByteArray data = m_serial.readAll();
    m_serialReadBuffer.append(data);
    emit sigSerialData(data);

    if ( -1 == m_serialReadBuffer.indexOf("\n")){
        return ;
    }

    while ( -1 != m_serialReadBuffer.indexOf("\n")){
        int idx = m_serialReadBuffer.indexOf("\n");
        QByteArray tmp = m_serialReadBuffer.left(idx+1);
        m_serialReadBuffer.remove(0, idx+1);

        QRegExp pattern("id=(\\d+), tvoc: (\\d+) ppb, co2: (\\d+) ppm");
        int pos = pattern.indexIn(tmp);

        if (pos >= 0){
            static int current_idx = 0;
            TvocData data;
            data.pkt_idx = current_idx++;
            data.real_idx = pattern.cap(1).toInt();
            data.tvoc = pattern.cap(2).toInt();
            data.co2 = pattern.cap(3).toInt();
            this->ui->map->addData(data);
            qInfo("%d, (%d) (%d)", data.real_idx, data.tvoc, data.co2);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent * ev){
    (void)ev;
    on_pushBtn_Close_clicked();
}

void MainWindow::onSerialWindowClosed(){
    qInfo("serial window closed");
    ui->pushBtn_asitant->setEnabled(true);
}

void MainWindow::on_pushBtn_asitant_clicked(){
    qInfo("open serial windows....");

    CSerialWindow * window = new CSerialWindow();
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->resize(500, 400);

    connect(this, SIGNAL(sigSerialData(QString)), window, SLOT(onSerialData(QString)));
    connect(window, SIGNAL(sigClosed()), this, SLOT(onSerialWindowClosed()));

    window->show();
    ui->pushBtn_asitant->setEnabled(false);
}

void MainWindow::on_pushBtn_Clear_clicked(){
    ui->map->clearData();
}
