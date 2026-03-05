#include "LerpedValue.h"


LerpedValue_float::LerpedValue_float() {
	SetAll(0.f);
}
LerpedValue_float::LerpedValue_float(const float& value) {
	SetAll(value);
}


void LerpedValue_float::SetAll(const float& value) {
	lerped = value;
	real = value;
}
void LerpedValue_float::Set(const float& real) {
	this->real = real;
}
const float& LerpedValue_float::GetLerped() const {
	return lerped;
}
const float& LerpedValue_float::GetReal() const {
	return real;
}

void LerpedValue_float::Update(const float& time) {
	lerped = lerped + time * (real - lerped);
}
