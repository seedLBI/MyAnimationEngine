#include "Segment.h"

glm::vec2 Segment::getDirection() const {
	return end - begin;
}