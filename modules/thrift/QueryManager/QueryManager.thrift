
/**
 *
 * VELaSSCo Query Manager Thrift RPC interface
 *
 */
 
namespace cpp VELaSSCo
namespace java VELaSSCo

enum Result {
    SUCCESS = 0,
    FAILURE = 1
}

typedef i64 SessionID

struct UserLogin_Result {
  1: required Result    result,
  2: optional SessionID sessionID
}

struct UserLogout_Result {
  1: required Result result
}

struct Query_Result {
  1: required Result result,
  2: optional binary data
}

struct StatusDB_Result {
  1: required Result result,
  2: optional string status
}

struct StopVELaSSCo_Result {
  1: required Result result,
  2: optional string status
}

service QueryManager {

    /**
     * User login
     */
    UserLogin_Result UserLogin(1:string url, 2:string name, 3:string password),

    /**
     * User logout
     */
    UserLogout_Result UserLogout(1:SessionID sessionID),

    /**
     * Query
     */
    Query_Result Query(1:SessionID sessionID, 2:string query)

    /**
      * StatusDB: status of the HBase system or the EDM system
      */
    StatusDB_Result GetStatusDB(1:SessionID sessionID)
    /**
      * StatusDB: status of the HBase system or the EDM system
      */
    StopVELaSSCo_Result StopVELaSSCo(1:SessionID sessionID)
}