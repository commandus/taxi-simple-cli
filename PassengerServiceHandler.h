#include "PassengerService.h"
#include <odb/core.hxx>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::taxi;

#pragma once
class PassengerServiceHandler : virtual public PassengerServiceIf {
private:
	int argc;
	char **argv;
	odb::database *db;
protected:
	// Return brand id by name
	ID getIdBrand(const STR& name);
	// Return driver id from service order
	ID getDriverIdFromServiceOrder(ID serviceorderid);
	// Return loader id from service order
	ID getLoaderIdFromServiceOrder(ID serviceorderid);
public:
	PassengerServiceHandler(int argc, char **argv);
	~PassengerServiceHandler();

	void getDocument(Document& _return, const Credentials& credentials, const UserDevice& userdevice, const ID id);
	void getOrg(Org& _return, const Credentials& credentials, const UserDevice& userdevice, const ID id);
	void loginDriver(Driver& _return, const Credentials& credentials, const UserDevice& userdevice);
	void loginPassenger(Passenger& _return, const Credentials& credentials, const UserDevice& userdevice);
	void findDictEntry(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const DictEntry& search, const RowRange& rowrange);
	void findDictTag(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const DictTag::type search, const RowRange& rowrange);
	void findBank(Banks& _return, const Credentials& credentials, const UserDevice& userdevice, const Bank& search, const RowRange& rowrange);
	void findBankByName(Banks& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange);
	void findOrgService(OrgServices& _return, const Credentials& credentials, const UserDevice& userdevice, const OrgService& search, const RowRange& rowrange);
	void findVehicle(Vehicles& _return, const Credentials& credentials, const UserDevice& userdevice, const Vehicle& search, const RowRange& rowrange);
	void findVehicleid(Vehicleids& _return, const Credentials& credentials, const UserDevice& userdevice, const Vehicle& search, const RowRange& rowrange);
	void addCity(City& _return, const Credentials& credentials, const UserDevice& userdevice, const City& value);
	void addDictEntry(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const DictEntry& value);
	void addBank(Bank& _return, const Credentials& credentials, const UserDevice& userdevice, const Bank& value);
	void addOrg(Org& _return, const Credentials& credentials, const UserDevice& userdevice, const Org& value);
	void addOrgService(OrgService& _return, const Credentials& credentials, const UserDevice& userdevice, const OrgService& value);
	void addCustomer(Customer& _return, const Credentials& credentials, const UserDevice& userdevice, const Customer& value);
	void addPassenger(Passenger& _return, const Credentials& credentials, const UserDevice& userdevice, const Passenger& value);
	void addDriver(Driver& _return, const Credentials& credentials, const UserDevice& userdevice, const Driver& value);
	void addRate(Rate& _return, const Credentials& credentials, const UserDevice& userdevice, const Rate& value);
	void addVehicleColor(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const NUMBER32 value);
	void addVehicleBrand(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const VehicleCategory::type category, const STR& name);
	void addVehicleModel(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const ID brandid, const STR& name, const NUMBER32 year);
	void addVehicleModelByBrandName(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& brandname, const STR& name, const NUMBER32 year);
	void addServiceOrder(ServiceOrder& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrder& value);
	void addServiceOrderDecline(ServiceOrderDecline& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrderDecline& value);
	void addClaim(Claim& _return, const Credentials& credentials, const UserDevice& userdevice, const Claim& value);
	ID addTrack(const Credentials& credentials, const UserDevice& userdevice, const Track& value);

	void rmRate(const Credentials& credentials, const UserDevice& userdevice, const ID id);
	void rmServiceOrder(const Credentials& credentials, const UserDevice& userdevice, const ID id);
	void rmServiceOrderDecline(const Credentials& credentials, const UserDevice& userdevice, const ID id);
	void rmClaim(const Credentials& credentials, const UserDevice& userdevice, const ID id);
	void rmTrack(const Credentials& credentials, const UserDevice& userdevice, const ID id);

	void findOrg(Orgs& _return, const Credentials& credentials, const UserDevice& userdevice, const Org& search, const RowRange& rowrange);
	void findOrgByName(Orgs& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange);
	void findColor(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange);
	void findBrand(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange);
	void findModel(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const ID brandid, const STR& name, const RowRange& rowrange);
	void findModelByBrandName(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& brandname, const STR& name, const RowRange& rowrange);
	void findCity(Cities& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange);
	void findDriver(Drivers& _return, const Credentials& credentials, const UserDevice& userdevice, const Driver& search, const RowRange& rowrange);
	void findCustomer(Customers& _return, const Credentials& credentials, const UserDevice& userdevice, const Customer& search, const RowRange& rowrange);
	void findPassenger(Passengers& _return, const Credentials& credentials, const UserDevice& userdevice, const Passenger& search, const RowRange& rowrange);
	void findRate(Rates& _return, const Credentials& credentials, const UserDevice& userdevice, const Rate& search, const RowRange& rowrange);
	void findServiceOrder(ServiceOrders& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrder& search, const DateRange& sheduletime, const RowRange& rowrange);
	void findServiceOrderDecline(ServiceOrderDeclines& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrderDecline& search, const DateRange& sheduletime, const RowRange& rowrange);
	void findClaim(Claims& _return, const Credentials& credentials, const UserDevice& userdevice, const Claim& search, const DateRange& sheduletime, const RowRange& rowrange);
	void findTrack(Tracks& _return, const Credentials& credentials, const UserDevice& userdevice, const Track& search, const DateRange& sheduletime, const RowRange& rowrange);
};
