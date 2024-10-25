#include "logic/SessinoLogic.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/detail/md5.hpp>

#include <iostream>
#include <iomanip>
#include <string>

using namespace logic;
using namespace std;

SessionLogicMemory::SessionLogicMemory()
{

}

int SessionLogicMemory::generate_token(string* token, string user_id) 
{
    boost::uuids::detail::md5 hash;
    boost::uuids::detail::md5::digest_type digest;

    if(_session_verify.find(user_id) != _session_verify.end()){
        return 1;
    }
    hash.process_bytes(user_id.data(), user_id.size());
    hash.get_digest(digest);

    stringstream ss;

    for(int i=0; i<4; i++){
        ss<<hex<<setw(8)<<setfill('0')<<digest[i];
    }

    *token = ss.str();

    _session.insert({ss.str(), user_id});
    _session_verify.insert({user_id, ss.str()});

    return 0; 
}

int SessionLogicMemory::verify_token(string* user_id, string token)
{
    auto iter = _session.find(token);

    if(iter == _session.end()){
        return 1;
    }
    *user_id = iter->second;

    return 0;
}

int SessionLogicMemory::delete_token(string token)
{
    auto iter = _session.find(token);
    std::string userid = iter->second;

    if(iter == _session.end()){
        return 1;
    }
    
    _session_verify.erase(userid);
    _session.erase(token);

    return 0;
}