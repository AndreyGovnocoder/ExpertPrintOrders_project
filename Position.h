#pragma once
#include <qstring.h>

class Position
{
public:
	Position(int id, const QString& position);
	Position(const QString& position);
	Position() = default;
	~Position() = default;

	void set_id(int id) { _id = id; };
	void set_position(const QString& position) { _position = position; };
	int get_id() const { return _id; };
	const QString& get_position() const { return _position; };

private:
	int _id;
	QString _position;
};