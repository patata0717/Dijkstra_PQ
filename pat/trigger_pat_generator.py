import math
from pat_generator import generate_connected_graph, save_graph_to_file

def main():
    print("Group 1: E = V")
    group1_V = [100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200]
    
    # Loop through V and calculate E = V * 1.5 for Group 1
    for V in group1_V:
        E = int(V * 1.5)
        V, E, edges = generate_connected_graph(V, E)
        save_graph_to_file(V, E, edges)
    print("")
    
    print("Group 2: E = V*logV")
    # Loop through V and calculate E = V * log(V) for Group 2)
    count = 1
    for V in group1_V:
        logV = math.log10(V)
        E = int(V * logV)
        V, E, edges = generate_connected_graph(V, E)
        save_graph_to_file(V, E, edges)
        count = count + 1
    print("")

    print("Group 3: E =  V2")
    # Loop through V and calculate E = V * 1.5 * V for Group 3
    count = 1
    for V in group1_V[:5]:
        E = int(V * V * 0.1)
        V, E, edges = generate_connected_graph(V, E)
        save_graph_to_file(V, E, edges)
        count = count + 1

if __name__ == "__main__":
    main()

