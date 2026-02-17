#ifndef LERPED_VALUE_H
#define LERPED_VALUE_H


struct LerpedValue_float {
private:
	float real;
	float lerped;

public:
	void SetAll(const float& value);
	void Set(const float& real);
	const float& GetLerped() const;
	const float& GetReal() const;

	void Update(const float& time);
};




#endif