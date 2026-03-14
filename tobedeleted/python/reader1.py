from collections import defaultdict


def analyze_facebook_graph_simple_lists(filepath):
    # Теперь используем list вместо set
    adj_list = defaultdict(list)
    edges_count = 0
    max_vertex = -1

    try:
        with open(filepath, "r") as f:
            for line in f:
                if line.startswith("#"):
                    continue

                u, v = map(int, line.split())

                # Имитируем твой C++ addEdge:
                adj_list[u].append(v)
                if u != v:  # Исключаем петли, как в твоем коде
                    adj_list[v].append(u)

                edges_count += 1
                max_vertex = max(max_vertex, u, v)

        print(f"Total lines processed: {edges_count}")
        print(f"Max vertex ID: {max_vertex}")
        print("-" * 30)

        for i in range(5):
            # Теперь neighbors содержит дубликаты, если они есть в файле
            raw_neighbors = adj_list[i]
            degree = len(raw_neighbors)
            print(f"Vertex {i}: degree = {degree}")
            print(f"  Neighbors (raw): {raw_neighbors}")

    except FileNotFoundError:
        print(f"Error: File {filepath} not found.")


if __name__ == "__main__":
    analyze_facebook_graph_simple_lists("../facebook/0.edges")
