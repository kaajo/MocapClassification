#include "bodynode.hpp"

#include <type_traits>

BodyNode::BodyNode(BodyNode::NODE nodeType, const cv::Vec3f &position) :
    m_nodeType(nodeType),
    m_position(position)
{

}

BodyNode::NODE BodyNode::getParentForType(BodyNode::NODE type)
{
    switch (type) {
    //left leg
    case NODE::LFEMUR:
    case NODE::LTIBIA:
    case NODE::LFOOT:
    case NODE::LTOES:
        //right leg
    case NODE::RFEMUR:
    case NODE::RTIBIA:
    case NODE::RFOOT:
    case NODE::RTOES:
        //spine
    case NODE::UPPERBACK:
    case NODE::THORAX:
    case NODE::LOWERNECK:
    case NODE::UPPERNECK:
    case NODE::HEAD:
        //left arm
    case NODE::LHUMERUS:
    case NODE::LRADIUS:
    case NODE::LWRIST:
    case NODE::LHAND:
        //right arm
    case NODE::RHUMERUS:
    case NODE::RRADIUS:
    case NODE::RWRIST:
    case NODE::RHAND:
        return NODE(static_cast<std::underlying_type<NODE>::type>(type) -1);
        //root
    case NODE::ROOT:
    case NODE::UNDEFINED:
        return NODE::UNDEFINED;
    case NODE::LHIPJOINT:
    case NODE::RHIPJOINT:
    case NODE::LOWERBACK:
        return NODE::ROOT;
        //thorax
    case NODE::LCLAVICLE:
    case NODE::RCLAVICLE:
        return NODE::THORAX;
        //left hand
    case NODE::LFINGERS:
    case NODE::LTHUMB:
        return NODE::LHAND;
        //right hand
    case NODE::RFINGERS:
    case NODE::RTHUMB:
        return NODE::RHAND;
    }

    return NODE::UNDEFINED;
}

std::vector<BodyNode::NODE> BodyNode::getChildrenForType(BodyNode::NODE type)
{
    switch (type) {
    //split points
    case NODE::ROOT:
        return {NODE::LHIPJOINT,NODE::RHIPJOINT,NODE::LOWERBACK};
    case NODE::THORAX:
        return {NODE::LOWERNECK,NODE::LCLAVICLE,NODE::RCLAVICLE};
    case NODE::LHAND:
        return {NODE::LFINGERS,NODE::LTHUMB};
    case NODE::RHAND:
        return {NODE::RFINGERS,NODE::RTHUMB};
    //end points
    case NODE::LTOES:
    case NODE::RTOES:
    case NODE::HEAD:
    case NODE::LFINGERS:
    case NODE::LTHUMB:
    case NODE::RFINGERS:
    case NODE::RTHUMB:
        return {NODE::UNDEFINED};
    //other
    default:
        return {NODE(static_cast<std::underlying_type<NODE>::type>(type) +1)};
    }

    return {NODE::UNDEFINED};
    }

    float BodyNode::angle(const BodyNode &other)
    {
    return std::atan2(cv::norm(m_position.cross(other.m_position)),m_position.dot(other.m_position));
    }

    BodyNode::NODE BodyNode::getNodeType() const
    {
        return m_nodeType;
    }

    void BodyNode::setParent(BodyNode *parent)
    {
        m_parent = parent;
    }

    void BodyNode::addChild(BodyNode *node)
    {
        m_children.push_back(node);
    }

    void BodyNode::setChildren(const std::vector<BodyNode *> &children)
    {
        m_children = children;
    }
