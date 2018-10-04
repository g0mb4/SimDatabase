#define S_FUNCTION_NAME  sfun_simdatabase
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#include "dconsole.h"
#include "mysql_wrapper.h"

#define GET_PARAM_STR( param, str ) \
	tmp_size = (size_t)(mxGetNumberOfElements(ssGetSFcnParam(S, param )) + 1); \
	char * str = (char *)malloc(tmp_size * sizeof(char));					\
	if( str == NULL ){														\
		ssSetErrorStatus(S, "malloc() failed.");							\
		return;																\
	}																		\
	mxGetString(ssGetSFcnParam(S, param ), str, (mwSize)tmp_size)

#define FREE_PARAM_STR( str )	if( str ) free( str )

static bool interpolate = false;

/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct * S){
	dconsole_init();
	dconsole_write("started.");

    ssSetNumSFcnParams(S, 12);  	// number of expected parameters

	/* return if number of expected != number of actual parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
		ssSetErrorStatus(S, "Function needs 12 parameters.");
        return;
    }

	if(!mxIsChar(ssGetSFcnParam(S, 0))){
		ssSetErrorStatus(S, "First parameter is the address, string (eg. '127.0.0.1').");
        return;
	}

	if(!mxIsNumeric(ssGetSFcnParam(S, 1)) || mxGetNumberOfElements(ssGetSFcnParam(S, 1)) != 1){
		ssSetErrorStatus(S, "Second parameter is the port, integer (eg. 3306).");
        return;
	}

	if(!mxIsChar(ssGetSFcnParam(S, 2))){
		ssSetErrorStatus(S, "Third parameter is the username, string (eg. 'root').");
        return;
	}

	if(!mxIsChar(ssGetSFcnParam(S, 3))){
		ssSetErrorStatus(S, "Fourth parameter is the password, string (eg. 'toor', '').");
        return;
	}

	if(!mxIsChar(ssGetSFcnParam(S, 4))){
		ssSetErrorStatus(S, "Fifth parameter is the database, string (eg. 'sim_test').");
        return;
	}

	if(!mxIsChar(ssGetSFcnParam(S, 5))){
		ssSetErrorStatus(S, "Sixth parameter is the tabe, string (eg. 'data').");
        return;
	}

	if(!mxIsChar(ssGetSFcnParam(S, 6))){
		ssSetErrorStatus(S, "Seventh parameter is the start day, string (eg. '2018-04-17').");
        return;
	}

	if(!mxIsChar(ssGetSFcnParam(S, 7))){
		ssSetErrorStatus(S, "Eighth parameter is the start time, string (eg. '06:00:00').");
        return;
	}

	if(!mxIsChar(ssGetSFcnParam(S, 8))){
		ssSetErrorStatus(S, "Ninth parameter is the end day, string (eg. '2018-04-17').");
        return;
	}

	if(!mxIsChar(ssGetSFcnParam(S, 9))){
		ssSetErrorStatus(S, "Tenth parameter is the end time, string (eg. '20:30:00').");
        return;
	}

	if(!mxIsChar(ssGetSFcnParam(S, 10))){
		ssSetErrorStatus(S, "Eleventh parameter is the sensors, string (eg. 'TW,I').");
        return;
	}

	if(!mxIsNumeric(ssGetSFcnParam(S, 11)) || mxGetNumberOfElements(ssGetSFcnParam(S, 11)) != 1){
		ssSetErrorStatus(S, "Twelfth parameter is the interpolation, integer (0 - no, 1 - yes).");
        return;
	}

	size_t tmp_size;
	GET_PARAM_STR( 0, addr );
	GET_PARAM_STR( 2, user );
	GET_PARAM_STR( 3, pass );
	GET_PARAM_STR( 4, db );
	GET_PARAM_STR( 5, table );
	GET_PARAM_STR( 6, start_day );
	GET_PARAM_STR( 7, start_time );
	GET_PARAM_STR( 8, end_day );
	GET_PARAM_STR( 9, end_time );
	GET_PARAM_STR( 10, sensors );

	interpolate = mxGetScalar(ssGetSFcnParam(S, 11)) != 0 ? true : false;

	if(mywrapper_init(addr,
		 			  mxGetScalar(ssGetSFcnParam(S, 1)),	// port
		 		  	  user,
		   			  pass,
		    		  db,
			 	  	  table,
			  		  start_day,
			   		  start_time,
			    	  end_day,
				 	  end_time,
				      sensors) < 0){
		ssSetErrorStatus(S, "Connection failed.");
		return;
	}

	FREE_PARAM_STR( addr );
	FREE_PARAM_STR( user );
	FREE_PARAM_STR( pass );
	FREE_PARAM_STR( db );
	FREE_PARAM_STR( table );
	FREE_PARAM_STR( start_day );
	FREE_PARAM_STR( start_time );
	FREE_PARAM_STR( end_day );
	FREE_PARAM_STR( end_time );
	FREE_PARAM_STR( sensors );

	if(mywrapper_query() < 0){
		ssSetErrorStatus(S, "Query failed.");
		return;
	}

	/* no input */
    if (!ssSetNumInputPorts(S, 0))
		return;

	/* 1 output */
    if (!ssSetNumOutputPorts(S, 1))
		return;

	/* size of the 1st output */
    ssSetOutputPortWidth(S, 0, mywrapper_get_data_size());

	ssSetNumContStates(S, 0);	// number of continuous states
	ssSetNumDiscStates(S, 0);	// number of discrate states

    ssSetNumSampleTimes(S, 1);	// number of sample times = 1
    ssSetNumRWork(S, 0);		// size of a block's floating-point work vector = 0
    ssSetNumIWork(S, 0);		// size of a block's integer work vector = 0
    ssSetNumPWork(S, 0);		// size of a block's pointer work vector = 0
    ssSetNumModes(S, 0);		// size of a block's mode vector = 0
    ssSetNumNonsampledZCs(S, 0);	// number of states for which a block detects zero crossings that occur between sample points = 0

    /* specify the sim state compliance to be same as a built-in block */
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);

    /* take care when specifying exception free code - see sfuntmpl.doc */
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
}

/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct * S){
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}

/* Function: mdlStart =======================================================
* Abstract:
*    This function is called once at start of model execution. If you
*    have states that should be initialized once, this is the place
*    to do it.
*/
static void mdlStart(SimStruct * S){
	/* not exectuted ?? */
}

/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block.
 */
static void mdlOutputs(SimStruct * S, int_T tid){
	UNUSED_ARG(tid);

	real_T * y = (real_T *)ssGetOutputPortSignal(S, 0);
	//real_T k = mxGetPr(PARAM1(S))[0];
	uint32_t size = mywrapper_get_data_size();
	double * out = (double *)malloc(size * sizeof(double));
	if(out == NULL){
		ssSetErrorStatus(S, "malloc() failed.");
		return;
	}
	mywrapper_step(ssGetT(S), interpolate, out);
	for(int i = 0; i < size; i++){
		y[i] = (real_T)out[i];
	}
	free(out);
}

/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct * S){
    UNUSED_ARG(S);

    mywrapper_deinit();
    dconsole_write("stoped.");
	dconsole_deinit();
}

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
    #include "simulink.c"      /* MEX-file interface mechanism */
#else
    #include "cg_sfun.h"       /* Code generation registration function */
#endif
