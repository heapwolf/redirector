#ifndef REDIRECTOR_H_
#define REDIRECTOR_H_

#include "http.h"
#include "route.h"
#include "debug.h"
#include "uri.h"
#include "leveldb/db.h"

#include <string>
#include <regex>

using namespace std;
using namespace http;
using namespace route;

namespace redirector {

  //
  // an instance of leveldb.
  //
  extern leveldb::DB *db;

  //
  // ensure a string is a valid url.
  //
  bool validate_url(string);

  //
  // return the next item in an 
  // alphanumeric sequence.
  //
  string getNext(string);

  //
  // put a new url into the database
  // and return a unique id for it.
  // if the url is already in the db
  // return its existing id.
  //
  string put(string url);

  //
  // return the url for an id.
  //
  string get(string id);
}

#endif

