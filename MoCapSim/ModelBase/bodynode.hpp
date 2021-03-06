/*
    Copyright (C) 2017  Miroslav Krajíček

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef BODYNODE_HPP
#define BODYNODE_HPP

#include <vector>
#include <opencv2/core/core.hpp>

class BodyNode
{
public:
    enum class NODE : int
    {
        UNDEFINED = 0,
        ROOT = 1,
        LHIPJOINT = 2,
        LFEMUR = 3,
        LTIBIA = 4,
        LFOOT = 5,
        LTOES = 6,
        RHIPJOINT = 7,
        RFEMUR = 8,
        RTIBIA = 9,
        RFOOT = 10,
        RTOES = 11,
        LOWERBACK = 12,
        UPPERBACK = 13,
        THORAX = 14,
        LOWERNECK = 15,
        UPPERNECK = 16,
        HEAD = 17,
        LCLAVICLE = 18,
        LHUMERUS = 19,
        LRADIUS = 20,
        LWRIST = 21,
        LHAND = 22,
        LFINGERS = 23,
        LTHUMB = 24,
        RCLAVICLE = 25,
        RHUMERUS = 26,
        RRADIUS = 27,
        RWRIST = 28,
        RHAND = 29,
        RFINGERS = 30,
        RTHUMB = 31
    };

    BodyNode(NODE nodeType, const cv::Vec3f &position);

    static NODE getParentForType(NODE type);
    static std::vector<NODE> getChildrenForType(NODE type);

    float angle(const BodyNode &other);

    NODE getNodeType() const;

    void setParent(BodyNode *parent);

    void addChild(BodyNode *node);
    void setChildren(const std::vector<BodyNode *> &children);

private:
    NODE m_nodeType = NODE::UNDEFINED;

    cv::Vec3f m_position;

    BodyNode* m_parent = nullptr;
    std::vector<BodyNode*> m_children;
};

#endif // BODYNODE_HPP
