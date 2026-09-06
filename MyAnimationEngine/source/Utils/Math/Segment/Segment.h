#ifndef SEGMENT_H
#define SEGMENT_H

#include <glm/glm.hpp>

struct Segment {
	glm::vec2 begin;
	glm::vec2 end;

	glm::vec2 getDirection() const;
};




#endif