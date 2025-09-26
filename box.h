#pragma once

#include <QColor>
#include <QGraphicsItem>

/**
 * @brief Класс квадрата (Box) на графической сцене.
 *
 * Этот класс представляет собой квадрат, который может быть использован на графической сцене.
 */
class Box : public QGraphicsItem
{
public:
    explicit Box(int size, QGraphicsItem *parent = nullptr);

    enum { Type = UserType + 1 };

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
    int type() const override { return Type; }  /**< Получение типа графического элемента  */

    bool isBusy() const;
    void setIsBusy(bool newIsBusy);

    bool start() const;
    void setStart(bool newStart);

    bool end() const;
    void setEnd(bool newEnd);

private:
    int m_size; /**< Размер квадрата. */
    bool m_start {false};   /**< Флаг, указывающий, является ли квадрат начальным элементом пути. */
    bool m_end {false};     /**< Флаг, указывающий, является ли квадрат конечным элементом пути. */
    bool m_isBusy {false};  /**< Флаг, указывающий, занят ли квадрат (недоступен). */
    QColor m_busyColor {QColor(Qt::darkGray)};  /**< Цвет занятого квадрата. */
    QColor m_freeColor {QColor(Qt::white)};     /**< Цвет свободного квадрата. */
};


