#ifndef LERPED_VALUE_H
#define LERPED_VALUE_H


template<typename T>
struct LerpedValue {
private:
	T real;
	T lerped;

public:
	LerpedValue();
	LerpedValue(const T& value);

	void SetAll(const T& value);
	void Set(const T& real);
	const T& GetLerped() const;
	const T& GetReal() const;

	void Update(const T& time);
};

template<typename T>
LerpedValue<T>::LerpedValue() {
	SetAll(0.f);
}

template<typename T>
LerpedValue<T>::LerpedValue(const T& value) {
	SetAll(value);
}

template<typename T>
void LerpedValue<T>::SetAll(const T& value) {
	lerped = value;
	real = value;
}

template<typename T>
void LerpedValue<T>::Set(const T& real) {
	this->real = real;
}

template<typename T>
const T& LerpedValue<T>::GetLerped() const {
	return lerped;
}

template<typename T>
const T& LerpedValue<T>::GetReal() const {
	return real;
}

template<typename T>
void LerpedValue<T>::Update(const T& time) {
	lerped = lerped + time * (real - lerped);
}


#endif