#pragma once

#include <QGraphicsView>

/**
 * @brief Класс представления (View) на основе QGraphicsView.
 *
 * Этот класс предоставляет функциональность для отображения графических сцен с возможностью масштабирования и обработкой событий колесика мыши.
 */
class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QWidget *parent = nullptr);

    void resetZoom();

protected:
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif

private:
    int m_zoom; /**< Текущее значение зума. */
    int m_maxZoom;  /**< Максимальное значение зума. */
    int m_minZoom;  /**< Минимальное значение зума. */

    void setupMatrix();
    void zoomOutBy(int level);
    void zoomInBy(int level);
};
