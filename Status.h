#pragma once
#include <qstring.h>

class Status
{
public:
	Status() = default;
	~Status() = default;

	void set_id(int id) { _id = id; };
	void set_status(const QString& status) { _status = status; };
	int get_id() const { return _id; };
	const QString& get_status() const { return _status; };

private:
	int _id;
	QString _status;
};