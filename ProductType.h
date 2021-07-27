#pragma once
#include <qstring.h>

class ProductType
{
public:
	ProductType(int id, const QString& type);
	ProductType(const QString& type);
	ProductType() = default;
	~ProductType() = default;

	void set_id(int id) { _id = id; };
	void set_type(const QString& type) { _type = type; };
	int get_id() const { return _id; };
	const QString& get_type() const { return _type; };

private:
	int _id;
	QString _type;
};