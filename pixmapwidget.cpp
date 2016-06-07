#include "pixmapwidget.h"
#include <QPainter>
#include <QWheelEvent>

PixmapWidget::PixmapWidget(QWidget* parent) : QLabel(parent)
{
    zoomFactor = 1.2f;
    hasText = true;
    text = QString(tr("<Перетащите_изображение>"));
    doWheelEvent = false;

    setMinimumSize( qimage.width()*zoomFactor, qimage.height()*zoomFactor );
}

void PixmapWidget::set_qimage0(const QImage& qimage1)
{
    hasText = false;
    doWheelEvent = true;
    if( qimage != qimage1 )
    {
        qimage = qimage1;
    }
    int w, h;
    w = qimage.width()*zoomFactor;
    h = qimage.height()*zoomFactor;
    setMinimumSize(w,h);

    QWidget* p = dynamic_cast<QWidget*>( parent() );
    if( p != NULL )
    {
        resize( p->width(), p->height() );
    }

    update();

    return;
}

void PixmapWidget::set_qimage(const QImage& qimage1)
{
    hasText = false;
    if( qimage != qimage1 )
    {
        qimage = qimage1;
    }
    update();

    return;
}

void PixmapWidget::setZoomFactor(float f)
{

    if( !hasText )
    {
        int w, h;

        if( f == zoomFactor )
        {
            return;
        }

        zoomFactor = f;
        emit( zoomFactorChanged(zoomFactor) );

        w = qimage.width()*zoomFactor;
        h = qimage.height()*zoomFactor;
        setMinimumSize(w,h);

        QWidget* p = dynamic_cast<QWidget*>( parent() );
        if( p == NULL )
        {
            resize( p->width(), p->height() );
        }

        update();
    }

    return;
}

void PixmapWidget::paintEvent(QPaintEvent* event)
{

    if( width() > qimage.width()*zoomFactor )
    {
        xoffset = (width()-qimage.width()*zoomFactor)/2;
    }
    else
    {
        xoffset = 0;
    }

    if( height() > qimage.height()*zoomFactor )
    {
        yoffset = (height()-qimage.height()*zoomFactor)/2;
    }
    else
    {
        yoffset = 0;
    }

    QPainter p(this);
    p.save();

    if( hasText )
    {
        p.drawText(width()/2-40, height()/2,text);
    }
    else
    {
        p.translate(xoffset,yoffset);
        p.scale(zoomFactor,zoomFactor);
        p.drawImage(0,0,qimage);
        //p.drawPixmap(0,0,QPixmap::fromImage(qimage));
    }

    p.restore();

    return;
}



int PixmapWidget::getPixWidth() const
{
    return int( float( qimage.width() ) * zoomFactor );
}

int PixmapWidget::getPixHeight() const
{
    return int( float( qimage.height() ) * zoomFactor );
}

int PixmapWidget::get_xoffset() const
{
    return xoffset;
}

int PixmapWidget::get_yoffset() const
{
    return yoffset;
}

void PixmapWidget::set_hasText(bool hasText1)
{
    hasText = hasText1;
    return;
}

void PixmapWidget::set_text(QString text1)
{
    text = text1;
    return;
}

void PixmapWidget::set_zoomFactor(float zoomFactor1)
{
    zoomFactor = zoomFactor1;
    return;
}

float PixmapWidget::get_zoomFactor() const
{
    return zoomFactor;
}

void PixmapWidget::set_doWheelEvent(bool doWheelEvent1)
{
    doWheelEvent = doWheelEvent1;
    return;
}

const QImage& PixmapWidget::get_qimage() const
{
    return qimage;
}
