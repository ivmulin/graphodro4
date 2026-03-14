from collections import defaultdict


def analyze_facebook_graph(filepath):
    adj_list = defaultdict(set)
    edges_count = 0
    max_vertex = -1

    try:
        with open(filepath, "r") as f:
            for line in f:
                # Пропускаем комментарии, если они есть
                if line.startswith("#"):
                    continue

                # Парсим строку (from, to)
                u, v = map(int, line.split())

                # Обновляем структуру (считаем граф неориентированным)
                adj_list[u].add(v)
                adj_list[v].add(u)

                edges_count += 1
                max_vertex = max(max_vertex, u, v)

        print(f"Total edges (lines in file): {edges_count}")
        print(f"Max vertex ID found: {max_vertex}")
        print("-" * 30)

        # Вывод для вершин 0...4
        for i in range(5):
            neighbors = sorted(list(adj_list[i]))
            degree = len(neighbors)
            print(f"Vertex {i}: degree = {degree}")
            print(f"  Neighbors: {neighbors}")

    except FileNotFoundError:
        print(f"Error: File {filepath} not found.")


if __name__ == "__main__":
    analyze_facebook_graph("../facebook/0.edges")
