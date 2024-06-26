#include "cserialwindow.h"

#include <QTextEdit>
#include <QLayout>
#include <QDebug>
#include <QTextBrowser>
#include <QTextCursor>

CSerialWindow::CSerialWindow(QWidget *parent)
    : QWidget{parent}
{
    m_txtEdit = new QTextBrowser(this);
    m_horizontalLayout = new QHBoxLayout(this);

    m_horizontalLayout->addWidget(m_txtEdit);
}

CSerialWindow::~CSerialWindow(){
    delete m_txtEdit;
    delete m_horizontalLayout;
}

void CSerialWindow::closeEvent(QCloseEvent * ev){
    (void)ev;
    emit sigClosed();
}

void CSerialWindow::onSerialData(QString str){
    m_txtEdit->moveCursor(m_txtEdit->textCursor().End);
    m_txtEdit->insertPlainText(str);
}
