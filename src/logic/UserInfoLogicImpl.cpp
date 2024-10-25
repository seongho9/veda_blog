#include "logic/UserInfoLogic.hpp"

using namespace logic;

UserInfoLogicImpl::UserInfoLogicImpl()
{
    _user_storage = storage::UserStorageODBC::getInstance();
    _session_logic = SessionLogicMemory::getInstance();
}

int UserInfoLogicImpl::get_postlist(std::vector<domain::Post>* post, std::string token)
{
    std::string user_id;
    if(_session_logic->verify_token(&user_id, token)){
        return -1;
    }

    int ret = _user_storage->get_post_byuser(post, user_id);
    if(ret){
        return ret;
    }

    return 0;
}

int UserInfoLogicImpl::get_commentlist(std::vector<domain::Comment>* comment, std::string token)
{
    std::string user_id;
    if(_session_logic->verify_token(&user_id, token)){
        return -1;
    }

    int ret = _user_storage->get_comment_byuser(comment, user_id);
    if(ret){
        return ret;
    }

    return 0;   
}

int UserInfoLogicImpl::get_userinfo(domain::User* user, std::string token)
{
    std::string user_id;
    if(_session_logic->verify_token(&user_id, token)){
        return -1;
    }

    int ret = _user_storage->find_user_byid(user, user_id);
    if(ret) {
        return ret;
    }
    return 0;
}