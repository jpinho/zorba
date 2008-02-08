
#include "zorba/zorba_singlethread.h"

#include <iostream>

using namespace std;
using namespace xqp;

/*
	Using Zorba in single thread mode.
	Init the engine, create a query and execute it.
	Set the current time in dynamic context.
*/


int uc3c_currentTime(int argc, char* argv[])
{
	bool original_throw_mode = ZorbaAlertsManager::setThrowExceptionsMode(true);
	//init the engine
	ZorbaSingleThread_t zorba_engine = ZorbaSingleThread::getInstance();
	try{

		XQuery_t				xquery;
		DynamicQueryContext_t		dctx;

		//create a dynamic context object
		dctx = zorba_engine->createDynamicContext();
    //set current time to a time from future
    struct ::tm   tm1;
    memset(&tm1, 0, sizeof(tm1));
    tm1.tm_year = 2009;
    tm1.tm_mon = 2;//feb
    tm1.tm_mday = 8;
    tm1.tm_hour = 14;
    tm1.tm_min = 6;
    tm1.tm_sec = 0;

    //timezone +2
		dctx->setCurrentDateTime(tm1, +2*60*60);


		//create and compile a query
    xquery = zorba_engine->createQuery("fn:current-dateTime()");

		//execute the query and serialize its result
		xquery->initExecution(dctx);
		xquery->serializeXML(std::cout);

	}catch(ZorbaException &x)
	{
		//output the error message
		cerr << x;
    assert(false);
	}

	ZorbaAlertsManager::setThrowExceptionsMode(original_throw_mode);

	return 0;
}
