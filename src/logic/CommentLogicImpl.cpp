#include "logic/CommentLogic.hpp"



using namespace logic;

CommentLogicImpl::CommentLogicImpl()
{
    _comment_storage = storage::CommentStorageODBC::getInstance();
    _session_logic = SessionLogicMemory::getInstance();
}

int CommentLogicImpl::add_comment(domain::Comment comment, std::string token)
{
    std::string user_id;

    if(_session_logic->verify_token(&user_id, token)){
        return -1;
    }

    comment.get_author() = user_id;
    int retcode = _comment_storage->insert_comment(comment.get_post_id(), comment);

    return retcode;
}

int CommentLogicImpl::delete_comment(uint32_t id, std::string token)
{
    std::string user_id;

    if(_session_logic->verify_token(&user_id, token)){
        return -1;
    }

    int ret = _comment_storage->delete_comment(id); 

    return ret;
}

int CommentLogicImpl::modify_comment(domain::Comment comment, std::string token)
{
    std::string user_id;

    if(_session_logic->verify_token(&user_id, token)){
        return -1;
    }

    comment.get_author() = user_id;

    return _comment_storage->modify_comment(comment);
}

int CommentLogicImpl::get_commentlist(std::vector<domain::Comment>* comments, uint32_t postid, std::string token)
{
    std::string user_id;

    if(_session_logic->verify_token(&user_id, token)){
        return -1;
    }

    return _comment_storage->get_commentlist(comments, postid);
}