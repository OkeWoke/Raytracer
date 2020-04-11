/*
#include "OctTree.h"

OctTree::OctTree()
{
    //ctor
}

OctTree::OctTree(std::vector<Mesh*>& meshes, int depth): meshes(meshes), max_depth(depth)
{
    // compute max limits, parse limits to root,
    for(int i =0; i<meshes.size(); i++)
    {
        meshes[i]->center;
    }

    root_node = TreeNode(max_depth, limits);
    //begin insertion of triangles into octree nodes
    for(int i=0;i<meshes.size();i++)
    {
        for(int k=0; k< meshes[i]->triangles.size ; k++)
        {
            root_node.insert(meshes[i]->trianges[k])
        }
    }
}

*/
