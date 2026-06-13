#ifndef TYPE_PROPERTIES_H
#define TYPE_PROPERTIES_H

#include "Core/Animation/Keyframe/KeyframeResolver.h"

template<typename T, uint8_t N>
struct TypeProperties {
	KeyframeResolver<T, N> kr;
};

struct TPBool : public TypeProperties<uint8_t,1> {
	
	TPBool() : TypeProperties<uint8_t, 1>
		{ 
			KeyframeResolver<uint8_t, 1>(
				{ 0 }, { 1 }, 
				{TypeKeyframe::BINARY}, 
				1
			) 
		} 
	{
	
	}

};

struct TPChar : public TypeProperties<uint64_t, 1> {

	TPChar() : TypeProperties<uint64_t, 1>
	{
		KeyframeResolver<uint64_t, 1>(
			{TypeKeyframe::BINARY},
			1
		)
	}
	{

	}

};

struct TPString : public TypeProperties<uint64_t, 1> {

	std::vector<std::string> values;

	TPString() : TypeProperties<uint64_t, 1>
	{
		KeyframeResolver<uint64_t, 1>(
			{TypeKeyframe::BINARY},
			1
		)
	}
	{

	}

};


struct TPInt : public TypeProperties<int64_t, 1> {
	TPInt() : TypeProperties<int64_t, 1>{}
	{
	}
};

struct TPFloat : public TypeProperties<float, 1> {
	TPFloat() : TypeProperties<float, 1>{}
	{
	}
};

struct TP2Float : public TypeProperties<float, 2> {
	TP2Float() : TypeProperties<float, 2>{}
	{
	}
};

struct TP3Float : public TypeProperties<float, 3> {
	TP3Float() : TypeProperties<float, 3>{}
	{
	}
};

struct TP4Float : public TypeProperties<float, 4> {
	TP4Float() : TypeProperties<float, 4>{}
	{
	}
};

struct TPColor : public TypeProperties<float, 4> {
	TPColor() : TypeProperties<float, 4>{ 
		KeyframeResolver<float, 4>(
			{ {0.f,0.f,0.f,0.f}, {1.f,1.f,1.f,1.f} }
		) 
	}
	{

	}
};

#endif