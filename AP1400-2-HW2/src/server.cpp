#include "server.h"
#include "client.h"
#include "crypto.h"
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <sstream>
#include <regex>

std::vector<std::string> pending_trxs;

void show_wallets(const Server& server)
{
	std::cout << std::string(20, '*') << std::endl;
	for(const auto& client: server.clients)
		std::cout << client.first->get_id() <<  " : "  << client.second << std::endl;
	std::cout << std::string(20, '*') << std::endl;
}

std::shared_ptr<Client> Server::add_client(std::string id) {
    //首先判断id是否已经存在
    for(const auto &client: clients) {
        //id已经存在，在id后面加上四位的随机数字
        //这里是否要循环判断？
        if(client.first->get_id() == id) {
            //利用random库随机生成四位数字
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<> distr(0, 9);

            std::string random_number;
            for(int i = 0; i < 4;i++) {
                random_number += std::to_string(distr(gen));
            }
            id += random_number;
        }
    }
    //这里使用id创建一个新的共享指针
    auto new_client = std::make_shared<Client>(id, *this);
    clients.emplace(new_client, 5.0);
    return new_client;
}

std::shared_ptr<Client> Server::get_client(std::string id) const{
    for (const auto & client : clients) {
        if(client.first->get_id() == id) {
            return client.first;
        }
    }
    return nullptr;
}

double Server::get_wallet(std::string id) const {
    for (const auto & client : clients) {
        if(client.first->get_id() == id) {
            return client.second;
        }
    }
    //这里如果没有找到怎么处理？
    return 0;
}

bool Server::parse_trx(std::string trx, std::string &sender, std::string &receiver, double &value) const{
    //这里利用正则表达式来判断trx是否标准
    std::regex pattern("^[a-zA-Z]+-[a-zA-Z]+-[0-9.]+$");
    //如果不匹配
    if (!std::regex_match(trx, pattern)) {
        throw std::runtime_error("trx is not standard");
    }
    std::stringstream ss(trx);
    std::getline(ss, sender, '-');
    std::getline(ss, receiver, '-');
    std::string str_value;
    std::getline(ss, str_value);
    value = std::stod(str_value);
    return true;
}

bool Server::add_pending_trx(std::string trx, std::string signature) const{
    std::string sender, receiver;
    double value;
    //extract useful information
    parse_trx(trx, sender, receiver, value);
    auto sender_client = get_client(sender);
    bool authentic = crypto::verifySignature(sender_client->get_publickey(), trx, signature);
    if(authentic && get_wallet(sender) >= value) {
        pending_trxs.push_back(trx);
        return true;
    }
    return false;
}

size_t Server::mine() {
    std::string mempool;
    for(auto &trx: pending_trxs) {
        mempool += trx;
    }
    bool is_success_mine = false;
    size_t nonce;
    while(!is_success_mine) {
        for(auto it = clients.begin(); it != clients.end(); it++) {
            std::string data = mempool;   
            nonce = it->first->generate_nonce();
            data += std::to_string(nonce);
            std::string hash_data = crypto::sha256(data);
            if (hash_data.substr(0,10).find("000")) {
                std::cout << "Miner id: " << it->first->get_id() << std::endl;
                it->second += 6.25;
                is_success_mine = true;
                break;
            }
        }
    }
    for(auto & trx : pending_trxs) {
        std::string sender, receiver;
        double value;
        parse_trx(trx, sender, receiver, value);
        auto sender_client = get_client(sender);
        sender_client->transfer_money(receiver, value);
    }
    pending_trxs.clear();
    return nonce;
}



