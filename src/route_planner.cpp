#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    // Below code will call copy constructor and hence will not work properly - Remove
    // RouteModel::Node startClosestNode = m_Model.FindClosestNode(start_x, start_y);
    // RouteModel::Node endClosestNode = m_Model.FindClosestNode(end_x, end_y);
    // std::cout<<endClosestNode.x << " " <<endClosestNode.y << " constructor \n";
    RoutePlanner::start_node = &(m_Model.FindClosestNode(start_x, start_y));
    RoutePlanner::end_node = &(m_Model.FindClosestNode(end_x, end_y));
}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    float HValue = 0.0;
    if(this->end_node)
        HValue = node->distance(*(this->end_node));
    return HValue;
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    // current_node->FindNeighbors();
    current_node->FindNeighbors();
    for(auto it: current_node->neighbors)
    {
        if(it->visited == false)
        {
            it->parent = current_node;
            it->h_value = CalculateHValue(it);
            // g_value will be current nodes g value plus distance of current node to neighbor
            it->g_value = current_node->g_value + it->distance(*current_node);
            
            open_list.emplace_back(it);
            it->visited = true;
        }
    }
    // current_node->neighbors
}

bool compareNode(RouteModel::Node* a, RouteModel::Node* b)
{
    auto a_sum = a->g_value + a->h_value;
    auto b_sum = b->g_value + b->h_value;
    return a_sum > b_sum;
}

// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

RouteModel::Node *RoutePlanner::NextNode() {
    // sort the list 
    std::sort(open_list.begin(), open_list.end(), compareNode);
    // pop the node with lowest sum
    RouteModel::Node* nextNode = open_list.back();
    // remove node from list
    open_list.pop_back();
    // return Node pointer
    return nextNode;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
        // add the vector to the path
    // if(path_found.begin() == path_found.end())
    // {
    path_found.push_back(*current_node);
    auto it = path_found.begin();
    // } 
    // TODO: Implement your solution here.
    // iterate from current node to parent node - for parent node parent will be nullptr
    while(current_node->parent != nullptr)
    {
        // cumulatively add the distances
        distance += current_node->distance(*(current_node->parent));
        // get the next node
        current_node = current_node->parent;
        // insert it into the path
        it = path_found.insert(it, *current_node);       
    }
    std::cout << "Reached here 1 \n";

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;
    auto i = 0;
    
    // TODO: Implement your solution here.
    current_node = start_node;
    // open_list.push_back(start_node);
    current_node->visited = true;

    AddNeighbors(current_node); 
    while(open_list.size() > 0)
    {
         
        current_node = NextNode();
        // std::cout<<current_node->g_value+current_node->h_value<<"\n";
        if((current_node->x == end_node->x) && (current_node->y == end_node->y))
        {
            std::cout << "Reached here \n";
            // goal has been reached
            m_Model.path = ConstructFinalPath(current_node);
            
            break;
        }
        AddNeighbors(current_node); 
        if(i++ == 50)
             break;
    }
    std::cout << "Reached here 2\n";
}