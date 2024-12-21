#include "server.h"
#include "client.h"
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

std::shared_ptr<Client> Server::get_client(std::string id) {
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

bool Server::parse_trx(std::string trx, std::string sender, std::string receiver, double value) {
    //这里利用正则表达式来判断trx是否标准
    std::regex pattern("^[a-zA-Z]+-[a-zA-Z]+-[0-9.]+$");
    //如果不匹配
    if (!std::regex_match(trx, pattern)) {
        throw std::runtime_error("trx is not standard");
    }
    std::stringstream ss(trx);
    std::string part;
    //利用getline来读取一行，用-做分割
    if(std::getline(ss,part,'-')) {
        sender = part;
        std::cout << sender << std::endl;
    }

    if(std::getline(ss,part,'-')) {
        receiver = part;
        std::cout << receiver << std::endl;
    }

    if(std::getline(ss, part)) {
        value = std::stod(part);
        std::cout << value << std::endl;
    }
    return true;
}

bool Server::add_pending_trx(std::string trx, std::string signature) {
    
}

size_t Server::mine() {

}



