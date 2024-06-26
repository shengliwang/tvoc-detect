#include "cmap.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

#define MAX_DRAW_DATA_NUM 128
#define ALIGN(v, unit) ( ((v)/(unit) + ((v)%(unit)?1:0) )*(unit) )

CMap::CMap(QWidget *parent)
    : QWidget{parent}
{
    //this->setStyleSheet(QString::fromUtf8("border:1px solid rgb(0, 0, 0)"));
}

CMap::~CMap()
{
}


void CMap::addData(const TvocData & data){

    TvocData tmpdata = data;

    m_stTvocData.push_back(tmpdata); // todo: should set max cached num.

    int min = 10000, max = 0, average = 0;

    int sum = 0;
    int n = m_stTvocData.size();
    for (int i = 0; i < n; ++i){
        int tvoc = m_stTvocData.at(i).tvoc;
        min = qMin(tvoc, min);
        max = qMax(tvoc, max);
        sum += tvoc;
    }

    if (0 != n){
        average = sum / n;
    }

    emit sigMaxChange(QString("%1").arg(max));
    emit sigMinChange(QString("%1").arg(min));
    emit sigAverageChange(QString("%1").arg(average));

    update();
}

void CMap::clearData(void){
    this->m_stTvocData.clear();
    update();
}

void CMap::drawRect(){
    QPainter painter(this);
    painter.setPen(QColor(0, 0, 0)); // black

    int w = this->width();
    int h = this->height();
    painter.drawRect(0, 0, w-1, h-1);
}

static int numofint(int i){
    int num = 0;
    while(i != 0){
        i /= 10;
        ++num;
    }
    return num;
}

void CMap::getDataLastIdx(int * begin, int * end){
    if (m_stTvocData.size() <= MAX_DRAW_DATA_NUM){
        *begin = 0;
    } else {
        *begin = m_stTvocData.size() - MAX_DRAW_DATA_NUM;
    }

    *end = m_stTvocData.size();
}

void CMap::drawAxies(){
    QPainter painter(this);
    painter.setPen(QColor(255, 0, 0)); // red

    int w = this->width();
    int h = this->height();

    int begin, end;
    getDataLastIdx(&begin, &end);

   // draw y ruler
    int max = 60;
    for (int i = begin; i < end; ++i){
        int tvoc = m_stTvocData.at(i).tvoc;
        max = qMax(tvoc, max);
    }
    max = ALIGN(max, max/m_ynum);   // alignment
    m_ymargin = 10 * numofint(max);  // update ymargin to fit ruler.
    int ystep = max / (m_ynum);
    m_yscale = (double)(h - 2 * m_xmargin) / max;  // update yscale
    qInfo("max=%d, ystep=%d, yscale=%f, h=%d", max, ystep, m_yscale, h);
    painter.drawText(m_ymargin - m_ymargin/1.2, h - m_xmargin/1.75, "0");
    for (int i = 0; i < m_ynum; ++i){
        int x = m_ymargin;
        int y = h - m_xmargin - (int)((i + 1) * ystep * m_yscale);
        painter.drawText(x-m_ymargin/1.2, y+m_ymargin/5, QString("%1").arg((i + 1) * ystep));
        painter.drawLine(x, y, x+2, y);
    }

    // draw y axis (5, 5) (5, h - 5)
    QPoint lineyPt1(m_ymargin, m_xmargin);
    QPoint lineyPt2(m_ymargin, h - m_xmargin);
    painter.drawLine(lineyPt1, lineyPt2);

    // draw x axis (5, h-5) (w-5, h-5)
    QPoint linexPt1(m_ymargin, h - m_xmargin);
    QPoint linexPt2(w - m_xmargin, h - m_xmargin);
    painter.drawLine(linexPt1, linexPt2);

    // draw x ruler
    m_xscale = (double)(w - m_xmargin - m_ymargin) / (MAX_DRAW_DATA_NUM); // update xscale
    for (int i = 0; i < MAX_DRAW_DATA_NUM; ++i){
        int x = (i+1)*m_xscale + m_ymargin;
        int y = h - m_xmargin;

        painter.setPen(QPen(Qt::red, 1));
        if ( (i + 1) % 10 == 0){
            painter.drawText(x - 8, y + 15, QString("%1").arg(i+1));
            painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(x, y, x, y - 4);
        } else {
            painter.setPen(QPen(Qt::red, 1));
            painter.drawLine(x, y, x, y - 2);
        }
    }
}


void CMap::drawTvocData(){
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 1)); // blue

    int begin, end;
    getDataLastIdx(&begin, &end);

    QPoint prevPt;

    for (int i = begin, j = 0; i < end; ++i, ++j){
        int tvoc = m_stTvocData.at(i).tvoc;

        int x = m_ymargin + (j+1) * m_xscale;
        int y = this->height() - m_xmargin - tvoc * m_yscale;

        QPoint currPt(x, y);
        painter.setPen(QPen(Qt::blue, 3));
        painter.drawPoint(currPt);

        if (0 != j){
            painter.setPen(QPen(Qt::black, 1));
            painter.drawLine(currPt, prevPt);
        }

        prevPt = currPt;
    }
}

void CMap::paintEvent(QPaintEvent *event){
    (void)event;
    static int i = 0;
    qInfo("update... %d", i++);

    drawRect();
    drawAxies();
    drawTvocData();
}
