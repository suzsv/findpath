#include "pathfinding.h"

#include <cfloat>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <unordered_map>

// Переопределение хеш-функции для класса Node
namespace std {
template <>
struct hash<Node> {
    size_t operator()(const Node& p) const {
        return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
    }
};
}

/**
 * @brief Вычисление манхэттенского расстояния между двумя узлами.
 *
 * Манхэттенское расстояние - это сумма абсолютных разностей координат двух точек.
 *
 * @param from Узел, от которого считается расстояние.
 * @param to Узел, до которого считается расстояние.
 * @return Манхэттенское расстояние между узлами.
 */
int manhattanDistance(const Node& from, const Node& to) {
    return abs(from.x - to.x) + abs(from.y - to.y);
}

/**
 * @brief Проверка валидности позиции на сетке.
 *
 * Проверяет, что координаты находятся внутри границ сетки и ячейка свободна (не заблокирована).
 *
 * @param x Координата x.
 * @param y Координата y.
 * @param width Ширина сетки.
 * @param height Высота сетки.
 * @param grid Сетка, представляющая блокировку ячеек.
 * @return true, если позиция валидна; false в противном случае.
 */
bool isValid(int x, int y, int width, int height, const std::vector<std::vector<bool>>& grid) {
    return x >= 0 && x < width && y >= 0 && y < height && !grid[x][y];
}

// Направления движения: Право, Вниз, Лево, Верх
const std::vector<std::pair<int, int>> directions{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};;

/**
 * @brief Поиск пути с использованием алгоритма A*.
 *
 * Алгоритм A* использует эвристическое оценивание для поиска кратчайшего пути от начального узла до конечного узла на сетке.
 *
 * @param grid Сетка, представляющая блокировку ячеек.
 * @param start Начальный узел.
 * @param end Конечный узел.
 * @return Вектор узлов, представляющий найденный путь. Если путь не найден, возвращается пустой вектор.
 */
std::vector<Node> a_star_search(const std::vector<std::vector<bool>>& grid, const Node& start, const Node& end) {

    std::priority_queue<Node> open_set;
    std::unordered_map<Node, Node, std::hash<Node>> came_from; // откуда мы пришли к каждому узлу
    std::unordered_map<Node, float, std::hash<Node>> g_score;

    g_score[start] = 0;
    open_set.push(start);
    came_from[start] = start;

    while(!open_set.empty()) {
        Node current = open_set.top();

        if(current.x == end.x && current.y == end.y) {
            std::vector<Node> path = {end};
            while(path.back().x != start.x || path.back().y != start.y)
                path.push_back(came_from[path.back()]);
            std::reverse(path.begin(), path.end());
            return path;
        }
        open_set.pop();

        for(const auto& [dx, dy]: directions) {
            Node neighbor{current.x + dx, current.y + dy};

            if(isValid(neighbor.x, neighbor.y, grid.size(), grid[0].size(), grid)) {
                float tentative_g_score = g_score[current] + manhattanDistance(current, neighbor);

                if(g_score.find(neighbor) == g_score.end() || tentative_g_score < g_score[neighbor]) {
                    came_from[neighbor] = current;
                    g_score[neighbor] = tentative_g_score;
                    neighbor.g = tentative_g_score;
                    neighbor.h = manhattanDistance(neighbor, end);
                    neighbor.f =  neighbor.g +  neighbor.h;
                    open_set.push(neighbor);
                }
            }
        }
    }
    return {}; // Если путь не найден
}
