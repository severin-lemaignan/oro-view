/*
    Copyright (c) 2010 Séverin Lemaignan (slemaign@laas.fr)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EDGE_H
#define EDGE_H

#include <vector>

#include "edge_renderer.h"
#include "styles.h"

class Graph;
class Node;
class NodeRelation;

class Edge
{
    Node* node1;
    Node* node2;

    void updateLength();

    vec2f spos;

    bool stepDone;
    bool renderingDone;

    EdgeRenderer renderer;

public:
    Edge(const NodeRelation& rel, const std::string& label = "");

    float length;
    float spring_constant;
    float nominal_length;

    //void addReferenceRelation(const NodeRelation& rel);
    //void removeReferenceRelation(const NodeRelation& rel);

    //bool coversRelation(const NodeRelation& rel);

    //int countRelations() const;
    //bool hasOutboundConnectionFrom(const Node* node) const;

    void step(Graph& g, float dt);
    void render(rendering_mode mode, OroView& env);

    const std::string& getId1() const;
    const std::string& getId2() const;

};

#endif // EDGE_H
