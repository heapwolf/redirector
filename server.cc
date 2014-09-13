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

string chrs ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

regex URLValidate(
  "\\b((?:[a-z][\\w-]+:(?:\\/{1,3}|[a-z0-9%])|www\\d{0,3}[.]|"
  "[a-z0-9.\\-]+[.][a-z]{2,4}\\/)(?:[^\\s()<>]+|\\(([^\\s()<>]+|"
  "(\\([^\\s()<>]+\\)))*\\))+(?:\\(([^\\s()<>]+|(\\([^\\s()<>]+\\)))*\\)|"
  "[^\\s`!()\\[\\]{};:'\".,<>?«»“”‘’]))");

bool validate_url(string s) {
  smatch sm;
  regex_match (s, sm, URLValidate);
  return sm.size() > 0;
}
 
leveldb::DB *db;

//
// Alpha-numeric increment.
//
string getNext(string str) {

  int l = str.length();
  int len = l;

  while(l--) {
    size_t pos = chrs.find(str[l]);
    if (pos != string::npos && chrs[pos + 1]) {
      str[l] = chrs[pos + 1];
      for(; l < len; l++) {
        str[l+1] = chrs[0];
      }
      break;
    }
    else if (l == 0) {
      return str + "0";
    }
  }
  return str;
}

//
// Put a new entry into the database return an id.
//
string put(string url) {

  string decoded_url = uri::UriDecode(url);
  if (!validate_url(decoded_url)) return "";

  // 
  // find out if an id has alreay been created for this url.
  //
  string value;
  leveldb::Status get_status;
  get_status = db->Get(leveldb::ReadOptions(), "!" + decoded_url, &value);

  if (get_status.ok()) {
    return value;
  }

  //
  // find the last key and increment it. 
  // add the key and decoded url.
  // add the url as a lookup.
  //
  leveldb::Iterator* itr = 
    db->NewIterator(leveldb::ReadOptions());

  itr->SeekToLast();

  string key = "0";

  if (itr->Valid()) {
    leveldb::Slice lkey = itr->key();
    leveldb::Slice lvalue = itr->value();
    key = getNext(lkey.ToString());
  }

  leveldb::Status status_put;
  leveldb::WriteOptions writeOptions;

  status_put = db->Put(writeOptions, key, decoded_url);
  status_put = db->Put(writeOptions, "!" + decoded_url, key);

  if (!status_put.ok()) {
    return "";
  }
  
  delete itr;
  return key;
}

//
// Get a url from a shortened id.
//
string get(string key) {

  string value;
  leveldb::ReadOptions readOptions;
  leveldb::Status status = db->Get(readOptions, key, &value);
  return value;
}

//
// Enter
//
int main() {

  leveldb::Options options;
  options.create_if_missing = true;

  string path = "./db";

  leveldb::Status status = leveldb::DB::Open(options, path, &db);

  if (!status.ok()) {
    cout << "Coun't open LevelDB" << endl;
    return 1;
  }

  Route router;
  Debug debug("server", Debug::verbose, cout);
 
  Server server([&](auto &req, auto &res) {

    auto u = uri::ParseHttpUrl(req.url);
    auto match = router.set(u.path);

    //
    // CREATE SHORT URL
    //
    if (match.test("/shorten")) {

      string id = put(u.query["url"]);

      int status = id.length() > 0 ? 200 : 500;

      res.setStatus(200);
      res.setHeader("Content-Type", "text/plain");
      res.setHeader("Connection", "keep-alive");
      res.end(id);
      return;
    }

    //
    // REDIRECT A URL
    //
    else if (match.test("/:id")) {

      string r = get(match.get("id"));

      if (r.length() > 0) {
        res.setStatus(301, "Moved Permanently");
        res.setHeader("Location", r);
        res.end();
        return;
      }
      else {
        res.setStatus(200);
      }
    }

    //
    // NOT FOUND
    //
    res.setStatus(404);
    res.end();
  });    

  server.listen("0.0.0.0", 8000);
}

