
#include "deadlock_detector.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include<tuple>
#include<set>
#include <iostream>

/// feel free to use the code below if you like:
/// ----------------------------------------------------------------
/// split(s) splits string s into vector of strings (words)
/// the delimiters are 1 or more whitespaces
static std::vector<std::string> split(const std::string & s)
{
    auto linec = s + " ";
    std::vector<std::string> res;
    bool in_str = false;
    std::string curr_word = "";
    for (auto c : linec) {
        if (isspace(c)) {
            if (in_str)
                res.push_back(curr_word);
            in_str = false;
            curr_word = "";
        } else {
            curr_word.push_back(c);
            in_str = true;
        }
    }
    return res;
}

/// this is the function you need to (re)implement
/// -------------------------------------------------------------------------
/// parameter edges[] contains a list of request- and assignment- edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge. As soon as you detect a deadlock, your function
/// needs to stop processing edges and return an instance of Result structure
/// with 'index' set to the index that caused the deadlock, and 'procs' set
/// to contain names of processes that are in the deadlock.
///
/// To indicate no deadlock was detected after processing all edges, you must
/// return Result with index=-1 and empty procs.


// Main function to detect deadlock
Result detect_deadlock(const std::vector<std::string> &edges) {

  /**
   * graph: graph of all the processes
   * sorted graph: copy of graph and used for the topple sort algorithm
   * total_nodes: set of all processes and reasources
   * processes, processes_2: set of all proccesses
   * incoming nodes, prev_incoming_nodes: variable used to store the incoming nodes of a process or easource
   * degree_0: all nodes with an outdegree of 0
   * value, value2, relationships: tuple storing a set of the incoming nodes in the first cell and outdegree in the second
   * out_degree: out_degree of a noded
   * 
  */
    Result result;
    std::vector<std::string> line;
    std::map<std::string, std::tuple<std::set<std::string>, int>> graph, sorted_graph;
    std::set<std::string> total_nodes, processes, incoming_nodes, processes_2, degree_0;
    std::tuple<std::set<std::string>, int> relationships;

    for (const auto &edge : edges){
        line = split(edge);
        relationships = std::make_tuple(incoming_nodes, 0);
        int res = line.at(1).compare("->");

        // inserts nodes if they are not alrady in the graph

        processes.insert(line.at(0));
        total_nodes.insert(line.at(0));
        total_nodes.insert(line.at(2) + "!");
        graph.insert({line.at(0), relationships});
        graph.insert({line.at(2) + "!", relationships});

        if (res == 0){ // if node 0 is requesting node 2
            // updating node 0 out_degree
            auto &value = graph.at(line.at(0));
            auto &out_degree = std::get<1>(value);
            ++out_degree;

            // Updating node 2 incoming, nodes
            auto &value2 = graph.at(line.at(2) + "!");
            auto &incoming_nodes = std::get<0>(value2);
            incoming_nodes.insert(line.at(0));

        } else{ // if node 0 is assigned node 2

            // Updating node 0 incoming nodes
            auto &value = graph.at(line.at(0));
            auto &incoming_nodes = std::get<0>(value);
            incoming_nodes.insert(line.at(2) + "!");

            // Updating node 2 out_degree
            auto &value2 = graph.at(line.at(2) + "!");
            auto &out_degree = std::get<1>(value2);
            ++out_degree;
        }

        // ------------------------------ SORTING BELOW --------------------------------------
        processes_2 = processes;
        sorted_graph = graph;

        for (auto &node : total_nodes){
            auto &value = sorted_graph.at(node);
            int out_degree = std::get<1>(value);
            if (out_degree == 0) {
                degree_0.insert(node);
            }
        }
        // if the out_degree is 0, get the tuple containing the incoming nodes
        while (!degree_0.empty()){
            auto iterator = degree_0.begin();
            auto key = *iterator;
            auto &value = sorted_graph.at(key);
            auto &incoming_nodes = std::get<0>(value);
            // look through all the incoming nodes and subtract their out_degrees by 1
            for (const auto &new_node : incoming_nodes){
                auto &value2 = sorted_graph.at(new_node);
                auto &out_degree = std::get<1>(value2);
                --out_degree;

                if (out_degree == 0) {
                    degree_0.insert(new_node);
                }
            }
            //printf("erasing key %s\n", key.c_str());
            // Erase key after changing out_degrees of incoming nodes
            degree_0.erase(key);
            processes_2.erase(key);
        }
    
        // If there is a deadlock
        if (!processes_2.empty()){
            std::string s;
            for (auto const& e : processes_2){
                s += e;
                s += ' ';
            }
            result.dl_procs = split(s);
            result.edge_index = &edge - &edges[0];
            return result;
        }  
    }
    // If there is no deadlock 
    result.dl_procs = split("");
    result.edge_index = -1;
    return result;
}
