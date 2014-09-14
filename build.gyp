{
  'includes': [ 'common.gypi' ],
  'targets': [
    {
      'target_name': 'server',
      'type': 'executable',
      'sources': [
        './deps/nodeuv-http/http.cc', 
        'redirector.cc',
        'server.cc',
      ],
      'include_dirs': [
        './deps/leveldb/include',
        './deps/nodeuv-http',
        './deps/nodeuv-http/deps/http-parser',
        './deps/nodeuv-uri',
        './deps/route',
        './deps/debug',
      ],
      'link_settings': {
        'libraries': [
          '-lleveldb',
          '-lhttp_parser',
          '-lroute',
          '-luv',
          '-lnodeuv-http',
        ],
        'ldflags': [
          '-lleveldb',
          '-lhttp_parser',
          '-lroute',
          '-luv',
          '-lnodeuv-http',
        ]
      }
    },
  ],
}

