// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "PassengerService.h"
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

class PassengerServiceHandler : virtual public PassengerServiceIf {
 public:
  PassengerServiceHandler() {
    // Your initialization goes here
  }

  void getDocument(Document& _return, const Credentials& credentials, const UserDevice& userdevice, const ID id) {
    // Your implementation goes here
    printf("getDocument\n");
  }

  void getOrg(Org& _return, const Credentials& credentials, const UserDevice& userdevice, const ID id) {
    // Your implementation goes here
    printf("getOrg\n");
  }

  void loginDriver(Driver& _return, const Credentials& credentials, const UserDevice& userdevice) {
    // Your implementation goes here
    printf("loginDriver\n");
  }

  void loginPassenger(Passenger& _return, const Credentials& credentials, const UserDevice& userdevice) {
    // Your implementation goes here
    printf("loginPassenger\n");
  }

  void findDictEntry(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const DictEntry& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findDictEntry\n");
  }

  void findDictTag(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const DictTag::type search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findDictTag\n");
  }

  void findBank(Banks& _return, const Credentials& credentials, const UserDevice& userdevice, const Bank& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findBank\n");
  }

  void findBankByName(Banks& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findBankByName\n");
  }

  void findOrgService(OrgServices& _return, const Credentials& credentials, const UserDevice& userdevice, const OrgService& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findOrgService\n");
  }

  void findVehicle(Vehicles& _return, const Credentials& credentials, const UserDevice& userdevice, const Vehicle& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findVehicle\n");
  }

  void findVehicleid(Vehicleids& _return, const Credentials& credentials, const UserDevice& userdevice, const Vehicle& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findVehicleid\n");
  }

  void addCity(City& _return, const Credentials& credentials, const UserDevice& userdevice, const City& value) {
    // Your implementation goes here
    printf("addCity\n");
  }

  void addDictEntry(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const DictEntry& value) {
    // Your implementation goes here
    printf("addDictEntry\n");
  }

  void addBank(Bank& _return, const Credentials& credentials, const UserDevice& userdevice, const Bank& value) {
    // Your implementation goes here
    printf("addBank\n");
  }

  void addOrg(Org& _return, const Credentials& credentials, const UserDevice& userdevice, const Org& value) {
    // Your implementation goes here
    printf("addOrg\n");
  }

  void addOrgService(OrgService& _return, const Credentials& credentials, const UserDevice& userdevice, const OrgService& value) {
    // Your implementation goes here
    printf("addOrgService\n");
  }

  void addManager(Manager& _return, const Credentials& credentials, const UserDevice& userdevice, const Manager& value) {
    // Your implementation goes here
    printf("addManager\n");
  }

  void addCustomer(Customer& _return, const Credentials& credentials, const UserDevice& userdevice, const Customer& value) {
    // Your implementation goes here
    printf("addCustomer\n");
  }

  void addPassenger(Passenger& _return, const Credentials& credentials, const UserDevice& userdevice, const Passenger& value) {
    // Your implementation goes here
    printf("addPassenger\n");
  }

  void addDispatcher(Dispatcher& _return, const Credentials& credentials, const UserDevice& userdevice, const Dispatcher& value) {
    // Your implementation goes here
    printf("addDispatcher\n");
  }

  void addDriver(Driver& _return, const Credentials& credentials, const UserDevice& userdevice, const Driver& value) {
    // Your implementation goes here
    printf("addDriver\n");
  }

  void addVehicleColor(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const NUMBER32 value) {
    // Your implementation goes here
    printf("addVehicleColor\n");
  }

  void addVehicleBrand(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const VehicleCategory::type category, const STR& name) {
    // Your implementation goes here
    printf("addVehicleBrand\n");
  }

  void addVehicleModel(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const ID brandid, const STR& name, const NUMBER32 year) {
    // Your implementation goes here
    printf("addVehicleModel\n");
  }

  void addVehicleModelByBrandName(DictEntry& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& brandname, const STR& name, const NUMBER32 year) {
    // Your implementation goes here
    printf("addVehicleModelByBrandName\n");
  }

  void addTariffPlan(TariffPlan& _return, const Credentials& credentials, const UserDevice& userdevice, const TariffPlan& value) {
    // Your implementation goes here
    printf("addTariffPlan\n");
  }

  void addRate(Rate& _return, const Credentials& credentials, const UserDevice& userdevice, const Rate& value) {
    // Your implementation goes here
    printf("addRate\n");
  }

  void addServiceOrder(ServiceOrder& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrder& value) {
    // Your implementation goes here
    printf("addServiceOrder\n");
  }

  void getServiceOrder(ServiceOrder& _return, const Credentials& credentials, const UserDevice& userdevice, const ID serviceorderid) {
    // Your implementation goes here
    printf("getServiceOrder\n");
  }

  void addServiceOrderDecline(ServiceOrderDecline& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrderDecline& value) {
    // Your implementation goes here
    printf("addServiceOrderDecline\n");
  }

  void addClaim(Claim& _return, const Credentials& credentials, const UserDevice& userdevice, const Claim& value) {
    // Your implementation goes here
    printf("addClaim\n");
  }

  void addTrack(Track& _return, const Credentials& credentials, const UserDevice& userdevice, const Track& value) {
    // Your implementation goes here
    printf("addTrack\n");
  }

  void rmManager(const Credentials& credentials, const UserDevice& userdevice, const ID id) {
    // Your implementation goes here
    printf("rmManager\n");
  }

  void rmTariffPlan(const Credentials& credentials, const UserDevice& userdevice, const ID id) {
    // Your implementation goes here
    printf("rmTariffPlan\n");
  }

  void rmRate(const Credentials& credentials, const UserDevice& userdevice, const ID id) {
    // Your implementation goes here
    printf("rmRate\n");
  }

  void rmServiceOrder(const Credentials& credentials, const UserDevice& userdevice, const ID id) {
    // Your implementation goes here
    printf("rmServiceOrder\n");
  }

  void rmServiceOrderDecline(const Credentials& credentials, const UserDevice& userdevice, const ID id) {
    // Your implementation goes here
    printf("rmServiceOrderDecline\n");
  }

  void rmClaim(const Credentials& credentials, const UserDevice& userdevice, const ID id) {
    // Your implementation goes here
    printf("rmClaim\n");
  }

  void rmTrack(const Credentials& credentials, const UserDevice& userdevice, const ID id) {
    // Your implementation goes here
    printf("rmTrack\n");
  }

  void rmOldTracks(const Credentials& credentials, const UserDevice& userdevice, const DATE olderthan) {
    // Your implementation goes here
    printf("rmOldTracks\n");
  }

  void findOrg(Orgs& _return, const Credentials& credentials, const UserDevice& userdevice, const Org& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findOrg\n");
  }

  void findOrgByName(Orgs& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findOrgByName\n");
  }

  void findColor(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findColor\n");
  }

  void findBrand(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findBrand\n");
  }

  void findModel(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const ID brandid, const STR& name, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findModel\n");
  }

  void findModelByBrandName(DictEntries& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& brandname, const STR& name, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findModelByBrandName\n");
  }

  void findCity(Cities& _return, const Credentials& credentials, const UserDevice& userdevice, const STR& name, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findCity\n");
  }

  void findManager(Managers& _return, const Credentials& credentials, const UserDevice& userdevice, const Manager& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findManager\n");
  }

  void findDriver(Drivers& _return, const Credentials& credentials, const UserDevice& userdevice, const Driver& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findDriver\n");
  }

  void getDriverOnlineAvailable(DriverOnlineMap& _return, const Credentials& credentials, const UserDevice& userdevice, const Cityid cityid) {
    // Your implementation goes here
    printf("getDriverOnlineAvailable\n");
  }

  void getDriverAvailable(Drivers& _return, const Credentials& credentials, const UserDevice& userdevice, const Cityid cityid) {
    // Your implementation goes here
    printf("getDriverAvailable\n");
  }

  void findCustomer(Customers& _return, const Credentials& credentials, const UserDevice& userdevice, const Customer& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findCustomer\n");
  }

  void findPassenger(Passengers& _return, const Credentials& credentials, const UserDevice& userdevice, const Passenger& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findPassenger\n");
  }

  void findTariffPlan(TariffPlans& _return, const Credentials& credentials, const UserDevice& userdevice, const TariffPlan& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findTariffPlan\n");
  }

  void findRate(Rates& _return, const Credentials& credentials, const UserDevice& userdevice, const Rate& search, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findRate\n");
  }

  void findServiceOrder(ServiceOrders& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrder& search, const DateRange& sheduletime, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findServiceOrder\n");
  }

  void findServiceOrderDecline(ServiceOrderDeclines& _return, const Credentials& credentials, const UserDevice& userdevice, const ServiceOrderDecline& search, const DateRange& sheduletime, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findServiceOrderDecline\n");
  }

  void findClaim(Claims& _return, const Credentials& credentials, const UserDevice& userdevice, const Claim& search, const DateRange& sheduletime, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findClaim\n");
  }

  void findTrack(Tracks& _return, const Credentials& credentials, const UserDevice& userdevice, const Track& search, const DateRange& sheduletime, const RowRange& rowrange) {
    // Your implementation goes here
    printf("findTrack\n");
  }

  void getEvents(NotificationEvents& _return, const Credentials& credentials, const UserDevice& userdevice, const RowRange& rowrange) {
    // Your implementation goes here
    printf("getEvents\n");
  }

  bool getOnline(const Credentials& credentials, const UserDevice& userdevice) {
    // Your implementation goes here
    printf("getOnline\n");
  }

  bool setOnline(const Credentials& credentials, const UserDevice& userdevice, const bool value) {
    // Your implementation goes here
    printf("setOnline\n");
  }

  bool takeOrder(const Credentials& credentials, const UserDevice& userdevice, const ID serviceorderid, const bool confirm, const NUMBER32 provisionminutes) {
    // Your implementation goes here
    printf("takeOrder\n");
  }

  bool startWaiting(const Credentials& credentials, const UserDevice& userdevice, const ID serviceorderid) {
    // Your implementation goes here
    printf("startWaiting\n");
  }

  bool startDriving(const Credentials& credentials, const UserDevice& userdevice, const ID serviceorderid) {
    // Your implementation goes here
    printf("startDriving\n");
  }

  bool stopDriving(const Credentials& credentials, const UserDevice& userdevice, const ID serviceorderid) {
    // Your implementation goes here
    printf("stopDriving\n");
  }

  bool completeOrder(const Credentials& credentials, const UserDevice& userdevice, const ID serviceorderid) {
    // Your implementation goes here
    printf("completeOrder\n");
  }

  bool cancelOrder(const Credentials& credentials, const UserDevice& userdevice, const ID serviceorderid) {
    // Your implementation goes here
    printf("cancelOrder\n");
  }

  bool failOrder(const Credentials& credentials, const UserDevice& userdevice, const ID serviceorderid) {
    // Your implementation goes here
    printf("failOrder\n");
  }

  void getDriverServiceOrder(ServiceOrders& _return, const Credentials& credentials, const UserDevice& userdevice, const RowRange& rowrange) {
    // Your implementation goes here
    printf("getDriverServiceOrder\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<PassengerServiceHandler> handler(new PassengerServiceHandler());
  shared_ptr<TProcessor> processor(new PassengerServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

