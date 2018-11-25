#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <queue>
#include <unordered_set>


#include "fft_noise.h"
#include "world/world_const.h"

class AStar {
public:
	struct Node{
		glm::vec3 pos;
		float f;
		float g;
		float h;
		std::shared_ptr<Node> parent;
	};

	using NodePtr = std::shared_ptr<Node>;

	static bool path(glm::vec3 start, const glm::vec3& end, const std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>>& height_map, std::vector<NodePtr>& path);
private:
	static float manhattan(const glm::vec3& start, const glm::vec3& end);
};

namespace std {
	template<>
	class hash<AStar::Node> {
	public:
		size_t operator()(const AStar::Node& v) const {
			//Courtesy of http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf

			constexpr size_t p1 = 73856093;
			constexpr size_t p2 = 19349663;
			constexpr size_t p3 = 83492791;

			return (static_cast<size_t>(v.pos.x) * p1) ^ (static_cast<size_t>(v.pos.y) * p2) ^ (static_cast<size_t>(v.pos.z) * p3);
		}
	};

	template<>
	inline bool operator==(const shared_ptr<AStar::Node>& lhs, const shared_ptr<AStar::Node>& rhs) noexcept {
		return lhs->pos == rhs->pos;
	}

	template<>
	inline bool operator<(const shared_ptr<AStar::Node>& lhs, const shared_ptr<AStar::Node>& rhs) noexcept {
		return lhs->f > rhs->f;
	}
};


class AStarOpenList {
public:
	AStarOpenList() = default;

	AStar::NodePtr pop();
	AStar::NodePtr get(const AStar::NodePtr& n) const;
	void push(const AStar::NodePtr& n);

	bool empty() const;
	size_t size() const;
private:
	std::unordered_set<AStar::NodePtr> m_lookup;
	std::priority_queue<AStar::NodePtr> m_queue;
};

