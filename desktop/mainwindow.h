#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

#include "cmap.h"
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void on_pushBtnScan_clicked();
    void on_pushBtn_Open_clicked();
    void on_pushBtn_Close_clicked();
    void on_pushBtn_asitant_clicked();
    void on_pushBtn_Clear_clicked();

    void onSerialReadData(void);
    void onSerialWindowClosed();

private:
    QSerialPort m_serial;
    QByteArray m_serialReadBuffer;

private:
    QSerialPort::BaudRate getUISerialBaud(void);

protected:
    void closeEvent(QCloseEvent * ev);
signals:
    void sigSerialData(QString);
};
#endif // MAINWINDOW_H
