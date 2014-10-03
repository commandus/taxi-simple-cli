#if !defined (ODB_DB_MYSQL) && !defined (ODB_DB_PGSQL)
#	error No MySQL or PgSQL is selected!
#endif

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include "taxi_types.h"

/*thrift isset not a macro */
#undef isset
#include "taxi_types-odb.hxx"

#ifdef ODB_DB_PGSQL
#include <odb/pgsql/database.hxx>
#include "taxi_odb_traits-pgsql.hxx"
#endif
#ifdef ODB_DB_MYSQL
#include <odb/mysql/database.hxx>
#include "taxi_odb_traits-mysql.hxx"
#endif

#include "PassengerServiceHandler.h"
#include "PassengerService.h"

using namespace odb::core;

time_t currentTime()
{
	time_t r(time(NULL));
	return r;
}

ID PassengerServiceHandler::getIdBrand(const STR& name)
{
	ID r = -1;
	if (name.empty())
		return r;
	try  {
		typedef odb::result<DictEntry> Result;
		typedef odb::query<DictEntry> Query;
		Query q((Query::tag == DictTag::TAG_VEHICLE_BRAND) && (Query::strvalue.like(name + "%")));
		Result rq (db->query<DictEntry> (q));
		if (!rq.empty())
			r = rq.begin()->id;
	}
	catch (const odb::exception& e)  { 
		printf("getIdBrand error: %s.\n", e.what());
		return r;
	}
	return r;
}

// Return driver id from service order
ID PassengerServiceHandler::getDriverIdFromServiceOrder(ID serviceorderid)
{
	try  {
		ServiceOrder *v = db->load<ServiceOrder>(serviceorderid);
		return v->crew.at(CrewRole::CREW_DRIVER);
	}
	catch (const odb::exception& e)  { 
		printf("getDriverIdFromServiceOrder error: %s.\n", e.what());
		return -1;
	}
}

// Return loader id from service order
ID PassengerServiceHandler::getLoaderIdFromServiceOrder(ID serviceorderid)
{
	try  {
		ServiceOrder *v = db->load<ServiceOrder>(serviceorderid);
		return v->crew.at(CrewRole::CREW_LOADER);
	}
	catch (const odb::exception& e)  { 
		printf("getLoaderIdFromServiceOrder error: %s.\n", e.what());
		return -1;
	}
}

PassengerServiceHandler::PassengerServiceHandler(int argc, char *argv[])
{
	this->argc = argc;
	this->argv = argv;
#ifdef ODB_DB_PGSQL
#include "taxi_odb_traits-pgsql.hxx"
	db = new odb::pgsql::database(argc, argv, false, "utf8");
#endif
#ifdef ODB_DB_MYSQL
	db = new odb::mysql::database(argc, argv, false, "utf8");
#endif
}

PassengerServiceHandler::~PassengerServiceHandler()
{
	delete db;
}

void PassengerServiceHandler::getDocument(Document& _return, const Credentials& credentials, const UserDevice& userdevice, const ID id)
{
	if (!credentials.__isset.personrole)
		return;
	if (!((credentials.personrole == PersonRole::DISPATCHER) || (credentials.personrole == PersonRole::ADMIN)))
		return;
	Document *r(db->load<Document>(id));
	_return = *r;
	free(r);
}

void PassengerServiceHandler::getOrg(Org& _return, const Credentials& credentials, const UserDevice& userdevice, const ID id)
{
	if (!credentials.__isset.personrole)
		return;
	if ((credentials.personrole == PersonRole::NOTAUTHORIZED) || (credentials.personrole == PersonRole::PASSENGER) || (credentials.personrole == PersonRole::GUEST))
		return;
	Org *r(db->load<Org>(id));
	_return = *r;
	free(r);
}

void PassengerServiceHandler::loginDriver(Driver& _return, const Credentials& credentials, const UserDevice& userdevice)
{
	// Your implementation goes here
	printf("loginDriver\n");
}

void PassengerServiceHandler::loginPassenger(Passenger& _return, const Credentials& credentials, const UserDevice& userdevice) 
{
	// Your implementation goes here
	printf("loginPassenger\n");
}

void PassengerServiceHandler::addBank(Bank& _return, const Credentials& credentials, const UserDevice& userdevice, const Bank& bank) {
	printf("addBank\n");
	try  {
		transaction t(db->begin());
		Bank v(bank);
		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addBank error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addCustomer(Customer& _return, const Credentials& credentials, const UserDevice& userdevice, const Customer& value)
{
	printf("addCustomer\n");
	try  {
		transaction t(db->begin());
		Customer v(value);
		// defaults
		if (!v.__isset.taxtype)
			v.taxtype = TaxType::TAXORG;
		if (!v.__isset.active)
			v.active = true;
		if (!v.__isset.enabled)
			v.enabled = true;
		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addCustomer error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addPassenger(Passenger& _return, const Credentials& credentials, const UserDevice& userdevice, const Passenger& value)
{
	printf("addPassenger\n");
	try  {
		transaction t(db->begin());
		Passenger v(value);
		// defaults
		if (!v.__isset.canorder)
			v.canorder = CanOrder::EVERYTHING;
		if (!v.__isset.isoperator)
			v.isoperator = false;
		if (!v.__isset.status)
			v.status = EmployeeStatus::ACTIVE;

		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addPassenger error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addDriver(Driver& _return, const Credentials& credentials, const UserDevice& userdevice, const Driver& value)
{
	printf("addDriver\n");
	try  {
		transaction t(db->begin());
		Driver v(value);
		// defaults
		if (!v.__isset.online)
			v.online = false;
		if (!v.__isset.status)
			v.status = EmployeeStatus::ACTIVE;

		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addDriver error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addOrg(Org& _return, const Credentials& credentials, const UserDevice& userdevice, const Org& value)
{
	printf("addOrg\n");
	try  {
		transaction t(db->begin());
		Org v(value);
		// defaults
		if (!v.__isset.orgrole)
			v.orgrole = OrgRole::ORGGUEST;
		if (!v.__isset.orgtype)
			v.orgtype = OrgType::ORGENTERPRISE;
		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addOrg error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addOrgService(OrgService& _return, const Credentials& credentials, const UserDevice& userdevice, const OrgService& value)
{
	printf("addOrgService\n");
	try  {
		transaction t(db->begin());
		OrgService v(value);
		// defaults none
		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addOrgService error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addCity(City& _return, const Credentials& credentials, const UserDevice& userdevice, const City& value)
{
	printf("addCity\n");
	try  {
		transaction t(db->begin());
		City v(value);
		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addCity error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addDictEntry(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const DictEntry& value)
{
	printf("addDictEntry\n");
	try  {
		transaction t(db->begin());
		DictEntry v(value);
		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addDictEntry error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addVehicleColor(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const NUMBER32 value)
{
	DictEntry v;
	v.tag = DictTag::TAG_VEHICLE_COLOR;
	v.intvalue = value;
	v.strvalue = name;
	addDictEntry(_return, credentials, userdevice, v);
}

void PassengerServiceHandler::addVehicleBrand(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const VehicleCategory::type category, const STR& name)
{
	DictEntry v;
	v.tag = DictTag::TAG_VEHICLE_BRAND;
	v.intvalue = category;
	v.strvalue = name;
	addDictEntry(_return, credentials, userdevice, v);
}

void PassengerServiceHandler::addVehicleModel(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const ID brandid, const STR& name, const NUMBER32 year)
{
	DictEntry v;
	v.parent = brandid;
	v.tag = DictTag::TAG_VEHICLE_MODEL;
	if ((year > 1700) && (year > 2700))
		v.intvalue = year;
	v.strvalue = name;
	addDictEntry(_return, credentials, userdevice, v);
}

void PassengerServiceHandler::addVehicleModelByBrandName(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& brandname, const STR& name, const NUMBER32 year)
{
	DictEntry v;
	ID brandid = getIdBrand(brandname);
	if (brandid < 0)
	{
		// raise?
		return;
	}
	v.parent = brandid;
	v.tag = DictTag::TAG_VEHICLE_MODEL;
	if ((year > 1700) && (year > 2700))
		v.intvalue = year;
	v.strvalue = name;
	addDictEntry(_return, credentials, userdevice, v);
}

void PassengerServiceHandler::addRate(Rate& _return, const Credentials& credentials, const UserDevice& userdevice, const Rate& value)
{
	printf("addRate\n");
	try  {
		transaction t(db->begin());
		Rate v(value);
		// defaults
		if (!v.__isset.active)
			v.active = true;
		if (!v.__isset.datestart)
			v.datestart = currentTime();

		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addRate error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addServiceOrder(ServiceOrder& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrder& value)
{
	printf("addServiceOrder\n");
	try  {
		transaction t(db->begin());
		ServiceOrder v(value);
		// defaults
		if (!v.__isset.ordertype)
			v.ordertype = OrderType::TAXI;
		if (!v.__isset.ordertimetype)
			v.ordertimetype = OrderTimeType::URGENT;
		if (!v.__isset.stage)
			v.stage = OrderStage::ORDERACCEPT;
		if (!v.__isset.created)
			v.created = currentTime();
		if (v.ordertimetype == OrderTimeType::URGENT)
			v.sheduletime = v.created;

		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addServiceOrder error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addServiceOrderDecline(ServiceOrderDecline& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrderDecline& value)
{
	printf("addServiceOrderDecline\n");
	if (!value.__isset.serviceorderid)
		return;

	try  {
		transaction t(db->begin());
		ServiceOrderDecline v(value);
		// defaults
		if (!v.__isset.cause)
			v.cause = DeclineOrderCause::DECLINE_BY_DRIVER;
		if (!v.__isset.driverid)	// driver added for faster search
			v.driverid = getDriverIdFromServiceOrder(v.serviceorderid);

		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addServiceOrderDecline error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::addClaim(Claim& _return, const Credentials& credentials, const UserDevice& userdevice, const Claim& value)
{
	printf("addClaim\n");
	if (!value.__isset.serviceorderid)
		return;

	try  {
		transaction t(db->begin());
		Claim v(value);
		// defaults
		if (!v.__isset.claimtime)
			v.claimtime = currentTime();
		if (!v.__isset.personrole)
			v.personrole = PersonRole::NOTAUTHORIZED;
		if (!v.__isset.driverid)	// driver added for faster search
			v.driverid = getDriverIdFromServiceOrder(v.serviceorderid);

		ID id = db->persist(v);
		_return = v;
		_return.id = id;
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addClaim error: %s.\n", e.what());
		return;
	}
}

ID PassengerServiceHandler::addTrack(const Credentials& credentials, const UserDevice& userdevice, const Track& value)
{
	printf("addTrack\n");
	ID id = -1;
	if (!value.__isset.driverid)
		return id;

	try  {
		transaction t(db->begin());
		Track v(value);
		// defaults
		if (!v.__isset.tracktime)
			v.tracktime = currentTime();
		id = db->persist(v);
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("addTrack error: %s.\n", e.what());
	}
	return id;
}

void PassengerServiceHandler::rmRate(const Credentials& credentials, const UserDevice& userdevice, const ID id)
{
	try  {
		transaction t(db->begin());
		db->erase<Rate> (id);
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("rmRate error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::rmServiceOrder(const Credentials& credentials, const UserDevice& userdevice, const ID id)
{
	try  {
		transaction t(db->begin());
		db->erase<ServiceOrder> (id);
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("rmServiceOrder error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::rmServiceOrderDecline(const Credentials& credentials, const UserDevice& userdevice, const ID id)
{
	try  {
		transaction t(db->begin());
		db->erase<ServiceOrderDecline> (id);
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("rmServiceOrderDecline error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::rmClaim(const Credentials& credentials, const UserDevice& userdevice, const ID id)
{
	try  {
		transaction t(db->begin());
		db->erase<Claim> (id);
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("rmClaim error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::rmTrack(const Credentials& credentials, const UserDevice& userdevice, const ID id)
{
	try  {
		transaction t(db->begin());
		db->erase<Track> (id);
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("rmTrack error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findRate(Rates& _return, const Credentials& credentials, const UserDevice& userdevice, const Rate& search, const RowRange& rowrange)
{
	printf("findRate\n");
	try  {
		transaction t(db->begin());
		Rate v(search);
		typedef odb::result<Rate> Result;
		typedef odb::query<Rate> Query;
		Query q("ORDER BY" + Query::name);

		// first unique id or name
		if ((v.__isset.id) && (v.id > 0))
			q = (Query::id == v.id);
		else
			if (v.__isset.name)
				q = ((Query::name.like(v.name+"%")) + "ORDER BY" + Query::name);
			else
			{
				// set defaults
				if (!v.__isset.active)
					v.active = true;
				if (v.active)
				{
					// show active
					if (v.__isset.cityid) 
					{
						if (v.__isset.orgserviceid) 
						{
							q = (((Query::active == v.active) && (Query::cityid == v.cityid) && (Query::orgserviceid == v.orgserviceid)) + "ORDER BY" + Query::name);
						}
						else
						{
							q = (((Query::active == v.active) && (Query::cityid == v.cityid)) + "ORDER BY" + Query::name);
						}
					}
					else
					{
						if (v.__isset.orgserviceid) 
						{
							q = (((Query::active == v.active) && (Query::orgserviceid == v.orgserviceid)) + "ORDER BY" + Query::name);
						}
						else
						{
							q = (((Query::active == v.active)) + "ORDER BY" + Query::name);
						}
					}
				}
				else
				{
					// search in archive
					if (v.__isset.datestart) 
					{
						if (v.__isset.datefinish)
							q = (((Query::datestart == v.datestart) && (Query::datestart == v.datestart)) + "ORDER BY" + Query::name);
						else
							q = ((Query::datestart == v.datestart) + "ORDER BY" + Query::name);
					}
					else
					{
						if (v.__isset.datefinish)
							q = ((Query::datestart == v.datestart) + "ORDER BY" + Query::name);
						else
							q = ("ORDER BY" + Query::name);
					}
				}
			}


		Result rq (db->query<Rate> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findRate error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findOrg(Orgs& _return, const Credentials& credentials, const UserDevice& userdevice, const Org& search, const RowRange& rowrange)
{
	printf("findOrg\n");
	try  {
		transaction t(db->begin());
		Org v(search);
		typedef odb::result<Org> Result;
		typedef odb::query<Org> Query;
		Query q("ORDER BY" + Query::name);
		// first unique id or name
		if ((v.__isset.id) && (v.id > 0))
			q = (Query::id == v.id);
		else
			if (v.__isset.name)
				q = ((Query::name.like(v.name+"%")) + "ORDER BY" + Query::name);

		Result rq (db->query<Org> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findOrg error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findOrgByName(Orgs& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange)
{
	Org v;
	if (!name.empty())
		v.name = name;
	findOrg(_return, credentials, userdevice, v, rowrange);
}

void PassengerServiceHandler::findBank(Banks& _return, const Credentials& credentials, const UserDevice& userdevice, const Bank& search, const RowRange& rowrange)
{
	printf("findBank\n");
	try  {
		transaction t(db->begin());
		Bank v(search);
		typedef odb::result<Bank> Result;
		typedef odb::query<Bank> Query;
		Query q("ORDER BY" + Query::name);
		// first unique id or name
		if ((v.__isset.id) && (v.id > 0))
			q = (Query::id == v.id);
		else
			if (v.__isset.name)
				q = ((Query::name.like(v.name+"%")) + "ORDER BY" + Query::name);

		Result rq (db->query<Bank> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findBank error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findBankByName(Banks& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange)
{
	printf("findBankByName\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<Bank> Result;
		typedef odb::query<Bank> Query;
		Query q = ((Query::name.like(name+"%")) + "ORDER BY" + Query::name);
		Result rq (db->query<Bank> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findBankByName error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findOrgService(OrgServices& _return, const Credentials& credentials, const UserDevice& userdevice, const OrgService& search, const RowRange& rowrange)
{
	printf("findOrgService\n");
	try  {
		transaction t(db->begin());
		OrgService v(search);
		typedef odb::result<OrgService> Result;
		typedef odb::query<OrgService> Query;
		Query q("ORDER BY" + Query::name);
		// first unique id or name
		if ((v.__isset.id) && (v.id > 0))
			q = (Query::id == search.id);
		else
			if (v.__isset.name)
				q = ((Query::name.like(search.name+"%")) + "ORDER BY" + Query::name);
		Result rq (db->query<OrgService> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findOrgService error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findVehicleid(Vehicleids& _return, const Credentials& credentials, const UserDevice& userdevice, const Vehicle& search, const RowRange& rowrange)
{
	Vehicles r;
	findVehicle(r, credentials, userdevice, search, rowrange);
	for (Vehicles::iterator i (r.begin ()); i != r.end (); ++i)  {
		_return.push_back(i->id);
	}
}

void PassengerServiceHandler::findVehicle(Vehicles& _return, const Credentials& credentials, const UserDevice& userdevice, const Vehicle& search, const RowRange& rowrange)
{
	printf("findVehicle\n");
	try  {
		transaction t(db->begin());
		Vehicle v(search);
		typedef odb::result<Vehicle> Result;
		typedef odb::query<Vehicle> Query;
		Query q;
		// first unique id
		if ((v.__isset.id) && (v.id > 0))
			q = (Query::id == search.id);
		else
			if (v.__isset.plate)
				q = (Query::plate == search.plate);
		else
			if (v.__isset.platenumber)
				q = (Query::platenumber == search.platenumber);
		else
			if (v.__isset.vin)
				q = (Query::vin == search.vin);
		else
		{
		// search
			if (v.__isset.vehiclestatus)
			{
				if (v.__isset.vehiclecategory)
				{
					if (v.__isset.vehicletype)
					{
						q = (Query::vehiclestatus == search.vehiclestatus) && (Query::vehiclecategory == search.vehiclecategory) && (Query::vehicletype == search.vehicletype);
					} else 
					{
						q = (Query::vehiclestatus == search.vehiclestatus) && (Query::vehiclecategory == search.vehiclecategory);
					}
				}
				else
				{
					if (v.__isset.vehicletype)
					{
						q = (Query::vehiclestatus == search.vehiclestatus) && (Query::vehicletype == search.vehicletype);
					} else 
					{
						q = (Query::vehiclestatus == search.vehiclestatus);
					}
				}
			}
			else
			{
				if (v.__isset.vehiclecategory)
				{
					if (v.__isset.vehicletype)
					{
						q = (Query::vehiclecategory == search.vehiclecategory) && (Query::vehicletype == search.vehicletype);
					} else 
					{
						q = (Query::vehiclecategory == search.vehiclecategory);
					}
				}
			}
		}
		Result rq (db->query<Vehicle> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findVehicle error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findDictEntry(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const DictEntry& search, const RowRange& rowrange)
{
	printf("findDictEntry\n");
	try  {
		transaction t(db->begin());
		DictEntry v(search);
		typedef odb::result<DictEntry> Result;
		typedef odb::query<DictEntry> Query;
		Query q("ORDER BY" + Query::strvalue);
		if ((v.__isset.id) && (v.id > 0))
			q = (((Query::id == search.id)) + "ORDER BY" + Query::strvalue);
		else
			if (v.__isset.tag)
			{
				if ((v.__isset.parent))
					q = (((Query::tag == search.tag) && (Query::parent == search.parent)) + "ORDER BY" + Query::strvalue);
				else
					q = ((Query::tag == search.tag) + "ORDER BY" + Query::strvalue);
			} 
			else
			{
				if ((v.__isset.parent))
					q = ((Query::parent == search.parent) + "ORDER BY" + Query::strvalue);
			}

		Result rq (db->query<DictEntry> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findDictEntry error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findDictTag(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const DictTag::type search, const RowRange& rowrange)
{
	printf("findDictTag\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<DictEntry> Result;
		typedef odb::query<DictEntry> Query;
 		Result rq (db->query<DictEntry> ((Query::tag == search) + "ORDER BY" + Query::strvalue));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findDictTag error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findColor(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange)
{
	printf("findColor\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<DictEntry> Result;
		typedef odb::query<DictEntry> Query;
		Query q((Query::tag == DictTag::TAG_VEHICLE_COLOR) + "ORDER BY" + Query::strvalue);
		if (name.length() > 0)
			q = (((Query::tag == DictTag::TAG_VEHICLE_COLOR) && (Query::strvalue.like(name + "%"))) + "ORDER BY" + Query::strvalue);
 		Result rq (db->query<DictEntry> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findColor error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findBrand(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange)
{
	printf("findBrand\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<DictEntry> Result;
		typedef odb::query<DictEntry> Query;
		Query q;
		if (name.length() > 0)
			q = (((Query::tag == DictTag::TAG_VEHICLE_BRAND) && (Query::strvalue.like(name + "%"))) + "ORDER BY" + Query::strvalue);
		else
			q = ((Query::tag == DictTag::TAG_VEHICLE_BRAND) + "ORDER BY" + Query::strvalue);
 		Result rq (db->query<DictEntry> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findBrand error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findModel(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const ID brandid, const STR& name, const RowRange& rowrange)
{
	printf("findModel\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<DictEntry> Result;
		typedef odb::query<DictEntry> Query;
		Query q ((Query::tag == DictTag::TAG_VEHICLE_MODEL) + "ORDER BY" + Query::strvalue);
		if (brandid != 0)
			if (name.length() > 0)
				q = (((Query::tag == DictTag::TAG_VEHICLE_MODEL) && (Query::parent == brandid) && (Query::strvalue.like(name + "%")))  + "ORDER BY" + Query::strvalue);
			else
				q = (((Query::tag == DictTag::TAG_VEHICLE_MODEL) && (Query::parent == brandid)) + "ORDER BY" + Query::strvalue);
		else
			if (name.length() > 0)
				q = (((Query::tag == DictTag::TAG_VEHICLE_MODEL) && (Query::strvalue.like(name + "%"))) + "ORDER BY" + Query::strvalue);

 		Result rq (db->query<DictEntry> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findModel error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findModelByBrandName(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& brandname, const STR& name, const RowRange& rowrange)
{
	printf("findModelByBrandName\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<DictEntry> Result;
		typedef odb::query<DictEntry> Query;

		ID brandid = getIdBrand(brandname);
		Query q("ORDER BY" + Query::strvalue);
		if (brandid >= 0)
		{
			if (name.length() > 0)
				q = (((Query::tag == DictTag::TAG_VEHICLE_MODEL) && (Query::parent == brandid) && (Query::strvalue.like(name + "%"))) + "ORDER BY" + Query::strvalue);
			else
				q = (((Query::tag == DictTag::TAG_VEHICLE_MODEL) && (Query::parent == brandid)) + "ORDER BY" + Query::strvalue);
		}
		else
			if (name.length() > 0)
				q = (((Query::tag == DictTag::TAG_VEHICLE_MODEL) && (Query::strvalue.like(name + "%"))) + "ORDER BY" + Query::strvalue);

 		Result rq (db->query<DictEntry> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findModelByBrandName error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findCity(Cities& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange)
{
	printf("findCity\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<City> Result;
		typedef odb::query<City> Query;
		Query q("ORDER BY" + Query::name);
		if (name.length() > 0)
			q = ((Query::name.like(name + "%")) + "ORDER BY" + Query::name);
 		Result rq (db->query<City> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findCities error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findDriver(Drivers& _return, const Credentials& credentials, const UserDevice& userdevice, const Driver& search, const RowRange& rowrange)
{
	printf("findDriver\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<Driver> Result;
		typedef odb::query<Driver> Query;
		Query q;
		
		// first unique id
		if ((search.__isset.id) && (search.id > 0))
			q = (Query::id == search.id);
		else
			if ((search.__isset.callsign) && (search.callsign > 0))
				q = (Query::callsign == search.callsign);
			else
				if ((search.__isset.nickname) && (!search.nickname.empty()))
					q = (Query::nickname == search.nickname);
				else
		{
		// search
			if (search.__isset.status)
			{
				if (search.__isset.online)
				{
					if (search.__isset.cityid)
					{
						q = ((Query::status == search.status) && (Query::online == search.online) && (Query::cityid == search.cityid)) + "ORDER BY" + Query::nickname;
					} 
					else 
					{
						q = ((Query::status == search.status) && (Query::online == search.online)) + "ORDER BY" + Query::nickname;
					}
				}
				else
				{
					if (search.__isset.cityid)
					{
						q = (Query::cityid == search.cityid) +  "ORDER BY" + Query::nickname;
					} 
				}
			}
			else
			{
				if (search.__isset.online)
				{
					if (search.__isset.cityid)
					{
						q = ((Query::online == search.online) && (Query::cityid == search.cityid)) + "ORDER BY" + Query::nickname;
					} 
					else 
					{
						q = (Query::online == search.online) + "ORDER BY" + Query::nickname;
					}
				}
				else
				{
					if (search.__isset.cityid)
					{
						q = (Query::cityid == search.cityid) + "ORDER BY" + Query::nickname;
					} 
				}
			}
		}
		
 		Result rq (db->query<Driver> (q));
		taxi::ID carpoolid = search.svc.at(taxi::TaxiServiceRole::TS_CARPOOL);
		taxi::ID dispatchid = search.svc.at(taxi::TaxiServiceRole::TS_DISPATCH);

		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			if (((carpoolid == 0) || (i->svc[taxi::TaxiServiceRole::TS_CARPOOL] == carpoolid)) &&
				((dispatchid == 0) || (i->svc[taxi::TaxiServiceRole::TS_CARPOOL] == dispatchid))
			)
				_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findDriver error: %s.\n", e.what());
		return;
	}
}

void PassengerServiceHandler::findCustomer(Customers& _return, const Credentials& credentials, const UserDevice& userdevice, const Customer& search, const RowRange& rowrange)
{
	printf("findCustomer\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<Customer> Result;
		typedef odb::query<Customer> Query;
		Query q;
		
		// first unique id
		if ((search.__isset.id) && (search.id > 0))
			q = (Query::id == search.id);
		else
			if ((search.__isset.nickname) && (!search.nickname.empty()))
				q = (Query::nickname == search.nickname);
			else
		{
		// search
			if (search.__isset.cityid)
			{
				if (search.__isset.active)
				{
					if (search.__isset.enabled)
					{
						if (search.__isset.taxtype)
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::cityid == search.cityid) && (Query::active == search.active) && (Query::enabled == search.enabled) && (Query::taxtype == search.taxtype) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::cityid == search.cityid) && (Query::active == search.active) && (Query::enabled == search.enabled) && (Query::taxtype == search.taxtype)) + "ORDER BY" + Query::nickname;
							}
						}
						else
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::cityid == search.cityid) && (Query::active == search.active) && (Query::enabled == search.enabled) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::cityid == search.cityid) && (Query::active == search.active) && (Query::enabled == search.enabled)) + "ORDER BY" + Query::nickname;
							}
						}
					} 
					else
					{
						if (search.__isset.taxtype)
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::cityid == search.cityid) && (Query::active == search.active) && (Query::taxtype == search.taxtype) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::cityid == search.cityid) && (Query::active == search.active) && (Query::taxtype == search.taxtype)) + "ORDER BY" + Query::nickname;
							}
						}
						else
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::cityid == search.cityid) && (Query::active == search.active) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::cityid == search.cityid) && (Query::active == search.active)) + "ORDER BY" + Query::nickname;
							}
						}
					}
				}
				else
				{
					if (search.__isset.enabled)
					{
						if (search.__isset.taxtype)
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::cityid == search.cityid) && (Query::enabled == search.enabled) && (Query::taxtype == search.taxtype) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::cityid == search.cityid) && (Query::enabled == search.enabled) && (Query::taxtype == search.taxtype)) + "ORDER BY" + Query::nickname;
							}
						}
						else
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::cityid == search.cityid) && (Query::enabled == search.enabled) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::cityid == search.cityid) && (Query::enabled == search.enabled)) + "ORDER BY" + Query::nickname;
							}
						}
					} 
					else
					{
						if (search.__isset.taxtype)
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::cityid == search.cityid) && (Query::taxtype == search.taxtype) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::cityid == search.cityid) && (Query::taxtype == search.taxtype)) + "ORDER BY" + Query::nickname;
							}
						}
						else
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::cityid == search.cityid) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::cityid == search.cityid)) + "ORDER BY" + Query::nickname;
							}
						}
					}
				}
			}
			else
			{
				if (search.__isset.active)
				{
					if (search.__isset.enabled)
					{
						if (search.__isset.taxtype)
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::active == search.active) && (Query::enabled == search.enabled) && (Query::taxtype == search.taxtype) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::active == search.active) && (Query::enabled == search.enabled) && (Query::taxtype == search.taxtype)) + "ORDER BY" + Query::nickname;
							}
						}
						else
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::active == search.active) && (Query::enabled == search.enabled) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::active == search.active) && (Query::enabled == search.enabled)) + "ORDER BY" + Query::nickname;
							}
						}
					} 
					else
					{
						if (search.__isset.taxtype)
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::active == search.active) && (Query::taxtype == search.taxtype) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::active == search.active) && (Query::taxtype == search.taxtype)) + "ORDER BY" + Query::nickname;
							}
						}
						else
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::active == search.active) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::active == search.active)) + "ORDER BY" + Query::nickname;
							}
						}
					}
				}
				else
				{
					if (search.__isset.enabled)
					{
						if (search.__isset.taxtype)
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::enabled == search.enabled) && (Query::taxtype == search.taxtype) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::enabled == search.enabled) && (Query::taxtype == search.taxtype)) + "ORDER BY" + Query::nickname;
							}
						}
						else
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::enabled == search.enabled) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::enabled == search.enabled)) + "ORDER BY" + Query::nickname;
							}
						}
					} 
					else
					{
						if (search.__isset.taxtype)
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::taxtype == search.taxtype) && (Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ((Query::taxtype == search.taxtype)) + "ORDER BY" + Query::nickname;
							}
						}
						else
						{
							if (search.__isset.preferreddriverid)
							{
								q = ((Query::preferreddriverid == search.preferreddriverid)) + "ORDER BY" + Query::nickname;
							}
							else
							{
								q = ("ORDER BY" + Query::nickname);
							}
						}
					}
				}
			}
		}

 		Result rq (db->query<Customer> (q));
		taxi::ID orgid = search.organization.at(taxi::OrgServiceDept::CUSTOMER);

		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			if ((orgid == 0) || (i->organization[taxi::OrgServiceDept::CUSTOMER] == orgid))
				_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findCustomer error: %s.\n", e.what());
		return;

	}
}

void PassengerServiceHandler::findPassenger(Passengers& _return, const Credentials& credentials, const UserDevice& userdevice, const Passenger& search, const RowRange& rowrange)
	{
		printf("findPassenger\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<Passenger> Result;
		typedef odb::query<Passenger> Query;
		Query q;
		
		// first unique id
		if ((search.__isset.id) && (search.id > 0))
			q = (Query::id == search.id);
		else
			if ((search.__isset.personid) && (search.personid > 0))
				q = (Query::personid == search.personid);
			else
		{
		// search
			if (search.__isset.cityid)
			{
				if (search.__isset.customerid)
				{
					if (search.__isset.isoperator)
					{
						if (search.__isset.isvip)
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
						else
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
					} 
					else
					{
						if (search.__isset.isvip)
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isvip == search.isvip) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isvip == search.isvip) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isvip == search.isvip) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::isvip == search.isvip) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
						else
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										&& (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) && (Query::customerid == search.customerid) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
					}
				}
				else
				{
					if (search.__isset.isoperator)
					{
						if (search.__isset.isvip)
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
						else
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) 
										&& (Query::isoperator == search.isoperator) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) 
										&& (Query::isoperator == search.isoperator) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) 
										&& (Query::isoperator == search.isoperator) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) 
										&& (Query::isoperator == search.isoperator) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
					} 
					else
					{
						if (search.__isset.isvip)
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) 
										&& (Query::isvip == search.isvip) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) 
										&& (Query::isvip == search.isvip) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) 
										&& (Query::isvip == search.isvip) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) 
										&& (Query::isvip == search.isvip) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
						else
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) 
										&& (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) 
										&& (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::cityid == search.cityid) 
										&& (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::cityid == search.cityid) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
					}
				}
			}
			else
			{
				if (search.__isset.customerid)
				{
					if (search.__isset.isoperator)
					{
						if (search.__isset.isvip)
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
						else
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::customerid == search.customerid) 
										&& (Query::isoperator == search.isoperator) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
					} 
					else
					{
						if (search.__isset.isvip)
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::customerid == search.customerid) 
										&& (Query::isvip == search.isvip) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::customerid == search.customerid) 
										&& (Query::isvip == search.isvip) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::customerid == search.customerid) 
										&& (Query::isvip == search.isvip) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::customerid == search.customerid) 
										&& (Query::isvip == search.isvip) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
						else
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = ((Query::customerid == search.customerid) 
										&& (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::customerid == search.customerid) 
										&& (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = ((Query::customerid == search.customerid) 
										&& (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = ((Query::customerid == search.customerid) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
					}
				}
				else
				{
					if (search.__isset.isoperator)
					{
						if (search.__isset.isvip)
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = (
										(Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = (
										(Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = (
										(Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = (
										(Query::isoperator == search.isoperator) && (Query::isvip == search.isvip) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
						else
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = (
										(Query::isoperator == search.isoperator) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = (
										(Query::isoperator == search.isoperator) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = (
										(Query::isoperator == search.isoperator) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = (
										(Query::isoperator == search.isoperator) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
					} 
					else
					{
						if (search.__isset.isvip)
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = (
										(Query::isvip == search.isvip) && (Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = (
										(Query::isvip == search.isvip) && (Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = (
										(Query::isvip == search.isvip) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = (
										(Query::isvip == search.isvip) 
										) + "ORDER BY" + Query::customerid;
								}
							}
						}
						else
						{
							if (search.__isset.status)
							{
								if (search.__isset.canorder)
								{
									q = (
										(Query::status == search.status) && (Query::canorder == search.canorder)
										) + "ORDER BY" + Query::customerid;
								}
								else {
									q = (
										(Query::status == search.status) 
										) + "ORDER BY" + Query::customerid;
								}
							}
							else
							{
								if (search.__isset.canorder)
								{
									q = (
										(Query::canorder == search.canorder)
										 + "ORDER BY" + Query::customerid);
								}
								else {
									q = ("ORDER BY" + Query::customerid);
								}
							}
						}
					}
				}
			}
		}

 		Result rq (db->query<Passenger> (q));
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findPassenger error: %s.\n", e.what());
		return;

	}
}

void PassengerServiceHandler::findServiceOrder(ServiceOrders& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrder& search, const DateRange& sheduletime, const RowRange& rowrange)
{
	printf("findServiceOrder\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<ServiceOrder> Result;
		typedef odb::query<ServiceOrder> Query;
		Query q;
		
		// first unique id
		if ((search.__isset.id) && (search.id > 0))
			q = (Query::id == search.id);
		else
			if (search.__isset.tag)
				q = ((Query::tag == search.tag) && (Query::sheduletime >= sheduletime.start) && (Query::sheduletime <= sheduletime.finish) + "ORDER BY" + Query::sheduletime + "DESC");
			else
				if (search.__isset.notes)
					q = ((Query::notes.like(search.notes + "%") && (Query::sheduletime >= sheduletime.start) && (Query::sheduletime <= sheduletime.finish)) + "ORDER BY" + Query::sheduletime + "DESC");
				else
				{
				// search
					if ((!search.__isset.cityid) || (!search.__isset.ordertype) || (!search.__isset.ordertimetype) || (!search.__isset.svc) || (!search.__isset.stage))
						return;
					q = ((Query::cityid == search.cityid) && (Query::ordertype == search.ordertype) && (Query::ordertimetype == search.ordertimetype) && (Query::stage == search.stage));
				}
 		Result rq (db->query<ServiceOrder> (q));
		
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			if (search.__isset.svc && (search.svc.at(taxi::TaxiServiceRole::TS_DISPATCH) == i->svc.at(taxi::TaxiServiceRole::TS_DISPATCH)))
				_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findServiceOrder error: %s.\n", e.what());
		return;

	}
}

void PassengerServiceHandler::findServiceOrderDecline(ServiceOrderDeclines& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrderDecline& search, const DateRange& sheduletime, const RowRange& rowrange)
{
	printf("findServiceOrderDecline\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<ServiceOrderDecline> Result;
		typedef odb::query<ServiceOrderDecline> Query;
		Query q;
		
		// first unique id
		if ((search.__isset.id) && (search.id > 0))
			q = (Query::id == search.id);
		else
			if (search.__isset.serviceorderid)
				q = (Query::serviceorderid == search.serviceorderid);
			else
				if (search.__isset.notes)
					q = ((Query::notes.like(search.notes + "%") && (Query::declinetime >= sheduletime.start) && (Query::declinetime <= sheduletime.finish)) + "ORDER BY" + Query::declinetime + "DESC");
				else
				{
				// search
					if (!search.__isset.driverid)
					{
						if (!search.__isset.cause)
						{
							q = (((Query::driverid == search.driverid) && (Query::cause == search.cause)) + "ORDER BY" + Query::declinetime + "DESC");				
						}
						else
						{
							q = ((Query::driverid == search.driverid) + "ORDER BY" + Query::declinetime + "DESC");				
						}
					}
					else
					{
						if (!search.__isset.cause)
						{
							q = ((Query::cause == search.cause) + "ORDER BY" + Query::declinetime + "DESC");				
						}
						else
						{
							q = ("ORDER BY" + Query::declinetime + "DESC");				
						}
					}
				}
 		Result rq (db->query<ServiceOrderDecline> (q));
		
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findServiceOrderDecline error: %s.\n", e.what());
		return;

	}
}

void PassengerServiceHandler::findClaim(Claims& _return, const Credentials& credentials, const UserDevice& userdevice, const Claim& search, const DateRange& sheduletime, const RowRange& rowrange)
{
	printf("findClaim\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<Claim> Result;
		typedef odb::query<Claim> Query;
		Query q;
		
		// first unique id
		if ((search.__isset.id) && (search.id > 0))
			q = (Query::id == search.id);
		else
			if (search.__isset.serviceorderid)
				q = (Query::serviceorderid == search.serviceorderid);
			else
				if (search.__isset.notes)
					q = ((Query::notes.like(search.notes + "%") && (Query::claimtime >= sheduletime.start) && (Query::claimtime <= sheduletime.finish)) + "ORDER BY" + Query::claimtime + "DESC");
				else
				{
				// search
					if (!search.__isset.driverid)
					{
						if (!search.__isset.personrole)
						{
							q = (((Query::driverid == search.driverid) && (Query::personrole == search.personrole) && (Query::claimtime >= sheduletime.start) && (Query::claimtime <= sheduletime.finish)) + "ORDER BY" + Query::claimtime + "DESC");				
						}
						else
						{
							q = (((Query::driverid == search.driverid) && (Query::claimtime >= sheduletime.start) && (Query::claimtime <= sheduletime.finish)) + "ORDER BY" + Query::claimtime + "DESC");				
						}
					}
					else
					{
						if (!search.__isset.personrole)
						{
							q = (((Query::personrole == search.personrole) && (Query::claimtime >= sheduletime.start) && (Query::claimtime <= sheduletime.finish)) + "ORDER BY" + Query::claimtime + "DESC");				
						}
						else
						{
							q = (((Query::claimtime >= sheduletime.start) && (Query::claimtime <= sheduletime.finish)) + "ORDER BY" + Query::claimtime + "DESC");				
						}
					}
				}
 		Result rq (db->query<Claim> (q));
		
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findClaim error: %s.\n", e.what());
		return;

	}
}

void PassengerServiceHandler::findTrack(Tracks& _return, const Credentials& credentials, const UserDevice& userdevice, const Track& search, const DateRange& sheduletime, const RowRange& rowrange)
{
	printf("findTrack\n");
	try  {
		transaction t(db->begin());
		typedef odb::result<Track> Result;
		typedef odb::query<Track> Query;
		Query q;
		
		// first unique id
		if ((search.__isset.id) && (search.id > 0))
			q = (Query::id == search.id);
		else
			if (search.__isset.serviceorderid)
				q = ((Query::serviceorderid == search.serviceorderid) + "ORDER BY" + Query::tracktime);
			else
			{
			// search
				if (!search.__isset.driverid)
				{
					q = (((Query::driverid == search.driverid) && (Query::tracktime >= sheduletime.start) && (Query::tracktime <= sheduletime.finish)) + "ORDER BY" + Query::tracktime);
				}
				else
				{
					q = (((Query::tracktime >= sheduletime.start) && (Query::tracktime <= sheduletime.finish)) + "ORDER BY" + Query::tracktime);
				}
			}
 		Result rq (db->query<Track> (q));
		
		for (Result::iterator i (rq.begin ()); i != rq.end (); ++i)  {
			_return.push_back(*i);
		}
		t.commit();
	}
	catch (const odb::exception& e)  { 
		printf("findTrack error: %s.\n", e.what());
		return;

	}
}

