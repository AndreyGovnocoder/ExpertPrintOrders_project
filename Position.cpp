#include "Position.h"

Position::Position(int id, const QString& position)
	: _id(id)
	, _position(position)
{
}

Position::Position(const QString& position)
	: _position(position)
{
}
