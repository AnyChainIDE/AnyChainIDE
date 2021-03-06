#ifndef CUSTOMSHADOWEFFECT_H
#define CUSTOMSHADOWEFFECT_H

#include <QColor>
#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEffect>
#include <QPainter>

class CustomShadowEffect : public QGraphicsEffect
{
	Q_OBJECT

public:
	CustomShadowEffect(QObject *parent=0);
	~CustomShadowEffect();
	void draw(QPainter* painter);
	QRectF boundingRectFor(const QRectF& rect) const;

	inline void setDistance(qreal distance) { _distance = distance; updateBoundingRect(); }
	inline qreal distance() const { return _distance; }

	inline void setBlurRadius(qreal blurRadius) { _blurRadius = blurRadius; updateBoundingRect(); }
	inline qreal blurRadius() const { return _blurRadius; }

	inline void setColor(const QColor& color) { _color = color; }
	inline QColor color() const { return _color; }

private:
	qreal  _distance;
	qreal  _blurRadius;
	QColor _color;
};

static void installBlurEffect(QWidget *widget,double radius=20.0,double distance=5.0,const QColor &color=QColor(150, 150, 150, 35))
{
    if(!widget) return;
    CustomShadowEffect *bodyShadow = new CustomShadowEffect(widget);
    bodyShadow->setBlurRadius(radius);
    bodyShadow->setDistance(distance);
    bodyShadow->setColor(color);
    widget->setGraphicsEffect(bodyShadow);
}
#endif //CUSTOMSHADOWEFFECT
