#include "logic/PostLogic.hpp"

#include "spdlog/spdlog.h"

using namespace logic;


PostLogicImpl::PostLogicImpl()
{
    _post_storage = storage::PostStorageODBC::getInstance();
    _session_logic = SessionLogicMemory::getInstance();
}

int PostLogicImpl::add_post(domain::Post post, const std::vector<std::string>files, std::string token)
{
    std::string user_id;

    //  유효하지 않은 사용자
    if (_session_logic->verify_token(&user_id, token)) {
        return -1;
    }
    
    post.get_author() = user_id;  // 게시글 작성자 설정
    int result = _post_storage->insert_post(post);  // 게시글 저장 
    if(result){
        return result;
    }
    return 0;
}

int PostLogicImpl::delete_post(uint32_t id, std::string token)
{
    std::string user_id;
    //  유효하지 않은 사용자
    if (_session_logic->verify_token(&user_id, token)) {
        return -1;
    }
    domain::Post verify_post;
    std::vector<domain::Comment> comm;

    _post_storage->get_post_byid(&verify_post, &comm, id);
    if(strcmp(verify_post.get_author().c_str(), user_id.c_str())){
        return 9;
    }

    int result = _post_storage->delete_post(id);
    if(result){
        return result;
    }
    return 0;
}

int PostLogicImpl::modify_post(domain::Post post, const std::vector<std::string> files, std::string token)
{

    std::string user_id;
    if (_session_logic->verify_token(&user_id, token)) {
        return -1;
    }
    domain::Post verify;
    std::vector<domain::Comment> comm;
    _post_storage->get_post_byid(&verify, &comm, post.get_id());
    spdlog::info("{} {}", verify.get_author(), user_id);

    if(strcmp(user_id.c_str(), verify.get_author().c_str())){
        return -2;
    }
    int result = _post_storage->modify_post(post, post.get_id());
    if(result){
        return result;
    }
    return 0;
}

int PostLogicImpl::get_postlist(std::vector<domain::Post>* posts) {
    return _post_storage->get_postlist(posts);
}

int PostLogicImpl::get_post(domain::Post* post, std::vector<domain::Comment>* comment, std::string token)
{
    std::string user_id;
    if(_session_logic->verify_token(&user_id, token)){
        return -1;
    }
    _post_storage->get_post_byid(post, comment, post->get_id());

    return 0;
}

int PostLogicImpl::get_file(std::string* file, uint32_t post_id, std::string file_name,  std::string token)
{
    return 0;
}