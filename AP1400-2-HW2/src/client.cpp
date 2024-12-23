#include "../include/client.h"
#include "../include/crypto.h"
#include "../include/server.h"
#include <chrono>
#include <random>
#include <string>

Client::Client(std::string id, const Server& server) : id{std::move(id)}, server{&server} {
    //生成公钥和私钥
    crypto::generate_key(this->public_key, this->private_key);
}

std::string Client::get_id() const{
    return id;
}

std::string Client::get_publickey() const{
    return public_key;
}

double Client::get_wallet() const{
    return server->get_wallet(id);
}

std::string Client::sign(std::string txt) const{
    std::string signature = crypto::signMessage(private_key, txt);
    return signature;
}

bool Client::transfer_money(std::string receiver, double value) {
    std::string sender = get_id();
    std::string trx = sender + "-" + receiver + "-" + std::to_string(value);
    std::string trx_signature = sign(trx);
    bool result = server->add_pending_trx(trx, trx_signature);
    return result;
}

size_t Client::generate_nonce() const{
    //获取当前时间戳
    size_t timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis;
    size_t random_value = dis(gen);

    size_t nonce = timestamp + random_value;
    return nonce;
}

