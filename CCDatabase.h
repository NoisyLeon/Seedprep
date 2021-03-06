/* An implementation of the Cross-Correlation database */
/* Consists of 3 data structs: CCPARAM, Seedlist, and Stationlist */

/* It reads in and checks all CC parameters through an input parameter file and
 * loads in the seed and station list provided by the parameter file on initialization*/

/* The methods 'NextRec()' and 'Relocate()' are used to extract and initialize
 * daily records from the database, which can then be processed and saved as needed */


#ifndef CCDATABASE_H
#define CCDATABASE_H

#include "InfoLists.h"
#include <cstdio>
#include <vector>
#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>


#ifndef FuncName
#define FuncName __FUNCTION__
#endif

/* ------------------------------ CCPARAM ------------------------------ */
/* CC parameter wrapper */
struct CCPARAM
{
    //SAC_DB *sdb;
    //-------------------------------------------------------------parameters----------------------------------------------------------//
    std::string rdsexe;				// rdseed excutable
    std::string evrexe;				// evalresp excutable
    std::string stafname;			// station list (J23A -129.6825 +44.844 TA optional_flag) (flag controls which sta-pairs to be ccd)
									// (skip cc between sta-pairs that are 1> both flagged 0 or 2> both not 0 but flagged with different numbers)
									// set optional_flag to Negative number to skip autocorrelation
    std::string seedfname;			// SEED file list (down_US_ALL/OBS_BHZ_2012.FEB.29.203762.seed 2012 2 29)
    std::string chlst_info;			// the line that contains channel name list
    int sps 		= NaN;			// target sampling rate
    float gapfrac 	= NaN;			// maximum allowed gap fraction in input sac record
    float t1 		= NaN;			// cutting begining time in sec
    float tlen 		= NaN;			// time-record length in sec
    float perl = NaN, perh = NaN;	// signal period band
    int tnorm_flag 	= NaN;			// temperal normalization method. (1 for onebit, 2 for running average, 3 for earthquake cutting)
    float Eperl = NaN, Eperh = NaN;	// estimated earthquake period band (no effect when tnorm_flag==1; set Eperl=-1 to turn off the filter)
    float timehlen 	= NaN;			// half len of time window for running average (temperal normalization) (no effect when tnorm_flag==1)
    float frechlen 	= NaN;			// half len of frec window for whitenning. recommended: 0.0002; 0: no norm; -1: use input-smoothing file.
    std::string fwname;				// input spectrum reshaping signal file (takes effect only when frechlen==-1)
    int ftlen 		= NaN;			// turn on/off (1/0) cross-correlation-time-length correction for amplitude
    int fprcs 		= NaN;			// turn on/off (1/0) precursor signal checking
    float memomax 	= NaN;			// maximum memory fraction to be used. (set according to number of threads)
    int lagtime 	= NaN;			// cross-correlation signal half length in sec
    int mintlen 	= NaN;			// allowed minimum time length for cross-correlation (takes effect only when ftlen = 1)
    int fdelosac 	= 0;			// delete original sac files after removing response when set to 1
    int fdelamph 	= 0;			// delete am&ph files after cross-correlation when set to 1
    int fskipesac 	= 0;			// skip ExtractSac() when set to 2, skip upon existence of target file when set to 1
    int fskipresp 	= 0;			// skip RmRESP() when set to 2, skip upon existence of target file when set to 1
    int fskipamph 	= 0;			// skip TempSpecNorm() when set to 2, skip upon existence of target file when set to 1
    int fskipcrco 	= 0;			// skip CrossCorr() if target file exists when set to 1
    int CorOutflag	= 0;     		// controls the output of cross-corellation results: 0=monthly, 1=daily, 2=both
    int fstackall	= 0;
    //---------------------------------------------------------------------------------------------------------------------------------//
    CCPARAM() {}
    CCPARAM( const std::string fname )
    {
        Load(fname);
    }
    void Load( const std::string );
    bool CheckAll();
private:
    static constexpr float NaN = -12345.;
    enum SetRet { Succeed = 1, Comment = 0, EmptyInfo = -1, EmptyField = -2, BadValue = -3, InvalidValue = -4 };
    SetRet Set( const std::string& );
};


/* ------------------------------ CCDatabase ------------------------------ */
class CCDatabase
{
private:
    /* input parameters */
    CCPARAM CCParams;
    /* channel list */
    Channellist chlst;
    /* seed rec list */
    Seedlist seedlst;
    /* station list */
    Stationlist stalst;
    /* current daily info */
    DailyInfo dinfo;
    bool dinfo_rdy;

    void FillDInfo();

public:
    /* constructor (read in parameters, seed list, and station list) */
    CCDatabase( const std::string& inname );

    /* return a copy of the CC Parameters */
    const CCPARAM& GetParams() const
    {
        return CCParams;
    }
	
	Stationlist GetstaList() 
    {
        return stalst;
    }
	
	Channellist GetchList() 
    {
        return chlst;
    }
	
	Seedlist GetSeedList()
	{
		return seedlst;
	}
    /* Get the next daily record from the database. Assign file names and make directory if necessary */
    bool NextRecTest();
    bool NextRec();
    bool NextEvent();
    void Rewind();
    bool GetRec(DailyInfo&);
    bool GetRec_AllCH( std::vector<DailyInfo>& dinfoV );
    /*
       void InitialPthread();
       void FillMonths();
       void FillEvents();
    */

    //destructor (deallocate memories)
    ~CCDatabase() {};
//   void CleanupPthread();
};



/* ------------------------------ CCDatabase Exceptions ------------------------------ */
namespace ErrorCD
{

class Base : public std::runtime_error
{
public:
    Base( const std::string funcname, const std::string message )
        : runtime_error(message), funcname(funcname)
    {
        //PrintStacktrace();
    }
private:
    std::string funcname;
};

class BadFile : public Base
{
public:
    BadFile(const std::string funcname, const std::string info = "")
        : Base(funcname, "Cannot access file ("+info+").") {}
};

class BadParam : public Base
{
public:
    BadParam(const std::string funcname, const std::string info = "")
        : Base(funcname, "Bad parameters ("+info+").") {}
};

};

#endif

