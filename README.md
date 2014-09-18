# SYNOPSIS
A simple redirect server / URL shortener.

# MOTIVATION
An example of how to use [`nodeuv-http`](https://github.com/hij1nx/nodeuv-http).

# USAGE
```bash
./server
```

# HTTP API

## SHORTEN
To shorten a urlencoded long url, add a `url` parameter to the query 
string with its value encoded.

### REQUEST
```
PUT http://mic.cm/shorten?url=http%3A%2F%2Fmic.com%2Fmy-long-url
```

### RESPONSE
```
HTTP/1.1 200 OK
Content-Type: plain/text

aZ9yB
```

## REDIRECT
To redirect to long url, look up the url by the provided id and respond 
with a `301` to the client with the long version of the url.

### REQUEST
```
GET http://mic.cm/aZ9yB
```

### RESPONSE
```
HTTP/1.1 301 Moved Permanently
Location: http://mic.com/my-long-url
```

## SHORTEN EXISTING
When the `/shorten` api call is made on an existing url, the url is looked
up and if it's already shortened, the existing id is returned.

### REQUEST
```
PUT http://mic.cm/shorten?url=http%3A%2F%2Fmic.com%2Fmy-long-url
```

### RESPONSE
```
HTTP/1.1 200 OK
Content-Type: plain/text

aZ9yB
```

