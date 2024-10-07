import random

def generate_connected_graph(V, E):
    # Ensure E >= V - 1 for a connected graph
    if E < V - 1:
        raise ValueError("Number of edges must be at least V-1 for a connected graph")
    
    edges = []
    
    # Step 1: Create a spanning tree to ensure the graph is connected
    available_nodes = list(range(V))
    random.shuffle(available_nodes)
    
    for i in range(V - 1):
        u, v = available_nodes[i], available_nodes[i + 1]
        w = random.randint(1, 10)  # Assign a random weight between 1 and 10
        edges.append((u, v, w))
    
    # Step 2: Add the remaining edges
    remaining_edges = E - (V - 1)
    added_edges = set((min(u, v), max(u, v)) for u, v, _ in edges)
    
    while remaining_edges > 0:
        u = random.randint(0, V - 1)
        v = random.randint(0, V - 1)
        if u != v and (min(u, v), max(u, v)) not in added_edges:
            w = random.randint(1, 10)  # Assign a random weight between 1 and 10
            edges.append((u, v, w))
            added_edges.add((min(u, v), max(u, v)))
            remaining_edges -= 1
    
    # Step 3: Return the generated graph
    return V, E, edges

# Function to save the result to a file
def save_graph_to_file(V, E, edges):
    filename = f"g_{V}_{E}.dat"
    with open(filename, 'w') as file:
        file.write(f"{V} {E}\n")
        for u, v, w in edges:
            file.write(f"{u} {v} {w}\n")
    print(f"Graph saved to {filename}")

# Interact with the user
def main():
    print("Welcome to the connected graph generator!")
    
    # Get the number of vertices and edges from the user
    V = int(input("Enter the number of vertices (|V|): "))
    E = int(input(f"Enter the number of edges (|E|), must be at least {V-1}: "))
    
    try:
        V, E, edges = generate_connected_graph(V, E)
        
        # Save the generated graph to a file with the format g_{V}_{E}.dat
        save_graph_to_file(V, E, edges)
        
    except ValueError as e:
        print(f"Error: {e}")

# Run the interactive program
if __name__ == "__main__":
    main()

