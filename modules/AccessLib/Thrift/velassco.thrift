
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
}