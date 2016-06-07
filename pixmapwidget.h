#ifndef PIXMAPWIDGET_H
#define PIXMAPWIDGET_H

#include <QLabel>
#include <QString>
#include <QScrollArea>

class QPixmap;
class QScollArea;


class PixmapWidget : public QLabel
{
    Q_OBJECT

public :
    PixmapWidget( QWidget* parent = 0 );
    void set_qimage0(const QImage& qimage1);
    void set_qimage(const QImage& qimage1);
    void set_doWheelEvent(bool doWheelEvent1);
    void set_zoomFactor(float zoomFactor1);
    float get_zoomFactor() const;
    int getPixWidth() const;
    int getPixHeight() const;
    int get_xoffset() const;
    int get_yoffset() const;
    void set_hasText(bool hasText1);
    const QImage& get_qimage() const;
    void set_text(QString text1);

public slots :
    void setZoomFactor(float f);

signals :
    void zoomFactorChanged(float f);

protected :
    void paintEvent(QPaintEvent* event);

private :
    QImage qimage;
    float zoomFactor;
    int xoffset;
    int yoffset;
    bool hasText;
    QString text;
    bool doWheelEvent;
};

#endif // PIXMAPWIDGET_H
