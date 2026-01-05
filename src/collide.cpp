#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#include <Collide.hpp>

using namespace sf;

// Спасибо DeepSeek за то, что помог с коллизией, сэкономил кучу нервов

// Вспомогательная функция для получения вершин прямоугольника
std::vector<Vector2f> GetRectangleVertices(const RectangleShape& rect) {
    std::vector<Vector2f> vertices;
    Transform transform = rect.getTransform();
    FloatRect bounds = rect.getLocalBounds();
    
    vertices.push_back(transform.transformPoint(0.f, 0.f));
    vertices.push_back(transform.transformPoint(bounds.width, 0.f));
    vertices.push_back(transform.transformPoint(bounds.width, bounds.height));
    vertices.push_back(transform.transformPoint(0.f, bounds.height));
    
    return vertices;
}

// Вспомогательная функция для получения вершин выпуклого многоугольника
std::vector<Vector2f> GetConvexVertices(const ConvexShape& conv) {
    std::vector<Vector2f> vertices;
    Transform transform = conv.getTransform();
    
    for (size_t i = 0; i < conv.getPointCount(); ++i) {
        vertices.push_back(transform.transformPoint(conv.getPoint(i)));
    }
    
    return vertices;
}

// Проекция многоугольника на ось
void ProjectPolygon(const Vector2f& axis, const std::vector<Vector2f>& vertices, float& min, float& max) {
    min = std::numeric_limits<float>::max();
    max = std::numeric_limits<float>::lowest();
    
    for (const auto& vertex : vertices) {
        float projection = (vertex.x * axis.x + vertex.y * axis.y) / 
                          (axis.x * axis.x + axis.y * axis.y);
        
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
}

// Нормаль к ребру
Vector2f GetNormal(const Vector2f& p1, const Vector2f& p2) {
    Vector2f edge = p2 - p1;
    Vector2f normal(-edge.y, edge.x);
    
    // Нормализуем нормаль
    float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
    if (length > 0) {
        normal.x /= length;
        normal.y /= length;
    }
    
    return normal;
}

// Основная функция проверки коллизии
bool HasCollision(RectangleShape &rect, ConvexShape &conv) {
    // Быстрая проверка по bounding box
    if (!rect.getGlobalBounds().intersects(conv.getGlobalBounds())) {
        return false;
    }

    // Получаем вершины обеих фигур
    std::vector<Vector2f> rectVertices = GetRectangleVertices(rect);
    std::vector<Vector2f> convVertices = GetConvexVertices(conv);

    // Проверяем оси прямоугольника
    for (size_t i = 0; i < rectVertices.size(); ++i) {
        Vector2f p1 = rectVertices[i];
        Vector2f p2 = rectVertices[(i + 1) % rectVertices.size()];
        
        Vector2f normal = GetNormal(p1, p2);
        
        float rectMin, rectMax, convMin, convMax;
        ProjectPolygon(normal, rectVertices, rectMin, rectMax);
        ProjectPolygon(normal, convVertices, convMin, convMax);
        
        if (rectMax < convMin || convMax < rectMin) {
            return false;
        }
    }

    // Проверяем оси выпуклого многоугольника
    for (size_t i = 0; i < convVertices.size(); ++i) {
        Vector2f p1 = convVertices[i];
        Vector2f p2 = convVertices[(i + 1) % convVertices.size()];
        
        Vector2f normal = GetNormal(p1, p2);
        
        float rectMin, rectMax, convMin, convMax;
        ProjectPolygon(normal, rectVertices, rectMin, rectMax);
        ProjectPolygon(normal, convVertices, convMin, convMax);
        
        if (rectMax < convMin || convMax < rectMin) {
            return false;
        }
    }

    // Если ни на одной оси нет разделения, есть коллизия
    return true;
}