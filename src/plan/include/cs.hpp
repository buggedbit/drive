#pragma once

#include <math.h>
#include <visualization_msgs/MarkerArray.h>

#include "obstacles/circle.hpp"
#include "obstacles/line.hpp"

struct ConfigurationSpace {
private:
    std::vector<Circle> circles;
    std::vector<Line> lines;

public:
    ConfigurationSpace() {}

    void add_circle(const float& x, const float& y, const float& radius, const float& agent_radius) { circles.push_back(Circle(Vec2(x, y), radius + agent_radius)); }

    void add_lines(const std::vector<Vec2>& points, const float& padding, const float& agent_radius) {
        if (padding <= agent_radius) {
            throw std::runtime_error("Route padding <= Agent radius.");
        }
        float cs_padding = padding - agent_radius;
        for (int i = 1; i < points.size(); ++i) {
            const Vec2& p1 = points[i - 1];
            const Vec2& p2 = points[i];
            const Vec2 unit = (p2 - p1).normalize();
            const Vec2 perpendicular1(unit.y, -unit.x), perpendicular2(-unit.y, unit.x);
            lines.push_back(Line(p1 + perpendicular1 * cs_padding, p2 + perpendicular1 * cs_padding));
            lines.push_back(Line(p1 + perpendicular2 * cs_padding, p2 + perpendicular2 * cs_padding));
        }
    }

    bool does_intersect(const Vec2& p1, const Vec2& p2) const {
        for (const auto& circle : circles) {
            if (circle.does_intersect(p1, p2)) {
                return true;
            }
        }
        for (const auto& line : lines) {
            if (line.does_intersect(p1, p2)) {
                return true;
            }
        }
        return false;
    }

    void draw(const ros::Publisher& viz) const {
        // Draw circles
        visualization_msgs::MarkerArray arr;
        for (int i = 0; i < circles.size(); ++i) {
            const Circle& circle = circles[i];
            visualization_msgs::Marker m;

            m.header.frame_id = "map";
            m.ns = "circles";
            m.id = i;
            m.header.stamp = ros::Time::now();

            m.action = visualization_msgs::Marker::ADD;

            m.type = visualization_msgs::Marker::CYLINDER;
            m.pose.orientation.w = 1.0;
            m.pose.position.x = circle.center.x;
            m.pose.position.y = circle.center.y;

            m.scale.x = circle.radius * 2;
            m.scale.y = circle.radius * 2;
            m.scale.z = 0.1;

            m.color.r = 1;
            m.color.g = 0;
            m.color.b = 1;
            m.color.a = 0.2;

            arr.markers.push_back(m);
        }
        // Draw lines
        for (int i = 0; i < lines.size(); ++i) {
            const Line& line = lines[i];
            visualization_msgs::Marker m;

            m.header.frame_id = "map";
            m.ns = "lines";
            m.id = i;
            m.header.stamp = ros::Time::now();

            m.action = visualization_msgs::Marker::ADD;

            m.type = visualization_msgs::Marker::LINE_STRIP;
            m.pose.orientation.w = 1;
            geometry_msgs::Point p1;
            p1.x = line.p1.x;
            p1.y = line.p1.y;
            m.points.push_back(p1);
            geometry_msgs::Point p2;
            p2.x = line.p2.x;
            p2.y = line.p2.y;
            m.points.push_back(p2);

            m.scale.x = 0.1;

            m.color.r = 1;
            m.color.g = 0;
            m.color.b = 1;
            m.color.a = 0.2;

            arr.markers.push_back(m);
        }
        viz.publish(arr);
    }
};

