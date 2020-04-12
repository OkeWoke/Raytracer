#ifndef TREENODE_H
#define TREENODE_H
#include "GObjects/Triangle.h"
#include<vector>
#include "BoundVolume.h"
#include<set>
#include <algorithm>
class TreeNode
{
    public:

        TreeNode();
        TreeNode(int depth, double limits[8]);
        TreeNode* leaves[8] = {nullptr};
        double limits[6];//min x, max x, min y, max y, min z, max z, defines the cube limits
        std::vector<Triangle*> triangles;
        BoundVolume* bv;
        Vector center;
        void insert(Triangle& triangle);

    private:
        int max_depth;
};

#endif // TREENODE_H
