#pragma once

#include <QGraphicsScene>

class Box;

/**
 * @brief Класс графической сцены (Scene) на основе QGraphicsScene.
 *
 * Этот класс предоставляет функциональность для управления графическими элементами сцены,
 * включая создание, удаление и анимацию элементов.
 */
class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Scene(QObject *parent = nullptr);

    Box *startItem() const;
    Box *endItem() const;

    void clearScene(bool all = true);
    void startAnimated(bool start);

signals:
    void animated();

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    bool m_startAnimated {false};   /**< Флаг запуска анимации отрисовки пути. */
    Box *m_currentItem;              /**< Текущий выбранный элемент. */
    Box *m_startItem {nullptr};     /**< Начальный элемент пути. */
    Box *m_endItem {nullptr};        /**< Конечный элемент пути. */

};
