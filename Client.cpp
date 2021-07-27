#include "Client.h"

Client::Client(int id, const QString& name, const QString& phone, const QString& mail, bool active)
	: _id(id)
	, _name(name)
	, _phone(phone)
	, _mail(mail)
	, _active(active)
{
}

Client::Client(const QString& name, const QString& phone, const QString& mail, bool active)
	: _name(name)
	, _phone(phone)
	, _mail(mail)
	, _active(active)
{
}
