#pragma once
#include <qstring.h>

class Account
{
public:
	Account() = default;
	~Account() = default;

	void set_id(const int id) { _id = id; };
	void set_name(const QString& name) { _name = name; };
	void set_surname(const QString& surname) { _surname = surname; }
	void set_position(const int position) { _position = position; };
	void set_login(const QString& login) { _login = login; }
	void set_password(const QString& password) { _password = password; }
	void set_active(bool active) { _active = active; }

	int get_id() const { return _id; };
	const QString& get_name() const { return _name; };
	const QString& get_surname() const { return _surname; }
	int get_position() const { return _position; };
	const QString& get_login() const { return _login; }
	const QString& get_password() const { return _password; }
	bool is_active() const { return _active; }

private:
	int _id;
	QString _name;
	QString _surname;
	int _position;
	QString _login;
	QString _password;
	bool _active;
};