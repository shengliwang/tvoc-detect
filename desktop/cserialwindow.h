#ifndef CSERIALWINDOW_H
#define CSERIALWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QTextBrowser>

class CSerialWindow : public QWidget
{
    Q_OBJECT
public:
    explicit CSerialWindow(QWidget *parent = nullptr);
    ~CSerialWindow();

private:
    QTextBrowser * m_txtEdit;
    QHBoxLayout * m_horizontalLayout;

protected:
    void closeEvent(QCloseEvent * ev);

signals:
    void sigClosed();

private slots:
    void onSerialData(QString);
};

#endif // CSERIALWINDOW_H
