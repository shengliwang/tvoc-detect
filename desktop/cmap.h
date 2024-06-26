#ifndef CMAP_H
#define CMAP_H

#include <QWidget>

struct TvocData {
    int pkt_idx;    // serial packet index
    int real_idx;   // idx in device
    int tvoc;
    int co2;
};


class CMap : public QWidget
{
    Q_OBJECT
public:
    explicit CMap(QWidget *parent = nullptr);
    ~CMap();

private:
    QVector<TvocData> m_stTvocData;
    int m_ymargin;   // will auto fit to y axis.
    const int m_xmargin = 20;
    const int m_ynum = 5;

    double m_xscale;  // will auto update.
    double m_yscale; // will auto update.

public:
    void addData(const TvocData & data);
    void clearData(void);

private:
    void getDataLastIdx(int * begin, int * end);
    void drawRect();    // juxing
    void drawAxies();
    void drawTvocData();

protected:
    void paintEvent(QPaintEvent * ev);

signals:
    void sigAverageChange(QString);
    void sigMaxChange(QString);
    void sigMinChange(QString);
};

#endif // CMAP_H
