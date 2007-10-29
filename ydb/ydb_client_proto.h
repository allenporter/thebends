// ydb_client_proto.h
// Author: Allen Porter <allen@thebends.org>

enum YdbCommand {
  OPEN_TRANS = 10,
  COMMIT_TRANS = 11,
  ABORT_TRANS = 12,

  GET = 20,
  PUT = 21,
  DELETE = 22,
};

enum YdbResponseCode {
  OK = 10,
  NOT_FOUND = 20,
};

typedef struct {
  uint32_t cmd;
  uint32_t cmd_size;
} YdbRequest;

typedef struct {
  uint32_t resp;
  uint32_t resp_size;
} YdbResponse;

// YdbRequestType = GET
typedef struct {
  YdbRequestType type;
  string key;
} YdbGetRequest; 

typedef struct {
  YdbResponseCode code;
  string key;
  string value; 
} YdbGetResponse;


