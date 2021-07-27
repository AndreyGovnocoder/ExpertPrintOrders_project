#include "ProductType.h"

ProductType::ProductType(int id, const QString& type)
	: _id(id)
	, _type(type)
{
}

ProductType::ProductType(const QString& type)
	: _type(type)
{
}
