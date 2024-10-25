#include "logic/UserLogic.hpp"

using namespace logic;

UserLogicImpl::UserLogicImpl()
{
    _user_storage = storage::UserStorageODBC::getInstance();
    _session_logic = SessionLogicMemory::getInstance();
}

int UserLogicImpl::login(std::string id, std::string password, std::string* token)
{
    std::string ret_token;
    domain::User user;

    //  user is not valid
    if(_user_storage->find_user_byid(&user, id)){
        return 1;
    }
    //  password not matched
    if(user.get_password() != password){
        return 2;
    }
    //  already login
    if(_session_logic->generate_token(token, id)){
        return 3;
    }

    return 0;
}

int UserLogicImpl::logout(std::string id, std::string token)
{
    std::string tmp_id;
    //  토큰이 없음
    if(_session_logic->verify_token(&tmp_id, token)){
        return 1;
    }
    //  id가 맞지 않음
    // if(id != tmp_id){
    //     return 2;
    // }
    _session_logic->delete_token(token);

    return 0;
}

int UserLogicImpl::register_user(domain::User user)
{
    
    int ret = _user_storage->insert_user(user);
    user.get_is_valid() = true;

    //  data empty
    if(user.get_nickname() == "" || user.get_password() == "" || user.get_id() == ""){
        return 1;
    }

    //  ODBC error
    if(ret == 1){
        return 2;
    }
    //  Data  Bind error
    else if(ret == 2){
        return 2;
    }
    //  Query Execute Error
    else if(ret == 3){
        return 3;
    }

    return 0;
}

int UserLogicImpl::delete_user(domain::User user)
{
    //  id empty
    if(user.get_id() == ""){
        return 1;
    }

    int ret = _user_storage->delete_user_byid(user.get_id());

    if(ret){
        return 2;
    }

    return 0;
}