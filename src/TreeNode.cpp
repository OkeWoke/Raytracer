#include "TreeNode.h"

TreeNode::TreeNode(int depth, double limits[6]):max_depth(depth)
{
    this->limits[0] = limits[0];
    center = Vector((limits[1]+limits[0])/2, (limits[3]+limits[2])/2, (limits[5]+limits[4])/2);
}

 void TreeNode::insert(Triangle& triangle)
 {
    //we find out the cetner of the triangle, probs avg of the 3 vertices
    //figure out which leaf it belongs in and insert into that node, unless if depth is max depth then we just insert triangle into collection of triangles
    if(max_depth==0)
    {
        triangles.push_back(&triangle);
    }else
    {
        //center of mass

        Vector com = (triangle.v1+triangle.v2+triangle.v3)/3;
        std::set<int> node_index = {0,1,2,3,4,5,6,7};

        std::set<int> x_lhalf = {0,3,4,7};
        std::set<int> y_lhalf = {0,1,4,5};
        std::set<int> z_lhalf = {0,1,2,3};


        std::set<int> x_rhalf = {1,2,5,6};
        std::set<int> y_rhalf = {2,3,7,6};
        std::set<int> z_rhalf = {4,5,6,7};

        std::set<int> current;

        auto inter = [](std::set<int> lhs , std::set<int> rhs)
        {
            std::set<int> intersect;
            std::set_intersection(lhs.begin(),lhs.end(),rhs.begin(),rhs.end(),
            std::inserter(intersect,intersect.begin()));

            return intersect;
        };
        double new_limits[6];
        //deep copy
        for(int i =0; i<6; i++)
        {
            new_limits[i] = limits[i];
        }

        if(com.x < center.x)
        {
            current = inter(current, x_lhalf);
            new_limits[1] = center.x;
        }else
        {
            current = inter(current, x_rhalf);
            new_limits[0] = center.x;
        }

        if(com.y < center.y)
        {
            current = inter(current, y_lhalf);
            new_limits[3] = center.y;
        }else
        {
            current = inter(current, y_rhalf);
            new_limits[2] = center.y;
        }

        if(com.z < center.z)
        {
            current = inter(current, z_lhalf);
            new_limits[5] = center.z;
        }else
        {
            current = inter(current, z_rhalf);
            new_limits[4] = center.z;
        }

        int li = *current.begin();
        if(leaves[li] == nullptr)
        {
            leaves[li] = new TreeNode(max_depth-1, new_limits);
        }

        leaves[li]->insert(triangle);
    }
 }
