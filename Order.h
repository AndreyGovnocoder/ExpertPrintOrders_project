#pragma once
#include <qdatetime.h>

class Order
{
public:
	Order() = default;
	Order(Order* order);
	~Order() = default;

	void set_id(int id) { _id = id; };
	void set_date(const QDate& date) { _date = date; };
	void set_client(int client) { _client = client; };
	void set_productType(int productType) { _productType = productType; }
	void set_manager(int manager) { _manager = manager; }
	void set_designer(int designer) { _designer = designer; }
	void set_productionTime(const QDate& productionTime) { _productionTime = productionTime; }
	void set_status(int status) { _status = status; }
	void set_mockUpTask(const QString& mockUpTask) { _mockUpTask = mockUpTask; }
	void set_productionTask(const QString& productionTask) { _productionTask = productionTask; }
	void set_isRead(bool isRead) { _isRead = isRead; }

	int get_id() const { return _id; };
	const QDate& get_date() const { return _date; };
	int get_client() const { return _client; };
	int get_productType() const { return _productType; }
	int get_manager() const { return _manager; }
	int get_designer() const { return _designer; }
	const QDate& get_productionTime() const { return _productionTime; }
	int get_status() const { return _status; }
	const QString& get_mockUpTask() const { return _mockUpTask; }
	const QString& get_productionTask() const { return _productionTask; }
	bool isRead() const { return _isRead; }

private:
	int _id;
	QDate _date;
	int _client;
	int _productType;
	int _manager;
	int _designer;
	QDate _productionTime;
	int _status;
	QString _mockUpTask;
	QString _productionTask;
	bool _isRead;
};