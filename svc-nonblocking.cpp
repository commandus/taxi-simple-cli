#include "PassengerService.h"
#include "PassengerServiceHandler.h"

// simple
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

// nb
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/server/TNonblockingServer.h>

// js
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/BoostThreadFactory.h>
#include <thrift/server/TThreadPoolServer.h>
//
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/THttpServer.h>

#include <argtable2.h>

using boost::shared_ptr;
using namespace apache::thrift::concurrency;

using namespace  ::taxi;

struct start_params
{
	int svctype;
	int port;
	int threadcount;
};

int done(void** argtable)
{
	/* deallocate each non-null entry in argtable[] */
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}

int parseCmd(int argc, char** argv, struct start_params *startparams)
{
	// defaults
	startparams->svctype = 0;
	startparams->port = 9090;
	startparams->threadcount = 4;

	// database options
	struct arg_str  *d_user = arg_str0(NULL, "user", "<user name>", "database user");
	struct arg_str  *d_password = arg_str0(NULL, "password", "<password>", "database password");
	struct arg_str  *d_database = arg_str0(NULL, "database", "<database name>", "database name");
	struct arg_str  *d_host = arg_str0(NULL, "host", "<host address>", "host name or IP address");
	struct arg_int  *d_port = arg_int0(NULL, "port", "<port>", "database port");
	struct arg_str  *d_socket = arg_str0(NULL, "socket", "<socket>", "socket");

	// thrift options
	struct arg_str  *t_service = arg_str0("s", "service", "simple|nonblocking|javascript", "service listener, default nonblocking");
	struct arg_int  *t_port = arg_int0("t", "listentport", "<port>", "thrift port, default 9090");
	struct arg_int  *t_threadscount = arg_int0(NULL, "threads", "<number>", "threads count: 1.., default 4 (javascript only)");

	// service options
	struct arg_lit  *verbose = arg_lit0("v", "verbose,debug", "verbose messages");
	struct arg_lit  *help = arg_lit0("h", "help", "print this help and exit");
	struct arg_end  *end = arg_end(20);
	
	void* argtable[] = { d_user, d_password, d_database, d_host, d_port, d_socket, t_service, t_port, t_threadscount, help, end };
	const char* progname = "taxi-simple-svc";
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
		printf("taxi standalone service\n");
		arg_print_glossary(stdout, argtable, "  %-25s %s\n");
		done(argtable);
		return 1;
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
		return 1;
	}

	if (t_service->count > 0)
	{

		if (strcmp(t_service->sval[0], "simple") == 0)
		{
			startparams->svctype = 1;
		}
		if (strcmp(t_service->sval[0], "javascript") == 0)
		{
			startparams->svctype = 2;
		}
	}

	if (t_port->count > 0)
	{
		if ((*t_port->ival <= 0) || (*t_port->ival > 65535))
		{
			printf("Invalid port number: %d.\n", *t_port->ival);
			done(argtable);
			return 1;
		}
		startparams->port = *t_port->ival;
	}

	if (t_threadscount->count > 0)
	{
		if ((*t_threadscount->ival <= 0) || (*t_threadscount->ival > 512))
		{
			printf("Invalid port number: %d.\n", *t_threadscount->ival);
			done(argtable);
			return 1;
		}
		startparams->threadcount = *t_threadscount->ival;
	}
	
	return 0;
}

int doSimpleServer(int argc, char **argv, struct start_params *startparams) {
	shared_ptr<PassengerServiceHandler> handler(new PassengerServiceHandler(argc, argv));
	shared_ptr<TProcessor> processor(new PassengerServiceProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(new TServerSocket(startparams->port));
	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
	server.serve();
	return 0;
}

int doJavaScriptServer(int argc, char **argv, struct start_params *startparams) {
	  // shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TJSONProtocolFactory());
  boost::shared_ptr<PassengerServiceHandler> handler(new PassengerServiceHandler(argc, argv));
  boost::shared_ptr<TProcessor> processor(new PassengerServiceProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(startparams->port));
  boost::shared_ptr<TTransportFactory> transportFactory(new THttpServerTransportFactory());

  TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
  
  printf("Starting JSON server...\n");
  server.serve();
  printf("done.\n");

  /*
	Dies somewhere with bad_alloc :(
	shared_ptr<PassengerServiceHandler> handler(new PassengerServiceHandler(argc, argv));
	shared_ptr<TProcessor> processor(new PassengerServiceProcessor(handler));

	shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(startparams->threadcount);
	shared_ptr<BoostThreadFactory> threadFactory = shared_ptr<BoostThreadFactory>(new BoostThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();

	shared_ptr<TServerTransport> serverTransport(new TServerSocket(startparams->port));
	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory, threadManager);
	server.serve();
	*/

	return 0;
}

int doNonBlockingServer(int argc, char **argv, struct start_params *startparams) {

	shared_ptr<PassengerServiceHandler> handler(new PassengerServiceHandler(argc, argv));
	shared_ptr<TProcessor> processor(new PassengerServiceProcessor(handler));
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TNonblockingServer server(processor, protocolFactory, startparams->port);
	server.serve();
	return 0;
}

int doServer(int argc, char **argv)
{
	int r;
	start_params startparams;
	r = parseCmd(argc, argv, &startparams);
	if (r)
	{
		return r;
	}
	switch (startparams.svctype)
	{
	case 1:
		doSimpleServer(argc, argv, &startparams);
		break;
	case 2:
		doJavaScriptServer(argc, argv, &startparams);
		break;
	default:
		doNonBlockingServer(argc, argv, &startparams);
	}
	return 0;
}
