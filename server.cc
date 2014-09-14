#include "redirector.h"

using namespace redirector;

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
    }

    //
    // NOT FOUND
    //
    res.setStatus(404);
    res.end();
  });    

  server.listen("0.0.0.0", 8000);
}

