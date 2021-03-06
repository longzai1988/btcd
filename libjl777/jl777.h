
//  Created by jl777
//  MIT License
//
// clusterDefiningBlock

#ifndef gateway_jl777_h
#define gateway_jl777_h

#define NXT_GENESISTIME 1385294400
#define SMALLVAL .000000000000001
#define MAX_LFACTOR 3

#define ORDERBOOK_NXTID ('N' + ((uint64_t)'X'<<8) + ((uint64_t)'T'<<16))    // 5527630

// system includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <math.h>
//#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <float.h>
//#include <limits.h>
#include <zlib.h>
#include <pthread.h>

#ifndef WIN32
//#include <getopt.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include <sys/socket.h>
#include <sys/wait.h>
//#include <sys/time.h>
#include <sys/mman.h>

#include <fcntl.h>

#define ENABLE_DIRENT
#ifdef ENABLE_DIRENT
#include <sys/stat.h>   // only NXTmembers.h
#include <dirent.h>     //only NXTmembers.h
#endif

#include "includes/uv.h"
#include <curl/curl.h>
#include <curl/easy.h>

#else
#include "includes/curl.h"
#include "includes/easy.h"

#include <windows.h>
//#include "utils/pthread.h"
#include "includes/uv.h"
#include "includes/gettimeofday.h"

#ifdef __MINGW32__
#elif __MINGW64__
#else
#define STDIN_FILENO 0
void sleep(int32_t);
void usleep(int32_t);
#endif

#endif

#ifdef UDP_OLDWAY
#define portable_udp_t int32_t
#else
#define portable_udp_t uv_udp_t
#endif

#define portable_tcp_t uv_tcp_t

//#define portable_mutex_t pthread_mutex_t
#define portable_thread_t uv_thread_t

#define portable_mutex_t uv_mutex_t

// includes that include actual code
#include "includes/crypto_box.h"
#include "includes/randombytes.h"

//#include "utils/smoothers.h"
#include "bars.h"
#include "jdatetime.h"
#include "mappedptr.h"
#include "sorts.h"
//#include "utils/kdtree.c"
//#include "bitmap.h"

#include "cJSON.h"
#include "jl777str.h"
#include "cJSON.c"
#include "bitcoind_RPC.c"
#include "jsoncodec.h"

#define ILLEGAL_COIN "ERR"
#define ILLEGAL_COINASSET "666"

#define MGW_PENDING_DEPOSIT -1
#define MGW_PENDING_WITHDRAW -2
#define MGW_FUND_INSTANTDEX -3
#define MGW_INSTANTDEX_TONXTAE -4

#define NXT_COINASSET "0"

#define GENESISACCT "1739068987193023818"
#define NODECOIN_SIG 0x63968736
#define NXTCOINSCO_PORT 8777
#define NXTPROTOCOL_WEBJSON 7777
#define NXT_PUNCH_PORT 6777
#define NXTSYNC_PORT 5777

#define NUM_GATEWAYS 3
#define _NXTSERVER "requestType"
#define EMERGENCY_PUNCH_SERVER Server_names[0]
#define POOLSERVER Server_names[1]
#define MIXER_ADDR Server_names[2]

#define GENESISACCT "1739068987193023818"
#define GENESISBLOCK "2680262203532249785"

#define DEFAULT_NXT_DEADLINE 720
#define SATOSHIDEN 100000000L
#define NXT_TOKEN_LEN 160
#define MAX_NXT_STRLEN 24
#define MAX_NXTTXID_LEN MAX_NXT_STRLEN
#define MAX_NXTADDR_LEN MAX_NXT_STRLEN
#define POLL_SECONDS 10
#define NXT_ASSETLIST_INCR 100

typedef struct queue
{
	void **buffer;
    int32_t capacity,size,in,out,initflag;
	portable_mutex_t mutex;
	//pthread_cond_t cond_full;
	//pthread_cond_t cond_empty;
} queue_t;
//#define QUEUE_INITIALIZER(buffer) { buffer, sizeof(buffer) / sizeof(buffer[0]), 0, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER }

struct pingpong_queue
{
    char *name;
    queue_t pingpong[2],*destqueue,*errorqueue;
    int32_t (*action)();
};

union NXTtype { uint64_t nxt64bits; uint32_t uval; int32_t val; int64_t lval; double dval; char *str; cJSON *json; };

struct NXT_AMhdr
{
    uint32_t sig;
    int32_t size;
    uint64_t nxt64bits;
};

union _json_AM_data { unsigned char binarydata[sizeof(struct compressed_json)]; char jsonstr[sizeof(struct compressed_json)]; struct compressed_json jsn; };

struct json_AM
{
    struct NXT_AMhdr H;
	uint32_t funcid,gatewayid,timestamp,jsonflag;
    union _json_AM_data U;
};

union _NXT_str_buf { char txid[MAX_NXTTXID_LEN]; char NXTaddr[MAX_NXTADDR_LEN];  char assetid[MAX_NXT_STRLEN]; };

struct NXT_str
{
    uint64_t modified,nxt64bits;
    union _NXT_str_buf U;
};

struct Uaddr
{
    struct sockaddr addr;
    uint32_t numsent,numrecv,lastcontact;
    float metric;
};

#define PEER_HELLOSTATE 1
#define NUM_PEER_STATES (PEER_HELLOSTATE+1)

struct peerinfo
{
    uint64_t srvnxtbits,pubnxtbits,coins[4];
    struct Uaddr *Uaddrs;
    uint32_t srvipbits,numsent,numrecv;
    uint16_t srvport,numUaddrs;
    float startmillis[NUM_PEER_STATES + 1],elapsed[NUM_PEER_STATES + 1];
    uint8_t pubkey[crypto_box_PUBLICKEYBYTES],states[NUM_PEER_STATES + 1];
    char pubBTCD[36],pubBTC[36];
};

struct NXThandler_info
{
    double fractured_prob;  // probability NXT network is fractured, eg. major fork or attack in progress
    int32_t upollseconds,pollseconds,firsttimestamp,timestamp,RTflag,NXTheight,hashprocessing;
    int64_t acctbalance;
    uint64_t blocks[1000 * 365 * 10]; // fix in 10 years
    portable_mutex_t hash_mutex;
    void *handlerdata;
    char *origblockidstr,lastblock[256],blockidstr[256];
    queue_t hashtable_queue[2];
    struct hashtable **NXTaccts_tablep,**NXTassets_tablep,**NXTasset_txids_tablep,**NXTguid_tablep,**otheraddrs_tablep;
    cJSON *accountjson;
    uv_udp_t *udp;
    unsigned char loopback_pubkey[crypto_box_PUBLICKEYBYTES],loopback_privkey[crypto_box_SECRETKEYBYTES];
    unsigned char session_pubkey[crypto_box_PUBLICKEYBYTES],session_privkey[crypto_box_SECRETKEYBYTES];
    char pubkeystr[crypto_box_PUBLICKEYBYTES*2+1];
    uint64_t *privacyServers,coins[4];
    CURL *curl_handle,*curl_handle2,*curl_handle3;
    portable_tcp_t Punch_tcp;
    //uv_udp_t Punch_udp;
    int32_t initassets,Lfactor;
    int32_t height,extraconfirms,maxpopdepth,maxpopheight,lastchanged,GLEFU,numblocks,timestamps[1000 * 365 * 10];
    int32_t isudpserver,istcpserver,numPrivacyServers;
    char ipaddr[64],dispname[128],groupname[128];
};
struct NXT_acct *get_NXTacct(int32_t *createdp,struct NXThandler_info *mp,char *NXTaddr);
extern struct NXThandler_info *Global_mp;

#define NXTPROTOCOL_INIT -1
#define NXTPROTOCOL_IDLETIME 0
#define NXTPROTOCOL_NEWBLOCK 1
#define NXTPROTOCOL_AMTXID 2
#define NXTPROTOCOL_TYPEMATCH 3
#define NXTPROTOCOL_ILLEGALTYPE 666

struct NXT_protocol_parms
{
    cJSON *argjson;
    int32_t mode,type,subtype,priority,height;
    char *txid,*sender,*receiver,*argstr;
    void *AMptr;
    char *assetid,*comment;
    int64_t assetoshis;
};

typedef void *(*NXT_handler)(struct NXThandler_info *mp,struct NXT_protocol_parms *parms,void *handlerdata,int32_t height);

struct NXT_protocol
{
    int32_t type,subtype,priority;
    uint32_t AMsigfilter;
    void *handlerdata;
    char **assetlist,**whitelist;
    NXT_handler NXT_handler;
    char name[64];
    char *retjsontxt;
    long retjsonsize;
};

struct NXT_protocol *NXThandlers[1000]; int Num_NXThandlers;
#define MAX_COINTXID_LEN 128
#define MAX_COINADDR_LEN 128
#define MAX_COIN_INPUTS 32
#define MAX_COIN_OUTPUTS 2
#define MAX_PRIVKEY_SIZE 256

struct rawtransaction
{
    struct coin_value *inputs[MAX_COIN_INPUTS];
    char *destaddrs[MAX_COIN_OUTPUTS],txid[MAX_COINADDR_LEN];
    int64_t amount,change,inputsum,destamounts[MAX_COIN_OUTPUTS];
    int32_t numoutputs,numinputs,completed,broadcast,confirmed;
    char *rawtxbytes,*signedtx;
};

union _coin_value_ptr { char *script; char *coinbase; };

struct coin_value
{
    int64_t modified,value;
    char *txid;
    struct coin_txid *parent,*spent,*pendingspend;
    union _coin_value_ptr U;
    int32_t parent_vout,spent_vin,pending_spendvin,isconfirmed,iscoinbase,isinternal;
    char coinaddr[MAX_COINADDR_LEN];
};

struct coin_txid
{
    int64_t modified;
    uint64_t confirmedAMbits,NXTxferbits,redeemtxid;
    char *decodedrawtx;
    int32_t numvins,numvouts,hasinternal,height;
    struct coin_value **vins,**vouts;
    char txid[MAX_COINTXID_LEN];
};

struct coincache_info
{
    FILE *cachefp,*blocksfp;
    struct hashtable *coin_txids;
    char **blocks,*ignorelist;
    int32_t ignoresize,lastignore,numblocks,purgedblock;
};

struct coin_info
{
    int32_t timestamps[100];
    struct coincache_info CACHE;
    //struct pingpong_queue podQ;
    struct hashtable *telepods; void *changepod; uint64_t min_telepod_satoshis;
    void **logs;
    cJSON *ciphersobj;
    char pubaddr[128],NXTACCTSECRET[2048],coinpubkey[1024];
    char srvpubaddr[128],srvNXTACCTSECRET[2048],srvcoinpubkey[1024],srvNXTADDR[64];
    
    char name[64],backupdir[512],privacyserver[32],myipaddr[64];
    char *userpass,*serverport,assetid[64],*marker,*tradebotfname,*pending_ptr;
    uint64_t srvpubnxtbits,pubnxtbits,dust,NXTfee_equiv,txfee,markeramount,lastheighttime,height,blockheight,RTblockheight;
    int32_t coinid,maxevolveiters,initdone,nohexout,use_addmultisig,min_confirms,minconfirms,estblocktime,forkheight,backupcount,enabled,savedtelepods,M,N,numlogs,clonesmear,pending_ptrmaxlen,srvport;
};

#define TELEPORT_DEFAULT_SMEARTIME 3600

#define SETBIT(bits,bitoffset) (((unsigned char *)bits)[(bitoffset) >> 3] |= (1 << ((bitoffset) & 7)))
#define GETBIT(bits,bitoffset) (((unsigned char *)bits)[(bitoffset) >> 3] & (1 << ((bitoffset) & 7)))
#define CLEARBIT(bits,bitoffset) (((unsigned char *)bits)[(bitoffset) >> 3] &= ~(1 << ((bitoffset) & 7)))
#ifndef MIN
#define MIN(x,y) (((x)<=(y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y) (((x)>=(y)) ? (x) : (y))
#endif
typedef char *(*json_handler)(char *verifiedNXTaddr,char *NXTACCTSECRET,struct sockaddr *prevaddr,char *sender,int32_t valid,cJSON **objs,int32_t numobjs,char *origargstr);

char *bitcoind_RPC(CURL *curl_handle,char *debugstr,char *url,char *userpass,char *command,char *args);
#define issue_curl(curl_handle,cmdstr) bitcoind_RPC(curl_handle,"curl",cmdstr,0,0,0)
#define issue_NXT(curl_handle,cmdstr) bitcoind_RPC(curl_handle,"NXT",cmdstr,0,0,0)
#define issue_NXTPOST(curl_handle,cmdstr) bitcoind_RPC(curl_handle,"curl",NXTAPIURL,0,0,cmdstr)
#define fetch_URL(curl_handle,cmdstr) bitcoind_RPC(curl_handle,"fetch",cmdstr,0,0,0)
//void gen_testforms(char *secret);
extern uv_loop_t *UV_loop;
char Server_names[NUM_GATEWAYS+1][MAX_JSON_FIELD];
char Server_NXTaddrs[256][MAX_JSON_FIELD],SERVER_PORTSTR[MAX_JSON_FIELD];
char *MGW_blacklist[256],*MGW_whitelist[256],ORIGBLOCK[MAX_JSON_FIELD],NXTISSUERACCT[MAX_JSON_FIELD];
cJSON *MGWconf,**MGWcoins;
uint64_t MIN_NQTFEE = SATOSHIDEN;
int32_t MIN_NXTCONFIRMS = 10;
uint32_t GATEWAY_SIG;   // 3134975738 = 0xbadbeefa;
int32_t DGSBLOCK = 213000;
int32_t NXT_FORKHEIGHT;
char NXTAPIURL[MAX_JSON_FIELD] = { "http://127.0.0.1:6876/nxt" };
char NXTSERVER[MAX_JSON_FIELD] = { "http://127.0.0.1:6876/nxt?requestType" };

double picoc(int argc,char **argv,char *codestr);
int32_t init_sharenrs(unsigned char sharenrs[255],unsigned char *orig,int32_t m,int32_t n);
uint64_t call_libjl777_broadcast(char *destip,char *msg,int32_t len,int32_t duration);
void calc_sha256(char hashstr[(256 >> 3) * 2 + 1],unsigned char hash[256 >> 3],unsigned char *src,int32_t len);

#include "NXTservices.h"
#include "jl777hash.h"
#include "NXTutils.h"
#include "ciphers.h"
#include "coins.h"
#include "packets.h"
#include "udp.h"

#endif
