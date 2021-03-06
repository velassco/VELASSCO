/**
 * Autogenerated by Thrift Compiler (0.9.3)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef QueryManager_TYPES_H
#define QueryManager_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>


namespace VELaSSCo {

struct Result {
  enum type {
    SUCCESS = 0,
    FAILURE = 1
  };
};

extern const std::map<int, const char*> _Result_VALUES_TO_NAMES;

typedef int64_t SessionID;

class UserLogin_Result;

class UserLogout_Result;

class Query_Result;

class StatusDB_Result;

class StopVELaSSCo_Result;

typedef struct _UserLogin_Result__isset {
  _UserLogin_Result__isset() : sessionID(false) {}
  bool sessionID :1;
} _UserLogin_Result__isset;

class UserLogin_Result {
 public:

  UserLogin_Result(const UserLogin_Result&);
  UserLogin_Result& operator=(const UserLogin_Result&);
  UserLogin_Result() : result((Result::type)0), sessionID(0) {
  }

  virtual ~UserLogin_Result() throw();
  Result::type result;
  SessionID sessionID;

  _UserLogin_Result__isset __isset;

  void __set_result(const Result::type val);

  void __set_sessionID(const SessionID val);

  bool operator == (const UserLogin_Result & rhs) const
  {
    if (!(result == rhs.result))
      return false;
    if (__isset.sessionID != rhs.__isset.sessionID)
      return false;
    else if (__isset.sessionID && !(sessionID == rhs.sessionID))
      return false;
    return true;
  }
  bool operator != (const UserLogin_Result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UserLogin_Result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(UserLogin_Result &a, UserLogin_Result &b);

inline std::ostream& operator<<(std::ostream& out, const UserLogin_Result& obj)
{
  obj.printTo(out);
  return out;
}


class UserLogout_Result {
 public:

  UserLogout_Result(const UserLogout_Result&);
  UserLogout_Result& operator=(const UserLogout_Result&);
  UserLogout_Result() : result((Result::type)0) {
  }

  virtual ~UserLogout_Result() throw();
  Result::type result;

  void __set_result(const Result::type val);

  bool operator == (const UserLogout_Result & rhs) const
  {
    if (!(result == rhs.result))
      return false;
    return true;
  }
  bool operator != (const UserLogout_Result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UserLogout_Result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(UserLogout_Result &a, UserLogout_Result &b);

inline std::ostream& operator<<(std::ostream& out, const UserLogout_Result& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _Query_Result__isset {
  _Query_Result__isset() : data(false) {}
  bool data :1;
} _Query_Result__isset;

class Query_Result {
 public:

  Query_Result(const Query_Result&);
  Query_Result& operator=(const Query_Result&);
  Query_Result() : result((Result::type)0), data() {
  }

  virtual ~Query_Result() throw();
  Result::type result;
  std::string data;

  _Query_Result__isset __isset;

  void __set_result(const Result::type val);

  void __set_data(const std::string& val);

  bool operator == (const Query_Result & rhs) const
  {
    if (!(result == rhs.result))
      return false;
    if (__isset.data != rhs.__isset.data)
      return false;
    else if (__isset.data && !(data == rhs.data))
      return false;
    return true;
  }
  bool operator != (const Query_Result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Query_Result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(Query_Result &a, Query_Result &b);

inline std::ostream& operator<<(std::ostream& out, const Query_Result& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _StatusDB_Result__isset {
  _StatusDB_Result__isset() : status(false) {}
  bool status :1;
} _StatusDB_Result__isset;

class StatusDB_Result {
 public:

  StatusDB_Result(const StatusDB_Result&);
  StatusDB_Result& operator=(const StatusDB_Result&);
  StatusDB_Result() : result((Result::type)0), status() {
  }

  virtual ~StatusDB_Result() throw();
  Result::type result;
  std::string status;

  _StatusDB_Result__isset __isset;

  void __set_result(const Result::type val);

  void __set_status(const std::string& val);

  bool operator == (const StatusDB_Result & rhs) const
  {
    if (!(result == rhs.result))
      return false;
    if (__isset.status != rhs.__isset.status)
      return false;
    else if (__isset.status && !(status == rhs.status))
      return false;
    return true;
  }
  bool operator != (const StatusDB_Result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const StatusDB_Result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(StatusDB_Result &a, StatusDB_Result &b);

inline std::ostream& operator<<(std::ostream& out, const StatusDB_Result& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _StopVELaSSCo_Result__isset {
  _StopVELaSSCo_Result__isset() : status(false) {}
  bool status :1;
} _StopVELaSSCo_Result__isset;

class StopVELaSSCo_Result {
 public:

  StopVELaSSCo_Result(const StopVELaSSCo_Result&);
  StopVELaSSCo_Result& operator=(const StopVELaSSCo_Result&);
  StopVELaSSCo_Result() : result((Result::type)0), status() {
  }

  virtual ~StopVELaSSCo_Result() throw();
  Result::type result;
  std::string status;

  _StopVELaSSCo_Result__isset __isset;

  void __set_result(const Result::type val);

  void __set_status(const std::string& val);

  bool operator == (const StopVELaSSCo_Result & rhs) const
  {
    if (!(result == rhs.result))
      return false;
    if (__isset.status != rhs.__isset.status)
      return false;
    else if (__isset.status && !(status == rhs.status))
      return false;
    return true;
  }
  bool operator != (const StopVELaSSCo_Result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const StopVELaSSCo_Result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(StopVELaSSCo_Result &a, StopVELaSSCo_Result &b);

inline std::ostream& operator<<(std::ostream& out, const StopVELaSSCo_Result& obj)
{
  obj.printTo(out);
  return out;
}

} // namespace

#endif
