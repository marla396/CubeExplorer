#include "misc/astar.h"

#include <iostream>

bool AStar::path(glm::vec3 start, const glm::vec3& end, const std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>>& height_map, std::vector<NodePtr>& path) {

	start.y = std::floor(start.y);

	if (start == end) {
		return true;
	}

	const int MAX_SEARCH = 500;

	const int8_t directions[8][2] = { { 0, -1 },{ 1, 0 },{ 0, 1 },{ -1, 0 },{ 1, -1 },{ 1, 1 },{ -1, 1 },{ -1, -1 } };

	NodePtr current = std::make_shared<Node>();
	bool found = false;

	current->pos = start;
	current->g = 0.0f;
	current->h = manhattan(start, end);
	current->f = current->g + current->h;
	current->parent = nullptr;

	auto get_y = [&height_map](const glm::vec3& pos) {
		return height_map->noise[static_cast<int>(pos.x)][static_cast<int>(pos.z)];
	};

	std::unordered_set<NodePtr> closed_list;
	AStarOpenList open_list;

	open_list.push(current);
	int iterations = 0;



	while (!open_list.empty()) {
		++iterations;
		if (iterations >= MAX_SEARCH)
			break;

		current = open_list.pop();

		closed_list.insert(current);

		if (current->pos.x == end.x && current->pos.z == end.z) {
			found = true;
			break;
		}

		for (int i = 0; i < 8; i++) {

			NodePtr node = std::make_shared<Node>();
			node->pos.x = directions[i][1] + current->pos.x;
			node->pos.z = directions[i][0] + current->pos.z;

			if (node->pos.x < 0 || node->pos.x >= CHUNK_SIZE * WORLD_SIZE) {
				continue;
			}

			if (node->pos.z < 0 || node->pos.z >= CHUNK_SIZE * WORLD_SIZE) {
				continue;
			}

			node->pos.y = std::floor(get_y(node->pos));

			/*if (std::abs(node->pos.y - current->pos.y) > 1.0f) {
				continue;
			}*/

			node->parent = current;

			if (closed_list.find(node) != closed_list.end()) {
				continue;
			}

			node->g = i > 3 ? 1.0f : 1.4f + current->g;
			node->h = manhattan(node->pos, end);
			node->f = node->g + node->h;

			auto other = open_list.get(node);

			if (other != nullptr) {
				if (other->f > node->f) {
					other->f = node->f;
				}
				continue;
			}

			open_list.push(node);
		}
	}

	if (found) {
		do {
			path.push_back(current);
			current = current->parent;
		} while (current != nullptr);

		path.pop_back();
		//Simpler to have the path in reverse order
		//std::reverse(std::begin(path), std::end(path));
	}

	return found;
}

float AStar::manhattan(const glm::vec3& start, const glm::vec3& end) {
	return std::abs(start.x - end.x) + std::abs(start.z - end.z);
}

AStar::NodePtr AStarOpenList::pop() {
	auto ret = m_queue.top();
	m_lookup.erase(ret);
	m_queue.pop();

	return ret;
}

AStar::NodePtr AStarOpenList::get(const AStar::NodePtr& n) const{
	auto it = m_lookup.find(n);
	if (it != m_lookup.end()) {
		return *it;
	}

	return nullptr;
}

void AStarOpenList::push(const AStar::NodePtr& n) {
	m_queue.push(n);
	m_lookup.insert(n);
}

bool AStarOpenList::empty() const {
	return m_queue.empty();
}

size_t AStarOpenList::size() const {
	return m_queue.size();
}