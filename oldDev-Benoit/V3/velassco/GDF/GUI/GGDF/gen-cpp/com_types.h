/**
 * Autogenerated by Thrift Compiler (1.0.0-dev)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef com_TYPES_H
#define com_TYPES_H

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>




class message;

class InvalidOperation;

typedef struct _message__isset {
  _message__isset() : message1(false), message2(false), message3(false), message4(false), message5(false), message6(false) {}
  bool message1 :1;
  bool message2 :1;
  bool message3 :1;
  bool message4 :1;
  bool message5 :1;
  bool message6 :1;
} _message__isset;

class message {
 public:

  static const char* ascii_fingerprint; // = "319220C08A0EB9FCE7E7E52BE3BFA706";
  static const uint8_t binary_fingerprint[16]; // = {0x31,0x92,0x20,0xC0,0x8A,0x0E,0xB9,0xFC,0xE7,0xE7,0xE5,0x2B,0xE3,0xBF,0xA7,0x06};

  message(const message&);
  message& operator=(const message&);
  message() : message1(), message2(), message3(), message4(), message5(), message6() {
  }

  virtual ~message() throw();
  std::string message1;
  std::string message2;
  std::string message3;
  std::string message4;
  std::string message5;
  std::string message6;

  _message__isset __isset;

  void __set_message1(const std::string& val);

  void __set_message2(const std::string& val);

  void __set_message3(const std::string& val);

  void __set_message4(const std::string& val);

  void __set_message5(const std::string& val);

  void __set_message6(const std::string& val);

  bool operator == (const message & rhs) const
  {
    if (!(message1 == rhs.message1))
      return false;
    if (__isset.message2 != rhs.__isset.message2)
      return false;
    else if (__isset.message2 && !(message2 == rhs.message2))
      return false;
    if (__isset.message3 != rhs.__isset.message3)
      return false;
    else if (__isset.message3 && !(message3 == rhs.message3))
      return false;
    if (__isset.message4 != rhs.__isset.message4)
      return false;
    else if (__isset.message4 && !(message4 == rhs.message4))
      return false;
    if (__isset.message5 != rhs.__isset.message5)
      return false;
    else if (__isset.message5 && !(message5 == rhs.message5))
      return false;
    if (__isset.message6 != rhs.__isset.message6)
      return false;
    else if (__isset.message6 && !(message6 == rhs.message6))
      return false;
    return true;
  }
  bool operator != (const message &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const message & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(message &a, message &b);

typedef struct _InvalidOperation__isset {
  _InvalidOperation__isset() : what(false), why(false) {}
  bool what :1;
  bool why :1;
} _InvalidOperation__isset;

class InvalidOperation : public ::apache::thrift::TException {
 public:

  static const char* ascii_fingerprint; // = "3F5FC93B338687BC7235B1AB103F47B3";
  static const uint8_t binary_fingerprint[16]; // = {0x3F,0x5F,0xC9,0x3B,0x33,0x86,0x87,0xBC,0x72,0x35,0xB1,0xAB,0x10,0x3F,0x47,0xB3};

  InvalidOperation(const InvalidOperation&);
  InvalidOperation& operator=(const InvalidOperation&);
  InvalidOperation() : what(0), why() {
  }

  virtual ~InvalidOperation() throw();
  int32_t what;
  std::string why;

  _InvalidOperation__isset __isset;

  void __set_what(const int32_t val);

  void __set_why(const std::string& val);

  bool operator == (const InvalidOperation & rhs) const
  {
    if (!(what == rhs.what))
      return false;
    if (!(why == rhs.why))
      return false;
    return true;
  }
  bool operator != (const InvalidOperation &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const InvalidOperation & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(InvalidOperation &a, InvalidOperation &b);



#endif
