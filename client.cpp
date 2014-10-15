#include "client.h"

#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <streambuf>

#include <ctime>
#include "curl/curl.h"

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#include <codecvt>

#endif

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "PassengerService.h"
#include "utiltime.h"

#include "libconfig.h++" 

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace libconfig;
using namespace taxi;

#define DEF_CONFIG	"taxi-simple.cfg"

/**
	get person role from the string
*/
PersonRole::type readPersonRole(const std::string &str)
{
	if (str.compare("admin") == 0)
		return PersonRole::ADMIN;
	else 
		if (str.compare("manager") == 0)
			return PersonRole::MANAGER;
	else 
		if (str.compare("customer") == 0)
			return PersonRole::CUSTOMER;
	else 
		if (str.compare("dispatcher") == 0)
			return PersonRole::DISPATCHER;
	else 
		if (str.compare("driver") == 0)
			return PersonRole::DRIVER;
	else 
		if (str.compare("master") == 0)
			return PersonRole::MASTER;
	else 
		if (str.compare("operator") == 0)
			return PersonRole::OPERATOR;
	else 
		if (str.compare("passenger") == 0)
			return PersonRole::PASSENGER;
	else 
		if (str.compare("guest") == 0)
			return PersonRole::GUEST;
	else
		return PersonRole::NOTAUTHORIZED;
}

void couts(const STR &s)
{
	printf("%s", s.c_str());
}

void coutdate(const taxi::DATE &date)
{
	struct tm *tm = localtime(&date);
	// cout << std::put_time(tm, "%Y/%m/%d");
	char buffer [80];
	struct tm *ld = localtime(&date);
	strftime(buffer, sizeof(buffer), "%Y/%m/%d", ld);
	puts(buffer);
}						

void couttime(const taxi::DATE &date)
{
	struct tm *tm = localtime(&date);
	// cout << std::put_time(tm, "%Y/%m/%d %T");
        char buffer [80];
        struct tm *ld = localtime(&date);
        strftime(buffer, sizeof(buffer), "%Y/%m/%d %T", ld);
        puts(buffer);
}						

void coutPerson(taxi::Person &v)
{
	couts(v.lastname);
	cout << '\t';
	couts(v.firstname);
	cout << '\t';
	couts(v.middlename);
	cout << '\t';
	coutdate(v.birthdate);
	cout << '\t';
	couts(v.email);
	cout << '\t';
	couts(v.fax);
	cout << '\t';
	couts(v.phone1);
	cout << '\t';
	couts(v.phone2);
	cout << '\t';
}

void coutDocument(taxi::Document &v)
{
	cout << v.id;
	cout << '\t';
	couts(v.ser);
	cout << '\t';
	couts(v.number);
	cout << '\t';
	couts(v.issuer);
	cout << '\t';
	coutdate(v.issued);
	cout << '\t';
	coutdate(v.validity);
	cout << '\t';
	couts(v.category);
	cout << '\t';
	cout << v.expirience;
	cout << '\t';
}

void coutOrg(taxi::Org &v)
{
	cout << v.id << '\t' << v.orgrole << '\t' <<
		v.orgtype << '\t';
	coutdate(v.start);
	cout << v.tag << '\t';
	couts(v.name);
	cout << '\t';
	couts(v.fullname);
	cout << '\t';
	couts(v.shortname);
	cout << '\t';
	couts(v.inn);
	cout << '\t';
	couts(v.kpp);
	cout << '\t';
	couts(v.ogrn);
	cout << '\t';
	cout << v.orgaddress[OrgAddressType::OA_LEGAL] << '\t' << v.orgaddress[OrgAddressType::OA_REAL] << '\t' << v.orgaddress[OrgAddressType::OA_REPORT] << '\t';
	couts(v.phone);
	cout << '\t';
	couts(v.email);
	cout << '\t';
	for (OrgPosition::type op = OrgPosition::SELFEMP; op <= OrgPosition::MGR; op = static_cast<OrgPosition::type>(static_cast<int>(op) + 1))
	{
		coutPerson(v.employee[op]);
	}
	cout << v.bank[BankRole::BANK_ACCOUNT] << '\t';
	couts(v.currentaccount);
	cout << '\t';
	couts(v.correspondentaccount);
	cout << '\t';
	for (OrgBuilding::type ob = OrgBuilding::OFFICEMAIN; ob <= OrgBuilding::FACILITY; ob = static_cast<OrgBuilding::type>(static_cast<int>(ob) + 1))
	{
		couts(v.orglocation[ob].name);
		cout << '\t';
		couts(v.orglocation[ob].city);
		cout << '\t';
		couts(v.orglocation[ob].address);
		cout << '\t';
	}
	couts(v.description);
	cout << '\t';
	couts(v.note);
	cout << '\t'; 
}

void coutDocumentById(PassengerServiceClient &client, Credentials &credentials, UserDevice &userdevice,  taxi::ID &id)
{
	taxi::Document v;
	client.getDocument(v, credentials, userdevice, id);
	coutDocument(v);
}

void coutPassengerLimit(PassengerLimit& v)
{
	cout << v.id << '\t' << v.tripcount << '\t' << v.minutescount << '\t' << v.moneycount << '\t'; 
}

void coutPassengerUsage(PassengerUsage &v)
{
	cout << v.id << '\t' << v.tripcount << '\t' << v.minutescount << '\t' << v.moneycount << '\t' 
		<< '\t' << v.payload.passengercount << '\t' << v.payload.stopcount << '\t'
		<< v.payload.baggagecount << '\t' << v.payload.baggageweight << '\t' << v.payload.baggagewidth << '\t' << v.payload.baggageheight << '\t' 
		<< v.payload.papercount  << '\t'; 
}

/**
	Return current month number: 0 - 11
*/
taxi::Month::type getCurrentMonth()
{
	time_t r(time(NULL));
	struct tm *tm = localtime(&r);
	return static_cast<taxi::Month::type> (tm->tm_mon); 
}

/**
	Return current year
*/
int getCurrentYear()
{
	time_t r(time(NULL));
	struct tm *tm = localtime(&r);
	return tm->tm_year + 1900;
}

int done(void** argtable)
{
	/* deallocate each non-null entry in argtable[] */
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}

int init()
{
#ifdef WIN32
	try {
		std::locale("russian_russia.1251");
	} catch (exception e) {
	}
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	// _setmode(_fileno(stdout), _O_U8TEXT);
	// _setmode(_fileno(stdin), _O_U8TEXT);
#else
	try {
		std::locale("ru_RU.UTF-8");
	} catch (exception e) {
	}
#endif
	return 0;
}

const STR FMT_DATE = "%Y-%m-%d";
const STR FMT_TIME = "%H:%M";
const STR FMT_DATETIME = "%Y-%m-%d %H:%M";

struct cbCurlData 
{
	char *memory;
	size_t size;
};

static size_t cbCurl(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct cbCurlData *mem = (struct cbCurlData *) userp;
	mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) 
	{
		cerr << "not enough memory (realloc returned NULL)";
		return 3;
	}
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	printf("%s", mem->memory);
	return realsize;
}

size_t readstring(FILE *f, string &data)
{
	fseek(f, 0, SEEK_END);
	size_t r = ftell(f);
	data.resize(r);
	rewind(f);
	return fread(&data[0], sizeof(char), r, f);
}

int sendGCM(const STR &apikey, const vector<STR> ids, const STR &data, bool verbose)
{
	CURL *curl;
	CURLcode res;

	if (ids.size() <= 0)
		return -1;
	
	curl_global_init(CURL_GLOBAL_DEFAULT);
 	curl = curl_easy_init();
	if(curl) {
		struct curl_slist *headers = NULL; // init to NULL is important 
		
		STR authKey("Authorization: key=");
		authKey.append(apikey);
		headers = curl_slist_append(headers, authKey.c_str());  
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8"); 

		curl_easy_setopt(curl, CURLOPT_URL, "https://android.googleapis.com/gcm/send");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		if (verbose)
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

		STR sdata("{\"registration_ids\":[");
		stringstream strmids;
		for (vector<STR>::const_iterator it = ids.begin(); it != ids.end(); ++it )
		{
			strmids << "\"" << *it << "\",";
		}
		STR sids = strmids.str();
		sdata.append(sids.substr(0, sids.size()-1));
		sdata.append("],\"data\":");
		sdata.append(data);
		sdata.append("}");

		size_t lines = std::count(sdata.begin(), sdata.end(), '\n') + 1;
		size_t bytes = sdata.size();

		stringstream strmcontentlen;
		strmcontentlen << "Content-Length: " << bytes;
		headers = curl_slist_append(headers, strmcontentlen.str().c_str()); 

		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		if (res != CURLE_OK)
		{
			fprintf(stderr, "send GSM error: %d\n", res);
			return res;
		}
		
		curl_easy_setopt(curl, CURLOPT_POST, lines);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sdata.c_str());


		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cbCurl);
		/* we pass our 'chunk' struct to the callback function */
		struct cbCurlData chunk;
		chunk.memory = (char*) malloc(1);  /* will be grown as needed by the realloc above */
		chunk.size = 0;    /* no data at this point */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

 		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if (res != CURLE_OK)
			fprintf(stderr, "send GSM error: %s\n",
	
		curl_easy_strerror(res));
 		curl_easy_cleanup(curl);
	}
 	curl_global_cleanup();
	return res;
}

int doCmd(int argc, char** argv)
{
	init();
	// credentials
	struct arg_str  *cred_role = arg_str0(NULL, "myrole", "admin|manager|customer|dispatcher|driver|master|operator|passenger|guest", "role");
	struct arg_str  *cred_phone = arg_str0(NULL, "myphone", "<phone>", "Phone number e.g. +79140001111");
	struct arg_str  *cred_password = arg_str0(NULL, "mypassword", "<pwd>", "password");
	struct arg_str  *cred_token = arg_str0(NULL, "mytoken", "<pwd>", "token");

	struct arg_lit  *c_add = arg_lit0("a", "add", "add object");
	struct arg_lit  *c_ls = arg_lit0("l", "list", "list objects");
	struct arg_lit  *c_get = arg_lit0("g", "get", "get object by ID");
	struct arg_lit  *c_rm = arg_lit0(NULL, "rm", "remove object by ID");
	struct arg_lit  *c_license = arg_lit0(NULL, "license", "show license information");
	struct arg_int  *c_ls_ofs = arg_int0(NULL, "offset", "<number>", "0..");
	struct arg_int  *c_ls_cnt = arg_int0(NULL, "count", "<number>", "1..");
	struct arg_str  *obj = arg_str0("o", "object", "<object>", "Objects: bank,.. See documentation.");
	struct arg_lit  *verbose = arg_lit0("v", "verbose,debug", "verbose messages");
	struct arg_lit  *help = arg_lit0("?", "help", "print this help and exit");

	struct arg_int  *i_port = arg_int0("p", "port", "<number>", "1..65535, default 9090");
	struct arg_str  *s_hostname = arg_str0("h", "host", "<address>", "Address or IP");

	struct arg_str  *s_name = arg_str0("n", "name", "<name>", "Common Name");
	struct arg_int  *i_year = arg_int0(NULL, "year", "<number>", "1700..2700");
	struct arg_str  *s_notes = arg_str0(NULL, "notes", "<text>", "Remarks");
	struct arg_int  *i_areaid = arg_int0(NULL, "areaid", "<number>", "1..");
	struct arg_int  *i_tag = arg_int0(NULL, "tag", "<number>", "integer number");
	struct arg_int  *i_orgid = arg_int0(NULL, "orgid", "<ID>", "organization identifier");
	struct arg_str  *s_bik = arg_str0(NULL, "bik", "<snumber>", "BIK number string");
	struct arg_int  *i_value = arg_int0(NULL, "value", "<int>", "integer number");
	struct arg_str  *s_brandname = arg_str0(NULL, "brandname", "<brand name>", "brand name string");

	struct arg_int  *i_bankacc = arg_int0(NULL, "bank-acc", "<ID>", "bank identifier");
	struct arg_int  *i_orgrole = arg_int0(NULL, "orgrole", "<enum>", "org role id");
	struct arg_int  *i_orgtype = arg_int0(NULL, "orgtype", "<enum>", "org type id");
	struct arg_str  *s_fullname = arg_str0(NULL, "fullname", "<full name>", "full name string");

	struct arg_str  *s_shortname = arg_str0(NULL, "shortname", "<short name>", "abbreviation");
	struct arg_str  *s_inn = arg_str0(NULL, "inn", "<INN>", "number string");
	struct arg_str  *s_kpp = arg_str0(NULL, "kpp", "<KPP>", "number string");
	struct arg_str  *s_orgn = arg_str0(NULL, "orgn", "<OGRN>", "number string");
	struct arg_str  *s_phone = arg_str0(NULL, "phone", "<phone>", "number string");
	struct arg_str  *s_email = arg_str0(NULL, "email", "<email>", "string");
	struct arg_str  *s_currentaccount = arg_str0(NULL, "currentaccount", "<account>", "string");
	struct arg_str  *s_correspondentaccount = arg_str0(NULL, "correspondentaccount", "<account>", "string");
	struct arg_str  *s_description = arg_str0(NULL, "description", "<text>", "string");
	struct arg_str  *s_note = arg_str0(NULL, "note", "<text>", "string");
	struct arg_str  *s_nickname = arg_str0(NULL, "nickname", "<nick>", "string");
	struct arg_int  *i_cityid = arg_int0(NULL, "cityid", "<ID>", "city id");
	struct arg_int  *i_orgserviceid = arg_int0(NULL, "orgserviceid", "<ID>", "org service id");
	struct arg_int  *i_tariffplanid = arg_int0(NULL, "tariffplanid", "<ID>", "tariff plan id");
	struct arg_int  *i_ismaster = arg_int0(NULL, "ismaster", "0|1", "1- is master");

	struct arg_int  *i_svccarpoolid = arg_int0(NULL, "svccarpoolid", "<ID>", "car pool org id");
	struct arg_int  *i_svcdispatchid = arg_int0(NULL, "svcdispatchid", "<ID>", "dispatch org id");
	struct arg_int  *i_online = arg_int0(NULL, "online", "0|1", "1- online, 0- offline");
	struct arg_int  *i_empstatus = arg_int0(NULL, "empstatus", "1|2|3", "1- active, 2- inactive (vacation), 3- deleted (fired)");
	struct arg_int  *i_id = arg_int0(NULL, "id", "<ID>", "identifier");
	struct arg_int  *i_callsign = arg_int0(NULL, "callsign", "<int>", "call sign");

	struct arg_int  *i_active = arg_int0(NULL, "active", "<0|1>", "active- 1");
	struct arg_int  *i_enabled = arg_int0(NULL, "enabled", "<0|1>", "enabled- 1");
	struct arg_int  *i_taxtype = arg_int0(NULL, "taxtype", "<1..5>", "tax type: 1- org, 2- person, 3- farmer, 4- lawyer, 5- notary");
	struct arg_int  *i_preferreddriverid = arg_int0(NULL, "preferreddriverid", "<ID>", "driver id");

	struct arg_int  *i_personid = arg_int0(NULL, "personid", "<ID>", "person id");
	struct arg_int  *i_customerid = arg_int0(NULL, "customerid", "<ID>", "customer id");
	struct arg_int  *i_isoperator = arg_int0(NULL, "isoperator", "0|1", "1- is operator");
	struct arg_int  *i_canorder = arg_int0(NULL, "canorder", "1..3", "1- yes, 2- no, 3- route only");
	struct arg_int  *i_isvip = arg_int0(NULL, "isvip", "0|1", "1- is V.I.P.");
	
	struct arg_date *d_datestart = arg_date0(NULL, "datestart", "<date>", FMT_DATE.c_str(), "2014-09-29");
	struct arg_date *d_datefinish = arg_date0(NULL, "datefinish", "<date>", FMT_DATE.c_str(), "2014-09-29");
	struct arg_int  *i_isday = arg_int0(NULL, "isday", "0|1", "1- is part of day");
	struct arg_int  *i_hourstart = arg_int0(NULL, "hourstart", "0..23", "hour");
	struct arg_int  *i_hourfinish = arg_int0(NULL, "hourfinish", "0..23", "hour");
	struct arg_int  *i_isweekend = arg_int0(NULL, "isweekend", "0|1", "1- weekwend");
	struct arg_dbl  *d_costmin = arg_dbl0(NULL, "costmin", "<currency>", "minimum price round to");
	struct arg_dbl  *d_priceboarding = arg_dbl0(NULL, "priceboarding", "<currency>", "boarding price");
	struct arg_dbl  *d_priceminute = arg_dbl0(NULL, "priceminute", "<currency>", "price per minute");
	struct arg_dbl  *d_pricedelay = arg_dbl0(NULL, "pricedelay", "<currency>", "price per minute");
	struct arg_dbl  *d_pricewait = arg_dbl0(NULL, "pricewait", "<currency>", "price per minute");
	struct arg_int  *i_speedmin = arg_int0(NULL, "speedmin", "<speed>", "speed km/h");
	struct arg_int  *i_timedelayfree = arg_int0(NULL, "timedelayfree", "<seconds>", "free wait time");

	struct arg_int  *i_isadmin = arg_int0(NULL, "isadmin", "0|1", "1- is admin");
	struct arg_str	*s_pwd = arg_str0(NULL, "pwd", "<pwd>", "password");

	// sendGCM
	struct arg_lit  *c_sendgcm = arg_lit0(NULL, "sendgcm", "push GCM");
	struct arg_str  *s_apikey = arg_str0(NULL, "apikey", "<API key>", "GCM project id: taxi-b2b");
	struct arg_str  *s_data = arg_str0(NULL, "data", "<data>", "data to send");
	struct arg_file  *f_data = arg_file0(NULL, "file", "<file name>", "file to send");
	struct arg_str  *s_registrationid = arg_str0(NULL, NULL, "<registration_id>", "GCM registration id");

	struct arg_end  *end = arg_end(20);

	void* argtable[] = { 
		cred_role, cred_phone, cred_password, cred_token,
		c_license, c_add, c_get, c_rm, c_ls, c_ls_ofs, c_ls_cnt, obj, verbose, help, i_port, s_hostname,
		s_name, i_year, s_notes, i_areaid, i_tag, i_orgid, s_bik, i_value, s_brandname,
		i_bankacc, i_orgrole, i_orgtype, s_fullname, s_shortname, s_inn, s_kpp, s_orgn, s_phone, s_email, s_currentaccount, s_correspondentaccount, 
		s_description, s_note, s_nickname,
		i_cityid, i_orgserviceid, i_tariffplanid, i_ismaster, i_svccarpoolid, i_svcdispatchid, i_online, i_empstatus,
		i_id, i_callsign,
		i_active, i_enabled, i_taxtype, i_preferreddriverid,
		i_personid, i_customerid, i_isoperator, i_isvip,
		d_datestart, d_datefinish, i_isday, i_hourstart, i_hourfinish, i_isweekend, d_costmin, d_priceboarding, d_priceminute, d_pricedelay, d_pricewait, i_speedmin, i_timedelayfree,
		i_isadmin, s_pwd,
		c_sendgcm, s_apikey, s_data, s_registrationid, f_data,
		end
	};
	const char* progname = "taxi-simple-cli";
	int nerrors;
	
	/* verify the argtable[] entries were allocated sucessfully */
	if (arg_nullcheck(argtable) != 0)
	{
		/* NULL entries were detected, some allocations must have failed */
		printf("%s: insufficient memory\n", progname);
		done(argtable);
		return 1;
	}

	/* Parse the command line as defined by argtable[] */
	nerrors = arg_parse(argc, argv, argtable);

	/* special case: '--help' takes precedence over error reporting */
	if (help->count > 0)
	{
		printf("Usage: %s", progname);
		arg_print_syntax(stdout, argtable, "\n");
		printf("taxi command line utility\n");
		arg_print_glossary(stdout, argtable, "  %-25s %s\n");
		done(argtable);
		return 0;
	}

	/* special case: '--license' check UTF-8 codepage */
	if (c_license->count > 0)
	{
		puts("\xD0\x90\xD0\x91\xD0\x92\xD0\x93\xD0\x94\xD0\x95");
		// locale loc(std::locale("rus"), new codecvt_utf8_utf16<wchar_t>);
		// cout.imbue(std::locale(std::locale("russian_russia.1251")));
		// cout.imbue(std::locale("ru_RU.utf8"));
		// wcout.imbue(loc);
		// cout.imbue(loc);
		// cout << "\xD0\x90\xD0\x91\xD0\x92\xD0\x93\xD0\x94\xD0\x95" << std::endl;
		// wcout << "\xD0\x90\xD0\x91\xD0\x92\xD0\x93\xD0\x94\xD0\x95" << std::endl;
		done(argtable);
		return 0;
	}

	/* If the parser returned any errors then display them and exit */
	if (nerrors > 0)
	{
		/* Display the error details contained in the arg_end struct.*/
		arg_print_errors(stdout, end, progname);
		printf("Try '%s --help' for more information.\n", progname);
		done(argtable);
		return 1;
	}

	/* special case: uname with no command line options induces brief help */
	if (argc == 1)
	{
		printf("Try '%s --help' for more information.\n", progname);
		done(argtable);
		return 0;
	}

	if (c_sendgcm->count > 0)
	{
		if (s_apikey->count == 0)
		{
			printf("--apikey missed.\n");
			done(argtable);
			return 2;
		}

		if (s_registrationid->count <= 0)
		{
			printf("registration id missed.\n");
			done(argtable);
			return 2;
		}

		vector<STR> ids;
		for (int i = 0; i < s_registrationid->count; i++)
		{
			ids.push_back(s_registrationid->sval[i]);
		}

		STR data;
		if (s_data->count > 0)
		{
			data = *s_data->sval;
		} else {
			FILE *f;
			if (f_data->count > 0)
			{
				f = fopen(*f_data->filename, "r");
			} else {
				f = stdin;
			}
			if (f) {
				readstring(f, data);
				fclose(f);
			} else {
				printf("Error read input file\n");
			}
		}
		if (data.size() == 0)
			data = "{}";
		int r = sendGCM(*s_apikey->sval, ids, data, verbose->count > 0);
		exit(r);
	}

	std::string hostname("localhost");
	int port = 9090;
	Credentials credentials;
	credentials.personrole = PersonRole::NOTAUTHORIZED;

	// read from configuration file
	Config cfg;
	try
	{
		cfg.readFile(DEF_CONFIG);
	}
	catch(const FileIOException &fioex)
	{
	}
	catch(const ParseException &pex)
	{
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()	<< " - " << pex.getError() << std::endl;
		return (3);
	}

	cfg.lookupValue("client.port", port);

	try
	{
		hostname = (const char *) cfg.lookup("client.host");
	}
	catch(const SettingNotFoundException &nfex)
	{
	}
	try
	{
		std::string role = (const char *) cfg.lookup("client.credentials.role");
		credentials.personrole = readPersonRole(role);
	}
	catch(const SettingNotFoundException &nfex)
	{
	}
	try
	{
		credentials.token = (const char *) cfg.lookup("client.crededentials.token");
	}
	catch(const SettingNotFoundException &nfex)
	{
	}
	try
	{
		credentials.phone = (const char *) cfg.lookup("client.credentials.phone");
	}
	catch(const SettingNotFoundException &nfex)
	{
	}
	try
	{
		credentials.password = (const char *) cfg.lookup("client.credentials.password");
	}
	catch(const SettingNotFoundException &nfex)
	{
	}
	try
	{
		credentials.token = (const char *) cfg.lookup("client.credentials.token");
	}
	catch(const SettingNotFoundException &nfex)
	{
	}

	// read from command line 
	if (s_hostname->count != 0)
		hostname = *s_hostname->sval;
	if (i_port->count != 0)
		port = *i_port->ival;

	boost::shared_ptr<TTransport> socket(new TSocket(hostname, port));
	boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	PassengerServiceClient client(protocol);

	if (cred_role->count > 0)
		credentials.personrole = readPersonRole(std::string(*cred_role->sval));
	if (cred_phone->count > 0)
		credentials.phone = std::string(*cred_phone->sval);
	if (cred_password->count > 0)
		credentials.password = std::string(*cred_password->sval);
	if (cred_token->count > 0)
		credentials.token = std::string(*cred_token->sval);

	try {
		transport->open();
		UserDevice userdevice;
		STR empty;

		if (c_get->count > 0)
		{
			if (obj->count == 0)
			{
				printf("-o,--object missed.\n");
				done(argtable);
				return 2;
			}

			if (i_id->count == 0)
			{
				printf("--id missed.\n");
				done(argtable);
				return 2;
			}
			taxi::ID id = *i_id->ival;

			if (strcmp("document", *obj->sval) == 0)
			{
				Document r;
				client.getDocument(r, credentials, userdevice, id);
				coutDocument(r);
			}
			if (strcmp("org", *obj->sval) == 0)
			{
				Org r;
				client.getOrg(r, credentials, userdevice, id);
				coutOrg(r);
			}
		}

		if (c_add->count > 0)
		{
			if (obj->count == 0)
			{
				printf("-o,--object missed.\n");
				done(argtable);
				return 2;
			}

			if (strcmp("manager", *obj->sval) == 0)
			{
				Manager ret;
				Manager v;
				if (s_nickname->count == 0)
				{
					printf("--nickname missed.\n");
					done(argtable);
					return 2;
				}
				v.nickname = *s_nickname->sval;

				if (s_phone->count == 0)
				{
					printf("--phone missed.\n");
					done(argtable);
					return 2;
				}
				v.person.phone1 = *s_phone->sval;

				if (s_pwd->count == 0)
				{
					printf("--pwd missed.\n");
					done(argtable);
					return 2;
				}
				v.person.credentials.password = *s_pwd->sval;

				if (i_cityid->count > 0)
					v.cityid = *i_cityid->ival;

				if (i_isadmin->count > 0)
					v.isadmin = *i_isadmin->ival != 0;


				client.addManager(ret, credentials, userdevice, v);
			}

			if (strcmp("city", *obj->sval) == 0)
			{
				City ret;
				City v;
				if (s_name->count == 0)
				{
					printf("--name missed.\n");
					done(argtable);
					return 2;
				}
				v.name = *s_name->sval;
				
				if (s_notes->count > 0)
					v.notes = *s_notes->sval;

				if (i_areaid->count > 0)
					if (*i_areaid->ival >= 0)
						v.areaid = *i_areaid->ival;

				if (i_tag->count > 0)
					v.tag = *i_tag->ival;

				client.addCity(ret, credentials, userdevice, v);
			}

			if (strcmp("org", *obj->sval) == 0)
			{
				Org ret;
				Org v;

				if (s_name->count == 0)
				{
					printf("--name missed.\n");
					done(argtable);
					return 2;
				}
				v.name = *s_name->sval;

				if (i_orgrole->count != 0)
					v.orgrole = static_cast <OrgRole::type> (*i_orgrole->ival);
				if (i_orgtype->count != 0)
					v.orgtype = static_cast <OrgType::type> (*i_orgtype->ival);
				v.start = currentTime();
				if (i_tag->count > 0)
					v.tag = *i_tag->ival;

				if (s_fullname->count != 0)
					v.fullname = *s_fullname->sval;
				if (s_shortname->count != 0)
					v.shortname = *s_shortname->sval;
				if (s_inn->count != 0)
					v.inn = *s_inn->sval;
				if (s_kpp->count != 0)
					v.kpp = *s_kpp->sval;
				if (s_orgn->count != 0)
					v.ogrn = *s_orgn->sval;

				if (s_phone->count != 0)
					v.phone = *s_phone->sval;
				if (s_email->count != 0)
					v.email = *s_email->sval;
				
				// TODO v.employee[OrgPosition::ACCT

				if (s_currentaccount->count != 0)
					v.currentaccount = *s_currentaccount->sval;
				if (s_correspondentaccount->count != 0)
					v.correspondentaccount = *s_correspondentaccount->sval;
				if (i_bankacc->count != 0)
					v.bank[BankRole::BANK_ACCOUNT] = *i_bankacc->ival;
				if (s_description->count != 0)
					v.description = *s_description->sval;
				if (s_note->count != 0)
					v.note = *s_note->sval;

				client.addOrg(ret, credentials, userdevice, v);
			}

			if (strcmp("orgservice", *obj->sval) == 0)
			{
				if (s_name->count == 0)
				{
					printf("--name missed.\n");
					done(argtable);
					return 2;
				}
				OrgService ret;
				OrgService v;
				v.name = *s_name->sval;

				if (i_orgid->count > 0) 
					if (*i_orgid->ival >= 0)
					{
						v.organization[OrgServiceDept::CUSTOMER] = *i_orgid->ival;
						v.organization[OrgServiceDept::DRIVER] = *i_orgid->ival;
						v.organization[OrgServiceDept::OTHER] = *i_orgid->ival;
					}
				// TODO add other org id
				client.addOrgService(ret, credentials, userdevice, v);
			}

			if (strcmp("bank", *obj->sval) == 0)
			{
				if (s_name->count == 0)
				{
					printf("--name missed.\n");
					done(argtable);
					return 2;
				}
				Bank ret;
				Bank v;
				v.name = *s_name->sval;
				if (s_bik->count == 0)
					v.bik = *s_name->sval;
				client.addBank(ret, credentials, userdevice, v);
			}

			if (strcmp("customer", *obj->sval) == 0)
			{
				Customer ret;
				Customer v;

				if (s_nickname->count == 0)
				{
					printf("--nickname missed.\n");
					done(argtable);
					return 2;
				}
				v.nickname = *s_nickname->sval;

				if (i_cityid->count == 0)
				{
					printf("--cityid missed.\n");
					done(argtable);
					return 2;
				}
				v.cityid = *i_cityid->ival;

				v.active = true;
				if (i_active->count != 0)
				{
					v.active = (*i_active->ival != 0);
				}

				v.enabled = true;
				if (i_enabled->count != 0)
				{
					v.enabled = (*i_enabled->ival != 0);
				}

				v.taxtype = TaxType::TAXORG;
				if (i_taxtype->count != 0)
				{
					v.taxtype = static_cast <TaxType::type> (*i_taxtype->ival);
				}

				if (i_preferreddriverid->count != 0)
				{
					v.preferreddriverid = *i_preferreddriverid->ival;
				}

				client.addCustomer(ret, credentials, userdevice, v);
			}

			if (strcmp("passenger", *obj->sval) == 0)
			{
				Passenger ret;
				Passenger v;

				if (i_cityid->count == 0)
				{
					printf("--cityid missed.\n");
					done(argtable);
					return 2;
				}
				v.cityid = *i_cityid->ival;

				if (i_customerid->count == 0)
				{
					printf("--customerid missed.\n");
					done(argtable);
					return 2;
				}
				v.customerid = *i_customerid->ival;

				if (s_phone->count == 0)
				{
					printf("--phone missed.\n");
					done(argtable);
					return 2;
				}
				v.person.phone1 = *s_phone->sval;

				if (s_pwd->count == 0)
				{
					printf("--pwd missed.\n");
					done(argtable);
					return 2;
				}
				v.person.credentials.password = *s_pwd->sval;

				if (i_isoperator->count != 0)
				{
					v.isoperator = (*i_isoperator->ival != 0);
				}

				if (i_isvip->count != 0)
				{
					v.isvip = (*i_isvip->ival != 0);
				}

				if (i_empstatus->count != 0)
				{
					v.status = static_cast<EmployeeStatus::type> (*i_empstatus->ival);
				}

				if (i_canorder->count != 0)
				{
					v.canorder = static_cast<CanOrder::type>(*i_canorder->ival);
				}

				if (i_personid->count != 0)
				{
					v.person.id = *i_personid->ival;
				}

				client.addPassenger(ret, credentials, userdevice, v);
			}

			if (strcmp("driver", *obj->sval) == 0)
			{
				Driver ret;
				Driver v;
				v.status = EmployeeStatus::ACTIVE;
				v.online = false;

				if (s_phone->count == 0)
				{
					printf("--phone missed.\n");
					done(argtable);
					return 2;
				}
				v.person.credentials.phone = *s_phone->sval;
				v.person.phone1 = *s_phone->sval;

				if (s_pwd->count == 0)
				{
					printf("--pwd missed.\n");
					done(argtable);
					return 2;
				}
				v.person.credentials.password = *s_pwd->sval;
				v.person.credentials.personrole = PersonRole::DRIVER;

				if (s_nickname->count == 0)
				{
					printf("--nickname missed.\n");
					done(argtable);
					return 2;
				}
				v.nickname = *s_nickname->sval;

				if (i_cityid->count == 0)
				{
					printf("--cityid missed.\n");
					done(argtable);
					return 2;
				}
				v.cityid = *i_cityid->ival;

				
				v.ismaster = (i_ismaster->count > 0);

				if (i_svccarpoolid->count == 0)
					v.svc[taxi::TaxiServiceRole::TS_CARPOOL] = *i_svccarpoolid->ival;
				if (i_svcdispatchid->count == 0)
					v.svc[taxi::TaxiServiceRole::TS_DISPATCH] = *i_svcdispatchid->ival;
				client.addDriver(ret, credentials, userdevice, v);
			}

			if (strcmp("vehiclecolor", *obj->sval) == 0)
			{
				DictEntry ret;
				STR name;
				int v;

				if (s_name->count == 0)
				{
					printf("--name missed.\n");
					done(argtable);
					return 2;
				}
				name = *s_name->sval;
				if (i_value->count > 0)
					v = *i_value->ival;
				client.addVehicleColor(ret, credentials, userdevice, name, v);
			}

			if (strcmp("vehiclebrand", *obj->sval) == 0)
			{
				DictEntry ret;
				if (s_name->count == 0)
				{
					printf("--name missed.\n");
					done(argtable);
					return 2;
				}
				STR name = *s_name->sval;
				client.addVehicleBrand(ret, credentials, userdevice, VehicleCategory::VEHICLE_CAR, name);
			}

			if (strcmp("vehiclemodel", *obj->sval) == 0)
			{
				DictEntry ret;

				if (s_brandname->count == 0)
				{
					printf("--brandname missed.\n");
					done(argtable);
					return 2;
				}
				STR brandname = *s_brandname->sval;

				if (s_name->count == 0)
				{
					printf("--name missed.\n");
					done(argtable);
					return 2;
				}
				STR name = *s_name->sval;
				
				int year = 0;
				if (i_year->count > 0)
				{
					if ((i_year->count < 1700) && (i_year->count > 2700))
					{
						printf("--year missed or incorrect value. Correct: 1700..2700\n");
						done(argtable);
						return 2;
					}
					year = *i_year->ival;
				}				
				client.addVehicleModelByBrandName(ret, credentials, userdevice, brandname, name, year);
			}

			if (strcmp("tariffplan", *obj->sval) == 0)
			{
				TariffPlan ret;
				TariffPlan v;

				if (s_name->count == 0)
				{
					printf("--name missed.\n");
					done(argtable);
					return 2;
				}
				v.name = *s_name->sval;

				if (i_cityid->count == 0)
				{
					printf("--cityid missed.\n");
					done(argtable);
					return 2;
				}

				v.cityid = *i_cityid->ival;

				if (i_orgserviceid->count == 0)
				{
					printf("--orgserviceid missed.\n");
					done(argtable);
					return 2;
				}

				v.orgserviceid = *i_orgserviceid->ival;

				if (i_active->count == 0)
					v.active = (*i_active->ival != 0);
				if (s_notes->count == 0)
					v.notes = *s_notes->sval;
				client.addTariffPlan(ret, credentials, userdevice, v);
			}

			if (strcmp("rate", *obj->sval) == 0)
			{
				Rate ret;
				Rate v;

				if (s_name->count == 0)
				{
					printf("--name missed.\n");
					done(argtable);
					return 2;
				}
				v.name = *s_name->sval;

				if (i_tariffplanid->count == 0)
				{
					printf("--tariffplanid missed.\n");
					done(argtable);
					return 2;
				}

				v.tariffplanid = *i_tariffplanid->ival;

				if (d_datestart->count == 0)
					v.datestart = tm2time_tUTC(d_datestart->tmval);
				if (d_datefinish->count == 0)
					v.datefinish = tm2time_tUTC(d_datefinish->tmval);
				if (i_active->count == 0)
					v.active = (*i_active->ival != 0);
				if (i_isday->count == 0)
					v.isday = (*i_isday->ival != 0);
				if (i_hourstart->count == 0)
				{
					v.hourstart = *i_hourstart->ival;
					v.isday = true;
				}
				if (i_hourfinish->count == 0)
				{
					v.hourfinish = *i_hourfinish->ival;
					v.isday = true;
				}

				if (i_isweekend->count == 0)
					v.isweekend = (*i_isweekend->ival != 0);
				
				if (d_costmin->count == 0)
					v.costmin = *d_costmin->dval;
				else
					v.costmin = .0;
				if (d_priceboarding->count == 0)
					v.priceboarding = *d_priceboarding->dval;
				else
					v.priceboarding = .0;
				if (d_priceminute->count == 0)
					v.priceminute = *d_priceminute->dval;
				else
					v.priceminute = .0;
				if (d_pricedelay->count == 0)
					v.pricedelay = *d_pricedelay->dval;
				else
					v.pricedelay = v.priceminute;
				if (d_pricewait->count == 0)
					v.pricewait = *d_pricewait->dval;
				else
					v.pricewait = v.priceminute;
				if (i_speedmin->count == 0)
					v.speedmin = *i_speedmin->ival;
				else
					v.speedmin = 0;
				if (i_timedelayfree->count == 0)
					v.timedelayfree = *i_speedmin->ival;
				else
					v.timedelayfree = 0; // seconds
				client.addRate(ret, credentials, userdevice, v);
			}
		}

		// listing range
		RowRange rowrange;
		rowrange.len = 25;
		rowrange.start = 0;
		if (c_ls_ofs->count > 0)
			if (*c_ls_ofs->ival >= 0)
				rowrange.start = *c_ls_ofs->ival;
		if (c_ls_cnt->count > 0)
			if (*c_ls_cnt->ival > 0)
				rowrange.len = *c_ls_cnt->ival;

		// Listing
		if (c_ls->count > 0)
		{
			if (obj->count == 0)
			{
				printf("-o,--object missed.\n");
				done(argtable);
				return 2;
			}
			if (strcmp("bank", *obj->sval) == 0)
			{
				Bank v;
				Banks ret;
				if (s_name->count > 0)
					v.name = *s_name->sval;
				client.findBank(ret, credentials, userdevice, v, rowrange);
				for (Banks::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t'; 
					couts(it->bik);
					cout << '\t';
					couts(it->name);
					cout << std::endl; 
				}
			}

			if (strcmp("org", *obj->sval) == 0)
			{
				Orgs ret;
				Org v;
				if (s_name->count == 0)
					v.name = *s_name->sval;
				client.findOrg(ret, credentials, userdevice, v, rowrange);
				for (Orgs::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					coutOrg(*it);
				}
			}

			if (strcmp("orgservice", *obj->sval) == 0)
			{
				OrgServices ret;
				OrgService v;
				if (s_name->count == 0)
					v.name = *s_name->sval;
				client.findOrgService(ret, credentials, userdevice, v, rowrange);
				for (OrgServices::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->organization[OrgServiceDept::CUSTOMER] << '\t' <<
						it->organization[OrgServiceDept::DRIVER] << '\t' << 
						it->organization[OrgServiceDept::OTHER] << '\t';
					for (OrgBuilding::type ob = OrgBuilding::OFFICEMAIN; ob <= OrgBuilding::FACILITY; ob = static_cast<OrgBuilding::type>(static_cast<int>(ob) + 1))
					{
						couts(it->orglocation[ob].name);
						cout << '\t';
						couts(it->orglocation[ob].city);
						cout << '\t';
						couts(it->orglocation[ob].address);
						cout << '\t';
						couts(it->orglocation[ob].address);
						cout << '\t';
					}
					couts(it->name);
					cout << '\t' << std::endl; 
				}
			}

			if (strcmp("city", *obj->sval) == 0)
			{
				Cities ret;

				client.findCity(ret, credentials, userdevice, *s_name->sval, rowrange);
				for (Cities::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->areaid << '\t' << it->tag << '\t';
					couts(it->name);
					cout << '\t';
					couts(it->notes);
					cout << std::endl; 
				}
			}

			if (strcmp("vehiclebrand", *obj->sval) == 0)
			{
				DictEntries ret;
				client.findBrand(ret, credentials, userdevice, *s_name->sval, rowrange);
				for (DictEntries::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->parent << '\t' << it->tag << '\t' << it->intvalue << '\t';
					couts(it->strvalue);
					cout << std::endl; 
				}
			}

			if (strcmp("vehiclecolor", *obj->sval) == 0)
			{
				DictEntries ret;
				client.findColor(ret, credentials, userdevice, *s_name->sval, rowrange);
				for (DictEntries::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->parent << '\t' << it->tag << '\t' << it->intvalue << '\t'; 
					couts(it->strvalue);
					cout << std::endl;
				}
			}

			if (strcmp("vehiclemodel", *obj->sval) == 0)
			{
				DictEntries ret;
				int year = 0;
				if (i_year->count != 0)
					if ((*i_year->ival >= 1700) && (*i_year->ival <= 2700))
					year = *i_year->ival;

				if (s_brandname->count == 0) 
				{
					if (s_name->count == 0)
						client.findModelByBrandName(ret, credentials, userdevice, empty, empty, rowrange);
					else
						client.findModelByBrandName(ret, credentials, userdevice, empty, *s_name->sval, rowrange);
				} else {
					if (s_name->count == 0)
						client.findModelByBrandName(ret, credentials, userdevice, *s_brandname->sval, empty, rowrange);
					else
						client.findModelByBrandName(ret, credentials, userdevice, *s_brandname->sval, *s_name->sval, rowrange);
				}
				for (DictEntries::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->parent << '\t' << it->tag << '\t' << it->intvalue << '\t';
					couts(it->strvalue);
					cout << std::endl; 
				}
			}

			if (strcmp("driver", *obj->sval) == 0)
			{
				Drivers ret;
				Driver v;
				if (i_id->count == 0)
					v.id = *i_id->ival;
				if (s_nickname->count == 0)
					v.nickname = *s_name->sval;
				v.status = EmployeeStatus::ACTIVE;
				if (i_empstatus->count == 0)
					v.status = static_cast<EmployeeStatus::type> (*i_empstatus->ival);
				if (i_online->count == 0)
					v.online = (*i_online->ival > 1);
				if (i_cityid->count == 0)
					v.cityid = *i_cityid->ival;
				if (i_callsign->count == 0)
					v.callsign = *i_callsign->ival;
				if (i_svccarpoolid->count == 0)
					v.svc[taxi::TaxiServiceRole::TS_CARPOOL] = *i_svccarpoolid->ival;
				if (i_svcdispatchid->count == 0)
					v.svc[taxi::TaxiServiceRole::TS_DISPATCH] = *i_svcdispatchid->ival;
				client.findDriver(ret, credentials, userdevice, v, rowrange);

				for (Drivers::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->cityid << '\t' << 
						it->svc[taxi::TaxiServiceRole::TS_CARPOOL] << '\t' << 
						it->svc[taxi::TaxiServiceRole::TS_DISPATCH] << '\t' << 
						it->status << '\t';
					/*
					WAS
					for (OrgPosition::type op = OrgPosition::SELFEMP; op <= OrgPosition::MGR; op = static_cast<OrgPosition::type>(static_cast<int>(op) + 1))
					{
						coutPerson(it->person[op]);
					}
					*/
					coutPerson(it->person);
					for (DocumentType::type dt = DocumentType::PASSPORT; dt <= DocumentType::PAYMENTRECEIPT; dt = static_cast<DocumentType::type>(static_cast<int>(dt) + 1))
					{
						coutDocumentById(client, credentials, userdevice, it->license[dt]);
					}
					cout << it->nickname << '\t' << 
						it->callsign << '\t' << 
						it->cabclass << '\t' << 
						it->rating << '\t' << 
						it->online << '\t'; 
					cout << std::endl;
				}
			}
		
			if (strcmp("customer", *obj->sval) == 0)
			{
				Customers ret;
				Customer v;
				if (i_id->count == 0)
					v.id = *i_id->ival;
				if (s_nickname->count == 0)
					v.nickname = *s_name->sval;
				if (i_cityid->count == 0)
					v.cityid = *i_cityid->ival;

				v.active = EmployeeStatus::ACTIVE;
				if (i_active->count == 0)
					v.active = (*i_active->ival > 0);
				if (i_enabled->count == 0)
					v.enabled = (*i_enabled->ival > 1);
				if (i_taxtype->count == 0)
					v.taxtype = static_cast<taxi::TaxType::type> (*i_taxtype->ival);
				if (i_preferreddriverid->count == 0)
					v.preferreddriverid = *i_preferreddriverid->ival;
				client.findCustomer(ret, credentials, userdevice, v, rowrange);

				for (Customers::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->cityid << '\t' << it->tag << '\t' << it->organization[taxi::OrgServiceDept::CUSTOMER] << '\t' <<
						it->taxtype << '\t' << it->active << '\t' << it->enabled << '\t' << it->haspreferreddriver << '\t' << it->preferreddriverid << '\t';
					for (OrgServiceDept::type op = OrgServiceDept::CUSTOMER; op <= OrgServiceDept::OTHER; op = static_cast<OrgServiceDept::type>(static_cast<int>(op) + 1))
					{
						cout << it->organization[op] << '\t';
					}
					couts(it->nickname);
					cout << std::endl;
				}
			}

			if (strcmp("passenger", *obj->sval) == 0)
			{
				Passengers ret;
				Passenger v;
				if (i_id->count == 0)
					v.id = *i_id->ival;
				if (i_personid->count == 0)
					v.person.id = *i_personid->ival;
				if (i_cityid->count == 0)
					v.cityid = *i_cityid->ival;
				if (i_customerid->count == 0)
					v.customerid = *i_customerid->ival;
				if (i_isoperator ->count == 0)
					v.isoperator = (*i_isoperator->ival != 0);
				if (i_isvip ->count == 0)
					v.isvip = (*i_isvip ->ival != 0);
				if (i_canorder ->count == 0)
					v.canorder = static_cast<CanOrder::type>(*i_canorder->ival);
				if (i_empstatus->count == 0)
					v.status = static_cast<EmployeeStatus::type> (*i_empstatus->ival);
				if (i_tag ->count == 0)
					v.tag = *i_tag->ival;	// TODO search by tag is not implemented yet
				client.findPassenger (ret, credentials, userdevice, v, rowrange);

				for (Passengers::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->cityid << '\t' << it->tag << '\t' << it->customerid << '\t' <<
						it->isoperator << '\t' << it->isvip << '\t' << it->status << '\t' << it->person.id << '\t' << it->canorder << '\t';
					coutPassengerLimit(it->passengerlimitmonth.at(getCurrentMonth()));
					coutPassengerUsage(it->passengerusagemonth[getCurrentMonth()]);
					cout << std::endl;
				}
			}

			if (strcmp("tariffplan", *obj->sval) == 0)
			{
				TariffPlans ret;
				TariffPlan v;
				client.findTariffPlan(ret, credentials, userdevice, v, rowrange);
				for (TariffPlans::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->orgserviceid << '\t' << it->cityid << '\t';
					couts(it->name);
					cout << '\t';
					couts(it->notes);
					cout << '\t' << it->active;
					cout << std::endl; 
				}
			}

			if (strcmp("rate", *obj->sval) == 0)
			{
				Rates ret;
				Rate v;
				client.findRate(ret, credentials, userdevice, v, rowrange);
				for (Rates::iterator it = ret.begin(); it != ret.end(); ++it)
				{
					cout << it->id << '\t' << it->tariffplanid << '\t';
					couts(it->name);
					cout << '\t' << it->active << '\t';
					coutdate(it->datestart);
					cout << '\t';
					coutdate(it->datefinish);
					cout << '\t' << it->isweekend << '\t' << it->isday << '\t' << it->hourstart << '\t' << it->hourfinish << '\t'
						<<  it->costmin << '\t' << it->priceboarding << '\t' <<  it->priceminute << '\t'
						<<  it->pricedelay << '\t' <<  it->pricewait << '\t' <<  it->speedmin << '\t' <<  it->timedelayfree << '\t';
					cout << std::endl; 
				}
			}
		}

		transport->close();
	}
	catch (TException &tx) {
		printf("Error: %s\n", tx.what());
	}
	return 0;
}
