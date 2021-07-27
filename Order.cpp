#include "Order.h"

Order::Order(Order* order)
	: _id(order->get_id())
	, _date(order->get_date())
	, _client(order->get_client())
	, _productType(order->get_productType())
	, _manager(order->get_manager())
	, _designer(order->get_designer())
	, _productionTime(order->get_productionTime())
	, _status(order->get_status())
	, _mockUpTask(order->get_mockUpTask())
	, _productionTask(order->get_productionTask())
{
}
