#include "redirector.h"

leveldb::DB *redirector::db;

regex URLValidate(
  "\\b((?:[a-z][\\w-]+:(?:\\/{1,3}|[a-z0-9%])|www\\d{0,3}[.]|"
  "[a-z0-9.\\-]+[.][a-z]{2,4}\\/)(?:[^\\s()<>]+|\\(([^\\s()<>]+|"
  "(\\([^\\s()<>]+\\)))*\\))+(?:\\(([^\\s()<>]+|(\\([^\\s()<>]+\\)))*\\)|"
  "[^\\s`!()\\[\\]{};:'\".,<>?«»“”‘’]))");


string chrs ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");


bool redirector::validate_url(string s) {
  smatch sm;
  regex_match (s, sm, URLValidate);
  return sm.size() > 0;
}


string redirector::getNext(string str) {

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


string redirector::put(string url) {

  string decoded_url = uri::UriDecode(url);
  if (!validate_url(decoded_url)) return "";

  // 
  // find out if an id has alreay been created for this url.
  //
  string value;
  leveldb::Status get_status;
  get_status = redirector::db->Get(leveldb::ReadOptions(), "!" + decoded_url, &value);

  if (get_status.ok()) {
    return value;
  }

  //
  // find the last key and increment it. 
  // add the key and decoded url.
  // add the url as a lookup.
  //
  leveldb::Iterator* itr = 
    redirector::db->NewIterator(leveldb::ReadOptions());

  itr->SeekToLast();

  string key = "0";

  if (itr->Valid()) {
    leveldb::Slice lkey = itr->key();
    leveldb::Slice lvalue = itr->value();
    key = getNext(lkey.ToString());
  }

  leveldb::Status status_put;
  leveldb::WriteOptions writeOptions;

  status_put = redirector::db->Put(writeOptions, key, decoded_url);
  status_put = redirector::db->Put(writeOptions, "!" + decoded_url, key);

  if (!status_put.ok()) {
    return "";
  }
  
  delete itr;
  return key;
}


string redirector::get(string key) {

  string value;
  leveldb::ReadOptions readOptions;
  leveldb::Status status = redirector::db->Get(readOptions, key, &value);
  return value;
}

