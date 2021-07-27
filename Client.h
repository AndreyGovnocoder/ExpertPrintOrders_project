#pragma once
#include <qstring.h>

class Client
{
public:
	Client(int id, const QString& name, const QString& phone, const QString& mail, bool active);
	Client(const QString& name, const QString& phone, const QString& mail, bool active);
	Client() = default;
	~Client() = default;

	void set_id(int id) { _id = id; };	
	void set_name(const QString& clientName) { _name = clientName; };
	void set_phone(const QString& clientPhone) { _phone = clientPhone; };
	void set_mail(const QString& clientMail) { _mail = clientMail; };
	void set_active(bool active) { _active = active; }	

	int get_id() const { return _id; };
	const QString& get_name() const { return _name; };
	const QString& get_phone() const { return _phone; };
	const QString& get_mail() const { return _mail; };
	bool is_active() const { return _active; }

private:
	int _id;
	QString _name;
	QString _phone;
	QString _mail;
	bool _active;
};
